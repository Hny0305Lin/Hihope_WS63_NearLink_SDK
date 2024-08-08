/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal_file.h"
#include "littlefs_adapt.h"

int HalFileOpen(const char* path, int oflag, int mode)
{
    (void)mode;
    return fs_adapt_open(path, oflag);
}

int HalFileClose(int fd)
{
    return fs_adapt_close(fd);
}

int HalFileRead(int fd, char *buf, unsigned int len)
{
    return fs_adapt_read(fd, buf, len);
}

int HalFileWrite(int fd, const char *buf, unsigned int len)
{
    return fs_adapt_write(fd, buf, len);
}

int HalFileDelete(const char *path)
{
    return fs_adapt_delete(path);
}

int HalFileStat(const char *path, unsigned int *fileSize)
{
    return fs_adapt_stat(path, fileSize);
}

int HalFileSeek(int fd, int offset, unsigned int whence)
{
    return fs_adapt_seek(fd, offset, whence);
}
