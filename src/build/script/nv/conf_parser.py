#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

"""
* Description: Settings parsers.
* Create: 2020-1-2
"""

import os
import json

from build_utils import color_red
from build_utils import color_end

__all__ = ["MconfParser", "BuildConfParser"]

def nv_repeat_check(pairs):
    key_list = []
    for key_temp in pairs:
        if key_temp[0] not in key_list:
            key_list.append(key_temp[0])
        else:
            raise Exception("nv items(%s) repeat"%key_temp[0])
    pairs = dict(pairs)
    return pairs

class ParserError(Exception):
    """
    Parse errors, highlight in red.
    """
    def __init__(self, err):
        emsg = "%s%s%s"%(color_red(), err, color_end())
        Exception.__init__(self, emsg)
    pass

"""
Json format config file parser
"""
class BuildConfParser:
    def __init__(self, conf_path):
        if not os.path.isfile(conf_path):
            raise ParserError("Configration file %s NOT found!"%conf_path)

        with open(conf_path, 'r', encoding='utf-8') as conf:
            try:
                myread = conf.read()
                self.conf_data = json.loads(myread)
                self.nvconf_data = json.loads(myread, object_pairs_hook = nv_repeat_check)
            except Exception as err:
                msg = "%s\nParsing file:%s"%(err, conf_path)
                raise ParserError(msg)

        self.conf_data = self._parse(self.conf_data)
        self.nvconf_data = self._parse(self.nvconf_data)

    def get_conf_data(self):
        return self.conf_data

    def get_nvconf_data(self):
        return self.nvconf_data

    def get(self, option):
        return self.conf_data.get(option)

    def _parse(self, data):
        """
        parse the python sentence starts with ###
        """
        for key, value in data.items():
            if isinstance(value, dict):
                # Recursion
                value = self._parse(value)
            if isinstance(value, list):
                # Recursion
                data[key] = self._parse_list(value)
            if isinstance(value, int):
                data[key] = value
            if isinstance(value, str) and value.startswith('###'):
                value = self._exec(value)
                data[key] = value
        return data

    def _parse_list(self, values):
        new_list = []
        for val in values:
            if type(val) is str and val.startswith('###'):
                value = self._exec(val)
                new_list.append(value)
            elif isinstance(val, dict):
                new_list.append(self._parse(val))
            else:
                new_list.append(val)
        return new_list

    def _exec(self, code):
        """
        Execute the simple python sentence.
        For the security reason, only allows 'os.path.join' to be input, as a path string
        to support multiple platforms.
        If it needs to support more python features, please use compile and eval, but careful about
        the security issues.
        """
        start = code.find("os.path.join")
        if start < 0:
            raise ParserError("The input doesn't support!")
        lpt = code.find("(")
        if lpt < 0 or lpt < start:
            raise ParserError("The input doesn't support!")
        rpt = code.find(")")
        if rpt < 0 or rpt < lpt:
            raise ParserError("The input doesn't support!")
        path_parts = code[lpt + 1:rpt].split(",")
        ret = ""
        for part in path_parts:
            ret = os.path.join(ret, part.lstrip(" '\"").rstrip(" '\""))
        return ret

"""
Menuconfig format config file parser
"""
class MconfParser:
    def __init__(self, conf_path):
        if not os.path.isfile(conf_path):
            raise ParserError("Configration file %s NOT found!"%conf_path)

        with open(conf_path, 'r', encoding='utf-8') as conf:
            self.conf_data = conf.readlines()

        self.conf_data = self._parse(self.conf_data)

    def get(self, option):
        data = self.conf_data.get(option)
        if data is None:
            # Option not found be treated as false.
            return 'n'
        # strip " when met string values.
        return data.replace('"', '')

    def _parse(self, data):
        settings = {}
        for option in data:
            if self._option_is_valid(option) is True:
                key, value = self._parse_option(option)
                settings[key] = value.strip().replace('\n', '').replace('\r', '')
        return settings

    def _option_is_valid(self, option):
        option = option.strip()
        if (option is None) or (option == '') or (option.startswith('#') is True):
            # skip blanks and comments.
            return False
        return True

    def _parse_option(self, option):
        cfg = option.split('=')
        if len(cfg) == 2:
            # like "KEY=value", length always be 2. return in KEY, value
            return cfg[0], cfg[1]
        else:
            raise ParserError("Unknow format of the option:%s"%option)

def test():
    """
    Test only.
    """
    parser = BuildConfParser("build/config/riscv32_toolchain.json")
    print(parser.get('TargetFolder'))
    mparser = MconfParser("build/config/settings.json")
    print(mparser.get('CONFIG_TARGET_SOFT_VER'))

if __name__ == "__main__":
    test()

