#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    return 0;
  }

  umask(0113);

  key_t key;
  int shmDES;
  int semDES;
  int fd;
  if (strcmp(argv[1],"-c")==0 ) {
    key = ftok("/usr", 23);

    shmDES = shmget(key, 102400, IPC_CREAT | 0644);// | S_IRUSR | S_IWUSR);
    semDES = semget(key, 1, IPC_CREAT | 0644);
    fd = open("story.txt", O_TRUNC | O_CREAT, 0664);

    printf("shmDES: %d\n",shmDES);
    printf("semDES: %d\n",semDES);
    printf("file FD: %d\n",fd);

    close(fd);
  }
  if(strcmp(argv[1],"-r")==0) {
    //union semun arg;
    //struct shmid_ds *s1;
    //printf("shmDES: %d\n",shmDES);
    key = ftok("/usr", 23);;

    shmDES = shmget(key,1,IPC_EXCL | 0644);
    semDES = semget(key,1,IPC_EXCL | 0644);

    printf("shmDES: %d\n",shmDES);
    printf("semDES: %d\n",semDES);
    printf("removing shared memory, should be 0: %d\n",shmctl(shmDES,IPC_RMID,0));
    printf("removing semaphore, should be 0: %d\n",semctl(semDES,0,IPC_RMID,0));

    char* args[3] = {"cat","text", NULL};
    execvp(args[0],args);
  }
  if(strcmp(argv[1],"-v")==0) {
    char* args[] = {"cat", "text", NULL};
    execvp(args[0],args);
  }
  return 0;
}
