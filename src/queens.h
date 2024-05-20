typedef struct _queens queens;

static const int BOARD_SIZE = 100;

void queens_free(queens *q);
queens * queens_initial(int board_size);

queens * queens_random_successor(queens *q);
int queens_fitness(queens *q);
void queens_print(queens *q);
