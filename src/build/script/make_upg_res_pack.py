#!/usr/bin/python3
# coding=utf-8
# ============================================================================
# @brief    Build Update Resource Pkg File
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================

"""
用法: python pack.py [-i INDEX_FILE] [-d DATA_FILE] [res_dir]
  res_dir               待打包目录路径, 仅删除时无需填写此参数
  -i INDEX_FILE, --index-file INDEX_FILE
                        生成的索引文件名称
  -d DATA_FILE, --data-file DATA_FILE
                        生成的数据文件名称, 仅删除时此参数无效
"""

from enum import IntEnum, auto
import sys
import os
import struct
import ctypes
import shutil

file_dir = os.path.dirname(os.path.realpath(__file__))
g_root = os.path.realpath(os.path.join(file_dir, "..", ".."))

DEFAULT_INDEX_FILE = os.path.join(g_root, "output", "res", "res_index.bin")
DEFAULT_DATA_FILE  = os.path.join(g_root, "output", "res", "res_data.bin")

class UpgResourceOperation(IntEnum):
    ADD_FILE = 0
    AMEND_FILE = auto()
    DELETE_FILE = auto()
    REMOVE_DIR = auto()

class Structure(ctypes.LittleEndianStructure):
    def encode(self):
        return ctypes.string_at(ctypes.addressof(self), ctypes.sizeof(self))

    def decode(self, data):
        ctypes.memmove(ctypes.addressof(self), data, ctypes.sizeof(self))
        return len(data)

class UpgResourceNode(Structure):
    _fields_ = [
        ("file_path", ctypes.c_char * 128),
        ("offset", ctypes.c_uint32),
        ("file_len", ctypes.c_uint32),
        ("operation_type", ctypes.c_uint32),
    ]

class UpgResourceIndex(Structure):
    _fields_ = [
        ("file_num", ctypes.c_uint32),
        ("file_node", UpgResourceNode * 0),
    ]

def res_pack(res_dir, index_path=DEFAULT_INDEX_FILE, data_path=DEFAULT_DATA_FILE, deleted_files=[], removed_dirs=[]):
    """
    打包指定资源目录,会先删除文件或目录，再修改/增加文件
    :param res_dir: 待打包目录路径, 如果仅删除则填None
    :param index_path: 生成的索引文件路径
    :param data_path: 生成的数据文件路径, 仅删除时此参数无效
    :param deleted_files: 待删除文件列表
    :param removed_dirs: 待删除目录列表
    :returns: 是否成功打包
    """

    os.makedirs(os.path.dirname(os.path.realpath(index_path)), exist_ok=True)

    with open(index_path, "wb") as index_file:
        file_num = 0
        index_file.write(b"\x00" * ctypes.sizeof(UpgResourceIndex))

        for name in deleted_files:
            if not name.startswith("/"):
                name = "/" + name
            print(f"Delete file '{name}'")

            node = UpgResourceNode()
            node.file_path = name.encode()
            node.offset = 0
            node.file_len = 0
            node.operation_type = UpgResourceOperation.DELETE_FILE
            index_file.write(node.encode())

            file_num += 1

        for name in removed_dirs:
            if not name.startswith("/"):
                name = "/" + name
            print(f"Remove dir '{name}'")

            node = UpgResourceNode()
            node.file_path = name.encode()
            node.offset = 0
            node.file_len = 0
            node.operation_type = UpgResourceOperation.REMOVE_DIR
            index_file.write(node.encode())

            file_num += 1

        if res_dir is not None:
            pack_dir = os.path.realpath(res_dir)
            if not os.path.isdir(pack_dir):
                print("Error: invaild resource directory!")
                return False
            print(f"Packing resource directory: {pack_dir}")

            data_file = None
            for root, dirs, files in os.walk(pack_dir):
                for name in files:
                    if data_file is None:
                        os.makedirs(os.path.dirname(os.path.realpath(data_path)), exist_ok=True)
                        data_file = open(data_path, "wb")

                    full_file_path = os.path.join(root, name)
                    file_path = full_file_path[len(pack_dir):].replace("\\", "/")
                    if len(file_path.encode()) > UpgResourceNode.file_path.size - 1:
                        print(f"The length of '{file_path}' is greater than {UpgResourceNode.file_path.size - 1}, skip!")
                        continue
                    print(f"Add '{full_file_path}' -> '{data_path}:{file_path}'")

                    node = UpgResourceNode()
                    node.file_path = file_path.encode()
                    node.offset = data_file.tell()
                    node.file_len = os.path.getsize(full_file_path)
                    node.operation_type = UpgResourceOperation.AMEND_FILE
                    index_file.write(node.encode())
                    with open(full_file_path, "rb") as res_file:
                        shutil.copyfileobj(res_file, data_file)

                    file_num += 1

            if data_file is not None:
                data_file.close()
                print(f"Added {file_num} files data into {data_path}")

        index_file.seek(0)
        index_file.write(struct.pack("<I", file_num))

        print(f"Finished, merge {file_num} nodes into {index_path}")
        return True

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("res_dir", nargs="?", default=None, type=str, help="Resource directory path")
    parser.add_argument("-i", "--index-file", default=DEFAULT_INDEX_FILE, type=str, help="Generated index file path")
    parser.add_argument("-d", "--data-file", default=DEFAULT_DATA_FILE, type=str, help="Generated data file path")
    args = parser.parse_args()

    test_deleted_files = [ # 待删除文件
    ]
    test_removed_dirs = [ # 待删除目录
    ]
    res_pack(args.res_dir, args.index_file, args.data_file, test_deleted_files, test_removed_dirs)
