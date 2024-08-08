#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# Description: API format check \n
#
# ============================================================================

import re
import os
import sys
import logging

MODULE_LIST = ["driver", "bts", "wifi", "utils"]
ERROR_LIST = ["td_void", "td_u8", "td_u32", " td_", "(td_"]
logging.basicConfig(
    filename="check_api_result.txt",
    level=logging.NOTSET,
    format='[%(asctime)s] [%(levelname)s] - %(message)s')


def file_path_list(folder_path):
    logging.debug("Start to read file name list...")
    file_paths = []  # 存储目录下的所有文件名，含路径
    for root, _, files in os.walk(folder_path):
        for file in files:
            file_paths.append(os.path.join(root, file))
    return file_paths


def api_read(file_path, api_dic):
    txt = ""
    if os.path.splitext(file_path)[-1] != ".h":
        return api_dic
    try:
        with open(file_path, 'r') as f:
            txt = f.read()
        # print(txt)
    except OSError as e:
        logging.error("OSError file name: %s", file_path)
        # raise OSError('invalid encoding byte because {}.'.format(e))
    except UnicodeDecodeError as f:
        logging.warning("UnicodeDecodeError file name: %s", file_path)
        with open(file_path, 'r') as fin:
            try:
                for line in fin.readlines():
                    txt += line.encode('utf-8', 'ignore').decode('utf-8')
            except UnicodeDecodeError as f:
                logging.error("line:UnicodeDecodeError file name: %s", file_path)
                pass

    res = re.findall(r"/\*\*.+? \* @brief(.*?)\n.*?\*/.*?\n(.*?);", txt, re.S | re.M)

    for i in res:
        i = list(i)
        if "(" in i[1] and "{" not in i[1] and "#define" not in i[1]:
            api_dic[i[1]] = file_path
    return api_dic


def function_handle(name):
    class_name = name.split(" ", 1)[0]
    function_name = name.split(" ", 1)[1]
    return class_name, function_name


def count_duplicate_module(count_dict, module, sub_module):
    if "total" in count_dict:
        count_dict["total"] += 1
    else:
        count_dict["total"] = 1

    if module in count_dict:
        count_dict[module]["total"] += 1
        if sub_module in count_dict[module]:
            count_dict[module][sub_module] += 1
        else:
            count_dict[module][sub_module] = 1
    else:
        count_dict[module] = {}
        count_dict[module][sub_module] = 1
        count_dict[module]["total"] = 1
    return count_dict


g_tabs_indent_bar = 10 * '-'
g_tabs_indent_space = 10 * ' '


def report_module_api(count_dict, detail):
    print("CFBB total APIs:", str(count_dict["total"]))
    for module in count_dict:
        if module in "total":
            continue
        print(g_tabs_indent_bar, module, " total APIs:", count_dict[module]["total"])
        if not detail:
            continue
        module_sum = 0
        for sub_module in count_dict[module]:
            if sub_module in "total":
                continue
            module_sum += count_dict[module][sub_module]
            print(g_tabs_indent_space, g_tabs_indent_bar, sub_module, " APIs:\t", count_dict[module][sub_module])

        if count_dict[module]["total"] != module_sum:
            raise ImportError("module_sum APIs is not correct")


def save_to_excel(dic, save2file):
    if save2file:
        try:
            import openpyxl
        except ImportError:
            raise ImportError("please install openpyxl")
        wb = openpyxl.Workbook()
        wb.create_sheet("log", 0)
        ws = wb["log"]
        ws.append(["API name", "module name", "file path"])

    count_dict = {}
    for i in dic:
        class_name, func_name = function_handle(i)
        module_name = "unknown"
        for m in MODULE_LIST:
            if m in dic[i]:
                module_name = m
                break
        d = ("%s %s" % (class_name, func_name)), module_name, dic[i]
        sub_module = os.path.basename(dic[i])
        count_dict = count_duplicate_module(count_dict, module_name, sub_module)
        if class_name != "typedef":
            if func_name.startswith("uapi_") is not True and module_name == "driver":
                logging.error("API format is incorrect(don't start with uapi_): %s", func_name)
        if save2file:
            ws.append(d)
    report_module_api(count_dict, True)
    if save2file:
        filename = "api_list.xlsx"
        wb.save(filename)
    logging.info("Saved successfully.")


def save_to_txt(dic):
    with open("error_api.txt", "w+") as f:
        for i in dic.keys():
            f.write(dic[i] + ":" + i + "\n")


def print_error_type(dic):
    error_dic = {}
    for i in dic.keys():
        for n in ERROR_LIST:
            if n in i:
                error_dic[i] = dic[i]
    for i in error_dic.keys():
        print("error type: " + i + ":" + error_dic[i])
    return error_dic


def main():
    if len(sys.argv) == 2:
        curr_path = sys.argv[1]
    else:
        curr_path = os.getcwd()
    print("curr_path:", curr_path)
    if os.path.isdir(curr_path):
        path_list = file_path_list(curr_path)
        result = {}
        for k in path_list:
            result = api_read(k, result)
        logging.info("API count: %s", len(result))
        error_dic = print_error_type(result)
        print(error_dic)
        save_to_txt(error_dic)
        if error_dic:
            return -1
        else:
            return 0
    else:
        logging.error("error path!")
        return -1


if __name__ == '__main__':

    ret = main()
    if ret == -1:
        print(-1)
    else:
        print(0)
