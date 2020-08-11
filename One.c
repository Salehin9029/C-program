#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

void search(char *, char *);

int main(int argc, char *argv[]){
  char curdir[255]=".";

  if(argc != 2){
    fprintf(stderr, "Usage: %s <file-name>\n", argv[0]);
    exit(0);
  }
  search(curdir, argv[1]);
  exit(0);
}

void search(char *dirname, char *filename){
  DIR *dp, *subdir;
  struct dirent *dirp;
  char nextDir[255];

  dp = opendir(dirname);
  if(dp == NULL)          // return if not a directory
    return;

  strcat(dirname, "/");  // add / as a separator

  while ((dirp=readdir(dp)) != NULL){
    if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, "..")) // ignore . and ..
      continue;

    if(!strcmp(dirp->d_name, filename)){         // if a match
      printf("%s%s : ", dirname, dirp->d_name);

      if(dirp->d_type == DT_DIR)             // check if a directory
	printf("this is a directory\n");
      else
	printf("this is a file\n");
    }
    strcpy(nextDir, dirname);               // search in the next subdirectory
    strcat(nextDir, dirp->d_name);
    search(nextDir, filename);
  }
  closedir(dp);
}
