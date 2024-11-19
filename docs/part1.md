---
tags:
  - presentation
  - perf
  - work
---
Welcome to Part 1
%%
This will hopefully be a fun workshop for everyone regardless of prior
knowledge. The idea is to explore the profiling tool perf, and it will be up to
you which level of complexity you target. Both newcomers and experienced
engineers are welcome.

But before the fun can begin we need to get some things out of the way. You are
expected to have either installed and verified that perf works well on you
machine, or paired yourself with a colleague with a perf installation.
Regardless I encourage you to work together but alone is also fine.

These slides will be available together with the source code you will be
working with in a moment.
%%

---

Perf
%%
This will NOT be a thorough introduction to perf! I will introduce you to the
basict so we can save time for you to explore it on your own. 

With perf we can essentially let the processor tell us what it is up to by
telling it to count some events such as clock cycles, data cache misses,
context switches and many more.
%%

```
perf list
```

%%
In this workshop we will use perf to figure out if we can improve the running
time of 2 programs, one in each part. In part 1 we will look at a program which
tries to solve the `8-queens` toy problem using simulated annealing. In part 2
David will let you loose on a Gstreamer Pipeline.

To save all of you a lot of time, I will describe the first program now.
%%

---
8-queens  


Place 8 queens on an 8x8 chess board such that no queen attacks another queen.

%%
In other words, no queens should share the same diagonal, row or column. There
are about 4 billion arrangements, but only 92 solutions. 


[https://en.wikipedia.org/wiki/Eight_queens_puzzle]
%%

---

8-queens impl.
%%
An arrangement of queens, aka a board, is simply a list of which row the queen
in the column at that index is. Perhaps an example would make it clearer:
%%
```bash
  --------
3|- - - Q |
2|Q - - - |
1|- - Q - |           is encoded as [2, 0, 1, 3]
0|- Q - - |
  --------
  0 1 2 3
```
%%
This actually kind of cheats since we cannot express an arrangement where two
queens are in the same column. 
%%

---
Simulated Annealing

```bash
fun hill-climber(problem)
    current = make-node(problem.initial-state)
    loop:
        neighbor = the highest value neighbor of current
        if neighbor.value <= current.value
            return current
        current = neighbor
```
%%
This is not a difficult problem to solve efficiently, so instead of looking at
an good solution, you will be examining a solution that uses simulated
annealing.

Before we go into simulated annealing though, lets look at a very simple way of
quickly getting to a *decent* solution: the hill-climbing algorithm.
1. The idea is to start with any valid arrangement of the problem for example
   imagine all the queens on a line on the first row, a shitty arrangement, but
   valid.
2. Examine the neighbors and pick the best one. (the 'move uphill' part)
3. If the best neighbor is as good as the current solution we are done
4. Otherwise repeat.

This algorithm is both amazing and horrible: If we start with a randomly
selected arrangement it only succeeds at finding a solution about 14 percent of
the time, however when it finds a solution it does so in only 4 steps on
average!

The reason that it fails most of the time is because it is prone to get stuck
on an local maximum, i.e all neighbors are worse but the current solution is
not the best.

Simulated annealing, the algorithm you will be looking at, tries to remedy this.
%%

---

Simulated Annealing
```bash
fun simulated-annealing(problem, schedule)
  current = make-node(problem.initial-state)
  for t = 1 to infinity:
    T = schedule(t)
    if T = 0:
      return current
    next = get_random_neighbor(current)
    fitness_diff = next.value - current.value
    if fitness_diff > 0:
      current = next
    else if accept_worse_solution(T, fitness_diff):
      current = next
```
%%
It does so by accepting a worse arrangement with some probability that
decreases with time and with the difference in the arrangements value.

At every iteration the *temperature* will decrease according to a schedule
which will lower the probability of accepting a worse arrangement. At some
point the temperature reaches 0 and we halt.
%%

---
Implementation
%%
In order to apply simulated annealing to the 8 queens problem we only need 3
things: a schedule,
%%

Schedule
```c
src/main.c

double
cooling_schedule (int step)
{
  return 1.0 / step;
}
```

---
Implementation
%%
a way of generating neighbors and
%%

neighbors
```c
src/queens.c

queens *
queens_random_successor(queens *q)
{
  ...
  int column = rand_from_to(0, next->size);
  int new_row = rand_from_to(0, next->size);
  next->board[column] = new_row;

  return next;
}
```

---
Implementation
%%
a fitness evaluation function.
%%

fitness evaluation
```c
src/queens.c

/* returns the number non-attacking *pairs* of queens */
int
queens_fitness(queens *q)
{
    ...
}
```

---

Implementation
```c
src/main.c

int seed = 1;
/* use random seed for testing edge cases */
//int seed = (int) time(NULL);
srand(seed);
```

%%
Since the algorithm includes randomness it is a good idea to use a fixed seed
to make sure your optimizations don't change the output when they are not
supposed to. But if you do make optimizations that do change the output (which
is fine) make sure to test your program several times with different seeds to
verify that it still works as expected.

One more thing, this implementation is far from optimized. You will see several
things that are prime targets for optimizing. But resist the urge to make
changes until you have verified that they have an impact with perf!
%%

---

Back to perf

---

Step 1:
```bash
                name of event to be profiled (cycles, branch-misses,
                alignment-faults, L1-icache-loads-misses, ...). Specify
                multiple events by separating them with commas.
                ^
                |
perf record -e <EVENT> <command>
                        |
                        v
                        name of the program
```

---

Example
```bash
perf record -e cycles ./main.out
```

creates

```bash
./perf.data
```


---

Step 2

```bash
perf report
```

%%
presents us with all symbols and the number of times they were sampled.
%%
```bash
perf annotate <symbol_name>
               |
               v
               For example 'queens_fitness'
```

---
Step 3

Explore

%%
Find out which part of the program takes time, and importantly: WHY?
Tip use `perf list`
%%


---
Step 4

Fix it

```bash
hyperfine ./main.out
hyperfine ./slow.out ./fast.out
```

%%
See if you can make the program run faster without changes the output of the
program!

Use a benchmarking tool such as hyperfine to determine if your changes had the
desired effect!
%%

---
Good luck!

```
https://github.com/jacobjohnsson/simulated-annealing
```
