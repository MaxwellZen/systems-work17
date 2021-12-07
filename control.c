#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define KEY 49620
#define KEY2 453345

void printusage() {
	printf("ERROR: Please run with a proper command line argument, ex:\n\t$ ./control create\n\t$ ./control remove\n");
}

long long get_file_size(char *f) {
	struct stat a;
	if (stat(f, &a)==-1) {
		printf("The file [%s] was not successfully opened\n", f);
		printf("%s\n", strerror(errno));
		return -1;
	}
	return a.st_size;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printusage();
	} else if (!strcmp(argv[1], "create")) {

		int fd = open("story.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
		printf("Created file\n");
		close(fd);

		int shmid = shmget(KEY, sizeof(int), IPC_CREAT | 0777);
		int *data = shmat(shmid, 0, 0);
		*data = 0;
		shmdt(data);
		printf("Created shared memory\n");

		int semid = semget(KEY2, 1, IPC_CREAT | IPC_EXCL | 0777);
		union semun us;
		us.val = 1;
		int r = semctl(semid, 0, SETVAL, us);
		printf("Created semaphore\n");

	} else if (!strcmp(argv[1], "remove")) {

		int fd = open("story.txt", O_RDONLY, 0777);
		long long sz = get_file_size("story.txt");
		char c[sz+1];
		read(fd, c, sz+1);
		c[sz] = 0;
		printf("Full story:\n\n%s\n", c);
		remove("story.txt");
		printf("Removed file\n");

		int shmid = shmget(KEY, 0, 0);
		shmctl(shmid, IPC_RMID, 0);
		printf("Removed shared memory\n");

		int semid = semget(KEY2, 1, IPC_CREAT | 0777);
		semctl(semid, IPC_RMID, 0);
		printf("Removed semaphore\n");

	} else {
		printusage();
	}
}
