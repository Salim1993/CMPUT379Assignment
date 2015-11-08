
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
/* 
http://www.programmingsimplified.com/c-program-read-file 
https://ece.uwaterloo.ca/~dwharder/icsrts/Tutorials/fork_exec/  
http://www.tutorialspoint.com/c_standard_library/c_function_time.htm
http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
http://rosettacode.org/wiki/Read_a_file_line_by_line#C
http://www.cs.umd.edu/~srhuang/teaching/cmsc212/gdb-tutorial-handout.pdf
*/

int numberProcessesKilled;

struct process
{
   int  processID;
   char  name[256];
   int   time;
};

void killOtherProcnanny();

int main(int argc, char *argv[])
{
	//initial configuration
	char fileInput[128][256];
	int counter = 0;
	int processPID;
	int killTime;
	char *PROCNANNYLOGS = "logfile.log";

	struct process processList[128];

	pid_t	pid; 

	time_t currentTime;
	struct tm *timeInfo;
    char timeString[80];

    killOtherProcnanny();
 
 	//opening file
	FILE *fp = fopen(argv[1],"r"); // read mode
 
	if( fp == NULL ){
    	printf("Error while opening the file.\n");
    }

	while (fgets (fileInput[counter], 256, fp)) {
    	counter++;
	}
	if (ferror(fp)) {
    	fprintf(stderr,"Oops, error reading the file\n");
	}	
	
    fclose(fp);

    FILE *logRewrite = fopen(PROCNANNYLOGS, "w");
    fclose(logRewrite);

    char processName[256] = "";

    //creating pipe command
    char origCommand[] = "pgrep ";
    char command[256] = "pgrep ";

	char procPID[256];

    int i;
    int found;
    char *token;
    int processCounter = 0;

    for(i=0;i < counter; i++){
    	//iterating input for different process names
    	strcpy(command,origCommand);
    	
    	token = strtok(fileInput[i], " ");
    	strcpy(processName,token);
    	token = strtok(NULL, " ");
    	killTime = atoi(token);

    	strtok(processName, "\n");
    	strcat(command, processName);

    	FILE *file;
    	found = 0;
    	
    	//getting time
    	currentTime = time(NULL);
    	timeInfo = localtime(&currentTime);
    	strftime (timeString,80, "[%c]", timeInfo);

		file = popen(command, "r");
		if (file == NULL){
		//do some eror stuff
		}
		while(fgets(procPID, 256, file) != NULL){
			// fork child for each process id found
			found = 1;
			processPID = atoi(procPID);
			
			processList[processCounter].processID = processPID;
			strcpy(processList[processCounter].name, processName);
			processList[processCounter].time = killTime;

			time(&currentTime);
    		timeInfo = localtime(&currentTime);
    		strftime (timeString,80, "[%c]", timeInfo);

			FILE *logfile = fopen(PROCNANNYLOGS, "a");
			strtok(procPID,"\n");
			fprintf(logfile,"[%s] Info: Initializing monitoring of process '%s'(PID %s).\n",timeString,processName,procPID);
			fclose(logfile);

			pid = fork();
			if(pid < 0){
    			//err_sys("fork error");
    		}
			else if (pid == 0 ) {
				sleep(processList[processCounter].time);
				kill(processList[processCounter].processID,SIGKILL);

				time(&currentTime);
    			timeInfo = localtime(&currentTime);
    			strftime (timeString,80, "[%c]", timeInfo);

				FILE *logfile = fopen(PROCNANNYLOGS, "a");
				strtok(procPID,"\n");
				fprintf(logfile, "[%s] Action:PID %d (%s) killed after %d seconds.\n",timeString, processList[processCounter].processID, processList[processCounter].name, processList[processCounter].time);
				fflush(logfile);
				fclose(logfile);
				exit(EXIT_SUCCESS);
			}
			else{
				    if (signal(SIGINT, sig_handler) == SIG_ERR){
        				printf("\ncan't catch SIGUSR1\n");
				    }
    				if (signal(SIGHUP, sig_handler) == SIG_ERR){
        				printf("\ncan't catch SIGKILL\n");
    				}
				numberProcessesKilled++;
			} 
			processCounter++;
			wait(NULL);
		}

		if(found == 0){
			//if no process found
			time(&currentTime);
    		timeInfo = localtime(&currentTime);
    		strftime (timeString,80, "[%c]", timeInfo);

			FILE *logfile = fopen(PROCNANNYLOGS, "a");
			fprintf(logfile, "[%s] Info: No '%s' processes found.\n",timeString,processName);
			fclose(logfile);
		}
		
	}
	int status;

	int j;

	wait(&status);

	for (j= 0; j <= numberProcessesKilled + counter; j++){
		wait(NULL);
	}

	int exited = WIFEXITED(status);
	while (!exited) {
		wait(&status);
		exited = WIFEXITED(status);
	}
	time(&currentTime);
    timeInfo = localtime(&currentTime);
    strftime (timeString,80, "[%c]", timeInfo);

	FILE *logfile = fopen(PROCNANNYLOGS, "a");
	fprintf(logfile, "[%s] Info: Exiting. %d  process(es) killed.\n",timeString,numberProcessesKilled);
	fclose(logfile);
	return 0;
}

void killOtherProcnanny(){
	char command[] = "pgrep procnanny";
	pid_t ownPID = getpid();
	char procPID[256];
	int pidkill;
	FILE *file;

	file = popen(command, "r");
	if (file == NULL){
		//do some eror stuff
	}
	while(fgets(procPID, 256, file) != NULL){
		pidkill = atoi(procPID);
		if(pidkill != ownPID){
			kill(pidkill,SIGKILL);
		}
	}
}

void sig_handler(int signo){
  if (signo == SIGINT)
    printf("received SIGINT\n");
  if (signo == SIGHUP)
    printf("received SIGHUP\n");
}