#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
  int pos_x;
  int pos_y;
  int fact_limit;
} fact_args;

int (* result)[6] = {0};

void * count_factorial(void * args) {
  fact_args * fact_item = args;

  if (fact_item->fact_limit == 0) {
    result[fact_item->pos_x][fact_item->pos_y] = 0;
  } else {
    int num = result[fact_item->pos_x][fact_item->pos_y];
    int fact_num = num;
    
    for (int i = 1; i < fact_item->fact_limit; i++) {
      if ((--num) == 0) {
        break;
      } else {
        fact_num *= num;
      }
    }

    result[fact_item->pos_x][fact_item->pos_y] = fact_num;
  }

  free(fact_item);
}

void main (int argc, char ** argv) {
  int row_result = 4;
  int col_result = 6;
  int matrix_input[row_result][col_result];
  for (int i = 0; i < row_result; i++) {
    for (int j = 0; j < col_result; j++) {
      scanf("%d", &matrix_input[i][j]);
    }
  }

  key_t sh_memory_key = 911;
  int sh_memory_id = shmget(sh_memory_key, sizeof(int[row_result][col_result]), IPC_CREAT | 0666);
  if (sh_memory_id == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  result = shmat(sh_memory_id, NULL, 0);
  if (result == (void *) -1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  int fact_thread_index = 0;
  pthread_t fact_thread[row_result * col_result];
  for (int i = 0; i < row_result; i++) {
    for (int j = 0; j < col_result; j++) {
      fact_args * args = malloc(sizeof(fact_args *));
      args->fact_limit = matrix_input[i][j];
      args->pos_x = i;
      args->pos_y = j;

      if (pthread_create(&fact_thread[fact_thread_index], NULL, count_factorial, args)) {
        free(args);
        continue;
      }

      pthread_join(fact_thread[fact_thread_index++], NULL);
    }
  }

  for (int i = 0; i < row_result; i++) {
    for (int j = 0; j < col_result; j++) {
      printf("%d ", result[i][j]);
    }
    printf("\n");
  }

  shmdt(result);
  shmctl(sh_memory_id, IPC_RMID, NULL);
  exit(EXIT_SUCCESS);
}