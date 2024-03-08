typedef struct _queens queens;

static const int BOARD_SIZE = 8;

queens * queens_new(int board_size);
void queens_free(queens *q);

void queens_randomize(queens *q);
queens * queens_random_successor(queens *q);
int queens_fitness(queens *q);
void queens_print(queens *q);
