#!/usr/bin/env python
# coding:utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.

import sys,string,re,os
from ctypes import *
from parse_basic import *
from auto_class import *
from config import *

OS_TASK_STATUS_UNUSED = 0x0001
OS_TASK_STATUS_SUSPEND = 0x0002
OS_TASK_STATUS_READY = 0x0004
OS_TASK_STATUS_PEND = 0x0008
OS_TASK_STATUS_RUNNING = 0x0010
OS_TASK_STATUS_DELAY = 0x0020
OS_TASK_STATUS_TIMEOUT = 0x0040
OS_TASK_STATUS_PEND_TIME = 0x0080
OS_TASK_FLAG_DETACHED = 0x0001
OS_TASK_FLAG_SYSTEM = 0x0002
OS_SWTMR_STATUS_UNUSED = 0
OS_SWTMR_STATUS_DELETING =1
OS_SWTMR_STATUS_CREATED=2
OS_SWTMR_STATUS_TICKING=3
#ctypes.addressof(parameter) - ctypes.addressof(parent_structure)


def _los_List_get_all_node(list_addr):
    node_list=[]
    node=parse_memory_2_class(list_addr,LOS_DL_LIST,sizeof(LOS_DL_LIST))
    if node:
        while node.pstNext!=list_addr:
            node_list.append(node.pstNext)
            node = parse_memory_2_class(node.pstNext,LOS_DL_LIST,sizeof(LOS_DL_LIST))
    return (DEFINE_SUCCESS,node_list)

def _addr_in_2_function_name(addr):
    name=parse_addr_in_2_function_name(addr)
    if name==None:
        return hex(addr)
    else:
        return name

class _PhaseLiteosCtrl:
    def __init__(self):
        self.core=None
        self.info_fd = sys.__stdout__
        self.task_cnt=None
        self.tcb_array = []
        self.ext_task_list=[]

        self.heap_start = None
        self.heap_end = None
        self.heap_head = None
        self.queue_cnt = LOSCFG_BASE_IPC_QUEUE_LIMIT
        self.queue_array=[]
        self.queue_ext_info=[]
        self.timer_cnt = LOSCFG_BASE_CORE_SWTMR_LIMIT
        self.timer_array=[]
        self.timer_ext_info=[]
        self.sem_cnt=LOSCFG_BASE_IPC_SEM_LIMIT
        self.sem_array=[]
        self.sem_ext_info=[]

        self.excInfo=None

        self.isr_ext_info=None
        self.version=None

    class _ExtPercpu:
        def __init__(self):
            self.handle_addr = None
            self.per_cpu=None
            self.g_tickCount=None
            self.g_sysClock=None
            self.g_tickPerSecond=None
            self.g_cycle2NsScale=None
            self.task_wait_addr=[]
            self.timer_wait_addr=[]
            self.g_intCount=None

    class _ExtTaskInfo:
        def __init__(self):
            self.handle_addr=None
            self.pend_addr=None
            self.name='unknown'
            self.buttom=0
            self.peek_used=0
            self.over=0
            self.status_str=None
            self.enter_name = 'unknown'
            self.task_context = None
            self.take_sem = None

    class _ExtTimerInfo:
        def __init__(self):
            self.handle_addr=None
            self.func_name=None

    class _ExtSemInfo:
        def __init__(self):
            self.handle_addr=None
            self.pend_task_list=[]

    class _ExtQueueInfo:
        def __init__(self):
            self.handle_addr=None
            self.pend_read_task_list = []
            self.pend_write_task_list =[]

    class ExtIsrInfo:
        def __init__(self):
            self.isr_cnt = LOSCFG_PLATFORM_HWI_LIMIT
            self.g_hwiForm=None

    def make_isr_info(self):
        self.isr_ext_info = self.ExtIsrInfo()
        addr = parse_get_symbol_addr('g_hwiForm')
        if self.version == 207:
            self.isr_ext_info.g_hwiForm = parse_memory_2_class(addr,HwiHandleInfo*self.isr_ext_info.isr_cnt,sizeof(HwiHandleInfo*self.isr_ext_info.isr_cnt))
        elif self.version == 208:
            self.isr_ext_info.g_hwiForm = parse_memory_2_class(addr,HwiHandleInfo*self.isr_ext_info.isr_cnt,sizeof(HwiHandleInfo*self.isr_ext_info.isr_cnt))

    def print_isr_info(self):
        print("\r\n--------------------content of 中断信息:--------------------",file=self.info_fd)
        if self.version == 207:
            for hwi in self.isr_ext_info.g_hwiForm:
                if hwi.hook!=0 or hwi.respCount!=0:
                    print("%s[0x%x][call_cnt=%d]"%(_addr_in_2_function_name(hwi.hook),hwi.hook,hwi.respCount),file=self.info_fd)
        elif self.version == 208:
            for hwi in self.isr_ext_info.g_hwiForm:
                if hwi.hook!=0 or hwi.respCount!=0:
                    print("%s[0x%x][call_cnt=%d]"%(_addr_in_2_function_name(hwi.hook),hwi.hook,hwi.respCount),file=self.info_fd)

    def make_per_cpu_info(self):
        self.core=self._ExtPercpu()
        self.core.handle_addr = parse_get_symbol_addr('g_percpu')
        self.core.per_cpu = parse_memory_2_class(self.core.handle_addr,Percpu,sizeof(Percpu))

        addr = parse_get_symbol_addr('g_tickCount')
        if addr:
            obj=parse_memory_2_class(addr,c_ulonglong,sizeof(c_ulonglong))
            self.core.g_tickCount=obj.value

        addr = parse_get_symbol_addr('g_sysClock')
        if addr:
            obj=parse_memory_2_class(addr,c_uint,sizeof(c_uint))
            self.core.g_sysClock=obj.value

        addr = parse_get_symbol_addr('g_intCount')
        if addr:
            obj=parse_memory_2_class(addr,c_uint,sizeof(c_uint))
            self.core.g_intCount=obj.value

        addr = parse_get_symbol_addr('g_tickPerSecond')
        if addr:
            obj=parse_memory_2_class(addr,c_uint,sizeof(c_uint))
            self.core.g_tickPerSecond=obj.value

        addr = parse_get_symbol_addr('g_cycle2NsScale')
        if addr:
            obj=parse_memory_2_class(addr,c_double,sizeof(c_double))
            self.core.g_cycle2NsScale=obj.value
        (ret,self.core.task_wait_addr) = _los_List_get_all_node(self.core.per_cpu.taskSortLink.sortLink)

        (ret,self.core.timer_wait_addr) = _los_List_get_all_node(self.core.per_cpu.swtmrSortLink.sortLink)

    def print_per_cpu_info(self):
        print("\r\n--------------------content of 调度信息汇总:--------------------",file=self.info_fd)
        print("说明:taskLockCnt锁任务嵌套次数",file=self.info_fd)
        if self.core.g_cycle2NsScale:
            print("g_tickCount=%ull,g_sysClock=%u,g_tickPerSecond=%u,g_cycle2NsScale=%f"%(self.core.g_tickCount,self.core.g_sysClock,self.core.g_tickPerSecond,self.core.g_cycle2NsScale),file=self.info_fd)
        else:
            print("g_tickCount=%ull,g_sysClock=%u,g_tickPerSecond=%u"%(self.core.g_tickCount,self.core.g_sysClock,self.core.g_tickPerSecond),file=self.info_fd)

        uapi_print_ctypes_obj('schedule',self.core.per_cpu,self.info_fd)

        delay_tick=0
        for x in self.core.task_wait_addr:
            x = x-ctype_member_offset('sortList.sortLinkNode',LosTaskCB)
            i=0
            while i< self.task_cnt:
                tcb=self.tcb_array[i]
                ext_info=self.ext_task_list[i]
                if ext_info.handle_addr==x:
                    delay_tick=delay_tick+tcb.sortList.idxRollNum
                    print("任务等待剩余时间:%s[%d][剩余时间 delay=%d tick][idxRollNum=%d]"%(ext_info.name,tcb.taskId,delay_tick,tcb.sortList.idxRollNum),file=self.info_fd)
                i=i+1

        delay_tick=0
        for x in self.core.timer_wait_addr:
            x = x-ctype_member_offset('sortList.sortLinkNode',LosSwtmrCB)
            i=0
            while i< self.timer_cnt:
                timer=self.timer_array[i]
                ext_info=self.timer_ext_info[i]
                if ext_info.handle_addr==x:
                    delay_tick=delay_tick+timer.sortList.idxRollNum
                    print("定时器到期剩余时间:%s[%d][剩余时间=%d tick][idxRollNum=%d]"%(ext_info.func_name,timer.timerId,delay_tick,timer.sortList.idxRollNum),file=self.info_fd)
                i=i+1

    def make_task_info(self):
        addr = parse_get_symbol_addr('g_taskMaxNum')
        obj=parse_memory_2_class(addr,c_uint,4)
        self.task_cnt = obj.value

        addr = parse_get_symbol_addr('g_osTaskCBArray')
        obj=parse_memory_2_class(addr,c_uint,4)
        addr=obj.value
        i=0
        while i < self.task_cnt:
            ext_info = self._ExtTaskInfo()
            obj=parse_memory_2_class(addr,LosTaskCB,sizeof(LosTaskCB))
            self.tcb_array.append(obj)
            self.ext_task_list.append(ext_info)
            ext_info.handle_addr=addr
            ext_info.pend_addr = ext_info.handle_addr+ctype_member_offset('pendList',LosTaskCB)
            addr=addr+sizeof(LosTaskCB)
            i=i+1
        self._make_task_extra_info()

    def _task_status_2_string(self,status):
        str=''
        if (status&OS_TASK_STATUS_UNUSED):
            str=str+"unused,"
        if (status&OS_TASK_STATUS_RUNNING):
            str=str+'runing,'
        if (status&OS_TASK_STATUS_READY):
            str=str+'ready,'
        if (status&OS_TASK_STATUS_SUSPEND):
            str=str+'suspend,'
        if (status&OS_TASK_STATUS_PEND):
            str=str+'pend,'
        if (status&OS_TASK_STATUS_DELAY):
            str=str+'delay,'
        if (status&OS_TASK_STATUS_TIMEOUT):
            str=str+'timeout,'
        if (status&OS_TASK_STATUS_PEND_TIME):
            str=str+'pendtime,'
        return str

    def _make_task_extra_info(self):
        i=0
        while i < self.task_cnt:
            tcb = self.tcb_array[i]
            ext_info=self.ext_task_list[i]
            if tcb.taskStatus & OS_TASK_STATUS_UNUSED: #未使用任务返回
                i=i+1
                continue;

            ext_info.buttom = tcb.topOfStack+tcb.stackSize #计算栈buttom
            ext_info.status_str = self._task_status_2_string(tcb.taskStatus)
            if tcb.taskSem!=0:
                ext_info.take_sem = parse_memory_2_class(tcb.taskSem,LosSemCB,sizeof(LosSemCB))

            #生成taskname
            addr=tcb.taskName
            str = parse_memory_2_string(addr,32)
            if str!=None:
                ext_info.name=str

            #判断栈是否溢出
            obj = parse_memory_2_class(tcb.topOfStack,c_uint,sizeof(c_uint))
            if obj.value==0xCCCCCCCC:
                ext_info.over=0
            else:
                ext_info.over=1

            #计算栈峰值
            addr = tcb.topOfStack+4
            while addr < ext_info.buttom:
                obj = parse_memory_2_class(addr,c_uint,sizeof(c_uint))
                if obj.value != 0xCACACACA:
                    break
                addr+=4
            ext_info.peek_used = ext_info.buttom-addr

            enter_name=parse_addr_2_function_name(tcb.taskEntry)
            if enter_name!=None:
                ext_info.enter_name = enter_name
            if self.version == 207:
                ext_info.task_context=parse_memory_2_class(tcb.stackPointer,TaskContext,sizeof(TaskContext))
            elif self.version == 208:
                ext_info.task_context=parse_memory_2_class(tcb.stackPointer,TaskContext,sizeof(TaskContext))
            else:
                ext_info.task_context=parse_memory_2_class(tcb.stackPointer,td_u32,sizeof(td_u32))
            i=i+1

    def print_task_short_info(self):
        print("\r\n--------------------content of 任务信息摘要:--------------------",file=self.info_fd)
        print('%4s|%20s|%4s|%10s|%10s|%s|%10s|%10s|%10s|%4s|%15s|'%('id','task_name','pri','stackSize','topOfStack','ButtomOfStack','sp','cur_used','peak_used','ovf','status'),file=self.info_fd)
        i=0
        while i < self.task_cnt:
            tcb = self.tcb_array[i]
            ext_info = self.ext_task_list[i]
            if not tcb.taskStatus & OS_TASK_STATUS_UNUSED:
                print('%4d|%20s|%4d|0x%08x|0x%08x|0x%08x   |0x%08x|0x%08x|0x%08x|%4d|%15s|'
                    %(i,ext_info.name,tcb.priority,tcb.stackSize,tcb.topOfStack,ext_info.buttom,tcb.stackPointer,0,ext_info.peek_used,ext_info.over,ext_info.status_str),file=self.info_fd)
            i=i+1

    def print_task_detail_info(self):
        print("\r\n--------------------content of 任务详细信息:--------------------",file=self.info_fd)
        print("说明:tcb任务控制块信息,context:任务上下文的寄存器信息,backtrace 栈回溯",file=self.info_fd)
        i=0
        while i < self.task_cnt:
            print('',file=self.info_fd)
            tcb = self.tcb_array[i]
            uapi_print_ctypes_obj('tcb',tcb,self.info_fd)
            ext_info = self.ext_task_list[i]
            i=i+1
            if tcb.taskStatus & OS_TASK_STATUS_UNUSED:
                continue
            #打印寄存器信息
            print("[TASK]%s 寄存器:"%(ext_info.name),file=self.info_fd)
            uapi_print_ctypes_obj('context',ext_info.task_context,self.info_fd)

            #打印back_trace
            print("[TASK]%s 栈回溯:"%(ext_info.name),file=self.info_fd)
            if self.version == 207:
                sp = tcb.stackPointer+sizeof(TaskContext)
            elif self.version == 208:
                sp = tcb.stackPointer+sizeof(TaskContext)
            else:
                sp = tcb.stackPointer
            self.stack_back_trace(sp,tcb.topOfStack + tcb.stackSize)
            print("[TASK]%s 其它状态:"%(ext_info.name),file=self.info_fd)
            if ext_info.take_sem:
                print("[waitting_sem][id=0x%08x]"%(ext_info.take_sem.semId),file=self.info_fd)

    def make_heap_info(self):
        addr = parse_get_symbol_addr('m_aucSysMem0')
        obj=parse_memory_2_class(addr,c_uint,4)
        self.heap_start=obj.value
        if self.version == 207:
            self.heap_head=parse_memory_2_class(self.heap_start,LosMemPoolInfo,sizeof(LosMemPoolInfo))
            if self.heap_head:
                self.heap_end = self.heap_start + self.heap_head.size
        elif self.version == 208:
            self.heap_head=parse_memory_2_class(self.heap_start,LosMemPoolInfo,sizeof(LosMemPoolInfo))
            if self.heap_head:
                self.heap_end = self.heap_start + self.heap_head.size


    def print_heap_info(self):
        print("--------------------content of 内存堆信息:--------------------",file=self.info_fd)
        self.print_heap_head_info()
        self.print_heap_from_head()
        self.print_heap_from_tail()

    def print_heap_head_info(self):
        pass

    def print_heap_from_head(self):
        print("从头遍历内存池:",file=self.info_fd)
        print("说明:node内存节点地址,usr用户空间地址(申请对齐内存场景不准),size内存大小,used是否占用",file=self.info_fd)
        if self.heap_head:
            addr = self.heap_head.head
            while addr >= self.heap_start and addr <= self.heap_end:
                node = parse_memory_2_class(addr,LosHeapNode,sizeof(LosHeapNode))
                if(node.size==0):
                    break;
                str="[node=0x%x][usr=0x%x][prev=0x%x][size=0x%x][used=%d]"%(addr,addr+8,node.prev,node.size,node.used)
                if hasattr(LosHeapNode,'lr'):
                    i=0
                    while i< sizeof(node.lr)/sizeof(c_uint):
                        str="%s[%s][0x%x]"%(str,_addr_in_2_function_name(node.lr[i]),node.lr[i])
                        i=i+1
                print(str,file=self.info_fd)
                addr=addr+sizeof(LosHeapNode)+node.size

    def print_heap_from_tail(self):
        print("从尾遍历内存池:",file=self.info_fd)
        print("说明:node内存节点地址,usr用户空间地址(申请对齐内存场景不准),size内存大小,used是否占用",file=self.info_fd)
        if self.heap_head:
            addr = self.heap_head.tail
            while addr >= self.heap_start and addr <= self.heap_end:
                node = parse_memory_2_class(addr,LosHeapNode,sizeof(LosHeapNode))
                str="[node][addr=0x%x][usr=0x%x][prev=0x%x][size=0x%x][used=%d]"%(addr,addr+8,node.prev,node.size,node.used)
                print(str,file=self.info_fd)
                if addr==self.heap_head.head:
                    break
                addr=node.prev

    def make_queue_info(self):
        addr = parse_get_u32_symbol_val('g_osAllQueue')
        i=0
        while i< self.queue_cnt:
            ext_info=self._ExtQueueInfo()
            obj=parse_memory_2_class(addr,LosQueueCB,sizeof(LosQueueCB))
            ext_info.handle_addr=addr
            self.queue_array.append(obj)
            self.queue_ext_info.append(ext_info)
            addr=addr+sizeof(LosQueueCB)
            i=i+1
        self._make_queue_ext_info()

    def _make_queue_ext_info(self):
        i=0
        while i< self.queue_cnt:
            queue=self.queue_array[i]
            ext_info=self.queue_ext_info[i]
            if queue and queue.queueState==1: #used
                if self.version == 207:
                    (ret,addr_list)=_los_List_get_all_node(ext_info.handle_addr + ctype_member_offset('readWriteList',LosQueueCB))
                elif self.version == 208:
                    (ret,addr_list)=_los_List_get_all_node(ext_info.handle_addr + ctype_member_offset('readWriteList',LosQueueCB))
                else:
                    (ret,addr_list)=_los_List_get_all_node(ext_info.handle_addr + ctype_member_offset('readWriteList',LosQueueCB) + 4)
                for node_addr in addr_list:
                    ext_info.pend_read_task_list.append(node_addr)
                if self.version == 207:
                    (ret,addr_list)=_los_List_get_all_node(ext_info.handle_addr + ctype_member_offset('readWriteList',LosQueueCB)+sizeof(LOS_DL_LIST))
                elif self.version == 208:
                    (ret,addr_list)=_los_List_get_all_node(ext_info.handle_addr + ctype_member_offset('readWriteList',LosQueueCB)+sizeof(LOS_DL_LIST))
                else:
                    (ret,addr_list)=_los_List_get_all_node(ext_info.handle_addr + ctype_member_offset('readWriteList',LosQueueCB) + 4 + sizeof(LOS_DL_LIST))
                for node_addr in addr_list:
                    ext_info.pend_write_task_list.append(node_addr)
            i=i+1

    def print_queue_info(self):
        print("\r\n--------------------content of 消息队列信息:--------------------",file=self.info_fd)
        print("说明:queueHandle队列的内存空间,queueLen队列长度,queueSize单个节点大小,queueId队列id,readWriteableCnt[0]当前队列中有几个数据,readWriteableCnt[1]当前队列中有几个剩余空间",file=self.info_fd)
        print("",file=self.info_fd)
        i=0
        while i< self.queue_cnt:
            queue=self.queue_array[i]
            ext_info=self.queue_ext_info[i]
            if queue.queueState==1:
                uapi_print_ctypes_obj('queue',queue,self.info_fd)
                list=self.get_pending_task(ext_info.pend_read_task_list)
                print("等待读该消息队列的任务ID:",list,file=self.info_fd)
                list=self.get_pending_task(ext_info.pend_write_task_list)
                print("等待写该消息队列的任务ID:",list,file=self.info_fd)
                print("",file=self.info_fd)
            i=i+1

    def _make_timer_ext_info(self):
        i=0
        while i< self.timer_cnt:
            timer=self.timer_array[i]
            ext_info=self.timer_ext_info[i]
            if timer.state==OS_SWTMR_STATUS_UNUSED or timer.state==OS_SWTMR_STATUS_DELETING:
                i=i+1
                continue
            ext_info.func_name = parse_addr_2_function_name(timer.handler)
            i=i+1

    def make_timer_info(self):
        addr = parse_get_u32_symbol_val('g_osSwtmrCBArray')
        i=0
        while i< self.timer_cnt:
            obj=parse_memory_2_class(addr,LosSwtmrCB,sizeof(LosSwtmrCB))
            ext_info=self._ExtTimerInfo()
            self.timer_ext_info.append(ext_info)
            self.timer_array.append(obj)
            ext_info.handle_addr=addr

            addr=addr+sizeof(LosSwtmrCB)
            i=i+1
        self._make_timer_ext_info()

    def print_timer_info(self):
        print("--------------------content of 定时器信息:--------------------",file=self.info_fd)
        print("说明:timer_id定时器id,handler回调函数的地址(已解析),state(0:未使用 1:删除 2:创建 3:执行中),mode(1:周期 2:单次)",file=self.info_fd)
        print("\t overrun周期性定时器执行次数,arg用户参数,inProcess到期已添加到待执行队列",file=self.info_fd)
        print('',file=self.info_fd)
        i=0
        while i< self.timer_cnt:
            timer=self.timer_array[i]
            ext_info=self.timer_ext_info[i]
            if timer.state==OS_SWTMR_STATUS_UNUSED or timer.state==OS_SWTMR_STATUS_DELETING:
                i=i+1
                continue
            print("timer_id=%d %s[0x%x]"%(timer.timerId,ext_info.func_name,timer.handler),file=self.info_fd)
            uapi_print_ctypes_obj('timer',timer,self.info_fd)
            print('',file=self.info_fd)
            i=i+1

    def _make_sem_extra_info(self):
        i=0
        while i< self.sem_cnt:
            sem=self.sem_array[i]
            ext_info=self.sem_ext_info[i]
            if sem.semStat==1: #used
                (ret,addr_list)=_los_List_get_all_node(ext_info.handle_addr + ctype_member_offset('semList',LosSemCB))
                for node_addr in addr_list:
                    ext_info.pend_task_list.append(node_addr)
            i=i+1

    def make_sem_info(self):
        addr = parse_get_u32_symbol_val('g_osAllSem')
        i=0
        while i< self.sem_cnt:
            ext_info=self._ExtSemInfo()
            obj=parse_memory_2_class(addr,LosSemCB,sizeof(LosSemCB))
            ext_info.handle_addr=addr
            self.sem_array.append(obj)
            self.sem_ext_info.append(ext_info)
            addr=addr+sizeof(LosSemCB)
            i=i+1
        self._make_sem_extra_info()

    def get_pending_task(self,addr_list):
        list=[]
        for addr in addr_list:
            i=0
            while i < self.task_cnt:
                ext_task = self.ext_task_list[i]
                tcb = self.tcb_array[i]
                if addr== ext_task.pend_addr:
                    list.append(tcb.taskId)
                i=i+1
        return list

    def print_sem_info(self):
        print("--------------------content of 信号量:--------------------",file=self.info_fd)
        print("说明:semType(0:计数信号量 1:互斥信号量),semCount剩余信号量个数",file=self.info_fd)
        print("",file=self.info_fd)
        i=0
        while i< self.sem_cnt:
            sem=self.sem_array[i]
            ext_info=self.sem_ext_info[i]
            if sem.semStat:
                uapi_print_ctypes_obj('sem',sem,self.info_fd)
                list=self.get_pending_task(ext_info.pend_task_list)
                print("等待该信号量的任务ID:",list,file=self.info_fd)
                print('',file=self.info_fd)
            i=i+1

    def make_crash_info(self):
        addr = parse_get_symbol_addr('g_excInfo')
        self.excInfo = parse_memory_2_class(addr,ExcInfo,sizeof(ExcInfo))

    def print_crash_info(self):
        print("--------------------content of 死机信息:--------------------",file=self.info_fd)
        uapi_print_ctypes_obj('g_excInfo',self.excInfo,self.info_fd)
        if self.excInfo.context!=0:
            obj=parse_memory_2_class(self.excInfo.context,ExcContext,sizeof(ExcContext))
            uapi_print_ctypes_obj('context',obj,self.info_fd)
            if self.version == 207:
                self.stack_back_trace(obj.taskContext.sp+sizeof(TaskContext),obj.taskContext.sp+0x1000)
            elif self.version == 208:
                self.stack_back_trace(obj.taskContext.sp+sizeof(TaskContext),obj.taskContext.sp+0x1000)
            else:
                self.stack_back_trace(obj.SP, obj.SP + 0x1000)



    def stack_back_trace(self,start_addr,end_addr):
        sp=start_addr
        while sp < end_addr:
            obj=parse_memory_2_class(sp,c_uint,sizeof(c_uint))
            if obj:
                name = parse_addr_in_2_function_name(obj.value-4)
                if name:
                    print("[BACK Trace][addr=0x%08x][val=0x%08x]%s"%(sp,obj.value,name),file=self.info_fd)
                else:
                    pass
                    #print("[BACK Trace][addr=0x%08x][val=0x%08x]"%(sp,obj.value),file=self.info_fd)

            sp=sp+4

_g_parse_liteos_ctrl=_PhaseLiteosCtrl()
def make_liteos_info():
    try:
        _g_parse_liteos_ctrl.make_per_cpu_info()
    except:
        print("[EXCEPTION]make_per_cpu_info fail.")
    try:
        _g_parse_liteos_ctrl.make_task_info()
    except:
        print("[EXCEPTION]make_task_info fail.")
    if _g_parse_liteos_ctrl.version != 208:
        try:
            _g_parse_liteos_ctrl.make_queue_info()
        except:
            print("[EXCEPTION]make_queue_info fail.")
        try:
            _g_parse_liteos_ctrl.make_heap_info()
        except:
            print("[EXCEPTION]make_heap_info fail.")
        try:
            _g_parse_liteos_ctrl.make_timer_info()
        except:
            print("[EXCEPTION]make_timer_info fail.")
    try:
        _g_parse_liteos_ctrl.make_sem_info()
    except:
        print("[EXCEPTION]make_sem_info fail.")
    try:
        _g_parse_liteos_ctrl.make_crash_info()
    except:
        print("[EXCEPTION]make_crash_info fail.")
    try:
        _g_parse_liteos_ctrl.make_isr_info()
    except:
        print("[EXCEPTION]make_isr_info fail.")

def print_liteos_info():
    try:
        _g_parse_liteos_ctrl.print_isr_info()
    except:
        print("[EXCEPTION]print_isr_info fail.")
    try:
        _g_parse_liteos_ctrl.print_per_cpu_info()
    except:
        print("[EXCEPTION]print_per_cpu_info fail.")
    if _g_parse_liteos_ctrl.version != 208:
        try:
            _g_parse_liteos_ctrl.print_queue_info()
        except:
            print("[EXCEPTION]print_queue_info fail.")
        try:
            _g_parse_liteos_ctrl.print_timer_info()
        except:
            print("[EXCEPTION]print_timer_info fail.")
        try:
            _g_parse_liteos_ctrl.print_heap_info()
        except:
            print("[EXCEPTION]print_heap_info fail.")
    try:
        _g_parse_liteos_ctrl.print_sem_info()
    except:
        print("[EXCEPTION]print_sem_info fail.")
    try:
        _g_parse_liteos_ctrl.print_task_short_info()
    except:
        print("[EXCEPTION]print_task_short_info fail.")
    try:
        _g_parse_liteos_ctrl.print_task_detail_info()
    except:
        print("[EXCEPTION]print_task_detail_info fail.")
    try:
        _g_parse_liteos_ctrl.print_crash_info()
    except:
        print("[EXCEPTION]print_crash_info fail.")

def parse_liteos206_info(log_fp):
    _g_parse_liteos_ctrl.info_fd = log_fp
    _g_parse_liteos_ctrl.version = 206
    make_liteos_info()
    print_liteos_info()

def parse_liteos207_info(log_fp):
    _g_parse_liteos_ctrl.info_fd = log_fp
    _g_parse_liteos_ctrl.version = 207
    make_liteos_info()
    print_liteos_info()

def parse_liteos208_info(log_fp):
    _g_parse_liteos_ctrl.info_fd = log_fp
    _g_parse_liteos_ctrl.version = 208
    make_liteos_info()
    print_liteos_info()

'''
context.mstatus(mcause)=0x80207800
context.mepc=0x12530
context.tp=0xcacacaca
context.sp=0xcacacaca
context.s11=0x4040404--未压栈(中断场景)
context.s10=0x5050505--未压栈
context.s9=0x6060606--未压栈
context.s8=0x7070707--未压栈
context.s7=0x8080808--未压栈
context.s6=0x9090909--未压栈
context.s5=0x80206088--未压栈
context.s4=0x20008000--未压栈
context.s3=0x80206080--未压栈
context.s2=0x200079dc--未压栈
context.s1=0x0--未压栈
context.s0=0x20007a74--未压栈

context.t6=0x4040404
context.t5=0x5050505
context.t4=0x6060606
context.t3=0x7070707
context.a7=0x8080808
context.a6=0x9090909
context.a5=0xa0a0a0a
context.a4=0xb0b0b0b
context.a3=0xc0c0c0c
context.a2=0xd0d0d0d
context.a1=0xe0e0e0e
context.a0=0xf0f0f0f
context.t2=0x10101010
context.t1=0x11111111
context.t0=0x12121212
context.ra=0x13131313

context.fs11=0x20202020--(进中断)未压栈
context.fs10=0x21212121--未压栈
context.fs9=0x22222222--未压栈
context.fs8=0x23232323--未压栈
context.fs7=0x24242424--未压栈
context.fs6=0x25252525--未压栈
context.fs5=0x26262626--未压栈
context.fs4=0x27272727--未压栈
context.fs3=0x28282828--未压栈
context.fs2=0x29292929--未压栈
context.fs1=0x2a2a2a2a--未压栈
context.fs0=0x2b2b2b2b--未压栈

context.ft11=0x20202020
context.ft10=0x21212121
context.ft9=0x22222222
context.ft8=0x23232323
context.fa7=0x24242424
context.fa6=0x25252525
context.fa5=0x26262626
context.fa4=0x27272727
context.fa3=0x28282828
context.fa2=0x80206088
context.fa1=0x20008000
context.fa0=0x80206080
context.ft7=0x20009000
context.ft6=0x20007ad4
context.ft5=0x20007ad4
context.ft4=0x11f5a
context.ft3=0x32
context.ft2=0x20007ad4
context.ft1=0x20007a70
context.ft0=0x117a2
context.fcsr=0x0
context.reserved[0]=0x0
context.reserved[1]=0x20007a74
context.reserved[2]=0x12530


typedef struct {
    uint32_t      ccause;
    uint32_t      mcause;
    uint32_t      mtval;
    uint32_t      gp;
    TaskContext taskContext;
} ExcContext;

typedef struct {
    uint16_t phase;           /**< Exception occurrence phase: 0 indicates that the exception occurs during
                             *   initialization, 1 indicates that the exception occurs during task, 2
                             *   indicates that the exception occurs during interrupt, and 3 indicates that
                             *   the exception occurs during exception.
                             */
    uint16_t type;            /**< Type of exception, refer to no. 1-19 listed above for exceptions */
    uint32_t faultAddr;       /**< A precise address access error indicates the error access address where
                             *   the exception occurred.
                             */
    uint32_t thrdPid;         /**< An exception occurs in the interrupt, indicating the interrupt number. An
                             *   exception occurs in the task, indicating the task id, or 0xffffffff if it
                             *   occurs during initialization
                             */
    uint16_t nestCnt;         /**< The number of nested exceptions, currently only support the first time the
                             *   exception into the implementation of the registered hook function
                             */
    uint16_t reserved;        /**< Reserved */
    ExcContext *context; /**< The hardware context at which an exception to an automatic stack floating point
                             *   register occurs
                             */
} ExcInfo;
'''
