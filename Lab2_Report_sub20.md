# Lab 2 Grading Report

## Table of Contents
[[_TOC_]]

## test0
Compilation & compatibility with evaluation framework ~ calls osKernelInit, k_mem_init, and printf.

### expected output
```
---- test0 ----
PASS: build successful
<serial timeout>
<end of test>
```
### your output
```
---- test0 ----
PASS: build successful
<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test1
Allocate function is able to find free space if it exists.

### expected output
```
---- test1 ----
k_mem_alloc 933 bytes, ptr=0x2000a7e8
k_mem_alloc 743 bytes, ptr=0x2000abe8
k_mem_alloc 262 bytes, ptr=0x2000afe8
k_mem_alloc 529 bytes, ptr=0x2000b3e8
k_mem_alloc 700 bytes, ptr=0x2000b7e8
Validating buffer contents... 
PASS: no corrupted buffers 
PASS: all k_mem_alloc calls successful 
<serial timeout>
<end of test>
```
### your output
```
---- test1 ----
k_mem_alloc 933 bytes, ptr=0x2000a860
k_mem_alloc 743 bytes, ptr=0x2000ac60
k_mem_alloc 262 bytes, ptr=0x2000b060
k_mem_alloc 529 bytes, ptr=0x2000b460
k_mem_alloc 700 bytes, ptr=0x2000b860
Validating buffer contents... 
PASS: no corrupted buffers 
PASS: all k_mem_alloc calls successful 
<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test2
Deallocation function makes a block available for allocation again.

### expected output
```
---- test2 ----
allocating 4 bytes...
allocating large block...
deallocating the first block...
PASS: k_mem_dealloc returned OK
allocating 4 bytes again... should fit into the first spot
PASS: freed memory was used again

<serial timeout>
<end of test>
```
### your output
```
---- test2 ----
allocating 4 bytes...
allocating large block...
deallocating the first block...
PASS: k_mem_dealloc returned OK
allocating 4 bytes again... should fit into the first spot
PASS: freed memory was used again

<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test3
Deallocated blocks are coalesced together correctly, according to the buddy scheme.

### expected output
```
---- test3 ----
1 byte block allocated, ptr=0x2000b3d8
33 byte block allocated, ptr=0x2000b418
deallocating 0x2000b3d8
testing if 33 bytes can fit at 0x2000b3d8 
PASS: k_mem_dealloc coalesced with its buddy
257 byte block allocated, ptr=0x2000b5d8
deallocating 0x2000b418
testing if 257 bytes can fit at 0x2000b3d8 
PASS: k_mem_dealloc coalesced three iterations
<serial timeout>
<end of test>
```
### your output
```
---- test3 ----
1 byte block allocated, ptr=0x2000b450
33 byte block allocated, ptr=0x2000b490
deallocating 0x2000b450
testing if 33 bytes can fit at 0x2000b450 
PASS: k_mem_dealloc coalesced with its buddy
257 byte block allocated, ptr=0x2000b650
deallocating 0x2000b490
testing if 257 bytes can fit at 0x2000b450 
PASS: k_mem_dealloc coalesced three iterations
<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test4
Managed memory blocks are always powers of two in size, smallest block is 32B, which can fit a 4-byte allocation.

### expected output
```
---- test4 ----
1 byte block allocated, ptr=0x2000b478
4 byte block allocated, ptr=0x2000b498
PASS: smallest block is 32B
4 byte block allocated, ptr=0x2000b4b8
PASS: 4 bytes could fit into 32B block
k_mem_alloc 533 bytes, ptr=0x2000b878
k_mem_alloc 243 bytes, ptr=0x2000b678
k_mem_alloc 362 bytes, ptr=0x2000bc78
k_mem_alloc 329 bytes, ptr=0x2000be78
k_mem_alloc 300 bytes, ptr=0x2000c078
k_mem_alloc 708 bytes, ptr=0x2000c478
k_mem_alloc 52 bytes, ptr=0x2000b4f8
k_mem_alloc 156 bytes, ptr=0x2000b578
k_mem_alloc 656 bytes, ptr=0x2000c878
k_mem_alloc 19 bytes, ptr=0x2000c278
k_mem_alloc 711 bytes, ptr=0x2000cc78
k_mem_alloc 851 bytes, ptr=0x2000d078
k_mem_alloc 543 bytes, ptr=0x2000d478
k_mem_alloc 605 bytes, ptr=0x2000d878
k_mem_alloc 708 bytes, ptr=0x2000dc78
k_mem_alloc 293 bytes, ptr=0x2000e078
k_mem_alloc 30 bytes, ptr=0x2000c2b8
k_mem_alloc 66 bytes, ptr=0x2000c2f8
k_mem_alloc 569 bytes, ptr=0x2000e478
k_mem_alloc 332 bytes, ptr=0x2000e278
Validating block sizes...
PASS: all blocks could be power of 2
<serial timeout>
<end of test>

```
### your output
```
---- test4 ----
1 byte block allocated, ptr=0x200094f0
4 byte block allocated, ptr=0x20009510
PASS: smallest block is 32B
4 byte block allocated, ptr=0x20009530
PASS: 4 bytes could fit into 32B block
k_mem_alloc 533 bytes, ptr=0x200098f0
k_mem_alloc 243 bytes, ptr=0x200096f0
k_mem_alloc 362 bytes, ptr=0x20009cf0
k_mem_alloc 329 bytes, ptr=0x20009ef0
k_mem_alloc 300 bytes, ptr=0x2000a0f0
k_mem_alloc 708 bytes, ptr=0x2000a4f0
k_mem_alloc 52 bytes, ptr=0x20009570
k_mem_alloc 156 bytes, ptr=0x200095f0
k_mem_alloc 656 bytes, ptr=0x2000a8f0
k_mem_alloc 19 bytes, ptr=0x2000a2f0
k_mem_alloc 711 bytes, ptr=0x2000acf0
k_mem_alloc 851 bytes, ptr=0x2000b0f0
k_mem_alloc 543 bytes, ptr=0x2000b4f0
k_mem_alloc 605 bytes, ptr=0x2000b8f0
k_mem_alloc 708 bytes, ptr=0x2000bcf0
k_mem_alloc 293 bytes, ptr=0x2000c0f0
k_mem_alloc 30 bytes, ptr=0x2000a330
k_mem_alloc 66 bytes, ptr=0x2000a370
k_mem_alloc 569 bytes, ptr=0x2000c4f0
k_mem_alloc 332 bytes, ptr=0x2000c2f0
Validating block sizes...
PASS: all blocks could be power of 2
<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test5
32KB of heap memory are managed; bonus awarded if full heap size is managed

### expected output
```
---- test5 ----

allocating large block that fills up the heap...
PASS: largest block is 32744 bytes

checking if bonus was attempted...
found additional capacity of 0B
test completed
<serial timeout>
<end of test>
```
### your output
```
---- test5 ----

allocating large block that fills up the heap...
PASS: largest block is 32752 bytes

checking if bonus was attempted...
found additional capacity of 0B
test completed
<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test6
Deallocating a random pointer will return an error.

### expected output
```
---- test6 ----
allocating one block, then try to deallocate a pointer outside of this block...
PASS: k_mem_dealloc returned error (expected)
allocating large block...
deallocating a pointer part way in the large block
PASS: k_mem_dealloc returned error (expected)
try to deallocate a valid pointer twice...
PASS: k_mem_dealloc returned error on second call (expected)
<serial timeout>
<end of test>
```
### your output
```
---- test6 ----
allocating one block, then try to deallocate a pointer outside of this block...
PASS: k_mem_dealloc returned error (expected)
allocating large block...
deallocating a pointer part way in the large block
PASS: k_mem_dealloc returned error (expected)
try to deallocate a valid pointer twice...
PASS: k_mem_dealloc returned error on second call (expected)
<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test7
Only the owner can free a block of memory (this will require using the kernel to run 2 tasks and calling allocate & deallocate from those tasks).

### expected output
```
allocating from first task
deallocating from first task
PASS: k_mem_dealloc successful
allocating again from first task
deallocating from second task
PASS: k_mem_dealloc returned error (expected)
<serial timeout>
<end of test>
```
### your output
```
allocating from first task
deallocating from first task
PASS: k_mem_dealloc successful
allocating again from first task
deallocating from second task
PASS: k_mem_dealloc returned error (expected)
<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test8
The entire heap space should be back in one piece after deallocating everything (bonus version should be back to starting state, shown below)

### expected output
```
---- test8 ----
Maximum allocation accepted at the start = 32756 bytes
Found additional capacity of 2624B
Deallocating everything...

Commence random usage

itr=0, alloc 933 bytes, ptr=0x200135c4
itr=1, alloc 743 bytes, ptr=0x200139c4
itr=2, alloc 262 bytes, ptr=0x20013dc4
also dealloc mem from itr 1, ptr=0x200139c4, ~743 bytes
itr=3, alloc 700 bytes, ptr=0x200139c4
itr=4, alloc 508 bytes, ptr=0x2000b5c4
also dealloc mem from itr 0, ptr=0x200135c4, ~933 bytes
itr=5, alloc 256 bytes, ptr=0x2000b9c4
itr=6, alloc 256 bytes, ptr=0x2000bbc4
[...]
itr=47, alloc 352 bytes, ptr=0x2000f1c4
itr=48, alloc 665 bytes, ptr=0x2000cdc4
also dealloc mem from itr 47, ptr=0x2000f1c4, ~352 bytes
itr=49, alloc 738 bytes, ptr=0x200139c4
now deallocate everything...
dealloc mem from itr 7, ptr=0x2000bdc4, ~351 bytes
dealloc mem from itr 8, ptr=0x2000c1c4, ~843 bytes
[...]
dealloc mem from itr 46, ptr=0x20013fc4, ~7 bytes
dealloc mem from itr 48, ptr=0x2000cdc4, ~665 bytes
dealloc mem from itr 49, ptr=0x200139c4, ~738 bytes
retest capacity...

Maximum allocation accepted at the end = 32756 bytes
Found additional capacity of 2624B
PASS: heap space is same capacity as the start
<serial timeout>
<end of test>
```
### your output
```
---- test8 ----
Maximum allocation accepted at the start = 32752 bytes
Found additional capacity of 0B
Deallocating everything...

Commence random usage

itr=0, alloc 933 bytes, ptr=0x200095d8
itr=1, alloc 743 bytes, ptr=0x200099d8
itr=2, alloc 262 bytes, ptr=0x20009dd8
also dealloc mem from itr 1, ptr=0x200099d8, ~743 bytes
itr=3, alloc 700 bytes, ptr=0x200099d8
itr=4, alloc 508 bytes, ptr=0x2000a1d8
also dealloc mem from itr 0, ptr=0x200095d8, ~933 bytes
itr=5, alloc 256 bytes, ptr=0x20009fd8
itr=6, alloc 256 bytes, ptr=0x200095d8
also dealloc mem from itr 3, ptr=0x200099d8, ~700 bytes
itr=7, alloc 351 bytes, ptr=0x200097d8
itr=8, alloc 843 bytes, ptr=0x200099d8
also dealloc mem from itr 4, ptr=0x2000a1d8, ~508 bytes
itr=9, alloc 393 bytes, ptr=0x2000a1d8
itr=10, alloc 330 bytes, ptr=0x2000a3d8
also dealloc mem from itr 6, ptr=0x200095d8, ~256 bytes
itr=11, alloc 169 bytes, ptr=0x200095d8
itr=12, alloc 932 bytes, ptr=0x2000a5d8
also dealloc mem from itr 11, ptr=0x200095d8, ~169 bytes
itr=13, alloc 972 bytes, ptr=0x2000a9d8
itr=14, alloc 868 bytes, ptr=0x2000add8
also dealloc mem from itr 5, ptr=0x20009fd8, ~256 bytes
itr=15, alloc 549 bytes, ptr=0x2000b1d8
itr=16, alloc 592 bytes, ptr=0x2000b5d8
also dealloc mem from itr 9, ptr=0x2000a1d8, ~393 bytes
itr=17, alloc 551 bytes, ptr=0x2000b9d8
itr=18, alloc 427 bytes, ptr=0x2000a1d8
also dealloc mem from itr 16, ptr=0x2000b5d8, ~592 bytes
itr=19, alloc 624 bytes, ptr=0x2000b5d8
itr=20, alloc 635 bytes, ptr=0x2000bdd8
also dealloc mem from itr 10, ptr=0x2000a3d8, ~330 bytes
itr=21, alloc 862 bytes, ptr=0x2000c1d8
itr=22, alloc 484 bytes, ptr=0x2000a3d8
also dealloc mem from itr 13, ptr=0x2000a9d8, ~972 bytes
itr=23, alloc 301 bytes, ptr=0x20009fd8
itr=24, alloc 710 bytes, ptr=0x2000a9d8
also dealloc mem from itr 12, ptr=0x2000a5d8, ~932 bytes
itr=25, alloc 876 bytes, ptr=0x2000a5d8
itr=26, alloc 431 bytes, ptr=0x200095d8
also dealloc mem from itr 19, ptr=0x2000b5d8, ~624 bytes
itr=27, alloc 397 bytes, ptr=0x2000b5d8
itr=28, alloc 675 bytes, ptr=0x2000c5d8
also dealloc mem from itr 2, ptr=0x20009dd8, ~262 bytes
itr=29, alloc 344 bytes, ptr=0x20009dd8
itr=30, alloc 134 bytes, ptr=0x2000b7d8
also dealloc mem from itr 25, ptr=0x2000a5d8, ~876 bytes
itr=31, alloc 629 bytes, ptr=0x2000a5d8
itr=32, alloc 30 bytes, ptr=0x2000b8d8
also dealloc mem from itr 23, ptr=0x20009fd8, ~301 bytes
itr=33, alloc 126 bytes, ptr=0x20009fd8
itr=34, alloc 743 bytes, ptr=0x2000c9d8
also dealloc mem from itr 32, ptr=0x2000b8d8, ~30 bytes
itr=35, alloc 256 bytes, ptr=0x2000cdd8
itr=36, alloc 932 bytes, ptr=0x2000d1d8
also dealloc mem from itr 34, ptr=0x2000c9d8, ~743 bytes
itr=37, alloc 509 bytes, ptr=0x2000c9d8
itr=38, alloc 119 bytes, ptr=0x2000b8d8
also dealloc mem from itr 31, ptr=0x2000a5d8, ~629 bytes
itr=39, alloc 327 bytes, ptr=0x2000cfd8
itr=40, alloc 719 bytes, ptr=0x2000a5d8
also dealloc mem from itr 17, ptr=0x2000b9d8, ~551 bytes
itr=41, alloc 824 bytes, ptr=0x2000b9d8
itr=42, alloc 596 bytes, ptr=0x2000d5d8
also dealloc mem from itr 27, ptr=0x2000b5d8, ~397 bytes
itr=43, alloc 356 bytes, ptr=0x2000b5d8
itr=44, alloc 184 bytes, ptr=0x2000a0d8
also dealloc mem from itr 41, ptr=0x2000b9d8, ~824 bytes
itr=45, alloc 245 bytes, ptr=0x2000b9d8
itr=46, alloc 7 bytes, ptr=0x2000bbd8
also dealloc mem from itr 40, ptr=0x2000a5d8, ~719 bytes
itr=47, alloc 352 bytes, ptr=0x2000a5d8
itr=48, alloc 665 bytes, ptr=0x2000d9d8
also dealloc mem from itr 47, ptr=0x2000a5d8, ~352 bytes
itr=49, alloc 738 bytes, ptr=0x2000a5d8
now deallocate everything...
dealloc mem from itr 7, ptr=0x200097d8, ~351 bytes
dealloc mem from itr 8, ptr=0x200099d8, ~843 bytes
dealloc mem from itr 14, ptr=0x2000add8, ~868 bytes
dealloc mem from itr 15, ptr=0x2000b1d8, ~549 bytes
dealloc mem from itr 18, ptr=0x2000a1d8, ~427 bytes
dealloc mem from itr 20, ptr=0x2000bdd8, ~635 bytes
dealloc mem from itr 21, ptr=0x2000c1d8, ~862 bytes
dealloc mem from itr 22, ptr=0x2000a3d8, ~484 bytes
dealloc mem from itr 24, ptr=0x2000a9d8, ~710 bytes
dealloc mem from itr 26, ptr=0x200095d8, ~431 bytes
dealloc mem from itr 28, ptr=0x2000c5d8, ~675 bytes
dealloc mem from itr 29, ptr=0x20009dd8, ~344 bytes
dealloc mem from itr 30, ptr=0x2000b7d8, ~134 bytes
dealloc mem from itr 33, ptr=0x20009fd8, ~126 bytes
dealloc mem from itr 35, ptr=0x2000cdd8, ~256 bytes
dealloc mem from itr 36, ptr=0x2000d1d8, ~932 bytes
dealloc mem from itr 37, ptr=0x2000c9d8, ~509 bytes
dealloc mem from itr 38, ptr=0x2000b8d8, ~119 bytes
dealloc mem from itr 39, ptr=0x2000cfd8, ~327 bytes
dealloc mem from itr 42, ptr=0x2000d5d8, ~596 bytes
dealloc mem from itr 43, ptr=0x2000b5d8, ~356 bytes
dealloc mem from itr 44, ptr=0x2000a0d8, ~184 bytes
dealloc mem from itr 45, ptr=0x2000b9d8, ~245 bytes
dealloc mem from itr 46, ptr=0x2000bbd8, ~7 bytes
dealloc mem from itr 48, ptr=0x2000d9d8, ~665 bytes
dealloc mem from itr 49, ptr=0x2000a5d8, ~738 bytes
retest capacity...

Maximum allocation accepted at the end = 32752 bytes
Found additional capacity of 0B
PASS: heap space is same capacity as the start
<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test9
Execution time over a series of allocations; excution time over a series of deallocations. (Thresholds are calculated as the midpoint between the compiler's implementation and linear search through all blocks.)

### expected output
```
---- test9 ----
Using DWT for timing

k_mem_alloc runtime=80901 [this is the threshold]
Time per iteration:
... 

k_mem_dealloc runtime=65369 [this is the threshold]
Time per iteration:
... 

<serial timeout>
<end of test>

```
### your output
```
---- test9 ----
Using DWT for timing

k_mem_alloc runtime=46214
Time per iteration:
2822, 171, 467, 171, 727, 171, 467, 171, 987, 171, 467, 171, 727, 171, 467, 171, 1247, 171, 467, 171, 727, 171, 467, 171, 987, 171, 467, 171, 727, 171, 467, 171, 1507, 171, 467, 171, 727, 171, 467, 171, 987, 171, 467, 171, 727, 171, 467, 171, 1247, 171, 467, 171, 727, 171, 467, 171, 987, 171, 467, 171, 727, 171, 467, 171, 1767, 171, 467, 171, 727, 171, 467, 171, 987, 171, 467, 171, 727, 171, 467, 171, 1247, 171, 467, 171, 727, 171, 467, 171, 987, 171, 467, 171, 727, 171, 467, 171, 1507, 171, 467, 170, 

k_mem_dealloc runtime=51322
Time per iteration:
252, 503, 239, 1553, 239, 497, 239, 761, 239, 497, 239, 1025, 239, 497, 239, 761, 239, 497, 239, 1289, 239, 497, 239, 761, 239, 497, 239, 1025, 239, 497, 239, 761, 239, 497, 239, 1817, 239, 497, 239, 761, 239, 497, 239, 1025, 239, 497, 239, 761, 239, 497, 239, 1289, 239, 497, 239, 761, 239, 497, 239, 1025, 239, 497, 239, 761, 239, 497, 239, 1553, 239, 497, 239, 761, 239, 497, 239, 1025, 239, 497, 239, 761, 239, 497, 239, 1289, 239, 497, 239, 761, 239, 497, 239, 1025, 239, 497, 239, 761, 239, 497, 239, 2856, 

<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## test10
Execution time performance over random usage: allocations with random sizes interspersed with deallocations with random (valid) addresses; bonus awarded if top 10% in class.

### expected output
```
---- test10 ----
Using DWT for timing

Total runtime=69459.5 [this is the threshold]
Time per iteration (^ denotes deallocations):
...

<serial timeout>
<end of test>
```
### your output
```
---- test10 ----
Using DWT for timing

Total runtime=58012
Time per iteration (^ denotes deallocations):
2593, 2641^, 2584, 504, 208, 494, 516^, 504, 208, 208, 754, 262^, 207, 764, 208, 494, 516^, 504, 208, 1284, 198, 252^, 208, 504, 208, 198, 252^, 208, 504, 208, 754, 268^, 207, 208, 764, 198, 268^, 763, 208, 208, 1534, 1572^, 1284, 208, 764, 198, 258^, 208, 208, 764, 494, 268^, 504, 207, 208, 198, 258^, 208, 504, 208, 198, 258^, 764, 1024, 208, 198, 252^, 504, 208, 208, 494, 252^, 208, 208, 1284, 198, 262^, 208, 764, 503, 198, 268^, 207, 208, 504, 198, 258^, 1804, 208, 208, 754, 268^, 504, 208, 208, 1014, 268^, 207, 207, 208, 494, 268^, 208, 207, 1024, 198, 258^, 208, 208, 504, 494, 268^, 208, 764, 504, 198, 258^, 208, 764, 208, 198, 252^, 208, 1284, 207, 

<serial timeout>
<end of test>

```
### your score: 1.00/1.00

## Test 5 with build workaround
If your test 5 above did not build and this test builds, then this is your test 5 score. 
(It is normal if only one of test 5 or 11 manages to build). 

### expected output
```
---- test5 ----

allocating large block that fills up the heap...
PASS: largest block is 32744 bytes

checking if bonus was attempted...
found additional capacity of 0B
test completed
<serial timeout>
<end of test>
```
### your output
```
---- test5 ----

allocating large block that fills up the heap...
PASS: largest block is 32752 bytes

checking if bonus was attempted...
found additional capacity of 0B
test completed
<serial timeout>
<end of test>

```
### your score: 1.00/1.00



