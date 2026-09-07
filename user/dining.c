#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PHIL 5
#define CYCLES 5

void delay(int count) {
  volatile int x = 0;
  for(int i = 0; i < count * 1000; i++) x++;
}

int main(int argc, char *argv[]) {
  int forks[NUM_PHIL];
  for (int i = 0; i < NUM_PHIL; i++) {
    forks[i] = sem_init(1);
  }

  // Spawn 5 philosophers
  for (int i = 0; i < NUM_PHIL; i++) {
    int pid = fork();
    if (pid == 0) {
      int left = i;
      int right = (i + 1) % NUM_PHIL;

      // Deadlock Avoidance Strategy: Asymmetric resource acquisition
      // Even philosophers pick left then right; odd philosophers pick right then left.
      int first = (i % 2 == 0) ? left : right;
      int second = (i % 2 == 0) ? right : left;

      for (int cycle = 1; cycle <= CYCLES; cycle++) {
        // THINKING
        printf("Philosopher %d: THINKING [Cycle %d/%d]\n", i, cycle, CYCLES);
        delay(150);

        // HUNGRY
        printf("Philosopher %d: HUNGRY\n", i);
        sem_wait(forks[first]);
        sem_wait(forks[second]);

        // EATING
        printf("Philosopher %d: EATING\n", i);
        delay(200);

        // Put down forks
        sem_post(forks[second]);
        sem_post(forks[first]);
      }

      printf("Philosopher %d: FINISHED\n", i);
      exit(0);
    }
  }

  // Wait for all philosophers to finish
  for (int i = 0; i < NUM_PHIL; i++) {
    wait(0);
  }

  printf("Dining Philosophers Test: PASSED (No Deadlocks Detected)\n");
  exit(0);
}