#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main (int argc, char ** argv) {
  int pipe_ps_to_sort[2];
  if (pipe(pipe_ps_to_sort) == -1) {
    perror("pipe failed");
    exit(EXIT_FAILURE);
  }

  int pipe_sort_to_head[2];
  if (pipe(pipe_sort_to_head) == -1) {
    perror("pipe failed");
    exit(EXIT_FAILURE);
  }

  if (fork() == 0) {
    close(pipe_ps_to_sort[0]);
    dup2(pipe_ps_to_sort[1], 1);
    close(pipe_ps_to_sort[1]);
    
    char * args[] = {"ps", "aux", NULL};
    execv("/bin/ps", args);
  }
  
  if (fork() == 0) {
    close(pipe_ps_to_sort[1]);
    dup2(pipe_ps_to_sort[0], 0);
    close(pipe_ps_to_sort[0]);
    
    close(pipe_sort_to_head[0]);
    dup2(pipe_sort_to_head[1], 1);
    close(pipe_sort_to_head[1]);
    
    char * args[] = {"sort", "-nrk", "3,3", NULL};
    execv("/bin/sort", args);
  }

  close(pipe_ps_to_sort[0]);
  close(pipe_ps_to_sort[1]);

  if (fork() == 0) {
    close(pipe_sort_to_head[1]);
    dup2(pipe_sort_to_head[0], 0);
    close(pipe_sort_to_head[0]);
    
    char * args[] = {"head", "-5", NULL};
    execv("/bin/head", args);
  }

  close(pipe_sort_to_head[0]);
  close(pipe_sort_to_head[1]);
  
  exit(EXIT_SUCCESS);
}
