#!/bin/sh

# Extract strings from all source files.
# EXTRACT_TR_STRINGS extracts strings with lupdate and convert them to .pot with
# lconvert.

function find_files()
{
    while IFS='' read -r line ; do
        if echo "$line" | grep -q "${1}_LIB_SRCS" > /dev/null; then
            while IFS=' ' read -a file ; do
                if [ "$file" == ")" ]; then
                    break
                elif echo "$file" | grep -q "^\w*#" > /dev/null || [ ! -f "$file" ] || \
                     echo "$file" | grep -qv "\.cpp\w*$" > /dev/null; then
                    continue
                fi
                echo $file
            done
        fi
    done < CMakeLists.txt
}

$EXTRACT_TR_STRINGS `find_files kpropertycore` -o $podir/kpropertycore_qt.pot
$EXTRACT_TR_STRINGS `find_files kpropertywidgets` -o $podir/kpropertywidgets_qt.pot
