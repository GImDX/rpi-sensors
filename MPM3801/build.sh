#!/bin/bash
gcc MPM3801_test_old.c -o MPM3801_old
gcc main.c MPM3801.c -o MPM3801 -li2c
