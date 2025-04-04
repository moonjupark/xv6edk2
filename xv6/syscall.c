#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"

// User code makes a system call with INT T_SYSCALL. 유저코드가 시스템콜을 했다.
// System call number in %eax.                        시스템콜 넘버는 eax에 있다.
// Arguments on the stack, from the user call to the C 변수, 아규먼트는 스택에 있다.
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  struct proc *curproc = myproc(); //현재 프로세스 스택에 접근해서 

  if(addr >= curproc->sz || addr+4 > curproc->sz) //스택에 아규먼트가 있는걸 가져온다고 한다.
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;
  struct proc *curproc = myproc();

  if(addr >= curproc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)curproc->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint((myproc()->tf->esp) + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.     -----------------이걸 사용해야함!!!
int
argptr(int n, char **pp, int size)
{
  int i;
  struct proc *curproc = myproc();
 
  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= curproc->sz || (uint)i+size > curproc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_uthread_init(void); // 유저쓰레드 하나 추가 


//이게 실제 테이블이다. 
static int (*syscalls[])(void) = { //함수 포인터에 배열을 선언한것, syscall.h에 들어있는 순서다.
[SYS_fork]    sys_fork, //1번
[SYS_exit]    sys_exit, //2번
[SYS_wait]    sys_wait, //[]안에 해당하는 함수가 뒤에 함수이다. 뒤에있는게 함수 포인터라고한다.
[SYS_pipe]    sys_pipe, 
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_uthread_init]   sys_uthread_init,//여기에 내가 추가할 함수를 넣어야한다. 
};

void 
syscall(void)
{
  int num;
  struct proc *curproc = myproc();
  
  num = curproc->tf->eax; //ax에 번호를 넣고 부른다.
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) { // ax를 보고 테이블에 접근해서 시스템콜을 찾아 그래서 해당하는 함수를 부른다.
    curproc->tf->eax = syscalls[num](); //트릭인데.. 스택에 값을 바꿔서 거기로 컨텍스트 스위칭 시키는것. PCB에 tf에다가 ax값을 저장해두는것.
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
