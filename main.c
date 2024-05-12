#include "files.h"
#include <stdio.h>

int main() {
  char filename[20];
  printf("Masukkan nama file input: ");
  scanf("%s", filename);
  char** maze = readMaze(filename);

  printMatrix(11, 12, maze);
  return 0;
}