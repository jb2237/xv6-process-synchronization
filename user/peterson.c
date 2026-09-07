#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Shared layout in mapped page
struct peterson_data {
  volatile int flag[2];
  volatile int turn;
  volatile int shared_counter;
};

// Memory barrier instruction for RISC-V out-of-order execution prevention
static inline void fence(void) {
  asm volatile("fence rw,rw" ::: "memory");
}

void delay(int cycles) {
  volatile int x = 0;
  for (int i = 0; i < cycles * 1000; i++) {
    x += i;
  }
}

int main(int argc, char *argv[]) {
  struct peterson_data *shm = (struct peterson_data *)shm_get();
  if (!shm) {
    printf("peterson: shm_get failed\n");
    exit(1);
  }

  // Initialize shared variables
  shm->flag[0] = 0;
  shm->flag[1] = 0;
  shm->turn = 0;
  shm->shared_counter = 0;

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  int id = (pid == 0) ? 1 : 0; // 0 = Parent, 1 = Child
  int other = 1 - id;

  for (int iter = 0; iter < 10; iter++) {
    // Entry Section
    shm->flag[id] = 1;
    shm->turn = other;
    fence();

    while (shm->flag[other] && shm->turn == other) {
      // Busy wait
    }

    // Critical Section
    int curr = shm->shared_counter;
    delay(200); // Expose race conditions if synchronization were faulty
    shm->shared_counter = curr + 1;
    printf("Process %d in CS, counter = %d\n", id, shm->shared_counter);

    // Exit Section
    fence();
    shm->flag[id] = 0;

    // Remainder Section
    delay(300);
  }

  if (pid == 0) {
    exit(0);
  } else {
    wait(0);
    printf("Final Counter Value: %d (Expected: 20)\n", shm->shared_counter);
    if (shm->shared_counter == 20) {
      printf("Peterson Mutual Exclusion Test: PASSED\n");
    } else {
      printf("Peterson Mutual Exclusion Test: FAILED\n");
    }
  }

  exit(0);
}