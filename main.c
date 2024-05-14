#include "files.h"
#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  char filename[20];
  printf("Masukkan nama file input: ");
  scanf("%s", filename);

  int** maze;
  int row, col;
  Position* start = malloc(sizeof(Position));
  Position* dest = malloc(sizeof(Position));

  readMaze(filename, &maze, &row, &col, start, dest);
  // printMatrix(row, col, maze);
  // printf("\n");

  clock_t begin = clock();
  aStarSearch(maze, row, col, start, dest);
  clock_t end = clock();
  double time = (double)(end - begin)/CLOCKS_PER_SEC * 1000;
  printf("Time: %f ms\n", time);

  return 0;
}