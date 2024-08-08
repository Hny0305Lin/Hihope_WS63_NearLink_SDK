#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

"""
* Description: Utilities of compile system.
* Create: 2020-1-2
"""

import os
import sys
import time
import subprocess
import re
import shutil
import fnmatch
import struct
import hashlib
import platform
import filecmp

root_path = os.path.join(os.path.split(os.path.realpath(__file__))[0], '..', '..', '..')
root_path = os.path.abspath(root_path)
script_path = os.path.join(root_path, 'build', 'script')
target_config_path = os.path.join(root_path, 'build', 'config', 'target_config')
output_root = os.path.join(root_path, 'output')
sdk_output_path = os.path.join(output_root, 'sdk')
pkg_tools_path = os.path.join(root_path, 'tools', 'pkg')
jlink_tools_path = os.path.join(root_path, 'tools', 'bin', 'jlink_tool')
lzma_tools_path = os.path.join(root_path, 'tools', 'bin', 'lzma_tool')
sign_tools_path = os.path.join(root_path, 'tools', 'bin', 'sign_tool')
derived_tools_path = os.path.join(root_path, 'tools', 'bin', 'derived_key_tool')

"""
Colors defines. To highlight important output.
"""
__colors__ = {'purple': '\033[95m', 'red': '\033[91m', 'blue': '\033[94m', 'green': '\033[92m', 'end': '\033[0m'}


class BuildError(Exception):
    """
    Error handling, highlight in red.
    """
    def __init__(self, err):
        emsg = "%s%s%s" % (color_red(), err, color_end())
        Exception.__init__(self, emsg)


class BuildTimer:
    """
    timer
    """
    def __init__(self, name='A'):
        self._start = -1
        self._name = name

    def start(self):
        self._start = time.time()

    def stop(self):
        if self._start == -1:
            raise BuildError("Timer %s never been started!" % self._name)
        retval = time.time() - self._start
        self._start = -1
        return retval


class CopyModule:
    def __init__(self, replace_root='', replace_suffix='', pattern='*', copy_header = True):
        self.replace_root = replace_root
        if replace_suffix and isinstance(replace_suffix, str):
            replace_suffix = [replace_suffix]

        self.replace_suffix = replace_suffix
        self.mask = ['__pycache__', '.git*']
        self.pattern = pattern
        self.copy_header = copy_header

    def append_mask(self, mask):
        if isinstance(mask, list):
            self.mask.extend(mask)
            return
        self.mask.append(mask)

    def __ignore(self, path, names):
        ignored_names = []
        names = set(names)
        for mask in self.mask:
            ignored_names.extend(fnmatch.filter(names, mask))
        if self.pattern == "*.h" and 'drivers/hal' not in path:
            for name in names:
                if "." in name and not name.endswith('.h'):
                    ignored_names.append(name)
        else:
            ignored_names.extend(names - set(fnmatch.filter(names, self.pattern)))
        return set(ignored_names)

    def copy_file(self, file, dest=None):
        if dest is None and self.replace_root:
            dest = file.replace(root_path,  self.replace_root)
        if os.path.exists(dest) and os.path.isfile(dest):
            return None
        path = os.path.split(dest)[0]
        if not os.path.exists(path):
            os.makedirs(path, exist_ok=True)
        if self.replace_suffix:
            for suffix in self.replace_suffix:
                if os.path.exists(file + suffix):
                    file = file + suffix
                    break
        try:
            shutil.copy2(file, dest, follow_symlinks=True)
        except:
            print("[WARN] copy %s exception" % file)
            return
        if self.copy_header:
            self.copy_file_header(file)
        return dest

    #add for copy header file
    def copy_file_header(self, file):
        file_path = os.path.dirname(file)
        for inner_file in os.listdir(file_path):
            if not inner_file.endswith('.h'):
                continue
            tmp_src = "%s/%s" %(file_path, inner_file)
            tmp_dest = "%s/%s" %(file_path.replace(root_path, sdk_output_path), inner_file)
            if self.replace_suffix:
                for suffix in self.replace_suffix:
                    if os.path.exists(tmp_src + suffix):
                        tmp_src = tmp_src + suffix
                        break
            if not os.path.exists(tmp_dest):
                if not os.path.isdir(os.path.dirname(tmp_dest)):
                    os.makedirs(os.path.dirname(tmp_dest), exist_ok=True)
                shutil.copy(tmp_src, tmp_dest)

    def copy_folder(self, path, dest=None):
        if dest is None and self.replace_root:
            dest = path.replace(root_path, self.replace_root)
        shutil.copytree(path, dest, copy_function=self.copy_file, dirs_exist_ok=True, ignore=self.__ignore)
        return dest

    def copy(self, src, dest=None):
        if not os.path.exists(src):
            print("WARNING : Sdk copy src: %s is not exists!!" % src)
            return
        if os.path.isfile(src):
            return self.copy_file(src, dest)
        else:
            return self.copy_folder(src, dest)


# End of class BuildTimer

def color_red():
    return __colors__.get('red')


def color_purple():
    return __colors__.get('purple')


def color_blue():
    return __colors__.get('blue')


def color_green():
    return __colors__.get('green')


def color_end():
    return __colors__.get('end')


def print_info(msg):
    print(msg)


def print_tips(msg):
    print("%s%s%s" % (color_purple(), msg, color_end()))


def print_warning(msg):
    print("%s%s%s" % (color_green(), msg, color_end()))


def print_alert(msg):
    print("%s%s%s" % (color_red(), msg, color_end()))

def cmp_file(f1, f2):
    filecmp.clear_cache()
    return filecmp.cmp(f1, f2, shallow = False)

def fn_filter_dirs(dirs, filters=None):
    if filters is None:
        filters = []
    retval = list(dirs)
    for dir_path in dirs:
        for item in filters:
            fstr = "%s%s%s" % (os.sep, item, os.sep)
            if dir_path.find(fstr) >= 0:
                try:
                    print("remove dir_path:%s" % dir_path)
                    retval.remove(dir_path)
                except ValueError as e:
                    print(e)

    return retval


def fn_search_all_files(top_dir, file_name, excludes=[]):
    """
    Traverse sub-folders to find all files named "file_name".
    """
    retval = []
    for dir_path, dir_names, file_names in os.walk(top_dir, followlinks=True):
        # remove useless folder first
        dir_names = [dir_names.remove(x) for x in dir_names if x.startswith(".")]
        if file_name in file_names:
            retval.append(os.path.join(dir_path, file_name))
    return fn_filter_dirs(retval, excludes)


def fn_search_all_dirs(top_dir, dir_name, excludes=None):
    """
    Traverse sub-folders to find all files named "dir_name".
    """
    if excludes is None:
        excludes = []
    retval = []
    for dir_path, dir_names, file_names in os.walk(top_dir, followlinks=True):
        if not dir_names:
            continue
        # remove useless folder first
        temp_dirs = list(dir_names)
        dirnames = [x for x in dir_names if not x.startswith(".")]
        for dirname in dirnames:
            if dirname and dirname == dir_name:
                retval.append(os.path.join(dir_path, dirname))
    return fn_filter_dirs(retval, excludes)


def fn_get_subdirs(dir_path):
    lst = [name for name in os.listdir(dir_path) if os.path.isdir(os.path.join(dir_path, name)) and name[0] != '.']
    lst.sort()
    return lst


def fn_str_to_int(text, num=None):
    if num is not None:
        return int(text, num)
    match1 = re.match(r'\s*0x', text)
    match2 = re.match(r'\s*0X', text)
    if match1 or match2:
        return int(text, 16)
    else:
        return int(text, 10)


def bf_to_str(bf):
    """
    Convert build error from scons to string.
    """
    if bf is None:
        return '(unknown targets product None in list)'
    elif bf.node:
        return str(bf.node) + ': ' + bf.errstr
    elif bf.filename:
        return bf.filename + ': ' + bf.errstr
    else:
        return str(bf)


def exec_shell(cmd, logfile=None, cmd_dump=False):
    """
    call shell
    """
    cmd_list = cmd
    if isinstance(cmd, str):
        cmd_list = cmd.split(' ')

    if cmd_dump:
        print(str(cmd_list))

    subp = subprocess.Popen(cmd_list, shell=False)
    subp.wait()
    code = subp.returncode

    return code


def add_temp_sys_path(path):
    env_path = os.environ.get('PATH')
    if path.startswith(os.sep):
        work_path = path
    else:
        work_path = os.path.join(os.getcwd(), path)

    if work_path not in env_path:
        new_env_path = ':'.join([work_path, env_path])
        os.environ['PATH'] = new_env_path
    return os.environ.get('PATH')


def rm_all(items):
    for item in items:
        if os.path.isdir(item):
            shutil.rmtree(item)
        elif os.path.isfile(item):
            os.unlink(item)
        else:
            pass

def add_len_and_sha256_info_to_ssb(source, chip=None):
    with open(source, "rb+") as bin_file:
        length = len(bin_file.read())
        if chip == "brandy":
            bin_file.seek(364, 0) # ssb length offset addr 0x16c
        elif chip == "socmn1":
            bin_file.seek(404, 0) # ssb length offset addr 0x194
        else:
            bin_file.seek(360, 0) # ssb length offset addr 0x168
        bin_file.write(struct.pack('<L', length))
        bin_file.close()

    with open(source, "rb+") as bin_file:
        sha = hashlib.sha256(bin_file.read())
        bin_file.write(sha.digest()[0:32])
        bin_file.close()

def create_hex_file(target, source):
    with open(source, "rb") as binfile, open(
                target, "wb") as hexfile:
        while True:
            bindata = binfile.read(4)
            if not bindata:
                break
            longdata, = struct.unpack("<L", bindata)
            hexstr = '{:x}'.format(longdata)  # dec to hex number str
            hexstr = '%s\n' % '{:0>8}'.format(hexstr).upper()
            hexfile.write(str.encode(hexstr))

def compare_bin(bin1, bin2):
    print("Comparing:")
    print(bin1)
    print(bin2)
    if not cmp_file(bin1, bin2):
        print("DIFF")
        return False
    print("SAME")
    return True

def rm_pyc(root):
    pyc_dirs = fn_search_all_dirs(root, "__pycache__")
    rm_all(pyc_dirs)


def get_diff(list0, list1):
    diff = list(set(list0) - set(list1))
    return diff


def copy(src, dest, replace_suffix='', pattern="*"):
    c = CopyModule(replace_suffix=replace_suffix, pattern=pattern)
    c.copy(src, dest)

def copy_force(src, dest, pattern = ".a"):
    for file_name in os.listdir(src):
        if file_name.endswith(pattern):
            tmp_src = "%s/%s" %(src, file_name)
            shutil.copy(tmp_src, dest)

def get_platform_name():
    return platform.system().lower()

if __name__ == "__main__":
    func_list = [
        "copy",
        "add_len_and_sha256_info_to_ssb",
        "create_hex_file",
        "copy_force"
    ]
    func = sys.argv[1]
    if func not in func_list:
        print("ERROR! WRONG FUNC!! YOU CAN ONLY INVOKE FUNC BELOW:")
        for function in func_list:
            print(function)
        sys.exit(1)
    arg_num = locals()[func].__code__.co_argcount
    if arg_num == 1:
        locals()[func](sys.argv[2])
    elif arg_num == 2:
        locals()[func](sys.argv[2], sys.argv[3])
    elif arg_num == 3:
        locals()[func](sys.argv[2], sys.argv[3], sys.argv[4])
    elif arg_num == 4:
        locals()[func](sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5])
    elif arg_num == 5:
        locals()[func](sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6])
    elif arg_num == 6:
        locals()[func](sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6], sys.argv[7])
    else:
        print("ERROR! arg number out of range")
