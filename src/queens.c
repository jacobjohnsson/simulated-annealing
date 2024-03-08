#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queens.h"

struct _queens {
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
};

/* return a random number in [min, max] */
static int
rand_from_to(int min, int max)
{
  return rand() / (RAND_MAX / (max - min)) + min;
}


queens *
queens_new(int board_size)
{
  queens *q = malloc(sizeof(queens));
  q->board = calloc(sizeof(char), board_size);
  q->size = board_size;

  return q;
}

void
queens_free(queens *q)
{
  free(q->board);
  free(q);
}

void
queens_randomize(queens *q)
{
  for (int i = 0; i < BOARD_SIZE; i++) {
    q->board[i] = rand_from_to(0, BOARD_SIZE);
  }
}

queens *
queens_random_successor(queens *q)
{
  queens *next = queens_new(BOARD_SIZE);
  memcpy(next->board, q->board, next->size * sizeof(char));

  int column = rand_from_to(0, next->size);
  int new_row = rand_from_to(0, next->size);
  next->board[column] = new_row;

  return next;
}

/* returns the number of queens that sees other queens */
int
queens_fitness(queens *q)
{
  int sum = 0;

  /* check same row */
  for (int col = 0; col < q->size - 1; col++) {
    for (int j = col + 1; j < q->size; j++) {
      if (q->board[col] == q->board[j]) {
        sum += 1;
      }
    }
  }

  /* check up diagonal */
  for (int col = 0; col < q->size - 1; col++) {
    for (int j = col + 1; j < q->size; j++) {
      if (q->board[col] + j - col == q->board[j]) {
        sum += 1;
      }
    }
  }

  /* check down diagonal */
  for (int col = 0; col < q->size - 1; col++) {
    for (int j = col + 1; j < q->size; j++) {
      if (q->board[col] - (j - col) == q->board[j]) {
        sum += 1;
      }
    }
  }

  return sum;
}

void
queens_print(queens *q)
{
  printf("[");
  for (int i = 0; i < q->size; i++) {
    printf("%d ", q->board[i]);
  }
  printf("]: %d\n", queens_fitness(q));
}

