#!/usr/bin/env python3
# coding=utf-8

#===============================================================================
# @brief    create database
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
#===============================================================================

import xml.dom.minidom
import sys
import re
import os
import shutil
import xml.etree.ElementTree as ET

def write_enum_struct(name, enums, file):
    file.write('typedef enum {\r\n')
    for enum in enums:
        file.write(''.join(['    ', enum.getAttribute('Name'), '=', enum.getAttribute('Value'), ',\r\n']))
    file.write(''.join(['} ', name, ';\r\n']))

def write_nomal_struct(name, fields, file):
    file.write('typedef struct {\r\n')
    for field in fields:
        if field.nodeType != xml.dom.Node.ELEMENT_NODE:
            continue
        if field.getAttribute('Enum') == '':
            if field.getAttribute('Length') == '1':
                file.write(''.join(['    ', field.getAttribute('Type'), ' ', field.getAttribute('FieldName'), ';\r\n']))
            else:
                file.write(''.join(['    ', field.getAttribute('Type'), ' ', field.getAttribute('FieldName'),  '[', field.getAttribute('Length') , '];\r\n']))
        else:
            if field.getAttribute('Length') == '1':
                file.write(''.join(['    ', field.getAttribute('Enum'), ' ', field.getAttribute('FieldName'), ';\r\n']))
            else:
                file.write(''.join(['    ', field.getAttribute('Enum'), ' ', field.getAttribute('FieldName'), '[', field.getAttribute('Length'), '];\r\n']))
    file.write(''.join(['} ', name, ';\r\n']))

def write_union(item, file):
    file.write('typedef union {\r\n')
    fields = item.getElementsByTagName("Field")
    for field in fields:
        file.write(''.join(['    ', field.getAttribute('Type'), ' ', field.getAttribute('FieldName'), ';\r\n']))
    file.write(''.join(['} ', item.getAttribute('Type'), ';\r\n']))
def find_union(fields, union_name_to_node):
    for field in fields:
        if field.getAttribute('FieldType') == 'union':
            union_name_to_node[field.getAttribute('Type')] = field
def write_msg_struct(name, fields, file):
    file.write('typedef struct {\r\n')
    for field in fields:
        if field.nodeType != xml.dom.Node.ELEMENT_NODE:
            continue
        if field.getAttribute('Enum') == '':
            if field.getAttribute('Length') == '1':
                file.write(''.join(['    ', field.getAttribute('Type'), ' ', field.getAttribute('FieldName'), ';\r\n']))
            else:
                file.write(''.join(['    ', field.getAttribute('Type'), ' ', field.getAttribute('FieldName'), '[', field.getAttribute('Length'), '];\r\n']))
        else:
            if field.getAttribute('Length') == '1':
                file.write(''.join(['    ', field.getAttribute('Enum'), ' ', field.getAttribute('FieldName'), ';\r\n']))
            else:
                file.write(''.join(['    ' , field.getAttribute('Enum'), ' ', field.getAttribute('FieldName'), '[', field.getAttribute('Length'), '];\r\n']))

    file.write(''.join(['} ', name, '_STRU;\r\n']))

def int_prim_xml_air_tree(tree):
    root = tree.getroot()
    layer_root_element = ET.Element('')
    for child_1 in root:
        if child_1.tag=='SUBSYSTEM':
            for child_2 in child_1:
                 if child_2.tag=='AIR_LOG':
                     layer_root_element = child_2
                     break
            break

    if layer_root_element.tag=='AIR_LOG':
        temp_attrib=layer_root_element.attrib
        temp_text=layer_root_element.text
        temp_tail = layer_root_element.tail
        #layer_root_element.clear()
        layer_root_element.attrib = temp_attrib
        layer_root_element.text = temp_text
        layer_root_element.tail = temp_tail
    return layer_root_element

def int_prim_xml_layer_tree(tree):
    root = tree.getroot()
    layer_root_element = ET.Element('')
    for child_1 in root:
        if child_1.tag=='SUBSYSTEM':
            for child_2 in child_1:
                 if child_2.tag=='LAYER_LOG':
                     layer_root_element = child_2
                     break
            break

    if layer_root_element.tag=='LAYER_LOG':
        temp_attrib=layer_root_element.attrib
        temp_text=layer_root_element.text
        temp_tail = layer_root_element.tail
        #layer_root_element.clear()
        layer_root_element.attrib = temp_attrib
        layer_root_element.text = temp_text
        layer_root_element.tail = temp_tail
    return layer_root_element

def int_prim_xml_msg_tree(tree):
    root = tree.getroot()
    layer_root_element = ET.Element('')
    for child_1 in root:
        if child_1.tag=='SUBSYSTEM':
            for child_2 in child_1:
                 if child_2.tag=='MSG_LOG':
                     layer_root_element = child_2
                     break
            break

    if layer_root_element.tag=='MSG_LOG':
        temp_attrib=layer_root_element.attrib
        temp_text=layer_root_element.text
        temp_tail = layer_root_element.tail
        #layer_root_element.clear()
        layer_root_element.attrib = temp_attrib
        layer_root_element.text = temp_text
        layer_root_element.tail = temp_tail
    return layer_root_element

def add_element_to_xml(root_element):
    global num_to_id
    sorted_list=sorted(num_to_id.items(),key=lambda e:e[1])

    for key,value in sorted_list:
        #print('key2=',key, 'value=',value)
        new_element=ET.Element('')
        new_element.tag='MSG'
        new_element.attrib['NAME']=key
        new_element.attrib['ID']=value
        new_element.attrib['DESCRIPTION']=''
        new_element.attrib['STRUCTURE']=''.join([key, '_STRU'])
        new_element.tail='\n\t\t\t'
        root_element.append(new_element)
def write_prim_xml_tree(tree, dest):
    dst_xml_dir = os.path.dirname(dest)
    if not os.path.exists(dst_xml_dir):
        os.makedirs(dst_xml_dir)
#    print(g_params['PRIM_XML_DST_XML_FILE'].strip())
    tree.write(dest,encoding="UTF-8",xml_declaration = '<?xml version="1.0" encoding="utf-8" ?>',method="xml")

try:
    msg_xml, mss_prim_db, destination = sys.argv[1:]
except ValueError:
    raise SystemExit("Usage: %s msg_xml mss_prim_db destination" % sys.argv[0])

msg_struct_def = os.path.join(destination, 'cco', 'system', 'diag', 'msg_struct_def.txt')
extern_datatype_def = os.path.join(destination, 'cco','system', 'diag', 'extern_datatype_def.txt')
mss_prim_db_new = os.path.join(destination, 'cco', 'system', 'hdbcfg', 'mss_prim_db.xml')

src_database_dir = os.path.join(os.path.dirname(sys.argv[0]), "database")

shutil.rmtree(destination, ignore_errors=True, onerror=None)
shutil.copytree(src_database_dir, destination)

dom_tree = xml.dom.minidom.parse(msg_xml)
collection = dom_tree.documentElement
enum_fields = collection.getElementsByTagName("EnumsEntry")
struct_fields = collection.getElementsByTagName("Structure")
msg_fields = collection.getElementsByTagName("Message")
all_fields = collection.getElementsByTagName("Field")

num_to_id={}
enum_to_size = {}
type_to_size = {}
union_name_to_node = {}
lp_struct_to_size = {}

for item in msg_fields:
    message_id = item.getAttribute('MessageID')
    name = item.getAttribute('Name')
    num_to_id[name] = message_id

with open(msg_struct_def,'w') as f:
    f.write('#include \"base_datatype_def.txt\"\r\n')
    f.write('#include \"extern_datatype_def.txt\"\r\n')

    for item in enum_fields:
        name = item.getElementsByTagName("Name")[0].childNodes[0].data
        enums = item.getElementsByTagName('Enum')
        write_enum_struct(name, enums, f)

    for item in struct_fields:
        name = item.getAttribute('Type')
        fields = item.getElementsByTagName('Field')
        write_nomal_struct(name, item.childNodes, f)

    find_union(all_fields, union_name_to_node)
    for key, value in union_name_to_node.items():
        write_union(value, f)
    for item in msg_fields:
        name = item.getAttribute('Name')
        fields = item.getElementsByTagName('Field')
        write_msg_struct(name, item.childNodes, f)
    f.close()

with open(extern_datatype_def,'w') as f:
    for field in all_fields:
        enum = field.getAttribute('Enum')
        if enum == '':
            continue
        size = field.getAttribute('Size')
        enum_to_size[enum] = size
    for item in enum_to_size.items():
        if item[1] == '1':
            line = ''.join([item[0], ' 8 0\r\n'])
        elif item[1] == '2':
            line = ''.join([item[0], ' 16 0\r\n'])
        elif item[1] == '4':
            line = ''.join([item[0], ' 32 0\r\n'])
        else:
            continue
        f.write(line)
    for field in all_fields:
        lp_struct = field.getAttribute('Type')
        if 'LP_' not in lp_struct:
            continue
        size = field.getAttribute('Size')
        lp_struct_to_size[lp_struct] = size
    for item in lp_struct_to_size.items():
        if item[1] == '1':
            line = ''.join([item[0], ' 8 0\r\n'])
        elif item[1] == '2':
            line = ''.join([item[0], ' 16 0\r\n'])
        elif item[1] == '4':
            line = ''.join([item[0], ' 32 0\r\n'])
        else:
            continue
        f.write(line)
    f.close()


tree = ET.parse(mss_prim_db)
add_element_to_xml(int_prim_xml_msg_tree(tree))
write_prim_xml_tree(tree, mss_prim_db_new)