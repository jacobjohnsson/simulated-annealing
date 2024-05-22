# Guide to some tools

## Operf

With Operf we can use the processor itself to figure out what it is up to. It
will do so by counting some events that it keeps track of. When that happens
it records which event was triggered and the address of the program at that
time. This information can then be collected and presented to the developer. In
other words, we are able to measure which instructions are executed to some
degree of accuracy.

Events include but are not limited to:

- clock cycles

- L1 data cache misses

- context switches

For more events, run 'perf list'.

### Debian Installation

    apt install linux-perf
    echo 1 > /proc/sys/kernel/perf_event_paranoid

### Ubuntu Installation

    apt install operf
    echo 1 > /proc/sys/kernel/perf_event_paranoid

### Debian Usage

We will measure a program running simulated annealing to solve the 8-queens
problem. This program optimized for readability and "un-performance". Feel free
to improve upon it *after* you have identified issues using a tool.

Begin by recording the program:

                    name of event to be profiled (cycles, branch-misses,
                    alignment-faults, L1-icache-loads-misses, ...)
                    ^
                    |
    perf record -e <EVENT> <command>
                            |
                            v
                            name of the program

This creates a file, 'perf.data' in the current directory.

For example:

    perf record -e cycles main.out

The output is saved in a directory called **perf.data**.

We can examine the data by running

  perf report

which presents us with all symbols, and the number of times they were sampled,
(among other stuff).

The second useful tools is

    perf annotate <symbol_name>
                   |
                   v
                   For example 'queens_fitness'

which annotates the source code. If the output seems alien it can be useful to
disable optimizations.

If one does this with the simulated annealing program one should find that the
program spends about 99 % of its time in *queens_fitness()*.

- Can you determine why?

### Ubuntu Usage

TODO

## Appendix

### Simulated Annealing in a Nutshell

When trying to solve a hard problem it is sometimes very difficult to find an
exact solution. So instead we design algorithms to approximate a good solution.
A very simple example of such an algorithm is the *Hill Climbing* algorithm. The
Hill Climbing algorithm starts with an arbitrary solution to the problem and
examines that solutions neghbours. If any of the neighbours are better, keep it
and continue looking for better neighbours. If no better neighbours are found
the algorithm is done and the current solution is the best solution so far. The
Hill Climbing algorithm can easily get stuck on a local maximum with zero chance
of escaping.

*Simulated Annealing* attempts to remedy this issue by accepting a worse
solution with some probability that depends on a temperature and the "badness"
of the solution. At every iteration the temperature will decrease according to a
schedule, thereby decreasing the probability of accepting a worse solution. If
the temperature is lowered slowly, the probability of finding the optimal
solution approaches 1, which is pretty cool I think (please excuse the pun).

    f SimulatedAnnealing(problem, schedule)
      current = new_problem_instance(problem)
      for t = 1 to infinity:
        T = schedule(t)
        if T = 0:
          return current
        fitness_diff = next.value - current.value
        next = randomly_selected_neighbour_of(current, fitness_diff)
        if fitness_diff > 0:
          current = next
        else if accept_worse_solution(T):
          current = next

The drawbacks are of course that the algorithm may get stuck on a local optimum
instead of the global optimum. The advantage of these types of algorithms (local
search heuristic algorithms) are that they require very little constant memory.
They can also be aborted mid-run and produce the so far best solution.

### 8-Queens

8-Queens is a toy problem where the goal is to place 8 queens on a chess board
such that no queens attack any other queen. Remember that a queen attacks any
other queen if it is in the same row, column or diagonal. An attemt at a
solution:

    Q . . . . . . .
    . . . . Q . . .
    . Q . . . . . .
    . . . . . Q . .
    . . Q . . . . .
    . . . . . . . Q
    . . . Q . . . .
    . . . . . . Q .

This is not a solution becase two pairs of queens attack eachother.

We only need to provide a **cooling schedule**, a way of **generating
neighbours** and a **fitness evaluation function** in order to solve this program
using simulated annealing. But before we define those things, we start with the
encoding:

A board of size 4 is represented with an array of numbers: [0, 1, 2, 3].
board[i] encodes at which row the queens in column i is located. For example the
board

      --------
    3|- - - Q |
    2|Q - - - |
    1|- - Q - |
    0|- Q - - |
      --------
      0 1 2 3

is encoded as [2, 3, 1, 3]. A consequense of this encoding is that no queens can
be located in the same column since it cannot be encoded.

While reviewing the code you *will* find many inefficiencies, that's the point.
Resist the urge to improve upon the code until you have identified the culprit
that is responsible for the largest inefficiency.

##### Cooling Schedule

Two cooling schedules are provided:

Cooling schedule 1

    1 / step

Cooling schedule 2

    0.25 / log(1 + step)

Feel free to test both or implement your own.

##### Neighbour Generation

Is done by randomly selecting a new row for a randomly selected queen. In other
words moving a queen up or down.

##### Fitness Evaluation Function

The current fitness function returns the number of queen pairs that are *not*
attacking eachother. The fitness value for the board above would be five.
