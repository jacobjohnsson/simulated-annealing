#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const int BOARD_SIZE = 8;
static const double FREEZE_POINT = 0.0001;

/* return a random number in [min, max] */
int
xrand(int min, int max)
{
  return rand() / (RAND_MAX / (max - min)) + min;
}

typedef struct _solution {
/* board: represents a solution, in other words, a board state.
 *
 * a board of size 4 is represented with an array of numbers:
 * [0, 1, 2, 3]. board[i] encodes at which row the queen in column i is
 * located.
 *
 * for example the board
 *   --------
 * 3|- - - Q |
 * 2|Q - - - |
 * 1|- - Q - |
 * 0|- Q - - |
 *   --------
 *   0 1 2 3
 * is encoded as [2, 0, 1, 3]
 * and its fitness function evaluates to 2 since the queens at (0,1) and (1,2)
 * can see eachother.
 *
 * a consequence of this encoding is that no queens can be located in the same
 * column.
 */
  char *board;
  int size;
} solution;

solution *
solution_new(int board_size)
{
  solution *s = malloc(sizeof(solution));
  s->board = calloc(sizeof(char), board_size);
  s->size = board_size;

  return s;
}

void
solution_free(solution *s)
{
  free(s->board);
  free(s);
}

void
solution_randomize(solution *s)
{
  for (int i = 0; i < BOARD_SIZE; i++) {
    s->board[i] = xrand(0, BOARD_SIZE);
  }
}

solution *
solution_random_successor(solution *s)
{
  solution *next = solution_new(BOARD_SIZE);
  memcpy(next->board, s->board, next->size * sizeof(char));

  int column = xrand(0, next->size);
  int new_row = xrand(0, next->size);
  next->board[column] = new_row;

  return next;
}

/* returns the number of queens that sees other queens */
int
solution_fitness(solution *s)
{
  int sum = 0;

  /* check same row */
  for (int col = 0; col < s->size - 1; col++) {
    for (int j = col + 1; j < s->size; j++) {
      if (s->board[col] == s->board[j]) {
        sum += 1;
      }
    }
  }

  /* check up diagonal */
  for (int col = 0; col < s->size - 1; col++) {
    for (int j = col + 1; j < s->size; j++) {
      if (s->board[col] + j - col == s->board[j]) {
        sum += 1;
      }
    }
  }

  /* check down diagonal */
  for (int col = 0; col < s->size - 1; col++) {
    for (int j = col + 1; j < s->size; j++) {
      if (s->board[col] - (j - col) == s->board[j]) {
        sum += 1;
      }
    }
  }

  //s->fitness = sum;

  return sum;
}

void
solution_print(solution *s)
{
  printf("[");
  for (int i = 0; i < s->size; i++) {
    printf("%d ", s->board[i]);
  }
  printf("]: %d\n", solution_fitness(s));
}

double
cooling_schedule (int step) {
  return 1.0 / (step);
}

bool
gamble(int diff, double temperature)
{
  double d = (double)diff;
  return (double)rand() / RAND_MAX < temperature * 1/diff;
}

solution *
simulated_annealing(solution *initial)
{
  solution *current = initial;
  solution *next;
  double temperature;
  double fitness_diff;
  int step;

  for (step = 1; ; step++) {
    temperature = cooling_schedule(step);
    if (temperature < FREEZE_POINT) {
      return current;
    }

    next = solution_random_successor(current);
    if (solution_fitness(next) == 0) {
      /* We've found the theoretically best solution */
      solution_free(current);
      return next;
    }

    fitness_diff = solution_fitness(next) - solution_fitness(current);
    if (fitness_diff < 0) {
      /* next is better than current so take next */
      solution_free(current);
      current = next;
    } else if (gamble(fitness_diff, temperature)) {
      /* accept a worse solution with some probability */
      solution_free(current);
      current = next;
    } else {
      /* stick with current solution */
      solution_free(next);
    }
  }

  printf("Iterations: %d\n", step);

  return current;
}

int main(int argc, char *argv)
{
  srand(1);
  //srand(time(NULL));
  int incorrect = 0;
  int i;

  for (i = 0; i < 10000; i++) {
    solution *initial = solution_new(BOARD_SIZE);
    solution_randomize(initial);
    //printf("Initial board:\n");
    //solution_print(initial);

    solution *s = simulated_annealing(initial);

    //printf("Final board:\n");
    //solution_print(s);

    /*
    if (s->fitness != 0) {
      incorrect++;
    }
    */
    solution_free(s);
  }

  //printf("%d / %d", incorrect, i);

  return 0;
}
