#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char* directory, char* name)
{
  char buf[512], * p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(directory, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", directory);
    return;
  }

  strcpy(buf, directory);
  p = buf + strlen(buf);
  *p++ = '/';
  
  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == 0)
      continue;
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    
    if (stat(buf, &st) < 0)
    {
      printf("find: cannot stat %s\n", buf);
      continue;
    }
    
    if (st.type == T_DIR)
      find(buf, name);
    else
      if (strcmp(de.name, name) == 0)
        printf("%s\n", buf);
  }

  close(fd);
}

int main(int argc, char* argvs[])
{
  if (argc != 3)
  {
    printf("usage: find <dir> <name>\n");
    exit(1);
  }

  find(argvs[1], argvs[2]);
  exit(0);
}
