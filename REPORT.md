# OSN Assignment 4 Report

## Overview :

In this assignment, Various improvements have been made to the XV6 OS, by adding few system calls and multiple schedulers which can be chosen during runtime by using SCHEDULER=FLAG during runtime.

## Specification 1:

In this specification we were supposed to add a syscall "trace" to XV6.

the following files have been modified to implement trace syscall :

→ defs.h

→ proc.c

→ proc.h

→ syscall.c

→ syscall.h

→ sysproc.c

→ strace.c

→ user.h

→ usys.pl

A user program strace.c has been added that invokes the trace function. An extra variable mask has been added to structure proc in proc.h. The trace function initialises mask variable and sets it to the mask value given by the user, for every process. After the syscall() is invoked, we check the mask of every syscall invoked by the command and check if the sycall number'th bit is set in the mask given by the user. If the bit is set to 1, we output the process id, name of syscall, along with the values of all the arguments passed into the syscall along with the return value.  

## Specification 2:

a) FCFS ( First come First Serve ) :

This involves 2 parts, we first loop through the process table and find the process with the minimum creation time, this can be done by having a ctime variable declared in proc.h and set to ticks in allocproc() function just when the process is created.

We can find the minimum process as follows :

```c
struct proc *min_proc = 0;
for(p = proc;p < &proc[NPROC]; p++) {
  if(p->state == RUNNABLE) { 
    if(min_proc==0) {
      min_proc = p;
    }
    else {
      if(p->ctime < min_proc->ctime) {
        min_proc = p;
      }
    }
  }
}
```

After we have acquired the minimum process, we will assign this process to the CPU.

This is done as follows :

```c
acquire(&min_proc->lock);
if(min_proc->state == RUNNABLE) {
  c->proc = min_proc;
  min_proc->state = RUNNING;
  swtch(&c->context, &min_proc->context);
  c->proc = 0;
}
release(&min_proc->lock);
```

We also have to disable yield() in FCFS as it is non preemptive, this is done in trap.c. 

b) PBS (Priority Based Scheduler) :

We define a priority variable in proc.h that stores the static priority of every process. It is initialised to 60 everytime its first created in allocproc(). There is also a dynamic priority that is calculated for every process everytime the scheduler is invoked. The dynamic priority is calculated by the formula 

> DP = max(0, min(SP − niceness + 5, 100))
> 

Niceness is another value that is calculated by the formula :

> niceness = Int(sleeptime/(sleeptime+runtime))*10
> 

sleeptime and runtime are calculated with respect to the last time the process is scheduled. We use the variable last_run to denote runtime since last time its scheduled and sleeptime to denote sleeping time since last time its scheduled and a sleep_last variable that stores the time when it was last scheduled. sleep_last is initialised to ticks in sleep() function and sleeptime is incremented by (ticks - sleep_last) in wakeup() function. The last_run and sleeptime variables are again set to 0 everytime the process is scheduled.

sched_no variable has also been added to keep track of how many times a process is being scheduled since its creation which is used to break ties.

The function to calculate dynamic priority is as follows :

```c
int dp(struct proc * p) {
  int nice = 5;
  //int tot_time = ticks - p->lastscheduled;
  int sleeptime = p->sleeptime;
  int runtime = p->lastrun;
  int tot_time = sleeptime + runtime;
  if(tot_time) {
    nice = (sleeptime * 10) / tot_time ;
  }
  int compute = p->priority - nice + 5 > 100 ? 100 : p->priority - nice + 5;
  int ret = compute > 0 ? compute : 0;
  return ret;
}
```

The rules to check to get the process that should be scheduled is as follows :

```c
if(dp(low_proc) > dp(p)) {        
  low_proc = p;
}
else if(dp(low_proc) == dp(p) && p->schedno < low_proc->schedno) {
  low_proc = p;
}
else if(dp(low_proc) == dp(p) && p->schedno == low_proc->schedno && low_proc->ctime < p->ctime) {
  low_proc = p;
}
```

set_priority system call has also been added that helps us to set the priority of the given process and yields it depending on if its priority has been increased or decreased. The system call also returns the old priority.

c) Multilevel Feedback queue (MLFQ)

For the multilevel feedback queue, 5 queues have been created, the queue number has been stored for every process in queue variable. every time the runtime of the process in a particular queue exceeds 2^queue_no, the queue number of the process is incremented. We also note the last time a process in enqueued into a new queue to find its relative position with respect to other processes in the process table in the same queue. 

 

```c
for (p = proc; p < &proc[NPROC]; p++){
	acquire(&p->lock);
	if(p->state == RUNNABLE) {
	  if (p->lastrun >= q_ticks_max[p->queue]) {
	    if (p->queue != 4)  {
	      p->queue++;
	      p->last_enqueue_tick=ticks;
	      p->lastrun = 0;
	    }
	  }
	}
	release(&p->lock);
}
```

Ageing has also been implemented with the max value to be 200 ticks. If a queue is in a process for more than 200 ticks, it is promoted to a higher priority queue as follows :

```c
if(ticks - p->last_executed > 200) {
  if(p->queue!=0) {
    p->queue--;
    p->last_enqueue_tick=ticks;
    p->lastrun = 0;
  }
}
```

Finally, we check which process is to be run by looping through all the queues priority wise, starting from 0 priority, we loop through the process table to find the suitable process.

```c
for (p = proc; p < &proc[NPROC]; p++) {
  acquire(&p->lock);
  if (p->state == RUNNABLE) {
    if (proc_to_run == 0 || p->queue < proc_to_run->queue || (p->queue == proc_to_run->queue && p->last_enqueue_tick < proc_to_run->last_enqueue_tick)) {
      proc_to_run = p;
    }
  }
  release(&p->lock);
}
```

After a process is selected by the scheduler its wait time is set to 0 and it is given the complete time slice to execute.

### Exploiting MLFQ :

A user process can use a lot of the CPU time by leaving the queue voluntarily just before it runs out of its total execution time and then gets pushed back in the same queue. This would make it spend all it's time in the highest priority queue, blocking other processes which are in the lower priority queues.

## Specification 3:

a) For MLFQ :

ticks[i] for every process ensured to calculate the time spent by a process in a particular queue. wtime was calculated to be ticks - p→rtime - p→ctime. 

b) For PBS :

similar to mlfq, the priority of process was computed by the dp function and shown in the table.

## Analysis :

These are the results using the schedulertest.c provided on moodle.

Can be run by selection appropriate flag in make qemu and running schedulertest.

Default : RR →rtime 16, wtime 114

FCFS → rtime 39, wtime 47

PBS → rtime 17, wtime 107

MLFQ (on 1 CPU) → rtime 15, wtime 162