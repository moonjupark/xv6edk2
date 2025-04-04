# Context switch
#
#   void swtch(struct context **old, struct context *new);
# 
# Save the current registers on the stack, creating
# a struct context, and save its address in *old.
# Switch stacks to new and pop previously-saved registers.

.globl swtch
swtch:
 # 첫 번째 인자(old context)를 %eax에 저장
 # 두 번째 인자(new context)를 %edx에 저장
 # 그러니까 총 두개의 아규먼트를 넘길 수 있다.
 # 32비트니까 4바이트씩 움직임.
 
  movl 4(%esp), %eax
  movl 8(%esp), %edx

  # Save old callee-save registers si, di는 일반적인 레지스터랑 같다. 
  pushl %ebp
  pushl %ebx
  pushl %esi
  pushl %edi 

 #  스택 바꾼다. 
  # Switch stacks
  movl %esp, (%eax)
  movl %edx, %esp

  # Load new callee-save registers
  popl %edi
  popl %esi
  popl %ebx
  popl %ebp
  ret

