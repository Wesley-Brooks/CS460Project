#!/bin/bash
kill `ps -ef | grep myShell | grep -v grep | awk '{print $2}'`