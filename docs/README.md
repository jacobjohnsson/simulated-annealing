# Guide to some tools

## Operf

With Operf we can use the processor itself to figure out what it is up to. It
will do so by counting some events that it keeps track of by raising
a **performance monitor exception** when a counter overflows. When that happens
it records which event was triggered and the address of the program at that
time. This information can then be collected and presented to the developer. In
other words, we are able to measure which instructions are executed to some
degree of accuracy.

Events include but are not limited to:

- clock cycles

    > Core cycles when the thread is not in halt state.

- arithmetic division cycles

    > Cycles when divide unit is busy executing divide or square root 
    > operations.

- resource stalls

    > Resource-related stall cycles

For more events, see ophelp. This depends entirely on the processor
For documentation, see man operf(1)

### Installation

    apt install operf
    echo 1 > /proc/sys/kernel/perf_event_paranoid

### Usage

We will measure a program running simulated annealing to solve the 8-queens
problem. This program optimized for readability and "un-performance". Feel free
to improve upon it *after* you have identified issues using a tool.

First issue

              > name of event to be profiled
              |                         > unit mask or name
              |                         |   > sample user code
              |                         |   |
    operf -e <event for cycles>:1000000:0:0:1 main.out
                                |         |   |
                                |         |   > name of the program
                                |         > sample kernel code
                                > sample rate

For example on an old Intel® Core™ i7-7700 CPU which outputs the following when
running ophelp:

    cpu_clk_unhalted: (counter: all)
         (min count: 2000003)
        Unit masks (default thread)
        ----------
        0x00: (name=thread) Core cycles when the thread is not in halt state
        0x01: (name=ref_tsc) Reference cycles when the core is not in halt state.
        0x00: (name=thread_p) Thread cycles when thread is not in halt state
        0x02: (name=thread_any) Core cycles when at least one thread on the physical core is not 
                  in halt state
        0x00: (name=thread_p_any) Core cycles when at least one thread on the physical core is 
                  not in halt state

we could run operf as 

    operf -e cpu_clk_unhalted:2000003:thread:0:1 main.out

The output is saved in a directory called **oprofile_data**.

By running 

  opreport -l

we are presented with all symbols, and the number of times they were sampled.

The second useful tools in 

    opannotate -s | less

which annotates the source code. It can be useful to disable optimizations at
this point.

If one does this with the simulated annealing program one should find that the
program spends about 99 % of its time in *queens_fitness()*. 

- Why?

### Subheading 2
