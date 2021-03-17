/****************************************************************

 CSCI 480 - Assignment 1 - Spring 2020

 PROGRAMMER:   Bryan Butz

 SECTION:  Section 1

 TA:  Joshua Boley

 zID:  z1836033

 DUE DATE:  1/24/2020

 PURPOSE:  This uses fork(), getpid(), getppid(), wait(), and
            system() to give an example of how processes are created.
            It forks 2 times makeing a total of 3 different processes,
            a parent, child, and grandchild. It then identifies
            each process by PID and uses system commands such as PS.

*****************************************************************/


#include<iostream>
#include<unistd.h>
#include <sys/wait.h>
using namespace std;

int main()
{
	pid_t pid; //for the child's pid

	cerr<<"I am the original process. My PID is " << getpid() << " and my parent's PID is " << getppid() <<"\n";
	cerr<<"Now we have the first fork." <<"\n\n";

	pid = fork();  //fork the parent to make the child


	if(pid < 0)  //If the first fork failed, inform and exit
	{
		cerr<<"The first fork failed! \n\n";
		exit(-1);
	}
	else if(pid == 0)  //If the fork succeeded, This is what the child process does
	{
		pid_t g_pid; //for grandchild's pid

		cerr<<"I am the child. My PID is " << getpid() << " and my parent's PID is " << getppid() <<"\n";
		cerr<<"Now we have the second fork. \n\n";

		g_pid = fork(); //fork again to produce the grandchild process


		if(g_pid < 0)	//If the second fork failed, inform and exit
		 {
			cerr<<"The second fork failed! \n\n";
			exit(-1);
		 }
		else if(g_pid == 0)//If fork succeeds, this is what the grandchild does
		 {
			cerr<<"I am the grandchild. My PID is " << getpid() << " and my parent's PID is " << getppid() <<"\n";
                        sleep(3); //sleep for 3 seconds

			cerr<<"\nI am the grandchild. I should now be an orphan.\n";
                        cerr<<"My PID is " << getpid() << " and my parent's PID is " << getppid() << "\n";
                        cerr<<"I will now call ps.\n\n";
                        system("ps"); //call system ps command
			cerr<<"\nI am the grandchild, about to exit\n";
			exit(0); //exit grandchild process
		 }
		else //what child process does after the second fork
		 {
			cerr<<"I am the child. My PID is " << getpid() << " and my parent's PID is " << getppid() <<"\n";
			sleep(2); //sleep for 2 seconds

			cerr<<"I am the child, about to exit. \n";
			exit(0); //exit child process
		 }
	}
	else //what the parent process does after the first fork
	{
		cerr<<"I am the parent. My PID is " << getpid() << " and my parent's PID is " << getppid() <<"\n";
		sleep(2); //sleep 2 seconds

		cerr<<"I am the parent, about to call 'ps'. The child should appear as a zombie. \n\n";
		system("ps"); //call the system ps command
                sleep(3); //sleep for 3 seconds

		wait(0); //wait for all child process to finish
                cerr<<"\nI am the parent, having waited on the child, about to call ps again.\n\n";
                system("ps");
		cerr<<"\nI am the parent, about to exit. \n";
		exit(0); //close parent process
	}

	return 0;
}


