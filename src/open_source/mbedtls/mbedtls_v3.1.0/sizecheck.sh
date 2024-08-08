#!/bin/bash

date > 		./sizelog.txt
echo "Check size for files:" >> ./sizelog.txt
find . -name "*.dll" -o -name "*.so" | xargs sha1sum >> ./sizelog.txt
find . -name "*.a" | grep -v ".dll." | xargs sha1sum >> ./sizelog.txt
#
find . -name "*.dll" -o -name "*.so" | xargs ls -l >> ./sizelog.txt
find . -name "*.a" | grep -v ".dll." | xargs ls -l >> ./sizelog.txt
#
find . -name "*.a" | grep -v ".dll." | xargs size >> ./sizelog.txt
find . -name "*.dll" -o -name "*.so" | xargs size >> ./sizelog.txt
echo "Size check down!"
