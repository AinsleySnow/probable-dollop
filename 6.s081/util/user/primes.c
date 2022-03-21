#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
  int p[2];
  pipe(p);
  for (char i = 2; i <= 35; ++i)
    write(p[1], &i, 1);
  close(p[1]);

  while (1)
  {
    int fd_read = p[0];
    char c, init;
    pipe(p);

    if (fork() == 0)
    {
      if(read(fd_read, &init, 1))
        printf("prime %d\n", init);
      else
        exit(0);

      while(read(fd_read, &c, 1))
        if (c % init)
          write(p[1], &c, 1);

      close(p[1]);
      close(fd_read);
    }
    else
    {
      close(fd_read);
      close(p[0]);
      close(p[1]);

      wait(0);
      exit(0);
    }
  }
}