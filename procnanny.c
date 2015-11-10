
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
char *PROCNANNYLOGS = "logfile.log";

struct process
{
   int  processID;
   char  name[256];
   int   time;
};

void killOtherProcnanny();
void sig_handler(int signo);

void main(int argc, char *argv[])
{
	//initial configuration
	char fileInput[128][256];
	int processPID;
	int killTime;

	struct process processList[128];
	char processNameList[128][256];

	pid_t	pid; 

	time_t currentTime;
	struct tm *timeInfo;
    char timeString[80];
    int totalProcess = 0;
    int totalProcessNames = 0;
    int processFound;
    int processNameFound;

    killOtherProcnanny();

    FILE *logRewrite = fopen(PROCNANNYLOGS, "w");
	fclose(logRewrite);

    while(1){

	 	int counter = 0;
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

	    	token = strtok(processName, "\n");
	    	strcat(command, processName);

	    	int q;
			processNameFound = 0;
			for(q = 0; q < totalProcessNames; q++){
				if (strcmp(processNameList[q],processName) == 0){
					processNameFound = 1;
				}
			}
			if (!processNameFound){
				strcpy(processNameList[totalProcessNames],processName);
				totalProcessNames++;
			}

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
				processPID = atoi(procPID);

				int l = 0;
				processFound = 0;
				for(l = 0; l <= totalProcess; l++){
					if (processList[l].processID == processPID){
						processFound = 1;
					}
				}

				found = 1;

				if(!processFound){
					totalProcess++;
					processList[totalProcess-1].processID = processPID;
					strcpy(processList[totalProcess-1].name, processName);
					processList[totalProcess-1].time = killTime;

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
						sleep(processList[totalProcess-1].time);
						kill(processList[totalProcess-1].processID,SIGKILL);

						time(&currentTime);
		    			timeInfo = localtime(&currentTime);
		    			strftime (timeString,80, "[%c]", timeInfo);

						FILE *logfile = fopen(PROCNANNYLOGS, "a");
						strtok(procPID,"\n");
						fprintf(logfile, "[%s] Action:PID %d (%s) killed after %d seconds.\n",timeString, processList[totalProcess-1].processID, processList[processCounter].name, processList[processCounter].time);
						fflush(logfile);
						fclose(logfile);
						exit(EXIT_SUCCESS);
					}
					else{
						    if (signal(SIGINT, sig_handler) == SIG_ERR){
		        				printf("\ncan't catch SIGINT\n");
						    }
		    				if (signal(SIGHUP, sig_handler) == SIG_ERR){
		        				printf("\ncan't catch SIGKILL\n");
		    				}
						numberProcessesKilled++;
					} 
					processCounter++;
					wait(NULL);
				}
			}

			if((found == 0) && (!processNameFound)){
				//if no process found
				time(&currentTime);
	    		timeInfo = localtime(&currentTime);
	    		strftime (timeString,80, "[%c]", timeInfo);

				FILE *logfile = fopen(PROCNANNYLOGS, "a");
				fprintf(logfile, "[%s] Info: No '%s' processes found.\n",timeString,processName);
				fclose(logfile);
			}
			
			/*
			int k;
			processFound = 0;
				for(k = 0; k < totalProcess; k++){
					if (processList[k].processID == processPID){
						processFound = 1;
						totalProcess++;
				}
			}
			*/
			
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
	}

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
	time_t currentTime;
	struct tm *timeInfo;
    char timeString[80];

	time(&currentTime);
    timeInfo = localtime(&currentTime);
    strftime (timeString,80, "[%c]", timeInfo);

  if (signo == SIGINT){
    printf("received SIGINT\n");

    FILE *logfile = fopen(PROCNANNYLOGS, "a");
	fprintf(logfile, "[%s] Info: Exiting. %d  process(es) killed.\n",timeString,numberProcessesKilled);
	fclose(logfile);
	}
  if (signo == SIGHUP){
    printf("received SIGHUP\n");
    }
}