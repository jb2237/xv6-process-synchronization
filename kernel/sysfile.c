// Append to kernel/sysfile.c
uint64
sys_shm_get(void)
{
  return shm_get();
}