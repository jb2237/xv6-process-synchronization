#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#define MAX_SEMAPHORES 32

struct sem {
  int value;
  int allocated;
  struct spinlock lock;
};

static struct sem sems[MAX_SEMAPHORES];
static struct spinlock sem_table_lock;
static int sems_inited = 0;

void
seminit(void)
{
  initlock(&sem_table_lock, "sem_table");
  for(int i = 0; i < MAX_SEMAPHORES; i++){
    initlock(&sems[i].lock, "semaphore");
    sems[i].allocated = 0;
    sems[i].value = 0;
  }
  sems_inited = 1;
}

uint64
sys_sem_init(void)
{
  int init_val;
  argint(0, &init_val);

  if(!sems_inited)
    seminit();

  acquire(&sem_table_lock);
  int sem_id = -1;
  for(int i = 0; i < MAX_SEMAPHORES; i++){
    if(!sems[i].allocated){
      sems[i].allocated = 1;
      sems[i].value = init_val;
      sem_id = i;
      break;
    }
  }
  release(&sem_table_lock);
  return sem_id;
}

uint64
sys_sem_wait(void)
{
  int sem_id;
  argint(0, &sem_id);

  if(sem_id < 0 || sem_id >= MAX_SEMAPHORES)
    return -1;

  acquire(&sems[sem_id].lock);
  while(sems[sem_id].value <= 0){
    sleep(&sems[sem_id], &sems[sem_id].lock);
  }
  sems[sem_id].value--;
  release(&sems[sem_id].lock);
  return 0;
}

uint64
sys_sem_post(void)
{
  int sem_id;
  argint(0, &sem_id);

  if(sem_id < 0 || sem_id >= MAX_SEMAPHORES)
    return -1;

  acquire(&sems[sem_id].lock);
  sems[sem_id].value++;
  wakeup(&sems[sem_id]);
  release(&sems[sem_id].lock);
  return 0;
}