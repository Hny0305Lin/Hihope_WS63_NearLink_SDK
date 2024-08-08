#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# Description: Provides dma driver source \n
#
# History: \n
# 2023-01-16， Create file. \n
# ============================================================================
import os
import shutil
import re
import sys
root_dir = os.path.split(os.path.realpath(__file__))[0]
sys.path.append(os.path.join(root_dir, '..', 'config'))
sys.path.append(root_dir)
from enviroment import TargetEnvironment
from utils.build_utils import output_root

class statistics(object):
    def __init__(self, file):
        self.name = []
        self.text_size = []
        self.data_size = []
        self.bss_size = []
        self.group = [[0 for i in range(6)] for j in range(100)]
        with open(file, encoding="utf-8") as f:
            cluster = f.read()
            component_name = cluster[cluster.find("[")+1:cluster.find("]")]
            component_name = component_name.replace("\n", '')
            component_name = component_name.replace(" ", '')
            component_name = component_name.replace("'", '')
            self.name = component_name.split(",")
            for i in range(0, len(self.name)):
                self.text_size.append(0)
                self.data_size.append(0)
                self.bss_size.append(0)
            group_name = cluster[cluster.find("group_details = {")+20:cluster.find("}")]
            group_name = group_name.split("\n")
            for i in range(0, len(group_name)):
                name_g = group_name[i].replace("[", '')
                name_g = name_g.replace("]", '')
                name_g = name_g.replace(" ", '')
                name_g = name_g.replace("'", '')
                name_g = re.split(':|,', name_g)
                if (len(name_g) > 0):
                    self.group[i] = name_g

    def data_set(self, file_name, index):
        cmds = 'size ' + file_name
        string = os.popen(cmds).read()
        for line in string.splitlines():
            number = re.findall("\d+",line)
            if len(number) > 0:
                self.text_size[index] = int(self.text_size[index]) + int(number[0])
                self.data_size[index] = int(self.data_size[index]) + int(number[1])
                self.bss_size[index] = int(self.bss_size[index]) + int(number[2])

    def data_find(self, file_name):
        file_name = file_name.replace(".a", '')
        file_name = file_name.replace("lib", '')
        for i in range(0, len(self.group)):
            if len(self.group[i]) == 0:
                break
            for j in range(1, len(self.group[i])):
                if (file_name == self.group[i][j]):
                    file_name = self.group[i][0]
        for i in range(0, len(self.name)):
            if self.name[i] == file_name:
                return i
        return -1

    def creat(self, path):
        with open(path + '/codesize_statistic.html', "w") as f:
            f.write('<h1>Codesize Statistics</h1>')
            f.write('<table border="1" width = "40%" cellspacing="0" cellpadding="0" align="left">')
            f.write('<tr><th>drivers</th><th>text</th><th>data</th><th>bss</th><tr>\n')

            total_text_size = 0
            total_data_size = 0
            total_bss_size = 0

            for i in range(0, len(self.name)):

                total = self.text_size[i] + self.data_size[i] + self.bss_size[i]
                if not self.name[i] == 0 and total > 0:
                    total_text_size += self.text_size[i]
                    total_data_size += self.data_size[i]
                    total_bss_size += self.bss_size[i]

                    result = '<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n' % \
                        (str(self.name[i]), str(self.text_size[i]), str(self.data_size[i]), str(self.bss_size[i]))
                    f.write(result)
            result = '<tr><td>Total</td><td>%s</td><td>%s</td><td>%s</td></tr>\n' % \
                (str(total_text_size), str(total_data_size), str(total_bss_size))
            f.write(result)
            f.write('</table>')

def find_listdir(stat, path):
    if os.path.exists(path):
        file_list = os.listdir(path)
        for file in file_list:
            fulldir = os.path.join(path, file)
            if os.path.isdir(fulldir):
                find_listdir(stat, fulldir)
            if os.path.isfile(fulldir):
                bool = file.endswith(".a")
                if bool:
                    i = stat.data_find(file)
                    if i >= 0:
                        org_work_path = os.getcwd()
                        os.chdir(path)
                        stat.data_set(file, i)
                        os.chdir(org_work_path)

if __name__ == '__main__':
    root_dir = os.path.split(os.path.realpath(__file__))[0]
    root_dir = os.path.join(root_dir, '..', '..')
    os.chdir(root_dir)
    sys.path.append(os.path.join(root_dir, 'build', 'config'))
    sys.path.append(os.path.join(root_dir, 'build', 'script'))

    env = TargetEnvironment(sys.argv[1])
    chip = env.get('chip')
    core = env.get('core')
    stat = statistics("./build/config/target_config/%s/codesize_statistic_config.json" % (chip))
    path = os.path.join(output_root, chip, core, sys.argv[1])
    find_listdir(stat, path)
    stat.creat(path)

    print('codesize statistics done')
