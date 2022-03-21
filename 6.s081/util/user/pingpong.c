#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
  int p[2];
  pipe(p);

  char buf = 'a';
  int pid;

  if (fork() == 0)
  {
    read(p[0], &buf, 1);

    pid = getpid();
    printf("%d: received ping\n", pid);

    write(p[1], &buf, 1);
    exit(0);
  }
  else
  {
    write(p[1], &buf, 1);
    wait(0);
    read(p[0], &buf, 1);

    pid = getpid();
    printf("%d: received pong\n", pid);
    exit(0);
  }
}