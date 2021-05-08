#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main (int argc, char ** argv) {
  // declare the first pipe
  int pipe_ps_to_sort[2];
  if (pipe(pipe_ps_to_sort) == -1) {
    perror("pipe failed");
    exit(EXIT_FAILURE);
  }

  // declare the second pipe
  int pipe_sort_to_head[2];
  if (pipe(pipe_sort_to_head) == -1) {
    perror("pipe failed");
    exit(EXIT_FAILURE);
  }

  // make a child process to perform first command
  if (fork() == 0) {
    // write to pipe
    close(pipe_ps_to_sort[0]);
    dup2(pipe_ps_to_sort[1], 1);
    close(pipe_ps_to_sort[1]);
    
    char * args[] = {"ps", "aux", NULL};
    execv("/bin/ps", args);
  }
  
  // make a child process to perform second command
  if (fork() == 0) {
    // read the result of first command
    close(pipe_ps_to_sort[1]);
    dup2(pipe_ps_to_sort[0], 0);
    close(pipe_ps_to_sort[0]);
    
    // write to pipe
    close(pipe_sort_to_head[0]);
    dup2(pipe_sort_to_head[1], 1);
    close(pipe_sort_to_head[1]);
    
    char * args[] = {"sort", "-nrk", "3,3", NULL};
    execv("/bin/sort", args);
  }

  // close the unused pipe
  close(pipe_ps_to_sort[0]);
  close(pipe_ps_to_sort[1]);

  // read the result of second command
  close(pipe_sort_to_head[1]);
  dup2(pipe_sort_to_head[0], 0);
  close(pipe_sort_to_head[0]);
  
  // perform the last command
  char * args[] = {"head", "-5", NULL};
  execv("/bin/head", args);
  
  exit(EXIT_SUCCESS);
}
