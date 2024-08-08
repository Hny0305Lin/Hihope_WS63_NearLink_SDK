import json
from xml.dom.minidom import Document
import re
import os


NV_JSON_FILE_DIR = ["./output/ws63/acore/nv_bin/temp/cfg/acore_nv.json"]
NV_TXT_FILE_DIR = ["./output/ws63/acore/nv_bin/temp/acore.etypes"]

NV_XML_PATH = "./output/ws63/database/cco/system/hdbcfg/mss_nvi_db.xml"
TXT_PATH = "./output/ws63/database/cco/system/nv/nv_struct_def.txt"


def nv_json_handle():
    file_dic = {}
    for file_path in NV_JSON_FILE_DIR:
        if not os.path.isfile(file_path):
            print("[!][nv handle]warning: ", file_path, " not found!")
            break
        nv_dic = {}
        with open(file_path, "r") as f:
            nv_api = json.load(f)
        for i in nv_api["common"].keys():
            if nv_api["common"][i] != 0:
                nv_dic[i] = nv_api["common"][i]
        file_dic[file_path] = nv_dic
    return file_dic


def dic_to_nv(nv_dic: dict):
    if not nv_dic:
        print("[!][nv handle]warning: dic is Null.")
        return
    nv_data = Document()
    root_node = nv_data.createElement("DebugKits")
    nv_data.appendChild(root_node)
    group_num = 0
    for file_name in nv_dic.keys():
        print("NV file reading: ", file_name)
        group_num += 1
        group_id = hex(group_num)

        file_cont = nv_dic[file_name]

        # first node, GROUP attribute
        first_son_node = nv_data.createElement("GROUP")
        root_node.appendChild(first_son_node)
        first_son_node.setAttribute("ID", group_id)
        first_son_node.setAttribute("NAME", "NV")
        first_son_node.setAttribute("PARAM_DEF_FILE", os.path.relpath(TXT_PATH, os.path.dirname(NV_XML_PATH)))
        for nv_name in file_cont.keys():
            son_node = nv_data.createElement("NV")
            first_son_node.appendChild(son_node)
            son_node.setAttribute("CATEGORY", "FTM")
            son_node.setAttribute("DESCRIPTION", "FTM")
            son_node.setAttribute("DEV", "CCO")
            son_node.setAttribute("ID", file_cont[nv_name]["key_id"])
            son_node.setAttribute("NAME", nv_name)
            son_node.setAttribute("PARAM_NAME", file_cont[nv_name]["structure_type"])
    print("nv xml writing in: ", NV_XML_PATH)
    try:
        f = open(NV_XML_PATH, "w", encoding="utf-8")
        f.write(nv_data.toprettyxml())
        f.close()
    except FileNotFoundError:
        print("[!][nv handle]warning: ", NV_XML_PATH, " not found!")
        pass
    except:
        print("[!][nv handle]warning: NV error!")
        pass

def struct_to_txt():
    struct_list = []
    for file_path in NV_TXT_FILE_DIR:
        if not os.path.isfile(file_path):
            print("[!][nv handle]warning: ", file_path, " not found!")
            break
        print("struct file reading: ", file_path)
        with open(file_path, "r", encoding="utf-8") as f:
            data = f.readlines()
            ret = re.findall("(typedef.*?;)", "".join(data))
            ret2 = re.findall("(typedef struct {.*?}.*?;)", "".join(data), re.S)
        struct_list.extend(ret)
        struct_list.extend(ret2)
    print("struct writing in:", TXT_PATH)
    try:
        with open(TXT_PATH, "w", encoding="utf-8") as t_file:
            t_file.write('''#include "base_datatype_def.txt"\n''')
            for i in struct_list:
                t_file.write(i + "\n")
    except FileNotFoundError:
        print("[!][nv handle]warning: ", TXT_PATH, " not found!")
        pass
    except:
        print("[!][nv handle]warning: NV error!")
        pass
    return


if __name__ == '__main__':
    work_dir = os.getcwd()
    print("current work direction: ", work_dir)
    dic = nv_json_handle()
    if dic:
        dic_to_nv(dic)
    struct_to_txt()
    os.chdir(work_dir)
