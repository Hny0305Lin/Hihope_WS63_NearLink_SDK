#!/bin/bash
# @shell build_install.sh
# copyright(c), 2021 - 2021, huawei tech. co., ltd. ALL RIGHTS RESERVED.
# @brief handle LiteOS head file(.h)
# @version 1.0
set -e
topdir_path="$1"
installdir="$2"
out="$3"

cp -R "$out"/lib/ "$installdir"/
mkdir -p "$installdir"/bin/
cp "$out"/*.bin  "$installdir"/bin/
