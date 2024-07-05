# Lab 1 Grading Report

## Table of Contents
[[_TOC_]]

## Marking Comments
{notes}

## test0
Compilation & compatibility with evaluation framework ~ calls osKernelInit and printf 

### expected output
```
---- test0 ----
PASS: build successful
<serial timeout>
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test1
osTaskCreate ~ calls osTaskCreate to create tasks, verifies task.tid is updated, calls osTaskInfo to retrieve and inspect the TCB 

### expected output
```
---- test1 ----
PASS: osCreateTask updated the input TCB with a valid TID 1

Values retrieved by osTaskInfo:
ptask=0x8000b69
stack_high=0x20017c00
tid=1
state=0x1
stack_size=0x400
PASS: TCB populated correctly

<serial timeout>
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test2
osKernelStart ~ creates tasks and calls osKernelStart, verifies the first task is started first and other tasks follow 

### expected output
```
---- test2 ----
PASS: kernel jumped to 1st created task
task-1
task-2
task-1
task-2
...
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test3
Round robin scheduling ~ creates & runs several tasks, looks at printfs to verify the tasks take turns executing 

### expected output
```
---- test3 ----
task-1
task-2
task-3
task-1
task-2
task-3
task-1
task-2
task-3
...
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test4
 osTaskExit ~ creates & runs several tasks, one of which calls osTaskExit, verifies the scheduler skips it and the remaining tasks continue to take turns executing

### expected output
```
task-1
task-2
task-3
task-1
task-2
task-3
...
task-1
task-3
task-1
task-3
task-1
task-3
...
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test5
Context switching should preserve stack contents ~ creates & runs tasks that use its stack for data storage, verifies the data can still be accessed after tasks yield & resume

### expected output
```
task-1: hello!
task-2: world!
task-1: hello!
task-2: world!
task-1: hello!
task-2: world!
task-1: hello!
task-2: world!
...
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test6
Context switching should preserve registers ~ creates & runs a task that explicitly writes to some registers, verifies their values are consistent after the task yields & resumes

### expected output
```
task-1: 0
task-2: 0
task-1: 1
task-2: 1
task-1: 2
task-2: 2
task-1: 3
task-2: 3
task-1: 4
task-2: 4
...
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test7
Supports up to 15 user tasks ~ creates and runs 15 tasks, verifies all tasks are operational 

### expected output
```
---- test7 ----
task-1
task-2
task-3
task-4
task-5
task-6
task-7
task-8
task-9
task-10
task-11
task-12
task-13
task-14
task-15
...
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test8
osTaskCreate returns error if all TIDs are already in use

### expected output
```
---- test8 ----
exhausting task slots...
task 1 created 
task 2 created 
task 3 created 
task 4 created 
task 5 created 
task 6 created 
task 7 created 
task 8 created 
task 9 created 
task 10 created 
task 11 created 
task 12 created 
task 13 created 
task 14 created 
task 15 created 
task not created
PASS: created 15 tasks, which is not more than 16
<serial timeout>
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test9
osTaskCreate returns error if stack space is exhausted (even if TIDs are available)

### expected output
```
---- test9 ----
PASS: osCreateTask with impossible stack_size returns RTX_FAIL
<serial timeout>
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test10
Resources from exited task is available for reuse ~ creates large tasks until stack space is exhausted, then exits one task and creates another task of ≤ the vacated stack size, verifies new task took over stack space of the exited task 

### expected output
```
---- test10 ----
creating tasks with 0x800 stack size until stack space is exhausted...
created task with tid-1, stack_high=0x20017800
created task with tid-2, stack_high=0x20017000
created task with tid-3, stack_high=0x20016800
created task with tid-4, stack_high=0x20016000
created task with tid-5, stack_high=0x20015800
created task with tid-6, stack_high=0x20015000
created task with tid-7, stack_high=0x20014800
osCreateTask failed (expected)
exhausted stack space. now testing stack reuse... starting tasks...
task-1
task-2
task-3
stack_high of exiting task: 0x20016800
task exiting...
task-4
creating new task
stack_high of newly created task: 0x20016800
PASS: task stack reused
task-5
task-6
task-7
task-8
<serial timeout>
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test11
Kernel robustness ~ e.g. a task that recreates itself then exits should be able to repeat indefinitely -- 500 reps to pass this test

### expected output
```
task-1
task-1
task-1
task-1
task-1
task-1
task-1
task-1
task-1
task-1
task-1
task-1
...
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00

## test12
Kernel robustness ~ various yielding patterns

### expected output
```
---- test12 ----
task-1
task-2
task-2
task-3
task-3
task-3
...
<end of test>
```
### your output
```
<build failed>

```
### your score: 0.00/1.00





