#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys,string,re,os
from ctypes import *
from auto_class import *
from config import *

class _PhaseCtrl:
    def __init__(self):
        self.dfx_stat=None

    def make_diag_info(self):
        addr = parse_get_symbol_addr('g_zdiag_dfx_stat')
        obj=parse_memory_2_class(addr,zdiag_dfx_stat,sizeof(zdiag_dfx_stat))
        self.dfx_stat=obj
    def print_diag_info(self):
        uapi_print_ctypes_obj('diag_dfx_stat',self.dfx_stat)

_g_parse_ctrl=_PhaseCtrl()

def parase_diag_info():
    _g_parse_ctrl.make_diag_info()
    _g_parse_ctrl.print_diag_info()