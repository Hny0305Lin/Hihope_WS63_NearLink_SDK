#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

"""
* Description: Utilities of compile system.
* Create: 2020-1-2
"""

import os
import sys
import stat
import time
import subprocess
import re
import shutil
import logging

"""
Colors defines. To highlight important output.
"""
__colors__ = {'purple':'\033[95m', 'red':'\033[91m', 'blue':'\033[94m', 'green':'\033[92m', 'end':'\033[0m'}

"""
Error handling, highlight in red.
"""
class BuildError(Exception):
    def __init__(self, err):
        emsg = "%s%s%s"%(color_red(), err, color_end())
        Exception.__init__(self, emsg)

# End of class BuildError

"""
timer
"""
class BuildTimer:
    def __init__(self, name='A'):
        self._start = -1
        self._name = name

    def start(self):
        self._start = time.time()

    def stop(self):
        if self._start == -1:
            raise BuildError("Timer %s never been started!"%self._name)
        retval = time.time() - self._start
        self._start = -1
        return retval
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
    print("%s%s%s"%(color_purple(), msg, color_end()))

def print_warning(msg):
    print("%s%s%s"%(color_green(), msg, color_end()))

def print_alert(msg):
    print("%s%s%s"%(color_red(), msg, color_end()))

def fn_filter_dirs(dirs, filters=[]):
    retval = list(dirs)
    for dir_path in dirs:
        for item in filters:
            fstr = "%s%s%s"%(os.sep, item, os.sep)
            if (dir_path.find(fstr) >= 0):
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

def fn_search_all_dirs(top_dir, dir_name, excludes=[]):
    """
    Traverse sub-folders to find all files named "dir_name".
    """
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
    match1 = re.match(r'\s*0x',text)
    match2 = re.match(r'\s*0X',text)
    if match1 or match2:
        return int(text, 16)
    else:
        return int(text, 10)

"""
Convert build error from scons to string.
"""
def bf_to_str(bf):
    if bf is None:
        return '(unknown targets product None in list)'
    elif bf.node:
        return str(bf.node) + ': ' + bf.errstr
    elif bf.filename:
        return bf.filename + ': ' + bf.errstr
    else:
        return str(bf)

"""
call shell
"""
def exec_shell(cmd, logfile=None):
    cmdlist = cmd
    logfp = None
    if isinstance(cmd, str):
        cmdlist = cmd.split(' ')

    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    if not logger.handlers:
        logger.addHandler(logging.StreamHandler(sys.stdout))

    if logfile:
        if os.path.isfile(logfile):
            os.unlink(logfile)
        if len(logger.handlers) < 2: # 1. console; 2. file
            logfp = logging.FileHandler(logfile, encoding='utf-8')
            logger.addHandler(logfp)
    try:
        logging.info(str(cmdlist))
        logging.info('\n')
        subp = subprocess.Popen(cmdlist, shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        while True:
            try:
                output = subp.stdout.readline()
                output = output.decode(encoding='UTF-8', errors='replace')
                if output == '' and subp.poll() is not None:
                    break
                if output:
                    logging.info(output.strip())
            except UnicodeDecodeError as err:
                pass
        return subp.returncode
    except FileNotFoundError as err:
        logging.error(err)
        raise Exception(err)
    except Exception as err:
        logging.error(err)
        raise Exception(err)
    finally:
        if logfile and logfp:
            logfp.close()

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

def rm_pyc(root):
    pyc_dirs = fn_search_all_dirs(root, "__pycache__")
    rm_all(pyc_dirs)

def get_diff(list0, list1):
    diff = list(set(list0) - set(list1))
    return diff

