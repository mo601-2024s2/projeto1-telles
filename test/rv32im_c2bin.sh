#!/bin/sh

riscv32-unknown-linux-gnu-gcc -S -march=rv32im -mabi=ilp32 $1.c

riscv32-unknown-linux-gnu-as -march=rv32im -mabi=ilp32 $1.s -o $1.out

riscv32-unknown-linux-gnu-ld $1.out -o $1.bin

riscv32-unknown-linux-gnu-objcopy -O binary $1.bin
