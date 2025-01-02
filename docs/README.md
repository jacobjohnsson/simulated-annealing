# Perf Workshop

In this workshop you will learn the basics of **perf**. In the first part we
will examine a short lived program by profiling it with perf. 

In part two, we will take a closer look on flame graphs...

## Part One

### Simulated Annealing in a Nutshell

When trying to solve a hard problem it is sometimes very difficult to find an
exact solution. So instead, we design algorithms to approximate an exact
solution. A very simple example of such an algorithm is the *Hill Climbing*
algorithm. The Hill Climbing algorithm starts with an arbitrary solution (no
matter how terrible, we just need a valid state) to the problem and examines
that solution's neighbors. If any of the neighbors are better, keep it and
continue looking for better neighbors. If no better neighbors are found, the
algorithm is done and the current solution is the best solution so far. The
Hill Climbing algorithm can easily get stuck on a local maximum with zero
chance of escaping.

*Simulated Annealing* attempts to remedy this issue by accepting a worse
solution with some probability that depends on a temperature and the fitness (or
score) of the solution. At every iteration, the temperature will decrease
according to a schedule, thereby decreasing the probability of accepting a worse
solution. If the temperature is lowered slowly, the probability of finding the
optimal solution approaches 1, which is pretty cool I think (please excuse the
pun).

    f SimulatedAnnealing(problem, schedule)
      current = new_problem_instance(problem)
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

The drawbacks are of course that the algorithm may get stuck on a local optimum
instead of the global optimum and that it is difficult to apply in continuous
state spaces. The advantages of these types of algorithms (local search
heuristic algorithms) are that they require very little constant memory, as long
as one doesn't care about the path to the solution. They can also be aborted
mid-run and produce the best solution so far.

### 8-Queens

8-Queens is a toy problem where the goal is to place 8 queens on a chess board
such that no queen attack any other queen. Remember that in chess a queen
attacks any other queen if it is in the same row, column or diagonal. An
attempt at a solution:

    Q . . . . . . .
    . . . . Q . . .
    . Q . . . . . .
    . . . . . Q . .
    . . Q . . . . .
    . . . . . . . Q
    . . . Q . . . .
    . . . . . . Q .

This is not a proper solution because two pairs of queens attack each other.

We only need to provide a **cooling schedule**, a way of **generating
neighbors** and a **fitness evaluation function** in order to solve a problem
using simulated annealing. But before we define those things, we start with the
encoding:

A board of size 4 will be represented with an array of numbers: [0, 1, 2, 3].
board[i] encodes at which row the queens in column i is located. For example,
the board

      --------
    3|- - - Q |
    2|Q - - - |
    1|- - Q - |
    0|- Q - - |
      --------
      0 1 2 3

is encoded as [2, 0, 1, 3]. This is easily extended to larger boards. A
consequence of this encoding is that no queens can be located in the same column
since it cannot be encoded.

While reviewing the code you *will* find many inefficiencies, that's the point.
**Resist the urge to improve upon the code until you have identified the culprit
that is responsible for the largest inefficiency.**

#### Cooling Schedule

Two cooling schedules are provided:

Cooling schedule 1

    1 / step

Cooling schedule 2

    0.25 / log(1 + step)

Feel free to test both or implement your own.

#### Neighbor Generation

Is done by randomly selecting a new row for a randomly selected queen. In other
words, moving a queen up or down.

#### Fitness Evaluation Function

The current fitness function returns the number of queen pairs that are *not*
attacking each other. The fitness value for the board above would be five.

### Hyperfine

A command-line benchmarking tool: https://github.com/sharkdp/hyperfine. It is
very easy to use for quick benchmarking.

#### Installation

Can be found on the github page, linked above.

#### Usage

We only need its basic functionality:

    hyperfine ./main.out

or

    hyperfine ./slow.out ./fast.out

to compare 2 versions.

I suggest that you use this tool (or any other benchmarking tool) to determine
if your changes has had the desired effect on execution time.

### Perf

With perf we can use the processor itself to figure out what it is up to. It
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

#### Debian Installation

    apt install linux-perf
    echo 1 > /proc/sys/kernel/perf_event_paranoid

#### Ubuntu Installation

    apt install operf
    echo 1 > /proc/sys/kernel/perf_event_paranoid

#### Usage

Now it's time to get our hands dirty.

We will measure the program running simulated annealing to solve the 8-queens
problem described above. This program is optimized for readability and
"un-performance". Feel free to improve upon it *after* you have identified
issues using a tool.

To include source code in the annotations the application must be compiled with
-ggdb.

##### Step 0

Familiarize yourself with the source code and run the program.

The program is compiled using 'make' which creates ./main.out.

Running the program will produce output that shows the results of attempting to
solve the queens problem a couple of times.

##### Step 1

Begin by recording the program:

                    name of event to be profiled (cycles, branch-misses,
                    alignment-faults, L1-icache-loads-misses, ...). Specify
                    multiple events by separating them with commas.
                    ^
                    |
    perf record -e <EVENT> <command>
                            |
                            v
                            name of the program

This creates a file, 'perf.data' in the current directory when the command has
finished.

For example:

    perf record -e cycles ./main.out

The output is saved in a file called **perf.data**.

##### Step 2

We can examine the data by running

    perf report

which presents us with all symbols, and the number of times they were sampled
(among other stuff).

The second useful tool is

    perf annotate <symbol_name>
                   |
                   v
                   For example 'queens_fitness'

which annotates the source code. If the output seems alien it can be useful to
disable optimizations.

If one does this with the simulated annealing program one should find that the
program spends about 99 % of its time in *queens_fitness()*.

##### Step 3

Use perf to determine why (or rule out why *not*) *queens_fitness()* takes so
much time.

Write down your thoughts, we will discuss your findings!

    TIP 1: Use 'perf list' to find other events than 'cycles'.

##### Step 4

Try to remedy the problem by modifying the source code.

    TIP 2: In src/main.c:main it is possible to either randomize the seed or
    hard code it. By hardcoding the seed you can verify that the programs
    behaviour has not changed. By randomizing the seed you can be confident that
    you haven't introduced some weird edge-case behaviour. Notice that there are
    no unittests.


    TIP 3: In src/main.c:11 You can modify the number of runs the program will
    do. This can be useful if you want to get more samples from perf, or a
    narrower confidence interval from hyperfine.

Use hyperfine (or other benchmarking tools) to make sure your change actually
improved running time.

Write down what you changed, *why* you made that change and what the result of
the change was.

## Part Two

This second part of the workshop focuses on _flame graphs_. We will
generate CPU frame graphs for a somewhat "real-life" program (a
GStreamer pipeline), aiming to find bottlenecks that are candidates
for optimization.

### Flame Graphs

Flame graphs is a way of visualizing **perf** data, for better
overview and inspection possibilities. There are tools, described at
[Brendan Gregg's site](https://www.brendangregg.com/flamegraphs.html),
for generating interactive SVGs, where you can search for relevant
parts of the data, and zoom in on them.

#### Installation

In addition to the **perf** installation, two manual steps are needed:

    sudo mkdir /usr/share/d3-flame-graph
    sudo wget -O /usr/share/d3-flame-graph/d3-flamegraph-base.html https://cdn.jsdelivr.net/npm/d3-flame-graph@4/dist/templates/d3-flamegraph-base.html

#### Usage

Generate a CPU flame graph for the execution of the **ls** command:

    perf record -e cycles --call-graph dwarf ls
    perf script report flamegraph

Open **flamegraph.html** in your browser. The graph shows percentages of
sampled CPU cycles where **ls** has been executing inside different
functions. A function called by another one is placed above the
latter, so the call stack goes upwards. The details of different
functions is not important here. See how you can zoom in on one
function by clicking on it, and search for symbols using the search
field.

We use DWARF mode for call graphs. See the **--call-graph** section of
the **perf record** man page (`man perf record`).

### A GStreamer Pipeline

Our object of investigation will be a
[GStreamer](https://gstreamer.freedesktop.org/) pipeline that streams
test video to a window. GStreamer is an open-source multimedia
framework.

#### Installation

If you don't already have the build tools Meson and Ninja (check using
`which meson ninja`), install them:

    apt install meson ninja-build

Install needed development packages:

    apt install libxv-dev

Clone GStreamer to a new git and check out revision 1.24.8:

    git clone https://gitlab.freedesktop.org/gstreamer/gstreamer.git
    cd gstreamer
    git checkout 1.24.8

Configure a slimmed-down GStreamer with compiler flags set for
**perf** measurements:

    meson setup -Dc_flags='-ggdb -O0' -Dgst-plugins-base:gl=disabled \
      -Dgood=disabled -Dbad=disabled -Dugly=disabled -Dlibav=disabled \
      -Ddevtools=enabled -Dges=disabled -Drtsp_server=disabled \
      -Dgst-examples=disabled -Dpython=disabled -Dtls=disabled \
      -Dlibnice=disabled -Dtests=disabled -Dexamples=disabled build

Finally, build GStreamer:

    ninja -C build

#### Usage

##### Step 1

Enter a development environment where your newly built GStreamer is
available:

    ninja -C build devenv

##### Step 2

Run a pipeline:

    gst-launch-1.0 videotestsrc pattern=snow ! 'video/x-raw, width=320, height=240' ! xvimagesink

##### Step 3

Generate a CPU framegraph for maybe 10 s of video, as for the **ls**
command above. Does the graph point towards code in any specific
GStreamer element that could be a bottleneck? Search the git for
symbols shown in the graph.

##### Step 4

Measure with **top** while playing video. Is the pipeline's CPU usage
significant?

##### Step 5

What impact does the video resolution have on the CPU friendliness
of the pipeline? Measure (graph and **top**) with higher resolutions,
such as 640x480, 1280x720 and 1920x1080.

##### Step 6

Is the performance the same for other **videotestsrc** patterns? See
`gst-inspect-1.0 videotestsrc`.

##### Step 7

Can you optimize **videotestsrc**? Edit the code, and rebuild with
`ninja -C build`, in a terminal outside the **devenv**. Changes can be
tested directly by running the pipeline inside the **devenv**.

Possibly, the scope of this optimization activity goes beyond the
scope of the workshop. If so, please continue for as long as you like
afterwards 😃
