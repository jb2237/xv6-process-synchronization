# CS301 – Operating Systems Lab

## Assignment 5: Process Synchronization in xv6-riscv

This assignment focuses on **process synchronization in xv6-riscv**. Since the standard xv6 implementation does not provide shared memory or general-purpose user-level semaphores, these features were added to the kernel before implementing the synchronization problems.

## What Was Added

### Shared Memory

A `shm_get()` system call was added to allow parent and child processes to access the **same physical memory page**.

The shared page is mapped at `0x3F000000`. The `fork()` implementation was also modified so that the shared page is inherited by the child without creating a separate copy.

### Counting Semaphores

Kernel-level counting semaphores were implemented using xv6's `sleep()` and `wakeup()` mechanisms.

The following system calls were added:

```text
sem_init()
sem_wait()
sem_post()
```

A table of 32 semaphores is maintained in the kernel, with locks used to safely access each semaphore.

---

## Synchronization Problems

### 1. Peterson's Algorithm

**File:** `user/peterson.c`

Peterson's algorithm is used to provide mutual exclusion between a parent and child process.

* Uses `flag[2]`, `turn`, and a shared counter.
* Does not use kernel locks or semaphores for the critical section.
* A RISC-V memory fence is used to maintain correct memory ordering.
* Both processes perform 10 iterations.
* The final shared counter reaches **20**.

### 2. Producer-Consumer

**File:** `user/prodcons.c`

A circular buffer of size 5 is used for communication between a producer and consumer.

Three semaphores are used:

* `empty = 5` — tracks available buffer slots
* `full = 0` — tracks available items
* `mutex = 1` — protects the buffer

The producer generates 20 items, and the consumer removes them in **FIFO order**. Processes block when the buffer is full or empty.

### 3. Readers-Writers

**File:** `user/readwrite.c`

The **reader-preference** solution to the Readers-Writers problem is implemented.

* Multiple readers can access the shared resource simultaneously.
* Writers get exclusive access.
* `read_count` keeps track of active readers.
* Semaphores are used to protect `read_count` and control writer access.

The test uses **3 readers and 2 writers**.

### 4. Dining Philosophers

**File:** `user/dining.c`

The problem is implemented using **5 philosophers and 5 binary semaphores**, one for each chopstick.

To avoid deadlock, an asymmetric ordering strategy is used:

* Even philosophers pick the **left** chopstick first.
* Odd philosophers pick the **right** chopstick first.

Each philosopher completes 5 cycles of:

```text
THINKING → HUNGRY → EATING
```

without deadlock.

---

## Files Modified

### Kernel

```text
kernel/syscall.h
kernel/syscall.c
kernel/defs.h
kernel/sysfile.c
kernel/vm.c
kernel/proc.c
kernel/syssem.c
```

### User Programs

```text
user/user.h
user/usys.pl
user/peterson.c
user/prodcons.c
user/readwrite.c
user/dining.c
```

The `Makefile` was also updated to include the new kernel object and user programs.

---

## How to Run

From the xv6-riscv root directory:

```bash
make clean
make qemu
```

Then run the programs inside the xv6 shell:

```text
$ peterson
$ prodcons
$ readwrite
$ dining
```

To exit QEMU:

```text
Ctrl + A
X
```

---

## Output

Screenshots and test outputs are available in the `output_logs/` directory:

```text
output_logs/
├── Q1_output.png
├── Q2_output.png
├── Q3_output.png
└── Q4_output.png
```

These outputs demonstrate successful synchronization, correct shared-memory access, semaphore operation, FIFO behavior, concurrent readers, exclusive writers, and deadlock-free execution.

## Conclusion

This assignment demonstrates how **shared memory, semaphores, mutual exclusion, process blocking, and deadlock avoidance** can be implemented and used in xv6-riscv to solve classic process synchronization problems.
