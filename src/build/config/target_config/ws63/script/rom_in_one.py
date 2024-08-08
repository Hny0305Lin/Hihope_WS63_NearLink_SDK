import sys
import os


def merge(file_first, file_second, file_out):
    ret = open(file_out, 'wb')
    with open(file_first, 'rb') as file_1:
        for i in file_1:
            ret.write(i)
    with open(file_second, 'rb') as file_2:
        for i in file_2:
            ret.write(i)
    ret.close()


def move_file(src_path, dst_path, file_name):
    src_file = os.path.join(src_path, file_name)
    if not os.path.exists(dst_path):
        os.mkdir(dst_path)
    dst_file = os.path.join(dst_path, file_name)
    shutil.move(src_file, dst_file)


# instead of linux dd
# max_size: number of k
def python_dd(file_name, max_size):
    file_size = os.path.getsize(file_name)
    print(file_name, "size: ", file_size)
    if file_size < max_size * 1024:
        file_content = open(file_name, 'wb')
        file_content.write(b'0' * (max_size * 1024 - file_size))


root_dir = sys.argv[1]
bin_name = sys.argv[2]

out_dir = os.path.join(root_dir, "output/ws63/acore/", bin_name)
romboot_bin = os.path.join(root_dir, "/output/ws63/acore/ws63-romboot/romboot.bin")

if not os.path.isfile(romboot_bin):
    print("[*] romboot not found, codepoint files will not be one!")
    sys.exit()

python_dd(romboot_bin, 36)
merge(romboot_bin, romboot_bin + "_rom.bin", romboot_bin + "_rompack.bin")