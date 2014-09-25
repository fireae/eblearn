#!/bin/sh

grep --color=always -r "TODO" * | grep -v ".svn" | grep -v "doc/documentation" | grep -v "cpp~" | grep -v "Binary file"
