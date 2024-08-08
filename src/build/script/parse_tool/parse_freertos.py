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
queueQUEUE_TYPE_BASE=0
queueQUEUE_TYPE_SET=0
queueQUEUE_TYPE_MUTEX=1
queueQUEUE_TYPE_COUNTING_SEMAPHORE=2
queueQUEUE_TYPE_BINARY_SEMAPHORE=3
queueQUEUE_TYPE_RECURSIVE_MUTEX=4
CONTEXT_OFFSET=4 * 4 + 33 * 4
#ctypes.addressof(parameter) - ctypes.addressof(parent_structure)


def _los_List_get_all_node(list_addr):
    node_list=[]
    node=parse_memory_2_class(list_addr,LOS_DL_LIST,sizeof(LOS_DL_LIST))
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

class _PhaseFreertosCtrl:
    def __init__(self):
        self.core=None
        self.info_fd = sys.__stdout__
        self.task_cnt=None
        self.tcb_array = []
        self.ext_task_list=[]
        self.ext_taskstatus_list=[]  ##freertos

        self.heap_start = None
        self.heap_end = None
        self.heap_head = None
        self.queue_cnt = configQUEUE_REGISTRY_SIZE
        self.queue_array=[]
        self.queue_ext_info=[]
        self.timer_cnt = 0
        self.timer_array=[]
        self.timer_ext_info=[]
        #self.sem_cnt=LOSCFG_BASE_IPC_SEM_LIMIT
        #self.sem_array=[]
        self.sem_ext_info=[]

        self.excInfo=None

        self.isr_ext_info=None

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

    class _ExtTaskStatus:
        def __init__(self):
            self.xHandle=None
            self.pcTaskName='unknown'
            self.xTaskNumber=0
            self.eCurrentState=0
            self.uxCurrentPriority=0
            self.uxBasePriority=0
            self.ulRunTimeCounter=0
            self.pxStackBase = None
            self.usStackHighWaterMark = 0

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
            self.isr_cnt = configHWI_MAX_VECTOR_CNT
            self.pxAddr=None
            self.pxHwiHandles_list = []

    def make_isr_info(self):
        self.isr_ext_info = self.ExtIsrInfo()
        addr = parse_get_symbol_addr('pxHwiHandles')
        self.isr_ext_info.pxAddr = parse_memory_2_class(addr,(uint32_t)*self.isr_ext_info.isr_cnt,sizeof((uint32_t)*self.isr_ext_info.isr_cnt))
        for temp_addr in self.isr_ext_info.pxAddr:
            if temp_addr!=0:
                pxHwiHandles = parse_memory_2_class(temp_addr, HwiHandle_t, sizeof(HwiHandle_t))
                self.isr_ext_info.pxHwiHandles_list.append(pxHwiHandles)


    def print_isr_info(self):
        print("\r\n---------------------content of 中断信息:---------------------",file=self.info_fd)
        for hwi in self.isr_ext_info.pxHwiHandles_list:
            if hwi.pxCb!=0 or hwi.xCounts!=0:
                print("%s[0x%x][call_cnt=%d]"%(_addr_in_2_function_name(hwi.pxCb),hwi.pxCb,hwi.xCounts),file=self.info_fd)
        print("",file=self.info_fd)

    def _make_task_info(self,pxList):
        pxCurrentTCB = parse_get_symbol_addr('pxCurrentTCB')
        if pxList.uxNumberOfItems > 0:
            num = 0
            while num < pxList.uxNumberOfItems:
                next_item = parse_memory_2_class(pxList.pxIndex, ListItem_t, sizeof(ListItem_t))
                if not next_item:
                    break
                pxList.pxIndex = next_item.pxNext
                pxTCB = parse_memory_2_class(next_item.pvOwner, TCB_t, sizeof(TCB_t))
                if pxTCB:
                    taskstatus = self._ExtTaskStatus()
                    taskstatus.xHandle = next_item.pvOwner
                    taskstatus.pcTaskName = pxTCB.pcTaskName
                    taskstatus.xTaskNumber = pxTCB.uxTCBNumber
                    if (next_item.pvOwner == pxCurrentTCB):
                        taskstatus.eCurrentState = 0
                    else:
                        taskstatus.eCurrentState = 1
                    taskstatus.uxCurrentPriority = pxTCB.uxPriority
                    taskstatus.uxBasePriority = pxTCB.uxBasePriority
                    #taskstatus.ulRunTimeCounter = pxTCB.ulRunTimeCounter
                    taskstatus.pxStackBase = pxTCB.pxStack
                    self.ext_taskstatus_list.append(taskstatus)
                    self.tcb_array.append(pxTCB)
                num = num + 1

    def make_all_task_info(self):
        addr = parse_get_symbol_addr('uxCurrentNumberOfTasks')
        obj=parse_memory_2_class(addr,c_uint,4)
        self.task_cnt = obj.value
        addr = parse_get_symbol_addr('pxReadyTasksLists')
        pxReadyLists=parse_memory_2_class(addr, List_t*configMAX_PRIORITIES, sizeof(List_t)*configMAX_PRIORITIES)
        for pxList in pxReadyLists:
            self._make_task_info(pxList)
        TaskListName = ['xDelayedTaskList1', 'xDelayedTaskList2', 'xPendingReadyList', 'xTasksWaitingTermination', 'xSuspendedTaskList']
        for tasklist_name in TaskListName:
            addr = parse_get_symbol_addr(tasklist_name)
            pxList = parse_memory_2_class(addr, List_t, sizeof(List_t))
            self._make_task_info(pxList)



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

            ext_info.task_context=parse_memory_2_class(tcb.stackPointer,TaskContext,sizeof(TaskContext))
            i=i+1

    def print_task_short_info(self):
        print("\r\n--------------------content of 任务信息摘要:--------------------",file=self.info_fd)
        print('%4s|%20s|%4s|%10s|%10s|%s|%10s'%('xHandle','TaskName','xTaskNumber','eCurrentState','CurrentPriority','BasePriority','StackBase'),file=self.info_fd)
        for taskstatus in self.ext_taskstatus_list:
            print('0x%08x|%20s|%4d|%4d|0x%08x|0x%08x|0x%08x'
                    %(taskstatus.xHandle,taskstatus.pcTaskName,taskstatus.xTaskNumber,taskstatus.eCurrentState,taskstatus.uxCurrentPriority,taskstatus.uxBasePriority,taskstatus.pxStackBase),file=self.info_fd)

    def print_task_detail_info(self):
        print("\r\n--------------------content of 任务详细信息:--------------------",file=self.info_fd)
        print("说明:tcb任务控制块信息,context:任务上下文的寄存器信息,backtrace 栈回溯",file=self.info_fd)
        for tcb in self.tcb_array:
            print('',file=self.info_fd)
            uapi_print_ctypes_obj('tcb',tcb,self.info_fd)
            ext_info = parse_memory_2_class(tcb.pxTopOfStack+CONTEXT_OFFSET,exc_context_freertos_t,sizeof(exc_context_freertos_t))
            #打印寄存器信息
            print("[TASK]%s 寄存器:"%(tcb.pcTaskName),file=self.info_fd)
            uapi_print_ctypes_obj('context',ext_info,self.info_fd)

            #打印back_trace
            print("[TASK]%s back trace..."%(tcb.pcTaskName))
            print("[TASK]%s 栈回溯:"%(tcb.pcTaskName),file=self.info_fd)
            sp = tcb.pxTopOfStack+CONTEXT_OFFSET
            self.stack_back_trace(tcb.pxStack, sp)
            #print("[TASK]%s 其它状态:"%(ext_info.name),file=self.info_fd)


    def make_queue_info(self):
        addr = parse_get_symbol_addr('xQueueRegistry')
        i=0
        while i< self.queue_cnt:
            obj=parse_memory_2_class(addr,QueueRegistryItem_t,sizeof(QueueRegistryItem_t))
            if obj:
                queue_item=parse_memory_2_class(obj.xHandle,Queue_t,sizeof(Queue_t))
                self.queue_array.append(queue_item)
            addr=addr+sizeof(QueueRegistryItem_t)
            i=i+1


    def print_queue_info(self):
        print("\r\n--------------------content of 消息队列信息:--------------------",file=self.info_fd)
        print("说明:queueName队列名字,queueHandle队列的内存空间, queueType队列类型, queueLen队列长度, queueItemSize每个节点大小，queueUsed已用节点个数, queueAvailable剩余节点个数",file=self.info_fd)
        i=0
        for queue in self.queue_array:
            if queue!=None:
                if queue.ucQueueType == queueQUEUE_TYPE_BASE:
                    uapi_print_ctypes_obj('queue',queue,self.info_fd)
                    print("",file=self.info_fd)
            i=i+1
        print("",file=self.info_fd)

    def make_timer_info(self):
        pxaddr = parse_get_u32_symbol_val('pxCurrentTimerList')
        addrList = parse_memory_2_class(pxaddr, uint32_t, sizeof(uint32_t))
        if addrList:
            pxList=parse_memory_2_class(addrList.value, List_t, sizeof(List_t))
            if pxList != None:
                if pxList.uxNumberOfItems > 0:
                    while self.timer_cnt < pxList.uxNumberOfItems:
                        next_item = parse_memory_2_class(pxList.pxIndex, ListItem_t, sizeof(ListItem_t))
                        pxList.pxIndex = next_item.pxNext
                        obj = parse_memory_2_class(next_item.pvOwner, Timer_t, sizeof(Timer_t))
                        self.timer_array.append(obj)
                        self.timer_cnt = self.timer_cnt + 1


    def print_timer_info(self):
        print("---------------------content of 定时器信息:---------------------",file=self.info_fd)
        print("说明:SwTmr_Used定时器个数；SwTmr_Total定时器个数上限；timer_id定时器id；pxCallbackFunction回调函数的地址；xTimerPeriodInTicks调用周期；ucStatus(1:ACTIVE 2:STATICALLY_ALLOCATED 3:AUTORELOAD)",file=self.info_fd)
        print("SwTmr_Used=%d, SwTmr_Total=unlimited"%(self.timer_cnt),file=self.info_fd)
        i=0
        while i< self.timer_cnt:
            timer=self.timer_array[i]
            print("timer_id=%d %s[0x%x]"%(timer.pvTimerID,_addr_in_2_function_name(timer.pxCallbackFunction),timer.pxCallbackFunction),file=self.info_fd)
            uapi_print_ctypes_obj('timer',timer,self.info_fd)
            print('',file=self.info_fd)
            i=i+1
        print("",file=self.info_fd)

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
        print("说明:SemName信号量名字,SemHandle信号量的地址, MaxCount信号量最大个数, Count已用个数",file=self.info_fd)
        i=0
        for queue in self.queue_array:
            if queue!=None:
                if queue.ucQueueType==queueQUEUE_TYPE_COUNTING_SEMAPHORE or queue.ucQueueType==queueQUEUE_TYPE_BINARY_SEMAPHORE or queue.ucQueueType==queueQUEUE_TYPE_RECURSIVE_MUTEX:
                    uapi_print_ctypes_obj('Sem',queue,self.info_fd)
                    print("",file=self.info_fd)
            i=i+1
        print("",file=self.info_fd)

    def make_crash_info(self):
        addr = parse_get_symbol_addr('g_exc_buff_addr')
        self.excInfo = parse_memory_2_class(addr,exc_context_t,sizeof(exc_context_t))

    def print_crash_info(self):
        print("--------------------content of 死机信息:--------------------",file=self.info_fd)
        uapi_print_ctypes_obj('g_exc_buff_addr',self.excInfo,self.info_fd)
        self.stack_back_trace(self.excInfo.task_context.sp+sizeof(task_context_t),self.excInfo.task_context.sp+0x1000)

    def stack_back_trace(self,start_addr,end_addr):
        sp=start_addr
        while sp < end_addr:
            obj=parse_memory_2_class(sp,c_uint,sizeof(c_uint))
            name = parse_addr_in_2_function_name(obj.value-4)
            if name:
                print("[BACK Trace][addr=0x%08x][val=0x%08x]%s"%(sp,obj.value,name),file=self.info_fd)
            else:
                pass
                #print("[BACK Trace][addr=0x%08x][val=0x%08x]"%(sp,obj.value),file=self.info_fd)

            sp=sp+4

_g_parse_freertos_ctrl=_PhaseFreertosCtrl()
def make_freertos_info():
    try:
        _g_parse_freertos_ctrl.make_all_task_info()
    except:
        print("[EXCEPTION]make_all_task_info fail.")
    try:
        _g_parse_freertos_ctrl.make_queue_info()
    except:
        print("[EXCEPTION]make_queue_info fail.")
    try:
        _g_parse_freertos_ctrl.make_timer_info()
    except:
        print("[EXCEPTION]make_timer_info fail.")
    try:
        _g_parse_freertos_ctrl.make_crash_info()
    except:
        print("[EXCEPTION]make_crash_info fail.")
    try:
        _g_parse_freertos_ctrl.make_isr_info()
    except:
        print("[EXCEPTION]make_isr_info fail.")

def print_freertos_info():
    try:
        _g_parse_freertos_ctrl.print_isr_info()
    except:
        print("[EXCEPTION]print_isr_info fail.")
    try:
        _g_parse_freertos_ctrl.print_queue_info()
    except:
        print("[EXCEPTION]print_queue_info fail.")
    try:
        _g_parse_freertos_ctrl.print_timer_info()
    except:
        print("[EXCEPTION]print_timer_info fail.")
    try:
        _g_parse_freertos_ctrl.print_sem_info()
    except:
        print("[EXCEPTION]print_sem_info fail.")
    try:
        _g_parse_freertos_ctrl.print_task_short_info()
    except:
        print("[EXCEPTION]print_task_short_info fail.")
    try:
        _g_parse_freertos_ctrl.print_task_detail_info()
    except:
        print("[EXCEPTION]print_task_detail_info fail.")
    try:
        _g_parse_freertos_ctrl.print_crash_info()
    except:
        print("[EXCEPTION]print_crash_info fail.")

def parse_freertos_info(log_fp):
    _g_parse_freertos_ctrl.info_fd = log_fp
    make_freertos_info()
    print_freertos_info()
    print("parse_freertos_info end!")
