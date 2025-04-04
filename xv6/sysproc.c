#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_uthread_init(void) {
  struct proc *p = myproc();  // 현재 프로세스 가져오기
  int addr;

  // 사용자로부터 정수형 주소 가져오기
  if (argint(0, &addr) < 0)
    return -1;

  // 주소 검증 (옵션: 사용자 공간인지 확인)
  if (addr < PGSIZE || addr >= KERNBASE) { // PGSIZE는 페이지 크기(보통 4096), KERNBASE는 커널 시작 주소
    return -1; // 잘못된 주소일 경우 반환
  }

  p->scheduler = addr;  // 유저 레벨 스케줄러 주소 저장
  return 0;
}

