#define SHM_VA 0x3F000000ULL

static void *shm_page = 0;
static struct spinlock shm_lock;

// Initialize shared memory spinlock during boot or lazily
void
shminit_lazy(void)
{
  static int initialized = 0;
  if(!initialized){
    initlock(&shm_lock, "shm");
    initialized = 1;
  }
}

uint64
shm_get(void)
{
  struct proc *p = myproc();
  shminit_lazy();

  acquire(&shm_lock);
  if(shm_page == 0){
    shm_page = kalloc();
    if(shm_page == 0){
      release(&shm_lock);
      return 0;
    }
    memset(shm_page, 0, PGSIZE);
  }

  // Map SHM_VA in calling process page table
  if(mappages(p->pagetable, SHM_VA, PGSIZE, (uint64)shm_page, PTE_R | PTE_W | PTE_U) < 0){
    release(&shm_lock);
    return 0;
  }
  release(&shm_lock);
  return SHM_VA;
}

// Clone SHM mapping to child process during fork
int
uvmcopyshm(pagetable_t old, pagetable_t new)
{
  pte_t *pte;
  if((pte = walk(old, SHM_VA, 0)) == 0)
    return 0; // Not mapped in parent
  if((*pte & PTE_V) == 0)
    return 0;
  
  uint64 pa = PTE2PA(*pte);
  uint flags = PTE_FLAGS(*pte);

  if(mappages(new, SHM_VA, PGSIZE, pa, flags) != 0){
    return -1;
  }
  return 0;
}

// Unmap SHM without freeing underlying physical page
void
uvmunmapshm(pagetable_t pagetable)
{
  pte_t *pte = walk(pagetable, SHM_VA, 0);
  if(pte && (*pte & PTE_V)){
    *pte = 0;
  }
}