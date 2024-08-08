#!/usr/bin/env python3
# coding=utf-8

#===============================================================================
# @brief    Merge per-core messages xml files.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
#===============================================================================

# Note: The merged messages file must be written in a separate process to
# SCons to avoid file-in-use errors on Windows where the SCons python
# process fails to call CloseFile even though close() is called in python.

import os
import sys
from  xml.etree import ElementTree

merged_messages_xml_filename = sys.argv[1]
messages_xml_list = sorted(sys.argv[2:])

def combine_messages_xml(files, output_file):
    ElementTree.register_namespace("","http://tempuri.org/xmlDefinition.xsd" )
    tree = ElementTree.parse(files[0])
    root = tree.getroot()
    for filename in files[1:]:
        data = ElementTree.parse(filename).getroot()
        core_data = data.findall("{http://tempuri.org/xmlDefinition.xsd}Core")
        root.extend(core_data)

    with open(output_file, 'wb') as output_file_handle:
        tree.write(output_file_handle, encoding='utf-8', method='xml', xml_declaration=True)

combine_messages_xml(messages_xml_list, merged_messages_xml_filename)
