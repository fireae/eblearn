#!/bin/sh

echo "Emacs/gdb debugger usage:"
echo "- at startup, press enter"
echo "- F12 to compile (then enter)"
echo "- in the gdb window, type 'r tester -nocolor'"
emacs -geometry 80x100 -font 9x15 --icon-type -fs -l .debug.el &
