#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

/* This program forks and and the prints whether the process is
 *   - the child (the return value of fork() is 0), or
 *   - the parent (the return value of fork() is not zero)
 *
 * When this was run 100 times on the computer the author is
 * on, only twice did the parent process execute before the
 * child process executed.
 *
 * Note, if you juxtapose two strings, the compiler automatically
 * concatenates the two, e.g., "Hello " "world!"
 */

 //https://ece.uwaterloo.ca/~dwharder/icsrts/Tutorials/fork_exec/

 struct Pipes
{
   int rdwr[2];   // 0 = read, 1 = write
};


int main( void ) {


	pid_t parentPID = getpid();
	int i;
	int busyChildArray[128];
	int idleChildArray[128];
	int counter = 3;
	int	fd[256][2];
	char line[256];

	struct Pipes pipeList[256];

	for(i=0;i < counter; i++){
		pipe(pipeList[i].rdwr);
		//pipe(fd[i]);
		int pid = fork();
		if(pid < 0){
	    	//err_sys("fork error");
	    }
		else if ( pid == 0 ) {  //child
			char *tempHolder = malloc(6);
			sprintf(tempHolder, "%d", getpid());
			close(pipeList[i].rdwr[0]);
			int status = write(pipeList[i].rdwr[1], tempHolder, 6);
			//close(fd[i][0]);
			//int status = write(fd[i][1], tempHolder, 6);

			printf("The child %d has attempted write (%d). \n",getpid(),status);
			free(tempHolder);
			exit(EXIT_SUCCESS);
		} else {  //parent
			busyChildArray[i] = pid;
			printf("this is the iteration %d and the pid is %d\n",i,pid );
		}
	}
	sleep(1);
	int j;
	for(j = 0; j < counter; j++){
		//close(fd[j][1]);
		close(pipeList[j].rdwr[1]);
		//int n = read(fd[j][0], line, 256);
		int n = read(pipeList[j].rdwr[0], line, 256);
		int propPID = atoi(line);
		printf("(%d)   child returned its pid which is %s\n",n, line);
	}
	return 0;
}