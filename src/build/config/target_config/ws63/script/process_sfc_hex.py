import struct
import sys

def create_hex_file(target, source):
    with open(str(source), "rb") as binfile, open(str(target), "wb") as hexfile:
        while True:
            bindata = binfile.read(4)
            if not bindata:
                break
            longdata, = struct.unpack("<L", bindata)
            hexstr = '{:x}'.format(longdata)  # dec to hex number str
            hexstr = '%s\n' % '{:0>8}'.format(hexstr).upper()
            hexfile.write(str.encode(hexstr))
    binfile.close()
    hexfile.close()

def create_sfc_file(target, source, input_len):
    len = 0
    with open(str(source), "r") as hexfile, open(str(target), "w") as sfcfile:
        for line in hexfile.readlines():
            #print("len is %d" % len(line))
            sfcfile.writelines(line[6:8])
            sfcfile.writelines("\n")
            sfcfile.writelines(line[4:6])
            sfcfile.writelines("\n")
            sfcfile.writelines(line[2:4])
            sfcfile.writelines("\n")
            sfcfile.writelines(line[0:2])
            sfcfile.writelines("\n")
            len += 4
        while len < int(input_len) :
            sfcfile.writelines('FF')
            sfcfile.writelines("\n")
            len += 1
    hexfile.close()
    sfcfile.close()

if __name__ == "__main__":
    if len(sys.argv) == 4:
        bin_file_name = sys.argv[1]
        hex_file_name = "temp.hex"
        sfc_file_name = sys.argv[2]
        len = sys.argv[3]
    else:
        print('hhhhhhhhhhhhhhhhh')
        sys.exit(0)

    create_hex_file(hex_file_name, bin_file_name)
    create_sfc_file(sfc_file_name, hex_file_name, len)
    print('success')
