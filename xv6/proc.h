// Per-CPU state
struct cpu { //멀티코어라 정의, 코어마다 스케쥴러를 돌릴 수 있게 정의 프로세스 리스트 유지 
  uchar apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null
};

extern struct cpu cpus[NCPU];
extern int ncpu;

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.

//si di, a,b,c,d같은 레지스터다. 

struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };
// 레디 웨이팅 러닝
// 레디가 러너블. 웨이팅이 슬리핑
// 프로세스가 끝나면 여기는 다 좀비로 표시한다.
// Embryo는 처음 생긴거 
// 링크드리스트로 구현 유지 


// Per-thread state
/*struct thread{
  enum procstate state;        // thread state
  int tid;                     // Thread ID
  void *chan;                  // If non-zero, sleeping on chan
  char *kstack;                // Bottom of kernel stack for this thread
  struct trapframe *tf;        // trap frame for current interrupt handler
  struct context *context;     // swtch() here to run process
  void* retval;                // return value
};*/


// Per-process state //PCB.
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)
  uint scheduler;              // 유저 스케줄러 함수 정수값으로 저장. 

  /*
  int tidx;                         // index of running thread
  struct thread threads[NTHREAD];   // thread pool
  char* kstacks[NTHREAD];           // kernel stack pool
  uint ustacks[NTHREAD];            // user stack pool

  struct {
    int level;                // scheduler level, -1 for stride, 0 ~ 3 for MLFQ
    int index;                // index of process table in scheduler
    uint elapsed;             // cpu time spent by process
    uint start;               // start tick.
  } mlfq;                     // member for MLFQ scheduler

  */


};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
