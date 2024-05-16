#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100

// Struktur untuk merepresentasikan titik dalam labirin
typedef struct {
    int x, y;
} Point;

// Struktur untuk menyimpan jalur yang terdiri dari beberapa titik
typedef struct {
    Point points[MAX * MAX];
    int length;
} Path;

// Deklarasi variabel global
char maze[MAX][MAX];
int rows, cols;
Point start, end;
Path paths[MAX];
int pathCount = 0;
int minPathLength = MAX * MAX, maxPathLength = 0;
Path minPath, maxPath;
int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

// Fungsi untuk membaca labirin dari file teks
void readMaze(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("File tidak ditemukan\n");
        exit(1);
    }

    rows = cols = 0;
    while (fgets(maze[rows], MAX, file)) {
        cols = strlen(maze[rows]) - 1;
        if (maze[rows][cols] == '\n') maze[rows][cols] = '\0';
        for (int j = 0; j < cols; j++) {
            if (maze[rows][j] == 'S') {
                start.x = rows;
                start.y = j;
            } else if (maze[rows][j] == 'E') {
                end.x = rows;
                end.y = j;
            }
        }
        rows++;
    }
    fclose(file);
}

// Fungsi untuk memeriksa apakah langkah ke posisi (x, y) sah
int isValidMove(int x, int y, int visited[MAX][MAX]) {
    return x >= 0 && y >= 0 && x < rows && y < cols && maze[x][y] != '#' && !visited[x][y];
}

// Fungsi rekursif untuk mencari semua jalur dari (x, y) ke titik akhir
void findPaths(int x, int y, int visited[MAX][MAX], Path currentPath) {
    if (x == end.x && y == end.y) {
        paths[pathCount++] = currentPath;
        if (currentPath.length < minPathLength) {
            minPathLength = currentPath.length;
            minPath = currentPath;
        }
        if (currentPath.length > maxPathLength) {
            maxPathLength = currentPath.length;
            maxPath = currentPath;
        }
        return;
    }

    for (int i = 0; i < 4; i++) {
        int newX = x + directions[i][0];
        int newY = y + directions[i][1];
        if (isValidMove(newX, newY, visited)) {
            visited[newX][newY] = 1;
            currentPath.points[currentPath.length++] = (Point){newX, newY};
            findPaths(newX, newY, visited, currentPath);
            visited[newX][newY] = 0;
            currentPath.length--;
        }
    }
}

// Fungsi untuk memulai pencarian jalur dari titik awal
void solveMaze() {
    int visited[MAX][MAX] = {0};
    Path initialPath = {{start}, 1};
    visited[start.x][start.y] = 1;
    findPaths(start.x, start.y, visited, initialPath);
}

// Fungsi untuk mencetak jalur
void printPath(Path path) {
    for (int i = 0; i < path.length; i++) {
        printf("(%d, %d)", path.points[i].x, path.points[i].y);
        if (i < path.length - 1) printf(" -> ");
    }
    printf("\n");
}

// Fungsi untuk mencetak hasil semua jalur yang ditemukan
void printResults() {
    printf("All possible paths from start to end:\n");
    for (int i = 0; i < pathCount; i++) {
        printf("Path %d: ", i + 1);
        printPath(paths[i]);
    }
    printf("Total number of paths: %d\n", pathCount);
    printf("Shortest path from start to end:\n");
    printPath(minPath);
    printf("Longest path from start to end:\n");
    printPath(maxPath);
}

// Fungsi utama
int main() {
    char filename[100];
    printf("Masukkan File Txt Struktur Maze: ");
    scanf("%s", filename);
    
    readMaze(filename);    // Membaca labirin dari file
    solveMaze();           // Mencari semua jalur dari start ke end
    printResults();        // Mencetak hasil pencarian jalur
    
    return 0;
}
