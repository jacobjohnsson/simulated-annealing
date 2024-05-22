#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "queens.h"

static const double FREEZE_POINT = 0.024;
static const int N_RUNS = 5;

double
cooling_schedule (int step)
{
  return 1.0 / step;
}

double
log_cooling_schedule (int step)
{
  double c = 0.25;
  return c / log(1 + step);
}

bool
gamble(int diff, double temperature)
{
  double d = (double)diff;
  return (double)rand() / RAND_MAX < temperature * 1 / d;
}

int
optimal_fitness()
{
  int i = BOARD_SIZE - 1;
  int sum = 0;
  while (i > 0) {
    sum += i;
    i--;
  }
  return sum;
}

queens *
simulated_annealing(queens *initial)
{
  queens *current = initial;
  queens *next;
  double temperature;
  double fitness_diff;
  int step;

  int optimal = optimal_fitness();

  for (step = 1; ; step++) {
    temperature = log_cooling_schedule(step);
    if (temperature < FREEZE_POINT) {
      break;
    }

    next = queens_random_successor (current);

    if (queens_fitness(next) == optimal) {
      /* We've found the theoretically best solution */
      queens_free(current);
      current = next;
      break;
    }

    fitness_diff = queens_fitness(next) - queens_fitness(current);
    if (fitness_diff > 0) {
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
  int correct = 0;
  int i;

  int seed = 1;
  /* use random seed for testing edge cases */
  //int seed = (int) time(NULL);
  srand(seed);

  printf("#### Goal: %d ####\n", optimal_fitness());
  for (i = 0; i < N_RUNS; i++) {
    queens *initial = queens_initial(BOARD_SIZE);

    queens *s = simulated_annealing(initial);

    printf("Final board:\n");
    queens_print(s);

    if (queens_fitness(s) == optimal_fitness(BOARD_SIZE)) {
      correct++;
    }
    queens_free(s);
  }

  printf("%d / %d\n", correct, i);
  printf("seed was %d\n", seed);

  return 0;
}
