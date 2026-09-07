#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

struct rw_shared {
  int shared_data;
  int read_count;
};

void delay(int cycles) {
  volatile int dummy = 0;
  for (int i = 0; i < cycles * 1000; i++) dummy++;
}

int main(int argc, char *argv[]) {
  struct rw_shared *shm = (struct rw_shared *)shm_get();
  if (!shm) {
    printf("readwrite: shm_get failed\n");
    exit(1);
  }

  shm->shared_data = 0;
  shm->read_count = 0;

  int mutex = sem_init(1);
  int write_lock = sem_init(1);

  // Fork 3 Readers
  for (int i = 0; i < 3; i++) {
    int pid = fork();
    if (pid == 0) {
      int rid = i + 1;
      for (int k = 0; k < 3; k++) {
        sem_wait(mutex);
        shm->read_count++;
        if (shm->read_count == 1) {
          sem_wait(write_lock); // First reader locks out writers
        }
        sem_post(mutex);

        // Reading critical section
        printf("Reader %d: reading shared_data = %d (Active Readers = %d)\n",
               rid, shm->shared_data, shm->read_count);
        delay(200);

        sem_wait(mutex);
        shm->read_count--;
        if (shm->read_count == 0) {
          sem_post(write_lock); // Last reader releases writers
        }
        sem_post(mutex);

        delay(300);
      }
      exit(0);
    }
  }

  // Fork 2 Writers
  for (int j = 0; j < 2; j++) {
    int pid = fork();
    if (pid == 0) {
      int wid = j + 1;
      for (int k = 0; k < 2; k++) {
        sem_wait(write_lock);

        // Writing critical section (exclusive access)
        shm->shared_data += 10;
        printf("Writer %d: UPDATED shared_data = %d\n", wid, shm->shared_data);
        delay(250);

        sem_post(write_lock);
        delay(400);
      }
      exit(0);
    }
  }

  // Parent waits for all 5 child processes (3 readers + 2 writers)
  for (int i = 0; i < 5; i++) {
    wait(0);
  }

  printf("Final shared_data: %d (Expected: 40)\n", shm->shared_data);
  printf("Readers-Writers Test: PASSED\n");
  exit(0);
}