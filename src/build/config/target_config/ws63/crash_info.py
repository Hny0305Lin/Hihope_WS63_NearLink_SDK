import struct
import sys
import os

path = sys.argv[1:]

bin_path = "./crashinfo.bin"
format_path  = "./crashinfo.txt"
if len(path) >= 1:
    for i in path:
        if i.endswith("bin") and os.path.isfile(i):
            bin_path = i
        elif i.endswith("txt"):
            format_path = i
        else:
            print("invalid parameter: ", i)
txt_path = "./crashinfo_bin.txt"
ret = []

def exc_info_get_phase(phase):
    str = {
        0 : "Init",
        1 : "Task",
        2 : "Irq",
        3 : "Exc"
    }
    return str.get(phase, None)

def task_info_get_status(status):
    if status & 0x10 == 0x10:
        return "Running"
    elif status & 0x4 == 0x4:
        return "Ready"
    elif status & 0x20 == 0x20:
        return "Delay"
    elif status & 0x82 == 0x82:
        return "SuspendTime"
    elif status & 0x88 == 0x88:
        return "PendTime"
    elif status & 0x8 == 0x8:
        return "Pend"
    elif status & 0x2 == 0x2:
        return "Suspend"
    elif status & 0x100 == 0x100:
        return "Zombie"
    else:
        return "Invalid"

if __name__ == "__main__":
    with open(bin_path, 'rb') as file:
        data = file.read()
        for byte in data:
            ret.append(f"{byte:02x}")

    with open(txt_path,'w') as file:
        line_index = 0
        for byte in ret:
            file.write(byte)
            line_index += 1
            if line_index >= 16 :
                file.write('\n')
                line_index = 0
            else:
                file.write(' ')

    file = open(bin_path, "rb")
    with open(format_path,'w') as write_file:
        flag = struct.unpack('<I', file.read(4))[0]
        write_file.write(f"{hex(flag)}" + '\n')
        group_cnt = struct.unpack('<I', file.read(4))[0]

        # GROUP SIZE INFO
        group_size = []
        for i in range(group_cnt):
            group_single_size = struct.unpack('<I', file.read(4))[0]
            group_size.append(group_single_size)
        file.seek(4,1)
        # TASK NAME
        write_file.write('task_name:')
        for i in range(group_size[0] - 1):
            char = struct.unpack('<c', file.read(1))[0]
            write_file.write(str(char)[2])
        write_file.write('\n')
        file.seek(1,1)
        # EXC INFO
        write_file.write('==== EXC INFO ====\n')
        exc_info_phase = struct.unpack('<H', file.read(2))[0]
        write_file.write('phase:' + exc_info_get_phase(exc_info_phase) + '\n')
        exc_info_type = struct.unpack('<H', file.read(2))[0]
        write_file.write('type:' + f"{hex(exc_info_type)}" + '\n')
        exc_info_faultAddr = struct.unpack('I', file.read(4))[0]
        write_file.write('faultAddr:' + f"{hex(exc_info_faultAddr)}" + '\n')
        exc_info_thrdPid = struct.unpack('<I', file.read(4)[0:4])[0]
        write_file.write('thrdPid:' + f"{hex(exc_info_thrdPid)}" + '\n')
        exc_info_nestCnt = struct.unpack('<H', file.read(2))[0]
        write_file.write('nestCnt:' + f"{hex(exc_info_nestCnt)}" + '\n')
        exc_info_reserved = struct.unpack('<H', file.read(2))[0]
        write_file.write('reserved:' + f"{hex(exc_info_reserved)}" + '\n')
        exc_info_context = struct.unpack('<I', file.read(4))[0]
        write_file.write('context:' + f"{hex(exc_info_context)}" + '\n')

        # EXC CONTEXT
        write_file.write('==== EXC CONTEXT INFO ====\n')
        exc_context_ccause = struct.unpack('<I', file.read(4))[0]
        write_file.write('ccause:' + f"{hex(exc_context_ccause)}" + '\n')
        exc_context_mcause = struct.unpack('<I', file.read(4))[0]
        write_file.write('mcause:' + f"{hex(exc_context_mcause)}" + '\n')
        exc_context_mtval = struct.unpack('<I', file.read(4))[0]
        write_file.write('mtval:' + f"{hex(exc_context_mtval)}" + '\n')
        exc_context_gp = struct.unpack('<I', file.read(4))[0]
        write_file.write('gp:' + f"{hex(exc_context_gp)}" + '\n')
        # EXC TASK CONTEXT
        exc_task_context_mstatus = struct.unpack('<I', file.read(4))[0]
        exc_task_context_mepc = struct.unpack('<I', file.read(4))[0]
        exc_task_context_tp = struct.unpack('<I', file.read(4))[0]
        exc_task_context_sp = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s11 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s10 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s9 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s8 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s7 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s6 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s5 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s4 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s3 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s2 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s1 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_s0 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_t6 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_t5 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_t4 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_t3 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_a7 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_a6 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_a5 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_a4 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_a3 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_a2 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_a1 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_a0 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_t2 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_t1 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_t0 = struct.unpack('<I', file.read(4))[0]
        exc_task_context_ra = struct.unpack('<I', file.read(4))[0]
        write_file.write('mstatus:' + f"{hex(exc_task_context_mstatus)}" + '\n')
        write_file.write('mepc:' + f"{hex(exc_task_context_mepc)}" + '\n')
        write_file.write('ra:' + f"{hex(exc_task_context_ra)}" + '\n')
        write_file.write('sp:' + f"{hex(exc_task_context_sp)}" + '\n')
        write_file.write('tp:' + f"{hex(exc_task_context_tp)}" + '\n')
        write_file.write('t0:' + f"{hex(exc_task_context_t0)}" + '\n')
        write_file.write('t1:' + f"{hex(exc_task_context_t1)}" + '\n')
        write_file.write('t2:' + f"{hex(exc_task_context_t2)}" + '\n')
        write_file.write('s0:' + f"{hex(exc_task_context_s0)}" + '\n')
        write_file.write('s1:' + f"{hex(exc_task_context_s1)}" + '\n')
        write_file.write('a0:' + f"{hex(exc_task_context_a0)}" + '\n')
        write_file.write('a1:' + f"{hex(exc_task_context_a1)}" + '\n')
        write_file.write('a2:' + f"{hex(exc_task_context_a2)}" + '\n')
        write_file.write('a3:' + f"{hex(exc_task_context_a3)}" + '\n')
        write_file.write('a4:' + f"{hex(exc_task_context_a4)}" + '\n')
        write_file.write('a5:' + f"{hex(exc_task_context_a5)}" + '\n')
        write_file.write('a6:' + f"{hex(exc_task_context_a6)}" + '\n')
        write_file.write('a7:' + f"{hex(exc_task_context_a7)}" + '\n')
        write_file.write('s2:' + f"{hex(exc_task_context_s2)}" + '\n')
        write_file.write('s3:' + f"{hex(exc_task_context_s3)}" + '\n')
        write_file.write('s4:' + f"{hex(exc_task_context_s4)}" + '\n')
        write_file.write('s5:' + f"{hex(exc_task_context_s5)}" + '\n')
        write_file.write('s6:' + f"{hex(exc_task_context_s6)}" + '\n')
        write_file.write('s7:' + f"{hex(exc_task_context_s7)}" + '\n')
        write_file.write('s8:' + f"{hex(exc_task_context_s8)}" + '\n')
        write_file.write('s9:' + f"{hex(exc_task_context_s9)}" + '\n')
        write_file.write('s10:' + f"{hex(exc_task_context_s10)}" + '\n')
        write_file.write('s11:' + f"{hex(exc_task_context_s11)}" + '\n')
        write_file.write('t3:' + f"{hex(exc_task_context_t3)}" + '\n')
        write_file.write('t4:' + f"{hex(exc_task_context_t4)}" + '\n')
        write_file.write('t5:' + f"{hex(exc_task_context_t5)}" + '\n')
        write_file.write('t6:' + f"{hex(exc_task_context_t6)}" + '\n')

        # BACKTRACE INFO
        write_file.write('==== BACKTRACE INFO ====\n')
        backtrace_count = struct.unpack('<I', file.read(4))[0]
        write_file.write('backtrace count:' + f"{str(backtrace_count)}" + '\n')
        for i in range(backtrace_count):
            backtrace_sp = struct.unpack('<I', file.read(4))[0]
            backtrace_sp_content = struct.unpack('<I', file.read(4))[0]
            write_file.write('sp addr : ' + f"{hex(backtrace_sp)}" + ' , sp content : ' + f"{hex(backtrace_sp_content)}" + '\n')
        move_size = 80 - backtrace_count * 8
        file.seek(move_size,1)
        task_num = struct.unpack('<I', file.read(4))[0]
        # TASK INFO
        write_file.write('==== TASK INFO ====\n')
        write_file.write('Name              TaskEntryAddr       TID    Priority   Status       StackSize    ' +
            'WaterLine      StackPoint   TopOfStack   SemID        EventMask\n')
        write_file.write('----              -------------       ---    --------   ------       ---------    ' +
            '---------      ----------   ----------   -----        ---------\n')
        ## WATERLINE INFO
        waterline_group = []
        for i in range(group_size[4] // 4):
            waterline = struct.unpack('<I', file.read(4))[0]
            waterline_group.append(waterline)
        ## TASK INFO
        task_info_size = group_size[7] if group_size[7] > 0 else 96
        task_info_cnt = struct.unpack('<I', file.read(4))[0]
        task_info_total_size = group_size[6] // task_info_size
        ## TASK NAME INFO
        task_semId_arr = []
        task_name_arr = []
        for i in range(task_info_cnt):
            task_semId = struct.unpack('<I', file.read(4))[0]
            task_semId_arr.append(task_semId)
            task_name_len = struct.unpack('<I', file.read(4))[0]
            task_name = []
            for i in range(task_name_len):
                char = struct.unpack('<c', file.read(1))[0]
                task_name.append(str(char)[2])
            struct.unpack('<c', file.read(1))[0]
            task_name_arr.append(task_name)

        task_info_addr = file.tell()
        task_info_index = 0
        for i in range(task_num):
            if task_info_index >= len(task_name_arr):
                break
            file.seek(task_info_addr + i * task_info_size, 0)
            task_info_stackPointer = struct.unpack('<I', file.read(4))[0]
            task_info_taskStatus = struct.unpack('<H', file.read(2))[0]
            if task_info_get_status(task_info_taskStatus) == "Invalid" :
                continue
            task_info_priority = struct.unpack('<H', file.read(2))[0]
            task_info_flag = struct.unpack('<I', file.read(4))[0]
            task_info_stackSize = struct.unpack('<I', file.read(4))[0]
            task_info_topOfStack = struct.unpack('<I', file.read(4))[0]
            task_info_taskId = struct.unpack('<I', file.read(4))[0]
            task_info_taskEntry = struct.unpack('<I', file.read(4))[0]
            task_info_taskSemCb = struct.unpack('<I', file.read(4))[0]
            task_info_param1 = struct.unpack('<I', file.read(4))[0]
            task_info_param2 = struct.unpack('<I', file.read(4))[0]
            task_info_param3 = struct.unpack('<I', file.read(4))[0]
            task_info_param4 = struct.unpack('<I', file.read(4))[0]
            task_info_param5 = struct.unpack('<I', file.read(4))[0]
            task_info_param6 = struct.unpack('<I', file.read(4))[0]
            task_info_param7 = struct.unpack('<I', file.read(4))[0]
            task_info_param8 = struct.unpack('<I', file.read(4))[0]
            task_info_param9 = struct.unpack('<I', file.read(4))[0]
            task_info_param10 = struct.unpack('<I', file.read(4))[0]
            task_info_param11 = struct.unpack('<I', file.read(4))[0]
            task_info_eventmask = struct.unpack('<I', file.read(4))[0]
            
            write_file.write(''.join(task_name_arr[task_info_index]).ljust(18))
            write_file.write(f"{hex(task_info_taskEntry)}".ljust(20))
            write_file.write(f"{hex(task_info_taskId)}".ljust(7))
            write_file.write(f"{str(task_info_priority)}".ljust(11))
            write_file.write(task_info_get_status(task_info_taskStatus).ljust(13))
            write_file.write(f"{hex(task_info_stackSize)}".ljust(13))
            write_file.write(f"{hex(waterline_group[i])}".ljust(15))
            write_file.write(f"{hex(task_info_stackPointer)}".ljust(13))
            write_file.write(f"{hex(task_info_topOfStack)}".ljust(13))

            write_file.write(f"{hex(task_semId_arr[task_info_index])}".ljust(13))
            write_file.write(f'{hex(task_info_eventmask)}' + '\n')
            task_info_index += 1
            line_index = 0
    print('crash info resolve done')