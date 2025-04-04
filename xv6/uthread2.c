#include "types.h"
#include "stat.h"
#include "user.h"

/* Possible states of a thread; */
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2
#define WAIT        0x3 // 이게 들어갔다. 기다리는 쓰레드가 존재하는것.

#define STACK_SIZE  8192
#define MAX_THREAD  10 //갯수변경 

typedef struct thread thread_t, *thread_p;
typedef struct mutex mutex_t, *mutex_p;

struct thread {
  int        tid;    /* thread id 이거 쓰레드 아이디 추가 */ 
  int        ptid;  /* parent thread id 이거 패런트 아이디 추가 */
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
  current_thread->tid=0; //요거 생겼고
  current_thread->ptid=0; //요것도 생겼다.
}

void 
thread_create(void (*func)())
{
  thread_p t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  t->sp = (int) (t->stack + STACK_SIZE);   // set sp to the top of the stack
  t->sp -= 4;                              // space for return address
  /* 
    set tid and ptid
  */
  * (int *) (t->sp) = (int)func;           // push return address on stack
  t->sp -= 32;                             // space for registers that thread_switch expects
  t->state = RUNNABLE;
}

static void 
thread_join_all(void)
{
  /*
    it returns when all child threads have exited.
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

static void 
mythread(void)
{
  int i;
  printf(1, "my thread running\n"); //요게 처음에 찍히고, 
  for (i = 0; i < 5; i++) {
    thread_create(child_thread); //5개의 차일드를 만들었다.
  }
  thread_join_all(); //막 ~~ 5 만들어서 500번 출력하고 나면 마지막에 
  printf(1, "my thread: exit\n"); // 차일드가 다끝나면 마이 쓰레드
  current_thread->state = FREE;
}
//조인 부르면 내상태를 그냥 웨이트를 바꿔버리면 스케쥴러안해도되는데,
//차일드가 다끝났을때 어떻게 마이를 다시 부르느냐 그걸 만드는게 관건이.

int 
main(int argc, char *argv[]) 
{
  thread_init();
  thread_create(mythread);
  thread_join_all();
  return 0;
}
