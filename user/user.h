// Shared memory and Semaphores
void* shm_get(void);
int   sem_init(int init_val);
int   sem_wait(int sem_id);
int   sem_post(int sem_id);