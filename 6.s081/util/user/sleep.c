#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argvs[])
{
  if (argc < 2)
  {
    fprintf(2, "sleep: need one more argument\n");
    exit(1);
  }

  int ticks = atoi(argvs[1]);
  sleep(ticks);

  exit(0);
}