#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

import os
import shutil
import sys
import time

from utils.build_utils import exec_shell, root_path, output_root, sdk_output_path, pkg_tools_path
from utils.build_utils import compare_bin
from enviroment import TargetEnvironment, BuildEnvironment
from pack_tool import packTool
from sdk_generator.sdk_generator import SdkGenerator
from rom_ram_callback.gen_rom_ram_callback import gen_rom_ram_callback
from rom_ram_callback.strip_undef_symbols import strip_undefined_symbols
from usr_config import mconfig
from custom_cmd import run_custom_cmd
from target_config.common_config import CommonConfig

class CMakeBuilder(BuildEnvironment):
    """ cmake builder, 接收并解析参数，启动构建
    """
    def __init__(self, param_list):
        super(CMakeBuilder, self).__init__(param_list)
        self.cmake_cmd = []
        self.sdk = None
        self.pack_tool = None

    def get_component(self, env):
        com_config = CommonConfig(env.get('arch'))
        components = []
        if env.get('just_build_components', False):
            just_build_components = env.get('just_build_components', False)
        else:
            just_build_components = self.component
        for component in just_build_components:
            if component in env.get('ram_component', False):
                components.append(component)
                continue
            if component not in env.get('ram_component_set', False):
                continue
            for comm in com_config.get_component_set(component):
                if comm in env.get('ram_component', False):
                    components.append(comm)
        return components

    def get_build_cmd(self, env):
        ext_cmd = []
        components = self.get_component(env)
        ext_cmd.extend(components)
        if self.generator == 'Ninja':
            return ['ninja'] + ext_cmd + ['-j%d' % self.thread]
        else:
            return ['make'] + ext_cmd + ['-j%d' % self.thread]

    def build(self):
        """
        """
        built_targets = []
        for group in self.group_names:
            need_pack = False
            if group.startswith('pack'):
                need_pack = True
                chip = self.get_chip_name(group)
                if os.path.exists(os.path.join(output_root, 'package', chip, group)):
                    shutil.rmtree(os.path.join(output_root, 'package', chip, group))

            for target in self.get_target_names_by_group_name(group):
                self.pack_tool = packTool(group, target)
                if self.is_group(target):
                    self.group_names.append(target)
                    continue

                if self.is_copy_target(target):
                    self.pack_tool.pack()
                    continue

                if target == 'fwpkg' and need_pack:
                    self.pack_fwpkg(chip, group)
                    continue

                if not self.is_target(target):
                    print("Invalid target %s" % target)
                    raise
                if not target in built_targets:
                    self.build_target(target)
                    built_targets.append(target)
                else:
                    print("%s has built, skip"  % target)
                target_env = TargetEnvironment(target, self.extr_defines)
                if target_env.get('build_type') == 'SDK':
                    continue
                if need_pack:
                    self.pack_tool.pack()

        if len(self.target_names) == 1 and self.open_kconfig == True:
            self.menuconfig_to_build(self.target_names[0])
            return

        for target in self.target_names:
            self.build_target(target)

    def menuconfig_to_build(self, target):
        env = TargetEnvironment(target)
        mconfig(self.kconfig_param, env.get("chip"), env.get("core"), target, None)

    def build_target(self, target):
        env = TargetEnvironment(target, self.extr_defines)
        if env.get('build_type') == 'SDK':
            self.build_sdk(env)
            return
        if not run_custom_cmd(env, target, 'build_pre'):
            self.print_build_result(target, 1)
            sys.exit(1)

        self.compile_target(target, env)

        if env.get('just_build_components'):
            return

        if env.get("fs_image"):
            fs_image_path = os.path.join(root_path, 'build', 'config', 'target_config', env.get('chip'), 'mk_fs_image')
            fs_script_path = os.path.join(fs_image_path, 'mkyaffs2tool.py')
            output_path = env.get_output_path()
            print(output_path)
            errcode = exec_shell([self.python_path, fs_script_path, output_path], None, True)
            if errcode != 0:
                print("creat fs image error!")
                self.print_build_result(target, errcode)
                sys.exit(1)
            print("fs image success!")

        if env.get('upg_pkg'):
            self.pack_fota(env.get('chip'), target, env.get('upg_pkg'))

        if not run_custom_cmd(env, target, 'build_post'):
            self.print_build_result(target, 1)
            sys.exit(1)

        if env.get('packet') :
            self.pack_fwpkg(env.get('chip'), target)

    def build_sdk(self, env):
        self.sdk = SdkGenerator(env, sdk_output_path)
        if os.path.exists(sdk_output_path):
            print("Cleaning SDK output path")
            shutil.rmtree(sdk_output_path)
        sdk_pkg_target_name = env.get('pkg_target_name', cmake_type=False)

        for sdk_target in sdk_pkg_target_name:
            sdk_target_env = TargetEnvironment(sdk_target, self.extr_defines)
            output_path = sdk_target_env.get_output_path()
            self.compile_target(sdk_target, sdk_target_env)
            self.sdk.copy_depends('%s/cmake_trace.txt' % output_path)
            if self.sdk.env.get('reload_kconfig'):
                cwd = os.getcwd()
                os.chdir(sdk_output_path)
                mconfig('reloadconfig', sdk_target_env.get("chip"), sdk_target_env.get("core"), sdk_target, None, root=sdk_output_path)
                os.chdir(cwd)

        if env.get('config') and 'lib_gen' in env.get('config'):
            lib_gen_tasks = env.get('config')['lib_gen']
            print(lib_gen_tasks)
            for libs in lib_gen_tasks:
                task = lib_gen_tasks[libs]
                target_name = task['base_target_name']
                defines = task.get("defines", [])
                self.component = task['components']
                target_env = TargetEnvironment(target_name, extra_defines=defines)
                target_env.add("LIB_GEN_NAME", libs)
                target_env.extend("SDK_LIBGEN_COMPONENTS", self.component)
                self.compile_target(target_name, target_env)
        chip = env.get('chip')
        self.sdk.genarate_efuse_cfg_bin(chip)
        self.sdk.sdk_build(self.build_time, self.no_hso, self.build_level)
        self.sdk = None
        self.component = []

    def compile_target(self, target_name, env):
        start_time = time.time()
        self.deal_symbol_link(env)
        env.add('build_level', self.build_level)
        if self.build_level == "debug":
            env.extend("ccflags", ['-funwind-tables', '-fasynchronous-unwind-tables'])
            env.extend("defines", "NO_TIMEOUT")
        if self.build_as_lib:
            env.add("GEN_ONLY_LIB_PATH")
        self.cmake_cmd = ['cmake', '-G', self.generator, '-Wno-dev', '--no-warn-unused-cli', '-DCMAKE_C_COMPILER_WORKS=TRUE', '-DCMAKE_CXX_COMPILER_WORKS=TRUE']
        if env.get('fp_enable'):
            env.append('defines', 'SUPPORT_CALLSTACK')
            env.append('ccflags', '-fno-omit-frame-pointer')
        self.add_build_param(env)

        output_path = env.get_output_path()
        self.pre_sdk(output_path, env)
        if env.get('libstd_option'):
            self.add_cmake_def(env, 'std_libs')
        self.cmake_cmd.append(root_path)

        if env.get('product_type'):
            self.cmake_cmd.append('-DPRODUCT_TYPE={0}'.format(env.get('product_type')))
        else:
            self.cmake_cmd.append('-DPRODUCT_TYPE=default')

        if self.dump:
            env.dump()

        if self.rom_callback(env, target_name, output_path):
            if not os.path.exists(output_path):
                os.makedirs(output_path)
            env.set('build_rom_callback', False)
            env.append('defines', '_PRE_FEATURE_VENEER_ROM')
            env.append('ram_component', 'rom_callback')
            self.redef_cmake_def(env, 'build_rom_callback')
            self.redef_cmake_def(env, 'defines')
            self.redef_cmake_def(env, 'ram_component')
        self.add_cmake_param("-DROM_CHECK=False")
        self.start(env, target_name, output_path, clean=self.need_clean, nhso=self.no_hso)
        self.rom_check(env, target_name, output_path)

        end_time = time.time()
        print("%s takes %f s" %  (target_name, end_time - start_time))

    def deal_symbol_link(self, env):
        if "rom_sym_path" not in env.config:
            return
        if self.no_symbol_link and not self.sdk:
            env.config["rom_sym_path"] = ""
        else:
            env.config["rom_sym_path"] = env.config["rom_sym_path"].replace("<root>", root_path)
            if os.path.exists(env.config["rom_sym_path"]):
                env.config["linkflags"].append("-Wl,--just-symbols=" + env.config["rom_sym_path"])
                env.config["defines"].append("ROM_SYMBOL_LINK")
            else:
                print("ERROR: rom_sym_path %s is not exists" % env.config["rom_sym_path"])
        if "bootrom_sym_path" not in env.config:
            return
        if env.config["bootrom_sym_path"] != "":
            env.config["bootrom_sym_path"] = env.config["bootrom_sym_path"].replace("<root>", root_path)
            if os.path.exists(env.config["bootrom_sym_path"]):
                env.config["linkflags"].append("-Wl,--just-symbols=" + env.config["bootrom_sym_path"])
                env.config["defines"].append("BOOTROM_SYMBOL_LINK")
            else:
                print("ERROR: bootrom_sym_path %s is not exists" % env.config["bootrom_sym_path"])

    def add_build_param(self, env):
        for item in env.config:
            self.add_cmake_def(env, item)
        self.add_cmake_param('-DCMAKE_TOOLCHAIN_FILE=%s' % env.get_tool_chain())
        self.add_cmake_param('-DPY_PATH=%s' % self.python_path)
        if self.build_time != '':
            self.add_cmake_param('-DBUILD_TIME=%s' % self.build_time)

    def pre_sdk(self, output_path, env):
        if not self.sdk:
            return

        self.sdk.register_org_target_path(output_path)
        self.add_cmake_param('-DSDK_OUTPUT_PATH=%s' % sdk_output_path)
        self.add_cmake_def(self.sdk.env, 'sdk_type')
        # closed_components takes precedence over open_components
        closed_components = self.sdk.env.get('closed_components', cmake_type=False)
        open_components = self.sdk.env.get('open_components', cmake_type=False)
        if None not in (closed_components, open_components):
            raise Exception(f'ERROR! closed_components and open_components cannot be set together!')
        self.add_cmake_def(self.sdk.env, 'closed_components')
        if not closed_components:
            self.add_cmake_def(self.sdk.env, 'open_components')

        self.add_cmake_def(env, 'main_component')

        # export trace of cmake invocation
        cmake_trace_file = os.path.join(output_path, 'cmake_trace.txt')
        self.cmake_cmd.append('--trace-format=json-v1')
        self.cmake_cmd.append('--trace-redirect=%s' % cmake_trace_file)

    def print_build_result(self, target_name, fail):
        print("######### Build target:%s %s" % (target_name, "failed" if fail else "success"))
        if self.sdk:
            print("######### Build sdk %s!!" % ("failed" if fail else "success"))

    def rom_callback(self, env, target_name, output_path):
        if not env.get('build_rom_callback'):
            return False
        if env.get('fixed_rom'):
            return True
        self.start(env, target_name, output_path, nhso=True, clean=self.need_clean)
        print("GENERATING AUTO ROM BIN FILE!!!")
        chip = env.get("chip")
        core = env.get("core")
        board = env.get('board')
        application = env.get('application')
        arch = env.get('arch')
        rom_config = os.path.join(root_path, 'drivers', 'chips', chip, 'rom_config', core)
        rom_output = os.path.join(rom_config, 'output')
        if not os.path.exists(rom_output):
            os.makedirs(rom_output)

        shutil.copy(os.path.join(output_path, "rom_bin_raw.undef"), rom_output)
        shutil.copy(os.path.join(output_path, "rom_symbol.list"), rom_output)
        shutil.copy(os.path.join(output_path, "rom_bin.rel"), rom_output)
        shutil.copy(os.path.join(output_path, "image_symbol.list"), rom_output)
        shutil.copy(os.path.join(rom_config, "undef_rom_filter.list"), rom_output)
        shutil.copy(os.path.join(rom_config, "region.list"), rom_output)

        strip_undefined_symbols(rom_output)
        if arch[:5] == "riscv":
            target = "riscv32"
        elif arch[:3] == "arm":
            target = "arm32"
        gen_rom_ram_callback(target, rom_output)
        print("ROM CALLBACK BUILD SUCCESSFULLY!!!")
        shutil.rmtree(output_path)
        return True

    def start(self, env, target_name, output_path, nhso=None, clean=None):
        # remember the root folder
        def _excute(cmd, log_file, is_dump):
            errcode = exec_shell(cmd, log_file, is_dump)
            if errcode != 0:
                self.print_build_result(target_name, errcode)
                sys.exit(1)
        org_work_path = os.getcwd()
        self.cmake_cmd.append('-DPKG_TARGET_NAME=%s' % target_name)
        target_name = target_name.replace('-', "_")
        if clean and os.path.exists(output_path):
            shutil.rmtree(output_path)
        if not os.path.exists(output_path):
            os.makedirs(output_path)
        self.cmake_cmd.append('-DBUILD_TARGET_NAME=%s' % (target_name))
        self.cmake_cmd.append('-DNHSO=%s' %nhso)
        chip = env.get('chip')
        core = env.get('core')

        # Generate the menuconfig header.
        mconfig_file_path = os.path.join(root_path, 'build', 'config', 'target_config', chip, 'menuconfig', core, f"{target_name}.config")
        if os.path.exists(mconfig_file_path):
            if env.get('reload_kconfig'):
                print("build reload %s" %mconfig_file_path)
                mconfig("reloadconfig", chip, core, target_name, output_path)
            mconfig("savemenuconfig", chip, core, target_name, output_path)

        os.chdir(output_path)
        log_file_name = "build_%s.log" % target_name
        log_file = os.path.join(output_root, chip, log_file_name)

        # run 'make'
        if self.is_command_refresh(output_path):
            self.dump_cmake_command(output_path)
            _excute(self.cmake_cmd, log_file, self.dump)
            _excute(self.get_build_cmd(env), log_file, self.dump)
        else:
            ext_cmd = []
            components_target = self.get_component(env)
            print(components_target)
            if components_target:
                ext_cmd = ['--target'] + components_target
            _excute(["cmake", "--build", output_path, '-j%d' % self.thread] + ext_cmd, log_file, self.dump)

        if len(self.get_component(env)) > 0:
            os.chdir(org_work_path)
            print("######### Build target:%s, component:[%s] success" % (target_name, ' '.join(self.get_component(env))))
            return 0

        if env.is_enable_hso() and not nhso:
            cmd = self.get_build_cmd(env)
            cmd.append("HSO_DB")
            _excute(cmd, log_file, self.dump)

        # switch the work folder back
        os.chdir(org_work_path)
        self.print_build_result(target_name, 0)
        return 0

    def add_cmake_param(self, param):
        """
        accept string only
        """
        self.cmake_cmd.append(param)

    def add_cmake_def(self, env, item):
        if env.get(item) is None or env.get(item) == '':
            return
        self.cmake_cmd.append('-D%s=%s' % (item.upper(), env.get(item)))

    def redef_cmake_def(self, env, item):
        if env.get(item) is None or env.get(item) == '':
            return
        val = env.get(item)
        item = item.upper()
        for i, para in enumerate(self.cmake_cmd):
            if not para.startswith('-D%s=' % item):
                continue
            self.cmake_cmd[i] = '-D%s=%s' % (item, val)
            break

    def rom_check(self, env, target_name, output_path):
        if env.get('rom_sym_path'):
            return True

        if env.get('fixed_rom'):
            fix_path = env.get('fixed_rom_path').replace('<root>', root_path)
            bin1 = os.path.join(output_path, '%s_rom.bin' % env.get('bin_name'))
            bin2 = fix_path
            if not compare_bin(bin1, bin2):
                print(f"ERROR! :{bin1} is not same with {bin2}")
                sys.exit(1)
            return True

        if env.get('rom_ram_check'):
            self.add_cmake_param("-DROM_CHECK=True")
            self.start(env, target_name, output_path, clean=False, nhso=True)
            if not env.get('rom_ram_compare'):
                return True
            bin1 = os.path.join(output_path, '%s_rom.bin' % env.get('bin_name'))
            bin2 = os.path.join(output_path, '%s_romcheck_rom.bin' % env.get('bin_name'))
            if not compare_bin(bin1, bin2):
                print(f"ERROR! :{bin1} is not same with {bin2}")
                sys.exit(1)

    def pack_fwpkg(self, chip, target_name):
        # bin packet all in one
        packet_script_path = os.path.join(pkg_tools_path, 'packet.py')
        print("packet ing...")
        errcode = exec_shell([self.python_path, packet_script_path, chip, target_name, " ".join(self.extr_defines)], None, True)
        if errcode != 0:
            print("packet error!")
            self.print_build_result(target_name, errcode)
            sys.exit(1)
        print("packet success!")

    def pack_fota(self, chip, target_name, option):
        fota_script_path = os.path.join(root_path, 'build', 'config', 'target_config', chip, 'build_' + chip + '_update.py')
        print("fota packet generating...")
        errcode = exec_shell([self.python_path, fota_script_path, target_name, option], None, True)
        if errcode != 0:
            print("fota packet error!")
            self.print_build_result(target_name, errcode)
            sys.exit(1)
        print("fota packet success!")

    def dump_cmake_command(self, output_path):
        with open(os.path.join(output_path, 'cmake_command.txt'), "w") as f:
            f.write("\n".join(self.cmake_cmd))

    def is_command_refresh(self, output_path):
        cmd_path = os.path.join(output_path, 'cmake_command.txt')
        if not os.path.exists(cmd_path):
            return True
        with open(cmd_path, "r") as f:
            text = f.read()
        return text != "\n".join(self.cmake_cmd)
