// Under // vm.c
uint64          shm_get(void);
int             uvmcopyshm(pagetable_t, pagetable_t);
void            uvmunmapshm(pagetable_t);

// Under // syssem.c (add new block)
void            seminit(void);