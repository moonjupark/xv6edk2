# The xv6 kernel starts executing in this file. This file is linked with
# the kernel C code, so it can refer to kernel symbols such as main().
# The boot block (bootasm.S and bootmain.c) jumps to entry below.
        
# Multiboot header, for multiboot boot loaders like GNU Grub.
# http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
#
# Using GRUB 2, you can boot xv6 from a file stored in a
# Linux file system by copying kernel or kernelmemfs to /boot
# and then adding this menu entry:
#
# menuentry "xv6" {
# 	insmod ext2
# 	set root='(hd0,msdos1)'
# 	set kernel='/boot/kernel'
# 	echo "Loading ${kernel}..."
# 	multiboot ${kernel} ${kernel}
# 	boot
# }

#include "asm.h"
#include "memlayout.h"
#include "mmu.h"
#include "param.h"

# Multiboot header.  Data to direct multiboot loader.
.p2align 2
.text
.globl multiboot_header
multiboot_header:
  #define magic 0x1badb002
  #define flags 0
  .long magic
  .long flags
  .long (-magic-flags)

# By convention, the _start symbol specifies the ELF entry point.
# Since we haven't set up virtual memory yet, our entry point is
# the physical address of 'entry'.
.globl _start
_start = V2P_WO(entry)

.globl wait_main
wait_main:
  .long 0
# Entering xv6 on boot processor, with paging off.
.globl entry
entry:
  #Set Data Segment
  mov $0x10,%ax
  mov %ax,%ds
  mov %ax,%es
  mov %ax,%ss
  mov $0,%ax
  mov %ax,%fs
  mov %ax,%gs

  #Turn off paing
  movl %cr0,%eax
  andl $0x7fffffff,%eax
  movl %eax,%cr0 

  #Set Page Table Base Address
  movl    $(V2P_WO(entrypgdir)), %eax
  movl    %eax, %cr3
  
  #Disable IA32e mode
  movl $0x0c0000080,%ecx
  rdmsr
  andl $0xFFFFFEFF,%eax
  wrmsr

  # Turn on page size extension for 4Mbyte pages
  movl    %cr4, %eax
  orl     $(CR4_PSE), %eax
  andl    $0xFFFFFFDF, %eax
  movl    %eax, %cr4

  #Turn on Paging
  movl    %cr0, %eax
  orl     $0x80010001, %eax
  movl    %eax, %cr0




  # Set up the stack pointer.
  movl $(stack + KSTACKSIZE), %esp
  # Jump to main(), and switch to executing at
  # high addresses. The indirect call is needed because
  # the assembler produces a PC-relative instruction
  # for a direct jump.
#  jz .waiting_main
  movl $main, %edx
  jmp %edx

.comm stack, KSTACKSIZE
