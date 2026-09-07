// Inside fork() right after:
// if(uvmcopy(p->pagetable, np->pagetable, p->sz) < 0){ ... }
if(uvmcopyshm(p->pagetable, np->pagetable) < 0){
  freeproc(np);
  release(&np->lock);
  return -1;
}