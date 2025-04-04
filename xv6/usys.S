#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \         
  .globl name; \
  name: \                       
    movl $SYS_ ## name, %eax; \ 
    int $T_SYSCALL; \
    ret 

# 이게 매크로다. 
# wait를 부르면 wait가 들어갈 것이다.
# wait면 eax에 3이 들어갈 것.
# 이걸 실행하면 시스템 콜 함수로 가서 발생할 것이다. 그니까 실제로 wait가 발동하는건 아니고, 시스템 콜이 3번으로 발동하는것.


SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(uthread_init)
