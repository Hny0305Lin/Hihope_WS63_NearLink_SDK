#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Script file
# Copyright HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# ============================================================================

'''
Read in an elf file and generate a 'du' output for it.
'''
from __future__ import print_function
from collections import OrderedDict
import subprocess
import re
import os
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Generate du output from an elf file')
    parser.add_argument('root_dir', metavar="Root_dir", help="Code root_dir")
    parser.add_argument('elf', metavar="ELF", help="ELF file to parse")
    parser.add_argument('nm', metavar="nm", help="gcc nm path")
    args = parser.parse_args()

    # Get nm and elf filenames from command line args
    root_dir = args.root_dir
    nm = args.nm
    elf = args.elf

    # build command line and run nm
    cmd = nm + ' -l -A -t x -f sysv -S ' + elf
    p = subprocess.Popen(
        cmd.split(),
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE)
    stdout, stderr = p.communicate()
    return_code = p.wait()
    if (return_code != 0):
        raise Exception(nm + " returned: " + str(return_code))

    # parse the output
    line_count = 0
    line_match = 0
    symbol_type = OrderedDict()

    # Convert stdout from bytes to a string
    stdoutstr = stdout.decode("utf-8")

    # Loop over each line in the string
    for line in iter(stdoutstr.splitlines()):
        line_count = line_count + 1

        sl = line.split("|")
        if (len(sl) == 7):
            elfsym = re.match(r'^(.*):([^\s]*)', sl[0])
            if elfsym:
                elf = elfsym.group(1)
                symbol = elfsym.group(2)

                if (len(sl[1].strip())):
                    location = int(sl[1], 16)
                else:
                    location = 0

                c = sl[2].strip()
                t = sl[3].strip()
                if (len(sl[4].strip())):
                    size = int(sl[4], 16)
                else:
                    size = 0
                u2 = sl[5]
                segmentfileline = re.match(r'([^\t]*)\t(.*):([^\|]*)', sl[6])
                if segmentfileline:
                    try:
                        segment = segmentfileline.group(1)
                        src_file = segmentfileline.group(2)
                        src_file_line = int(segmentfileline.group(3))
                        if segment not in symbol_type:
                            symbol_type[segment] = list()
                        symbol_type[segment].append((src_file, symbol, size))
                        line_match = line_match + 1
                    except Exception as e:
                        print("warning:", e.with_traceback())


    # Find common filename prefix string from all input files
    init_prefix = True
    file_count = 0
    # Loop through displaying each line and totals
    segment_total = 0
    # Loop over each segment
    for segment in symbol_type:
        filename_total = 0
        curr_filename = ''

        # Loop over each symbol in segment
        for symbol in symbol_type[segment]:
            filename = symbol[0]
            sym = symbol[1]
            size = symbol[2]

            # strip prefix from start of filename
            if (root_dir):
                filename = filename.replace(root_dir, '', 1)
            # convert \\ to / in filename
            filename = filename.replace('\\', '/')

            if filename != curr_filename:
                curr_filename = filename
                filename_total = 0
            filename_total = filename_total + size
            segment_total = segment_total + size
            # print line for filename/function
            out_str = os.path.normpath('%(sz)-8u./%(segment)s/%(file)s/%(sym)s' %
                  {'sz': size, 'segment': segment, 'file': filename,
                   'sym': sym})
            print(out_str)

        # print total for filename (last one)

        # print total for segment
        segment_total = 0

    print("done. Lines: %s, Matches: %s" % (str(line_count), str(line_match)))
