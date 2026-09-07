// Add extern declarations
extern uint64 sys_shm_get(void);
extern uint64 sys_sem_init(void);
extern uint64 sys_sem_wait(void);
extern uint64 sys_sem_post(void);

// Add to the syscalls function pointer array:
static uint64 (*syscalls[])(void) = {
// ... existing entries ...
[SYS_close]    sys_close,
[SYS_shm_get]  sys_shm_get,
[SYS_sem_init] sys_sem_init,
[SYS_sem_wait] sys_sem_wait,
[SYS_sem_post] sys_sem_post,
};