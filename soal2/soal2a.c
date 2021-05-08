#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>

void main (int argc, char ** argv) {
  int row1 = 4;
  int col1 = 3;
  int matrix_a[row1][col1];
  for (int i = 0; i < row1; i++) {
    for (int j = 0; j < col1; j++) {
      scanf("%d", &matrix_a[i][j]);
    }
  }

  int row2 = 3;
  int col2 = 6;
  int row_result = 4;
  int col_result = 6;
  int matrix_result[4][6];
  for (int i = 0; i < row2; i++) {
    for (int j = 0; j < col2; j++) {
      int value;
      scanf("%d", &value);

      for (int k = 0; k < row1; k++) {
        if (i == 0) {
          matrix_result[k][j] = 0;  
        }
        matrix_result[k][j] += matrix_a[k][i] * value;
      }
    }
  }

  key_t sh_memory_key = 911;
  int sh_memory_id = shmget(sh_memory_key, sizeof(int[row_result][col_result]), IPC_CREAT | 0666);
  if (sh_memory_id == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  int (* result)[col_result];
  result = shmat(sh_memory_id, NULL, 0);
  if (result == (void *) -1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < row_result; i++) {
    for (int j = 0; j < col_result; j++) {
      result[i][j] = matrix_result[i][j];
      printf("%d ", result[i][j]);
    }
    printf("\n");
  }

  shmdt(result);
  // shmctl(sh_memory_id, IPC_RMID, NULL);
  exit(EXIT_SUCCESS);
}