#include "types.h"
#include "stat.h"
#include "user.h"

/* Possible states of a thread; */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2
#define WAIT        0x3

#define STACK_SIZE  8192
#define MAX_THREAD  10

typedef struct thread thread_t, *thread_p;
typedef struct mutex mutex_t, *mutex_p;

struct thread {
  int        tid;    /* thread id */
  int        ptid;  /* parent thread id */
  int        sp;                /* saved stack pointer */
  char stack[STACK_SIZE];       /* the thread's stack */
  int        state;             /* FREE, RUNNING, RUNNABLE, WAIT */
};
static thread_t all_thread[MAX_THREAD];
thread_p  current_thread;
thread_p  next_thread;
extern void thread_switch(void);

static void 
thread_schedule(void)
{
  thread_p t;

  /* Find another runnable thread. */
  next_thread = 0;
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == RUNNABLE && t != current_thread) {
      next_thread = t;
      break;
    }
  }

  if (t >= all_thread + MAX_THREAD && current_thread->state == RUNNABLE) {
    /* The current thread is the only runnable thread; run it. */
    next_thread = current_thread;
  }

  if (next_thread == 0) {
    printf(2, "thread_schedule: no runnable threads\n");
    exit();
  }

  if (current_thread != next_thread) {         /* switch threads?  */
    next_thread->state = RUNNING;
    thread_switch();
  } else
    next_thread = 0;
}

void 
thread_init(void)
{
  uthread_init(thread_schedule);

  // main() is thread 0, which will make the first invocation to
  // thread_schedule().  it needs a stack so that the first thread_switch() can
  // save thread 0's state.  thread_schedule() won't run the main thread ever
  // again, because its state is set to RUNNING, and thread_schedule() selects
  // a RUNNABLE thread.
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
  current_thread->tid=0;
  current_thread->ptid=0;
}

int 
thread_create(void (*func)())
{
  thread_p t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  t->sp = (int) (t->stack + STACK_SIZE);   // set sp to the top of the stack
  t->sp -= 4;                              // space for return address
  /* 
    set tid and ptid 요기 쓰레드 아이디를 세팅하는 부분을 바꿔야한다.
  */
  * (int *) (t->sp) = (int)func;           // push return address on stack
  t->sp -= 32;                             // space for registers that thread_switch expects
  t->state = RUNNABLE;
  
  return t->tid; // 쓰레드 아이디 리턴.
}

static void 
thread_join(int tid)
{
  /*
    returns when the child thread tid has exited.
    아이디를 넣어서 기다리라고 하는것. 
    특정 쓰레드가 끝날때까지 기다려라 ! 
    1번하고똑같음.
  */
}

static void 
child_thread(void)
{
  int i;
  printf(1, "child thread running\n");
  for (i = 0; i < 100; i++) {
    printf(1, "child thread 0x%x\n", (int) current_thread);
  }
  printf(1, "child thread: exit\n");
  current_thread->state = FREE;
}
// 쓰레드 아이디를 다 알아야하면 그 전에 아이디를 알아야겠는데.. 하면 그런부분을 임의로 추가해라.
// 구조체나 리스트같은걸 넣을거면 .. 넣으라는 뜻이다.

static void 
mythread(void)
{
  int i;
  int tid[5];

  printf(1, "my thread running\n");

  for (i = 0; i < 5; i++) {
    tid[i]=thread_create(child_thread);
  }
  
  for (i = 0; i < 5; i++) {
    thread_join(tid[i]); //다섯개를 각각 기다리는 것이다.  다 끝나지 않을 것이라고하시네.
  }
  
  printf(1, "my thread: exit\n");
  current_thread->state = FREE;
}

int 
main(int argc, char *argv[]) 
{
  int tid;
  thread_init();
  tid=thread_create(mythread); 
  thread_join(tid);
  return 0;
}
