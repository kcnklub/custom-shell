

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void command_loop(void);
char *read_line(void);
char **parse_commands(char *);
int execute_command(char **);

int main(int argc, char **argv) {

  // load config

  // run command loop
  command_loop();

  return 0;
}

void command_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = read_line();
    args = parse_commands(line);
    status = execute_command(args);

    free(args);
    free(line);
  } while (status);
}

#define RL_BUFSIZE 1024
char *read_line(void) {
  int buf_size = RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buf_size);

  int c;

  if (!buffer) {
    // can't create buffer
    exit(-1);
  }

  while (1) {
    c = getchar();

    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    if (position >= buf_size) {
      buf_size += RL_BUFSIZE;
      buffer = realloc(buffer, buf_size);
      if (!buffer) {
        exit(-1);
      }
    }
  }
}

#define TOKEN_BUFF 64
#define TOKEN_DELIM " \t\r\n\a"
char **parse_commands(char *line) {
  int buf_size = TOKEN_BUFF;
  int position = 0;

  char **tokens = malloc(sizeof(char *) * buf_size);
  char *token;

  if (!tokens) {
    exit(-1);
  }

  token = strtok(line, TOKEN_DELIM);

  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= buf_size) {
      buf_size += TOKEN_BUFF;
      tokens = realloc(tokens, buf_size);
      if (!tokens) {
        exit(-1);
      }
    }

    token = strtok(NULL, TOKEN_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int execute_command(char **args) {
  pid_t pid;
  pid_t wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // child process
    if (execvp(args[0], args) == -1) {
      exit(-1);
    }
  } else if (pid < 0) {
    exit(-1);
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}
