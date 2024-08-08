#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Mem Script file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# ============================================================================
import os
import sys
import errno
import re

# Security Core - Test Suite
# --------------------------
#
#     Section             Start Addr   Size     Length
#     -------             ----------   ------   -------
#
#     Flash               0x10000000   0x31f80  0x13360 = 78688
#         startup         0x10000000               0xa0
#         .flash_version  0x100000c0               0x28
#         .text           0x10000100            0x118b4
#         .ARM.exidx      0x100119b4              0xf08
#         .ARM.extab      0x100128bc              0x924
#         .ram_text       0x100131e0               0x88
#         .data           0x10013268               0xf8
#
#     SRAM                0x38000000    0x2800   0x27A0 = 10144
#         .stacks         0x38000000              0x400
#         .ramtext        0x38000400              0x88 load address 0x100131e0
#         .data           0x38000488              0xf8 load address 0x10013268
#         .bss            0x38000580             0x1e34
#         .preserve       0x380023b4               0x9c
#         .heap           0x38002450                0x0
#         .ipc_mailbox    0x38002700               0xa0

flag_names = [
    'CONTENTS',
    'ALLOC',
    'LOAD',
    'READONLY',
    'CODE',
    'DATA',
    'DEBUGGING']


class Section:
    def __init__(self):
        self.name = ''
        self.size = 0
        self.vma = 0
        self.lma = 0
        self.flags = ''
        self.exclude = False

    def get_name(self):
        return self.name

    def get_size(self):
        return self.size

    def is_loaded(self):
        if(not self.exclude):
            if(self.lma != self.vma):
                if('LOAD' in self.flags):
                    return True
        return False

    def get_load_section(self):
        load_section = Section()
        load_section.name = self.name
        load_section.size = self.size
        load_section.vma = self.lma
        load_section.lma = self.lma
        load_section.flags = self.flags
        return load_section

    def display(self):
        size_str = "%d" % self.size
        if(self.vma == self.lma):
            print(
                "    %-16s  0x%08X  %10s  %8s                  " %
                (self.name, self.vma, "", size_str))
        else:
            print("    %-16s  0x%08X  %10s  %8s load address 0x%X" %
                  (self.name, self.vma, "", size_str, self.lma))

    def display_raw(self):
        print("<%-16s  0x%08X  0x%08X  0x%08X  %s>" %
              (self.name, self.vma, self.lma, self.size, self.flags))

    def get_all_size(self, size_list):
        temp = "{0}:{1}:{2}".format(self.name, self.vma, self.size)
        size_list.append(temp)


class Region:
    def __init__(self, name, origin, length):
        self.name = name
        self.origin = origin
        self.length = length
        self.sections = []

    def get_name(self):
        return self.name

    # Get used_size or region length, if it does not contain any sections
    def get_size(self):
        used_size = self.used_size()
        if(used_size > 0):
            return used_size
        return self.length

    def add_section(self, section):
        if(not section.exclude):
            if((section.vma >= self.origin) and
               (section.vma < (self.origin + self.length))):
                self.sections.append(section)
                return True
        return False

    def find_section(self, name):
        if name in self.name:
            return self

        for section in self.sections:
            if name in section.name:
                return Region(section.name, section.vma, section.size)
        return None

    # Calculate region size used based on contained sections
    def used_size(self):
        # Begin with start at end of memory region...
        start = self.origin + self.length
        end = self.origin                   # and end at beginning
        for section in self.sections:
            if(section.vma < start):
                start = section.vma
            if((section.vma + section.size) > end):
                end = (section.vma + section.size)
        if(start < end):
            return end - start  # Beginning of first section to end of last
        else:
            return 0            # No sections cotained by this memory region

    def display(self):
        used_size = self.used_size()
        if(used_size > 0):
            used_size_str = "0x%X" % used_size
            length_str = "0x%X" % self.length
            print("  ")
            print("  %-18s  0x%08X  %10s  %8s = %d" %
                  (self.name, self.origin, length_str, used_size_str,
                   used_size))
            for section in self.sections:
                section.display()

    def get_section_size(self, section_name):
        size_list = []
        for section in self.sections:
            section.get_all_size(size_list)
        for val in size_list:
            if section_name in val:
                return int(val.split(":")[2])
        return 0

    def get_section_used_size(self, section_name):
        size_list = []
        for section in self.sections:
            section.get_all_size(size_list)
        for i, x in enumerate(size_list):
            if section_name in x and i != (len(size_list) - 1):
                return x.split(":")[-1]
        return 0


class Memory:
    def __init__(self):
        self.regions = []
        self.bth_list = [0] * 21
        self.btc_list = [0] * 21
        self.plt_list = [0] * 21
        self.all_size = 0
        self.filename = ""
        self.chipname = ""
        self.all_code_rodata_list = 0
        self.all_data_list = 0
        self.all_bss_list = 0
        self.all_memory_size = 985088

    def add_region(self, region):
        self.regions.append(region)

    def add_section(self, section):
        for region in self.regions:
            if(region.add_section(section)):
                return True
        return False

    def find_section(self, name):
        for region in self.regions:
            section = region.find_section(name)
            if section is not None:
                return section
        return None

    def display(self):
        print("  %-18s  %10s  %10s  %8s" %
              ('Section', 'Start Addr', 'Size', 'Used'))
        print("  %-18s  %10s  %10s  %8s" %
              ('------------------', '----------', '----------', '--------'))
        for region in self.regions:
            region.display()

    def summary_display(self, lst_file_name):
        print("  ")
        print("  %-18s  %10s  %10s  %8s  %8s" %
              ('Section', 'Total', 'BTH', 'BTC', 'PLT'))
        print("  %-18s  %10s  %10s  %8s  %8s" %
              ('------------------', '----------', '----------', '--------',
               '--------'))
        print("  ")
        self.memory_display(lst_file_name)
        print("  %-18s  %10s  %10s  %8s" %
              ('Section', 'Start Addr', 'Size', 'Used'))
        print("  %-18s  %10s  %10s  %8s" %
              ('------------------', '----------', '----------', '--------'))
        for region in self.regions:
            region.display()

    def get_region_section_size(self, region_name, section_name):
        for region in self.regions:
            if region_name in region.name:
                return region.get_section_size(section_name)
        return 0

    def get_region_section_used_size(self, region_name, section_name):
        for region in self.regions:
            if region_name in region.name:
                return region.get_section_used_size(section_name)

    def set_init_param(self, name, index, value):
        if name in "btc_list":
            self.btc_list[index] = value
        elif name in "plt_list":
            self.plt_list[index] = value
        elif name in "bth_list":
            self.bth_list[index] = value


# Used to aid parsing lines of text from a Lst file
class SectionLine:
    def __init__(self, line):
        self.line = line

    def update(self, line):
        self.line = line

    def append(self, line):
        self.line += line

    def length(self):
        return len(self.line)

    def find_space(self, pos):
        length = len(self.line)
        while((pos < length) and not self.line[pos].isspace()):
            pos = pos + 1
        if(pos < length):
            return pos
        return length

    def find_nonspace(self, pos):
        length = len(self.line)
        while((pos < length) and self.line[pos].isspace()):
            pos = pos + 1
        if(pos < length):
            return pos
        return length

    def get_word(self, index):
        start = self.find_nonspace(0)
        end = self.find_space(start)
        while(index > 0):
            index = index - 1
            start = self.find_nonspace(end)
            end = self.find_space(start)
        return self.line[start:end]

    def contains(self, line):
        if(line in self.line):
            return True
        return False


class LstFile:
    def __init__(self, fp_filename, filename, lds_file, chip_name, chip_version):
        self.file = fp_filename
        self.line = SectionLine("")
        self.chip_name = chip_name
        self.memory = self.create_memory_map(lds_file)
        self.memory.chipname = chip_name
        self.filename = filename
        self.chip_version = chip_version

    def __del__(self):
        self.file.close()

    def valid_section_line(self):
        if(self.line.length() > 0):
            if(self.line.get_word(0).isdigit()):
                return True
        return False

    def get_next_section(self):
        if(self.read_section_line()):
            if(self.valid_section_line()):
                section = self.parse_section_line()
                if(section is not None):
                    # Move this knowledge into Section?
                    # Not a physical section
                    if(('READONLY' in section.flags) and
                       ('CODE' not in section.flags) and
                       ('DATA' not in section.flags)):
                        section.exclude = True
                    # Debugging sections do not appear in binary
                    if('DEBUGGING' in section.flags):
                        section.exclude = True
                    # Section might have a phantom load address
                    if((section.lma != section.vma) and
                       ('LOAD' not in section.flags)):
                        section.lma = section.vma
                    return section
        return None

    def get_first_section(self):
        self.file.seek(0)
        if(self.find_file_sections()):
            return self.get_next_section()
        return None

    def find_file_sections(self):
        file_line = self.file.readline()
        while(len(file_line) > 0):
            if(file_line.find('Sections:') == 0):
                return True
            file_line = self.file.readline()
        return False

    def read_section_line(self):
        file_line = SectionLine(self.file.readline())
        while(file_line.length() > 0):
            if(file_line.contains('SYMBOL TABLE:')):
                return False

            # Use split instead?  # Place in a function
            if(file_line.get_word(0).isdigit()):
                file_line.append(self.file.readline())
                self.line = file_line
                return True
            else:
                file_line.update(self.file.readline())
        return False

    def parse_section_line(self):
        # Use split instead?  # Place in a function
        if(self.line.get_word(0).isdigit()):
            section = Section()
            section.name = self.line.get_word(1)
            section.size = int(self.line.get_word(2), 16)
            section.vma = int(self.line.get_word(3), 16)
            section.lma = int(self.line.get_word(4), 16)
            section.flags = ''
            for flag_name in flag_names:
                if(self.line.contains(flag_name)):
                    section.flags = "%s_%s_" % (section.flags, flag_name)
            return section
        return None

    def create_memory_map(self, lds_file):
        memory = Memory()
        with open(lds_file, 'r') as f:
            text = f.read()
            mem_map = re.findall(r"([A-Za-z0-9_]*?) *: *ORIGIN *= *([\(\)0-9A-Fa-f/\+\-x* ]*), *LENGTH *= *([\(\)0-9A-Fa-f/\+\-x* ]*)", text)
            for item in mem_map:
                memory.add_region(Region(item[0], int(eval(item[1])), int(eval(item[2]))))

        memory.add_region(Region("Unexpected", 0x00000000, 0xFFFFFFFF))

        return memory

    # Should this create and return a ProcessedLstFile, which contains the two
    # function below?
    def process(self):
        section = self.get_first_section()
        while(section is not None):
            if(self.memory.add_section(section)):
                if(section.is_loaded()):
                    load_section = section.get_load_section()
                    self.memory.add_section(load_section)
            section = self.get_next_section()

    def find_section(self, name):
        return self.memory.find_section(name)

    def display(self):
        print("\n  Memory Usage Summary...")
        self.memory.filename = self.filename
        self.memory.display()
        print("\n  ====================================================\n")



display_depth = {
    '.': 99,
    'platform': 3,
    'drivers': 6,
    'hal': 6,}
min_display_level = 1


class SectionTree:
    last_display_level = 0
    last_section_size = 0

    def __init__(self, name, level):
        self.root = name
        self.level = level
        self.size = 0
        self.children = []

    def get_name(self):
        return self.root

    def get_size(self):
        return self.size

    def get_children(self):
        return self.children

    def find(self, name):
        if(self.root == name):
            return self
        for child in self.children:
            found_tree = child.find(name)
            if(found_tree is not None):
                return found_tree
        return None

    def add_content(self, size, content):
        assert len(content) > 0

        if(size > 0):
            self.size += size

            if(len(content) > 1):
                content_added = False
                for child in self.children:
                    if(child.root == content[0]):
                        child.add_content(size, content[1:])
                        content_added = True
                if not content_added:
                    new_child = SectionTree(content[0], self.level + 1)
                    self.children.append(new_child)
                    new_child.add_content(size, content[1:])

    def display(self, depth):
        if(self.root in display_depth):
            depth = display_depth[self.root]
        if(depth >= 0):
            if(self.level < SectionTree.last_display_level):
                print()
                SectionTree.last_section_size = 0
            SectionTree.last_display_level = self.level

            if(self.level == min_display_level):

                print("\n  Section                            Total     Size")
                print("\n  --------------------------------  --------  -----")
                print("\n")

            if(self.level >= min_display_level):
                if(self.root.endswith(".c") or self.root.endswith(".s")):
                    name = self.root[:len(self.root) - 2]
                else:
                    name = self.root

                # A top level section name
                if(self.level == min_display_level):
                    SectionTree.last_section_size = self.size
                    print("  %-32s  %8d" %
                          (((self.level - min_display_level) * '  ') + name,
                           self.size))
                    print()
                # Last entry being dealt with so show its size
                elif((depth == 0) or (len(self.children) == 0)):
                    SectionTree.last_section_size = 0
                    print("  %-42s  %8d" %
                          (((self.level - min_display_level) * '  ') + name,
                           self.size))
                # Has children and size not the same as last size displayed
                elif(len(self.children) > 0 and
                     (SectionTree.last_section_size != self.size)):
                    SectionTree.last_section_size = self.size
                    print("  %-32s  %8d" %
                          (((self.level - min_display_level) * '  ') + name,
                           self.size))

                else:
                    print("  %-42s" %
                          (((self.level - min_display_level) * '  ') + name))

            if(depth > 0):
                for child in self.children:
                    child.display(depth - 1)

    def get_section_child(
            self,
            depth,
            section_name,
            tree_data_list,
            start_flag,
            end_flag):

        if(self.root in display_depth):
            depth = display_depth[self.root]
        if(depth >= 0):
            if(self.level < SectionTree.last_display_level):
                SectionTree.last_section_size = 0
            SectionTree.last_display_level = self.level

            if(self.level == min_display_level):
                if start_flag[-1] == 1:
                    end_flag[-1] = 1

            if start_flag[-1] and end_flag[-1]:
                return 1

            if(self.level >= min_display_level):
                if(self.root.endswith(".c") or self.root.endswith(".s")):
                    name = self.root[:len(self.root) - 2]
                else:
                    name = self.root

                if(self.level == min_display_level):
                    SectionTree.last_section_size = self.size

                    if name == section_name:
                        tree_data_list.append(
                            ["{0}:{1}".format(name, self.size)])
                        start_flag[-1] = 1

                # Last entry being dealt with so show its size
                elif((depth == 0) or (len(self.children) == 0)):
                    SectionTree.last_section_size = 0

                    if start_flag[-1] == 1:
                        tree_data_list[-1].append(
                            "{0}:{1}".format(name, self.size))
                # Has children and size not the same as last size displayed
                elif(len(self.children) > 0 and
                     (SectionTree.last_section_size != self.size)):
                    SectionTree.last_section_size = self.size

                    if start_flag[-1] == 1:
                        tree_data_list[-1].append(
                            "{0}:{1}".format(name, self.size))

                else:
                    if start_flag[-1] == 1:
                        tree_data_list[-1].append("{0}:{1}".format(name, ""))

            if(depth > 0):
                for child in self.children:
                    if(child.get_section_child(depth - 1, section_name,
                       tree_data_list, start_flag, end_flag)):
                        return 1


class DuFile:
    def __init__(self, fp_filename):
        self.file = fp_filename
        # Construct empty tree with '.' as root, since all pathnames in .du
        # file
        # begin in the same way
        self.section_tree = SectionTree('.', 0)

    def __del__(self):
        self.file.close()

    def get_next_section_content(self):
        # e.g. "216
        # ./.text/drivers/non-os/ipc/shared/ipc.c/ipc_send_message"
        file_line = SectionLine(self.file.readline())
        if(file_line.length() > 0):
            # Obtains size e.g. "216"
            size = file_line.get_word(0)
            # Last line of .du file begins with non-numeric characters
            if(size.isdigit()):
                size = int(size)
                # Generates a list of words, separated by '/' e.g.
                # ".",".text","drivers","non-os","ipc" etc
                content = file_line.get_word(1).split(os.sep)
                return (size, content)
        return (0, None)

    def get_first_section_content(self):
        # Point to beginning of file
        self.file.seek(0)
        return self.get_next_section_content()

    # Should this create and return a ProcessedDuFile, which will contain the
    # two function below?
    def process(self):
        # Scan entire file contents
        (size, content) = self.get_first_section_content()
        while(content is not None):
            self.section_tree.add_content(size, content[1:])
            (size, content) = self.get_next_section_content()

    def find_section(self, name):
        return self.section_tree.find(name)

    def display(self):
        print("\n  Memory Usage Details...")
        # Use a 0 depth here as it will be overridden by entries in
        # display_depth
        self.section_tree.display(0)
        print("\n  ====================================================\n")

    def get_tree_size(self, section_name, path_name):
        tree_data_list = []
        flag_list = []
        start_flag = [0]
        end_flag = [0]
        self.section_tree.get_section_child(
            0, section_name, tree_data_list, start_flag, end_flag)

        for i in range(len(tree_data_list[-1])):
            if path_name[0] == tree_data_list[-1][i].split(":")[0]:
                flag_list.append(1)
                for path_index in range(len(path_name[1:])):
                    if path_name[1:][path_index] == tree_data_list[-1][
                        i + 1 + path_index].split(":")[0]:
                        flag_list.append(1)
                    else:
                        flag_list.append(0)
                if len(flag_list) == sum(flag_list):
                    size = tree_data_list[-1][i].split(":")[1]
                    if size == '':
                        size = 0
                    return int(size)
                else:
                    flag_list = []


def display_mismatches(lst_file, du_file):
    section_names = [
        "startup",
        ".flash_version",
        ".text",
        ".ramtext",
        ".data",
        ".bss",
        ".preserve",
        ".sys_status",
        ".ipc_mailbox",
        "btc_ramtext",
        "bth_ramtext",
        "plt_ramtext",
        "bth_share_ramtext",
        "btc_data",
        "bth_data",
        "plt_data",
        "btc_bss",
        "bth_bss",
        "plt_bss"]

    print("  Mismatched section sizes...\n")
    for section_name in section_names:
        lst_section = lst_file.find_section(section_name)
        du_section = du_file.find_section(section_name)
        if lst_section is not None and du_section is not None:
            lst_size = lst_section.get_size()
            du_size = du_section.get_size()
            if(lst_size != du_size):
                print(
                    "  %12s  .lst file = %d (0x%X)" %
                    (section_name, lst_size, lst_size))
                print("  %12s   .du file = %d (0x%X)" %
                      ("", du_size, du_size))
                print()
    print("  ====================================================\n")


def main(lst_file_name, du_file_name, lds_file, chip_name, chip_version=None):
    with open(lst_file_name, "r") as fp_lst_file_name, open(
            du_file_name, "r") as fp_du_file_name:
        lst_file = LstFile(
            fp_lst_file_name,
            lst_file_name,
            lds_file,
            chip_name,
            chip_version)
        lst_file.process()
        du_file = DuFile(fp_du_file_name)
        du_file.process()

        lst_file.display()

        du_file.display()
        display_mismatches(lst_file, du_file)


if __name__ == "__main__":
    if(len(sys.argv) == 5):
        main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        print(
            "Usage: %s <lstfile> <dufile> <chip>" %
            os.path.basename(
                sys.argv[0]))
