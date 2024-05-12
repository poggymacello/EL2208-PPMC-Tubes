#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char** read_maze(char* filename) {
  FILE* fp = fopen(filename, "r");
  char buf[1024];
  
  if (fp == NULL) {
    printf("File tidak ditemukan.");
    return NULL;
  }
  
  int n_col = 0;
  while (fgets(buf, 1024, fp) != NULL) {
    n_col++;
  }

  char** maze = malloc(sizeof(char*) * n_col);
  int i = 0;
  rewind(fp);
  while (fgets(buf, 1024, fp) != NULL) {
    buf[strcspn(buf, "\n")] = 0;
    char* row = malloc(sizeof(char) * strlen(buf));
    strcpy(row, buf);
    maze[i] = row;
    i++;
  } 

  fclose(fp);
  return maze;
}

void printMatrix(int rows, int cols, char** matrix) { // debug -> (char (*(*)[N]))maze
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
}