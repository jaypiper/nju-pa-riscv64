.globl  _start
_start:
  movq $0, %rbp
  call call_main
