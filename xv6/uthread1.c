#include "types.h"
#include "stat.h"
#include "user.h"

/* Possible states of a thread; */
//유저레벨 쓰레드는 여기서 다 알아서 한다. 커널하고 상관없다.
#define FREE        0x0
#define RUNNING     0x1
#define RUNNABLE    0x2

#define STACK_SIZE  8192
#define MAX_THREAD  4

typedef struct thread thread_t, *thread_p;
typedef struct mutex mutex_t, *mutex_p;

struct thread { //쓰레드 구조. 스택이 있다.
  int        sp;                /* saved stack pointer */
  char stack[STACK_SIZE];       /* the thread's stack */
  int        state;             /* FREE, RUNNING, RUNNABLE */
};    //|sp|stack|state| (*p) = p->sp 얘가 가르키는 첫번째가 무조껀 스태고인터.
static thread_t all_thread[MAX_THREAD];
thread_p  current_thread;
thread_p  next_thread;
extern void thread_switch(void);

static void //유저레벨에서 스케쥴러 선언. 전부 한 프로세스안에서 돌아가는거당.
thread_schedule(void)
{
  thread_p t;
  next_thread = 0;
  /* Find another runnable thread. */
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == RUNNABLE && t != current_thread) { //다음게 있음 다음걸 돌린다.
      next_thread = t;
      break;
    }
  }

  if (t >= all_thread + MAX_THREAD && current_thread->state == RUNNABLE) { //러너블 쓰레드를 찾아서 바뀐다. 
    /* The current thread is the only runnable thread; run it. */
    next_thread = current_thread;
  }

  if (next_thread == 0) {
    printf(2, "thread_schedule: no runnable threads\n");
    exit();
  }

  if (current_thread != next_thread) { 
    printf(1, "Switching from %p to %p\n", current_thread, next_thread);
    //current_thread->state = RUNNABLE; // 빠진 부분       /* switch threads?  */
    next_thread->state = RUNNING; //넥스트 쓰레드를 러닝으로 바꾼다. 커런트를 러닝으로 바꿔줘야하는데 그거 어디있어? 만들어줘야하나? 
    thread_switch();
  } else
    next_thread = 0;
}

void 
thread_init(void)
{
  uthread_init((int)thread_schedule); //이거 user.h에 만들어서 넣어야한다? 
  
  // 쓰레드 스케쥴을 커널에 등록.
  // 유저레벨인데 라운드로빈으로 할거다. 타이머인터럽트를 해야함. 그래서 커널이 지원하는 유저레벨쓰레드를 만드는 것이다..
  // main() is thread 0, which will make the first invocation to
  // thread_schedule().  it needs a stack so that the first thread_switch() can
  // save thread 0's state.  thread_schedule() won't run the main thread ever
  // again, because its state is set to RUNNING, and thread_schedule() selects
  // a RUNNABLE thread.

  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
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
  *(int*)(t->sp) = (int)func;           // push return address on stack
  t->sp -= 32;                             // space for registers that thread_switch expects

  //printf(1, "Thread created at %p with stack %p\n", t, t->stack);
  t->state = RUNNABLE;
}

static void 
mythread(void) //백번 돌면서 백번 출력 
{
  int i;
  printf(1, "my thread running\n");
  for (i = 0; i < 100; i++) { //주소를 백번찍고 끝난다.
    printf(1, "my thread 0x%x\n", (int) current_thread);
  }
  printf(1, "my thread: exit\n");
  current_thread->state = FREE;
}


int 
main(int argc, char *argv[]) 
{
  thread_init();
  thread_create(mythread); //두개의 쓰레드를 만들어서 동작시킨다.
  thread_create(mythread);
  thread_schedule();
  return 0;
}
