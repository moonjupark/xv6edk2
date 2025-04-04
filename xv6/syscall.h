// System call numbers
// 매크로로 구성되어있어서 컨벤션을 따라줘야한다.
// 기존에 있던 것들이 번호가 바뀌면 안되고 밑쪽으로 추가해나가면 된다.
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_pipe    4
#define SYS_read    5
#define SYS_kill    6
#define SYS_exec    7
#define SYS_fstat   8
#define SYS_chdir   9
#define SYS_dup    10
#define SYS_getpid 11
#define SYS_sbrk   12
#define SYS_sleep  13
#define SYS_uptime 14
#define SYS_open   15
#define SYS_write  16
#define SYS_mknod  17
#define SYS_unlink 18
#define SYS_link   19
#define SYS_mkdir  20
#define SYS_close  21
#define SYS_uthread_init 22 // 시스템 콜을 하나 추가했습니다.
