#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER_SIZE 5
#define TOTAL_ITEMS 20

struct pc_shared {
  int buffer[BUFFER_SIZE];
  int in;
  int out;
};

void delay(int count) {
  volatile int x = 0;
  for(int i = 0; i < count * 1000; i++) x++;
}

int main(int argc, char *argv[]) {
  struct pc_shared *shm = (struct pc_shared *)shm_get();
  if (!shm) {
    printf("prodcons: shm_get failed\n");
    exit(1);
  }

  shm->in = 0;
  shm->out = 0;

  int mutex = sem_init(1);
  int empty = sem_init(BUFFER_SIZE);
  int full  = sem_init(0);

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Producer Process
    for (int item = 1; item <= TOTAL_ITEMS; item++) {
      sem_wait(empty);
      sem_wait(mutex);

      shm->buffer[shm->in] = item;
      printf("Producer: Produced %d at slot %d\n", item, shm->in);
      shm->in = (shm->in + 1) % BUFFER_SIZE;

      sem_post(mutex);
      sem_post(full);

      delay(150);
    }
    exit(0);
  } else {
    // Consumer Process
    for (int i = 0; i < TOTAL_ITEMS; i++) {
      sem_wait(full);
      sem_wait(mutex);

      int item = shm->buffer[shm->out];
      printf("Consumer: Consumed %d from slot %d\n", item, shm->out);
      shm->out = (shm->out + 1) % BUFFER_SIZE;

      sem_post(mutex);
      sem_post(empty);

      delay(300);
    }
    wait(0);
    printf("Bounded Buffer Producer-Consumer Test: PASSED\n");
  }

  exit(0);
}