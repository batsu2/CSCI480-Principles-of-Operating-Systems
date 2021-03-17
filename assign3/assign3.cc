/****************************************************************

 CSCI 480 - ASSIGNMENT 3 - SPRING 2020

 PROGRAMMER:  Bryan Butz

 SECTION:  Section 1

 TA:  Joshua Boley

 zID:  z1836033

 DUE DATE:  2/20/2020

 PURPOSE:  This microshell will read an input, pars it, and if
           it finds a <+ or >+ will parse the string before as the
	   command and after as the file name. It also gives info
	   about the microshell as well as error checks for various
	   errors.

*****************************************************************/


#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <cstring>
#include <unistd.h>

using namespace std;


int main(void)
{
	char buffer[1024];	//buffer to read from
	char quit[5] = "quit";	//cstring with quit command
	char about[6] = "about"; //cstring with about command

	pid_t pid;	//pid of the fork
	int status;     //fork status


	cerr<< "480shell:";

	while( fgets(buffer,1024,stdin) != NULL )   //Read in the user input
        {
		int count = 0;	//As array subscript
		char * ptr;	//pointer for parsing the cstring
		char * command[1024] = {NULL, NULL, NULL}; //space for the command
		char * file[1024] = {NULL, NULL, NULL}; //space for the file name

		bool inRedirect = false;	//Redirections flags
		bool outRedirect = false;
		bool multiFile = false;         //Multiple File error flag

		buffer[strlen(buffer) - 1] = 0;	//remove the last char


		ptr = strtok(buffer, " ");	//start the search process



		// If there is a q or quit, exit microshell
		if(buffer[0] == 'q' || strcmp(buffer, quit) == 0)
			break;


		// If there is about, give info on the microshell
		if( strcmp(buffer, about) == 0)
			cerr << "The 480shell is the work of Bryan Butz, 2020.\n";



		while(ptr != NULL) //Parse the input
		{
			//If a redirection is found, act accordingly
			if(strcmp( ptr, ">+" ) == 0)
			{
				outRedirect = true;	//set flag
				count = 0;	//reset array count
				ptr = strtok(NULL, " ");  //keep looking
			}
                        else if(strcmp( ptr, "<+" ) == 0)
                        {
                                inRedirect = true;     //set flag
                                count = 0;      //reset array count
                                ptr = strtok(NULL, " ");  //keep looking
                        }
			else //No Redirections
			{
				if(inRedirect == false && outRedirect == false) //If no redirect, add to the command string
				{
					command[count] = ptr;	//add to the command string
					ptr = strtok(NULL, " "); //keep looking
					count++;
				}
				else if( inRedirect == true || outRedirect == true) //If redirect flag is set ie.after >+ or <+, this is the file(s)
				{
					file[count] = ptr;	//add to the file string
					ptr = strtok(NULL, " "); //keep looking


					//If there was a second file found, flag as error
					if( count == 1)
						multiFile = true;


					count++;
				}
			}

		}




		if ( (pid = fork()) == 0) //Fork for the child process
		 {
			if(outRedirect) //If out redirect exists
			{
				//If two files were found
				if ( multiFile )
				{
					cerr<<"Error!  An extra filename was found!\n";
					multiFile = false; //Reset flag
					exit(-3);
				}
				else
				{
					int fd0 = creat(file[0], 0644 );


					// Error check if creat failed
					if( fd0 == -1)
					{
						printf("Error Number % d\n", errno);
						perror("Command Error");

						exit(-4);
					}
					else
					{
						dup2(fd0, STDOUT_FILENO);
						close(fd0);
					}
				}

				outRedirect = false;  //Reset flag

			}
			else if(inRedirect) //If in redirection exists
			{
				//If two files were found
				if( multiFile )
				{
					cerr<<"Error!  An extra filename was found!\n";
					multiFile = false; //reset flag
					exit(-3);
				}
				else
				{
					int fd1 = open(file[0], O_RDONLY, 0);


                                	// Error check if open failed
                                	if( fd1 == -1)
                                	{
                                        	printf("Error Number % d\n", errno);
                                        	perror("Command Error");

						exit(-4);   //Exit with a negative value
                                	}
                                	else
                                	{
                                        	dup2(fd1, STDIN_FILENO);
						close(fd1);
                                	}
				}

				inRedirect = false; //reset flag

			}

			execvp(command[0], command); //Execute the command
			exit(127);

		}
		else if(pid < 0) //failed fork
		{
			cerr<<"The child fork failed!"<<endl;
			exit(-5);
		}
		else //Parent process
		{
			waitpid(pid, &status, 0); //Wait for the child process
		}


		cerr<<"480shell:";	//Prompt
	}



	return 0;
}
