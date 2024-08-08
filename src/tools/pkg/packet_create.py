#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Packing Scripts file
# ============================================================================

import struct
import sys
import os
import hashlib
# only use for nbiot

class crc16:
    POLYNOMIAL = 0x1021
    PRESET = 0x0000
    _tab = []
    def __init__(self):
        self._tab = [self._initial(i) for i in range(256)]

    def _initial(self, c):
        crc = 0
        c = c << 8
        for j in range(8):
            if (crc ^ c) & 0x8000:
                crc = (crc << 1) ^ self.POLYNOMIAL
            else:
                crc = crc << 1
            c = c << 1
        return crc

    def _update_crc(self, crc, c):
        cc = 0xff & int(c)

        tmp = (crc >> 8) ^ cc
        crc = (crc << 8) ^ self._tab[tmp & 0xff]
        crc = crc & 0xffff

        return crc

    def crc(self, str):
        crc = self.PRESET
        for c in str:
            crc = self._update_crc(crc, ord(c))
        return crc

    def crcb(self, i):
        crc = self.PRESET
        for c in i:
            crc = self._update_crc(crc, c)
        return crc

t = crc16()
ignoreFlag = 1
def parse_param(pathNameListNew, burnAddrListNew, burnSizeListNew, imageSizeListNew, typeListNew, inputList):
    for item in inputList:
        if '|' in item:
            path, burnAddr, burnSize, type = item.split("|")
            if ignoreFlag == 1 and not os.path.exists(path):
                continue
            imageSize = os.path.getsize(path)
            pathNameListNew.append(path)
            burnAddrListNew.append(int(burnAddr, 16))
            burnSizeListNew.append(int(burnSize, 16))
            imageSizeListNew.append(imageSize)
            typeListNew.append(int(type))
        else:
            pathNameListNew.append(item)
            imageSize = os.path.getsize(item)
            imageSizeListNew.append(imageSize)

def create_sha_file(source, target):
    for src in source:
        with open(src, "rb" ) as bin_file:
            sha = hashlib.sha256( bin_file.read() )
            bin_file.close()
            with open( str(target), "wb") as sha_file:
                sha_file.write( sha.digest())

def create_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, fileInfoList, outputPath):
    flag = 0xefbeaddf
    crc = 0
    imageNum = len(pathNameList)
    # sizeof(IMAGE_INFO) is 52, sizeof(FWPKG_HEAD) is 12
    headLen = imageNum * 52 + 12
    # each image has 16byte 0 in the end
    padLen = 16 * imageNum
    totalFileSize = sum(imageSizeList) + headLen + padLen
    # prepare
    outputRoot = os.path.dirname(outputPath)
    if not os.path.isdir(outputRoot):
        os.makedirs(outputRoot)
    with open(outputPath, 'wb+') as file:
        file.write(struct.pack('IHHI', flag, crc, imageNum, totalFileSize))
        startIndex = headLen
        times = 0
        for path in pathNameList:
            pathName = os.path.basename(path)
            file.write(
                struct.pack('32sIIIII', bytes(pathName, 'ascii'), startIndex, imageSizeList[times], burnAddrList[times],
                            burnSizeList[times], typeList[times]))
            # separate each image with 16 bytes 0
            startIndex = startIndex + imageSizeList[times] + 16
            times += 1

        for info in fileInfoList:
            file.write(info)
            file.write(struct.pack('IIII', 0, 0, 0, 0))

        file.flush()
        # crc range is FWPKG_HEAD.imageNum to end, begin index is 6bytes
        file.seek(6)
        newdata = file.read(headLen - 6)
        crc16 = t.crcb(newdata)
        file.seek(4)
        file.write(struct.pack('H', crc16))
        file.close()
def parse_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, startIndexList, fileInfoList, packet_path):
    with open(packet_path, 'rb+') as file:
        info = file.read(struct.calcsize('IHHI'))
        flag, crc, imageNum, totalFileSize = struct.unpack('IHHI', info)
        times = 0
        for index in range(0, imageNum):
            info = file.read(struct.calcsize('32sIIIII'))
            pathName, startIndex, imageSize, burnAddr, burnSize, type2 = struct.unpack('32sIIIII', info)
            #separate each image with 16 bytes 0
            pathName = str(pathName, encoding="utf-8")
            pathNameNew = ''
            for char in pathName:
                if char != '\x00':
                    pathNameNew += char
            print(pathNameNew)
            pathNameList.append(pathNameNew)
            startIndexList.append(startIndex)
            burnAddrList.append(burnAddr)
            burnSizeList.append(burnSize)
            imageSizeList.append(imageSize)
            typeList.append(type2)

        for index in range(0, imageNum):
            file.seek(startIndexList[index])
            fileInfoList.append(file.read(imageSizeList[index]))
        file.close()

def packet_bin(outputPath, inputList):
    pathNameList = []
    burnAddrList = []
    burnSizeList = []
    imageSizeList = []
    typeList = []
    fileInfoList = []
    parse_param(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, inputList)
    for path in pathNameList:
        with  open(path, 'rb+') as subfile:
            data = subfile.read()
            fileInfoList.append(data)
            subfile.close()
    print(pathNameList)
    print(burnAddrList)
    print(burnSizeList)
    print(imageSizeList)
    print(typeList)

    create_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, fileInfoList, outputPath)
def update_bin(packet_path, inputList):
    pathNameList = []
    burnAddrList = []
    burnSizeList = []
    imageSizeList = []
    typeList = []
    startIndexList = []
    fileInfoList = []

    pathNameListNew = []
    burnAddrListNew = []
    burnSizeListNew = []
    imageSizeListNew = []
    typeListNew = []
    parse_param(pathNameListNew, burnAddrListNew, burnSizeListNew, imageSizeListNew, typeListNew, inputList)
    parse_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, startIndexList, fileInfoList,
                   packet_path)

    for index in range(0, len(pathNameListNew)):
        pathNew = pathNameListNew[index]
        print(os.path.basename(pathNew))
        pathIn32Byte = os.path.basename(pathNew)
        if len(pathIn32Byte) > 32:
            pathIn32Byte = pathIn32Byte[0:32]
        if pathIn32Byte in pathNameList:
            for index2 in range(0, len(pathNameList)):
                path = pathNameList[index2]
                if path == pathIn32Byte:
                    with open(pathNew, 'rb+') as subfile:
                        fileInfoList[index2] = subfile.read()
                    pathNameList[index2] = os.path.basename(pathNew)
                    if len(burnAddrListNew) > index:
                        burnAddrList[index2] = burnAddrListNew[index]
                    if len(burnSizeListNew) > index:
                        burnSizeList[index2] = burnSizeListNew[index]
                    imageSizeList[index2] = imageSizeListNew[index]
                    if len(typeListNew) > index:
                        typeList[index2] = typeListNew[index]
                    break
        else:
            pathNameList.append(pathNameListNew[index])
            burnAddrList.append(burnAddrListNew[index])
            burnSizeList.append(burnSizeListNew[index])
            imageSizeList.append(imageSizeListNew[index])
            typeList.append(typeListNew[index])
            with open(pathNew, 'rb+') as subfile:
                fileInfoList.append(subfile.read())
    print(pathNameList)
    print(burnAddrList)
    print(burnSizeList)
    print(imageSizeList)
    print(typeList)

    create_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, fileInfoList, packet_path)
def delete_bin(packet_path, name_list):
    pathNameList = []
    burnAddrList = []
    burnSizeList = []
    imageSizeList = []
    typeList = []
    startIndexList = []
    fileInfoList = []

    pathNameListNew = name_list

    parse_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, startIndexList, fileInfoList,
                   packet_path)

    for pathNew in pathNameListNew:
        print(os.path.basename(pathNew))
        pathIn32Byte = os.path.basename(pathNew)
        if len(pathIn32Byte) > 32:
            pathIn32Byte = pathIn32Byte[0:32]
        if pathIn32Byte in pathNameList:
            for index2 in range(0, len(pathNameList)):
                path = pathNameList[index2]
                if path == pathIn32Byte:
                    del fileInfoList[index2]
                    del pathNameList[index2]
                    del burnAddrList[index2]
                    del burnSizeList[index2]
                    del imageSizeList[index2]
                    del typeList[index2]
                    break
    print(pathNameList)
    print(burnAddrList)
    print(burnSizeList)
    print(imageSizeList)
    print(typeList)

    create_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, fileInfoList, packet_path)
def split_bin(packet_path, output_path):
    pathNameList = []
    burnAddrList = []
    burnSizeList = []
    imageSizeList = []
    typeList = []
    startIndexList = []
    fileInfoList = []

    parse_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, startIndexList, fileInfoList,
                   packet_path)
    print(pathNameList)
    print(burnAddrList)
    print(burnSizeList)
    print(imageSizeList)
    print(typeList)

    imageNum = len(pathNameList)
    for index in range(0, imageNum):
        pathName = os.path.join(output_path, pathNameList[index])
        print(pathName)
        with open(pathName, 'wb+') as file:
            file.write(fileInfoList[index])
            file.close()
def show_bin(packet_path):
    pathNameList = []
    burnAddrList = []
    burnSizeList = []
    imageSizeList = []
    typeList = []
    startIndexList = []
    fileInfoList = []

    parse_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, startIndexList, fileInfoList,
                   packet_path)
    print("pathNameList")
    print(pathNameList)
    print("burnAddrList")
    print(burnAddrList)
    print("burnSizeList")
    print(burnSizeList)
    print("imageSizeList")
    print(imageSizeList)
    print("typeList")
    print(typeList)
'''
def merge_bin(packet_path, inputList):
    pathNameList = []
    burnAddrList = []
    burnSizeList = []
    imageSizeList = []
    typeList = []
    startIndexList = []
    fileInfoList = []

    parse_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, startIndexList, fileInfoList,
                   packet_path)

    hasKvBin = False
    kvBinIndex = -1
    for typeItem in typeList:
        kvBinIndex += 1
        if typeItem == 2:
            hasKvBin = True
            break
    if hasKvBin is False:
        return
    pathNew = inputList[0]
    subfile = open(pathNew, 'rb+')
    newBinArray = subfile.read()
    subfile.close()
    newKv = kv_create.decode_kv_bin(bytearray(newBinArray))
    srcBinArray = fileInfoList[kvBinIndex]
    srcKv = kv_create.decode_kv_bin(bytearray(srcBinArray))
    srcKv[0xb0].update(newKv[0xb0])
    srcKv[0xb2].update(newKv[0xb2])
    srcKv[0xb4].update(newKv[0xb4])
    fileInfoList[kvBinIndex] = kv_create.get_kv_bin(srcKv)

    print(pathNameList)
    print(burnAddrList)
    print(burnSizeList)
    print(imageSizeList)
    print(typeList)

    create_allinone(pathNameList, burnAddrList, burnSizeList, imageSizeList, typeList, fileInfoList, packet_path)
'''
if __name__=="__main__":
    args = len(sys.argv)
    argv = sys.argv
    if(args <= 1):
        print("parma error, please use -help for usage")
        sys.exit()
    if (argv[1] == '-hard'):
        ignoreFlag = 0
        del argv[1]
    if (argv[1] == '-packet'):
        outputPath = argv[2]
        print(outputPath)
        del argv[2]
        del argv[1]
        del argv[0]
        packet_bin(outputPath, argv)
    elif (argv[1] == '-update'):
        outputPath = argv[2]
        print(outputPath)
        del argv[2]
        del argv[1]
        del argv[0]
        update_bin(outputPath, argv)
    elif (argv[1] == '-split'):
        input_file = argv[2]
        outputPath = argv[3]
        split_bin(input_file, outputPath)
    elif (argv[1] == '-show'):
        input_file = argv[2]
        show_bin(input_file)
    elif (argv[1] == '-delete'):
        outputPath = argv[2]
        print(outputPath)
        del argv[2]
        del argv[1]
        del argv[0]
        delete_bin(outputPath, argv)
    elif (argv[1] == '-merge'):
        outputPath = argv[2]
        print(outputPath)
        del argv[2]
        del argv[1]
        del argv[0]
        merge_bin(outputPath, argv)
    elif (argv[1] == '-sha256'):
        outputPath = argv[2]
        print(outputPath)
        del argv[2]
        del argv[1]
        del argv[0]
        create_sha_file(argv, outputPath)
    elif (argv[1] == '-help'):
        print("\
        -packet, packet all bins as one\r\n\
            param should be : -packet \"outputfile\" \"binName1|burnAddr|burnSize|type\"...\r\n\
        -split, split inputefile as several bins\r\n\
            param should be : -split inputefile outputPath\r\n\
        -update, update bins in inputefile, if bin not in inputefile, it will be add.\r\n\
            param should be : -update \"inputefile\" \"binName1|burnAddr|burnSize|type\"...\r\n\
        -delete, delete bins in inputfile\r\n\
            param should be : -delete inputefile binName1 binName2...\r\n\
        -merge, merge kv bin in inputefile, if bin not in inputefile, it will be add.\r\n\
            param should be : -merge inputefile binName1...\r\n\
        -show, show the bins in inputfile\r\n\
            param should be : -show \"inputefile\"\r\n\
        -help\r\n")
    else:
        outputPath = argv[1]
        print(outputPath)
        del argv[1]
        del argv[0]
        packet_bin(outputPath, argv)

