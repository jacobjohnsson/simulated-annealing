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

### Debian installation

    apt install linux-perf
    echo 1 > /proc/sys/kernel/perf_event_paranoid

### Ubuntu installation

    apt install operf
    echo 1 > /proc/sys/kernel/perf_event_paranoid

### Debian usage

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

### Subheading 2
