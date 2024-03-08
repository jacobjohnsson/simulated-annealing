#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "queens.h"

static const double FREEZE_POINT = 0.001;
static const int N_RUNS = 10;

double
cooling_schedule (int step) {
  return 1.0 / (step);
}

bool
gamble(int diff, double temperature)
{
  double d = (double)diff;
  return (double)rand() / RAND_MAX < temperature * 1 / d;
}

queens *
simulated_annealing(queens *initial)
{
  queens *current = initial;
  queens *next;
  double temperature;
  double fitness_diff;
  int step;

  for (step = 1; ; step++) {
    temperature = cooling_schedule(step);
    if (temperature < FREEZE_POINT) {
      return current;
    }

    next = queens_random_successor (current);
    if (queens_fitness(next) == 0) {
      /* We've found the theoretically best solution */
      queens_free(current);
      return next;
    }

    fitness_diff = queens_fitness(next) - queens_fitness(current);
    if (fitness_diff < 0) {
      /* next is better than current so take next */
      queens_free(current);
      current = next;
    } else if (gamble(fitness_diff, temperature)) {
      /* accept a worse solution with some probability */
      queens_free(current);
      current = next;
    } else {
      /* stick with current solution */
      queens_free(next);
    }
  }

  printf("Iterations: %d\n", step);

  return current;
}

int main(int argc, char **argv)
{
  srand(1);
  /* use random seed for testing the application */
  //srand(time(NULL));
  int correct = 0;
  int i;

  for (i = 0; i < N_RUNS; i++) {
    queens *initial = queens_initial(BOARD_SIZE);
    printf("---------------------\n");
    printf("Initial board:\n");
    queens_print(initial);

    queens *s = simulated_annealing(initial);

    printf("Final board:\n");
    queens_print(s);

    if (queens_fitness(s) == 0) {
      correct++;
    }
    queens_free(s);
  }

  printf("%d / %d\n", correct, i);

  return 0;
}
