/****************************************************************

 CSCI 480 - Assignment 2 - Spring 2020

 PROGRAMMER: Bryan Butz

 SECTION:  Section 1

 TA:  Joshua Boley

 zID:  z1836033

 DUE DATE:  2/07/2020

 PURPOSE: This will create three processes which communicate with
	  each other using pipes. There will be a parent, child,
	  and grandchild process. The processes will take a c++
          string and convert it into a workable integer, do math
          with said integer, and pass it along to the next process
          after converting it back into a string and printing it's
          value.

*****************************************************************/

#include<iostream>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

using namespace std;


/***************************************************************
Function: PWork

Use: This starts the communication and sends data to the child
     over the pipe. It then recieves data from the grandchild
     over yet another pipe.

Arguments: 1. writeStream, an integer to represent the write
              end of the pipe.
           2. readStream, an integer to represent the read
              end of the pipe.

Returns: Nothing
***************************************************************/
void PWork(int writeStream, int readStream)
{
	string buffer = "1@";	//Strings for the input and output pipe data
	string value ="1";
	char ch;	//For reading the pipe
	long int m = 1;	//number to do math with

	cerr << "Parent        Value: " << m <<endl;
	write(writeStream, buffer.c_str(), buffer.length());	//send value to child
	buffer.clear();	//clear the strings
	value.clear();


        //Read things from the pipe
	while(true)
          {
		while(read(readStream, &ch, 1) > 0)	//save anything that comes over the pipe
                {
			if(ch == '@')	//If the end of the string stop reading
				break;

			value.push_back(ch);	//save char to string
		}



		try
                {
			m = stoi(value);	//convert to an integer
		}
                catch(...)
                {
			break;
		}




		if (m >= 999999999 || m <= -999999999)	//the number is to large or too small
                {
			write(writeStream, "*@", 2);	//send exit to all other processes
			break;	//return to main function
		}
		else
                {
			m = 200 - 3 * m; 	//calculate the math

			buffer = to_string(m);	//convert it to string
			cerr << "Parent        Value: " << m <<endl;	//display the value
			write(writeStream, buffer.c_str(), buffer.length());	//send it over the pipe
			write(writeStream, "@", 1);	//send the dilimiter
		}


		value.clear();	//clear the strings
		buffer.clear();
	}
}

/***************************************************************
Function: CWork

Use: This recieves a string over the pipe and converts it into
     workable integers, does arithmetic operation, and then
     converts it back to a string. It then sends it back over
     the pipe.

Arguments: 1. writeStream, an integer to represent the write
              end of the pipe.
           2. readStream, an integer to represent the read
              end of the pipe.

Returns: Nothing
***************************************************************/
void CWork(int writeStream, int readStream)
{
	string buffer;	//strings for input and output
	string value;
	char ch;	//char for reading from the pipe
	long int m = 1;	//int for the math


	//Read from the pipe
	while(true)
	{
		while(read(readStream, &ch, 1) > 0)	//Read single chars from the pipe
                {
			if(ch == '@')	//If the delimiter stop
				break;

			value.push_back(ch);
		}

		try
                {
			m = stoi(value);	//convert to integer
		}
		catch(...)
                {
			break;
		}




		if (m >= 999999999 || m <= -999999999)	//If the number is too large or too small
                {
			write(writeStream, "*@", 2);	//send stop to other process
			break;
		}
		else
                {
			m = 7 * m - 6;	//do the math on the number

			buffer = to_string(m);	//convert to string
			cerr << "Child:        Value: " << m <<endl;
			write(writeStream, buffer.c_str(), buffer.length());	//send it
			write(writeStream, "@", 1);	//send it
		}

		value.clear();	//clear it
		buffer.clear();
	}
}

/***************************************************************
Function: GWork

Use: This recieves a string over the pipe and converts it into
     numbers, performs arithmetic opperation, and then converts
     it back into a string. It then sends it back over the pipe.

Arguments: 1. writeStream, an integer to represent the write
              end of the pipe.
           2. readStream, an integer to represent the read
              end of the pipe.

Returns: Nothing
***************************************************************/
void GWork(int writeStream, int readStream)
{
	string buffer;	//Input and output strings
	string value;
	char ch;	//char using in reading from the pipe
	long int m = 1;	//used in doing math


	//Read from the pipe
	while(true)
           {
		while(read(readStream, &ch, 1) > 0)	//Read chars in from the pipe
                {
			if(ch == '@')	//If char is delimiter
				break;	//last char in pipe

			value.push_back(ch);	//save in string
		}



		try
                {
			m = stoi(value);	//convert to integer
		}
                catch(...)
                {
			break;
		}




		if (m >= 999999999 || m <= -999999999)	//If number is too big or too small
                {
			write(writeStream, "*@", 2);	//send stop to other processes
			break;
		}
		else
                {
			m = 30 - 4 * m;	//calculate the math

			buffer = to_string(m);	//conver to string
			cerr << "Grandchild:   Value: " << m <<endl;
			write(writeStream, buffer.c_str(), buffer.length());	//send it over the pipe
			write(writeStream, "@", 1);	//send delimiter
		}


		value.clear();	//clear strings
		buffer.clear();
	}
}

/***************************************************************
  Function: main()

  Use: This is the driver function of the program. It creates
       the three pipes to be used as well as the parent, child,
       and grandchild processes. It then sends data between the
       processes by utilizing the pipes.

  Arguments: None

  Returns: Nothing
***************************************************************/
int main()
{
	int pipeA[2], pipeB[2], pipeC[2];	//3 pipes
	pid_t pid;

	if(pipe(pipeA)) 	//test pipeA
        {
		cerr << "Pipe A error! Program will now exit" <<endl;
		exit(-5);
	}


	if(pipe(pipeB)) 	//test pipeB
        {
		cerr << "Pipe B error! Program will now exit" <<endl;
		exit(-5);
	}


	if(pipe(pipeC)) 	//test PipeC
        {
		cerr << "Pipe C error! Program will now exit" <<endl;
		exit(-5);
	}





	pid = fork();	//Fork and make a child

	if(pid < 0) 	//If first fork fails
        {
		cerr << "The Child fork failed!" <<endl;
		exit(-5);
	}
	else if(pid == 0)  //The child process
        {
		pid_t g_pid;	//to hold grandchilds pid
		g_pid = fork();	//the grandchild

		if(g_pid < 0) 	//If second fork fails
                {
			cerr << "The Grandchild fork failed!" <<endl;
			exit(-5);
		}
		else if (g_pid == 0)  //Grandchild process
                {
			close(pipeB[0]);//Close unused ends of the pipes
			close(pipeA[1]);

			close(pipeC[0]); //Close both ends of unused Pipe C
			close(pipeC[1]);

                        cerr << "The grandchild process is ready to proceed\n";

			GWork(pipeB[1], pipeA[0]);

			close(pipeB[1]);  //Close remaining ends of the pipes
			close(pipeA[0]);

			exit(0);	//kill the process
		}
		else //Rest of the child process
                {
                        close(pipeC[0]); //Close unused ends of the pipes
                        close(pipeB[1]);

			close(pipeA[0]); //Close both ends of unused Pipe A
			close(pipeA[1]);

                        cerr << "The child process is ready to proceed\n";

			CWork(pipeC[1], pipeB[0]);

			close(pipeC[1]);  //Close remaining ends of the pipes
                        close(pipeB[0]);

			wait(0); //wait for grandchild to finish
			exit(0); //kill the process
		}

	}
	else  //The rest of the parent process
        {
		close(pipeA[0]); //Close read end of pipeA
		close(pipeC[1]); //Close write end of pipeC

		close(pipeB[0]);  //Close both ends of unused Pipe B
		close(pipeB[1]);

                cerr << "The parent process is ready to proceed\n";

		PWork(pipeA[1], pipeC[0]);

		close(pipeA[1]);	//close remaining ends of the pipes
		close(pipeC[0]);

		wait(0);        //wait for the child to finish
		exit(0);	//end the program
	}



	return 0;
}
