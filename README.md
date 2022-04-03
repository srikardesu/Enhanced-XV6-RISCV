# General Instructions :

This code is a modification of the xv-6 OS, and contains few extra syscalls like sys_trace, setpriority and also different types of schedulers which can be chosen during runtime.

There are also few helper user programs that invoke the syscalls and help in testing them. Waitx has been implemented and schedulertest.c has been added to test each scheduler.

# To run :

First make clean make to always run in a clean state.

> make clean
> 

> make
> 

u can now select the scheduler to be run with, the default scheduler is the already implemented one which is round robin.

> make qemu
> 

or

> make qemu SCHEDULER=RR
> 

these help us to use the default scheduler.

> make qemu SCHEDULER=FCFS
> 

helps us to use first come first serve scheduler.

> make qemu SCHEDULER=PBS
> 

helps us to use the priority based scheduler.

> make qemu SCHEDULER=MLFQ CPUS=1
> 

helps us to run the multilevel feedback queue scheduler on 1 CPU.