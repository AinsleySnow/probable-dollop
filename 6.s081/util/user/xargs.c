#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFSIZE 512

int readline(char buf[])
{
  char c;
  int i = 0;

  while (read(0, &c, 1))
  {
    if (c != '\n')
      buf[i++] = c;
    else
      break;
  }

  buf[i] = 0;
  return i;  
}

int main(int argc, char* argvs[])
{
  char buf[BUFSIZE];
  char* argvss[MAXARG];

  while (readline(buf))
  {
    if (fork() == 0)
    {
      memcpy(argvss, argvs + 1, sizeof(char*) * (argc - 1));
      argvss[argc - 1] = buf;
      exec(argvss[0], argvss);
    }
    else
    {
      memset(argvss, 0, sizeof(char*) * MAXARG);
      wait(0);
    }
  }

  exit(0);
}