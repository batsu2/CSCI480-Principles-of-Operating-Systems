/****************************************************************

 CSCI 480 - ASSIGNMENT 5 - SPRING 2020

 PROGRAMMER:  Bryan Butz

 SECTION:  Section 1

 TA:  Joshua Boley

 zID:  z1836033

 DUE DATE:  4/06/2020

 PURPOSE:  This creates several producer and consumer threads and,
	   depending on whether in-line arguments were entered,
	   creates a number of producers and consumers to add and
           remove a Widget struct from a queue. This process is
	   controlled with mutexes and semaphores in order to allow
	   exclusive access.

*****************************************************************/

#include<iostream>
#include<semaphore.h>
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<iomanip>
#include<queue>

using namespace std;

#define BUFFER_SIZE 35 //maximum size of the buffer


static int P_NUMBER = 7; //number of producers
static int C_NUMBER = 5; //number of consumers
static int P_STEPS = 5; //number of iterations for each producer thread
static int C_STEPS = 7; //number of iterations for each consumer thread

int p_NUMBER;
int c_NUMBER;
int p_STEPS;
int c_STEPS;


void Insert(int threadID, int ctr);
void Remove(int threadID);
void *Produce(void *threadID);
void *Consume(void *threadID);
void printQueue();


int widget = 0; //total number of widgets

sem_t notFull; //not full semaphore
sem_t notEmpty; //not emtpy seamphore
pthread_mutex_t mutex; //mutex


struct Widget
{
        unsigned int producer;
        unsigned int widgetNum;
};


//Buffer queue
queue<Widget> buffer;


int main( int argc, char *argv[] )
{
	int rc; //return code from the pthread creates

	sem_init(&notFull, 0, BUFFER_SIZE); //set up semphore for not full
	sem_init(&notEmpty, 0, 0); //set up semphore for not empty


	//set based on if in-line args were entered
	if( argc > 1 )
	{
		p_NUMBER = atoi(argv[1]);
                c_NUMBER = atoi(argv[2]);
                p_STEPS = atoi(argv[3]);
                c_STEPS = atoi(argv[4]);
	}
	else
	{
		p_NUMBER = P_NUMBER;
		c_NUMBER = C_NUMBER;
		p_STEPS = P_STEPS;
		c_STEPS = C_STEPS;
	}


	pthread_t Consumer[c_NUMBER];	//array of consumer threads
	pthread_t Producer[p_NUMBER];	//array of producer threads

	pthread_mutex_init(&mutex, NULL);  //set up mutex

	cout << "Simulation of Producer and Consumers\n" <<endl;
	cout << "The semaphores and mutex have been initialized.\n" <<endl;


	//make the consumer threads
	for (long i = 0; i < c_NUMBER; i++)
	{
		rc = pthread_create(&Consumer[i], NULL, Consume, (void *) i);	//make the threads

		//if error, exit
		if(rc)
		{
			cerr << "Error in Consumer create\n";
			exit(-1);
		}
	}


	//make all the producer threads
	for (long i = 0; i < p_NUMBER; i++)
	{
		rc = pthread_create(&Producer[i], NULL, Produce, (void *) i);

		//If error, exit
		if(rc)
		{
			cerr << "Error in producer create\n";
			exit(-1);
		}
	}



	//join the threads
	for (int i = 0; i < c_NUMBER; i++)
	{
		pthread_join(Consumer[i], NULL);
	}



	//delete the threads
	for(int i = 0; i < p_NUMBER; i++)
	{
		pthread_join(Producer[i], NULL);
	}

	cout << "All the producer and consumer threads have been closed.\n" <<endl;
	cout << "The semaphores and mutex have been deleted.\n" << endl;

	sem_destroy(&notFull);  //delete not full semphore
        sem_destroy(&notEmpty); //delete not emtpy seamphore

	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);

	return 0;
}

/***************************************************************
  Function: Insert()

  Use: This locks the mutex, adds a widget to the buffer, then
       removes the mutex lock.

  Arguments: threadID, an integer used to represent the thread's
	               id number.

	     ctr, an integer used to act as a counter for which
		  widget is with which producer.

  Returns: Nothing
***************************************************************/
void Insert(int threadID, int ctr)
{
	int rc; //return code for mutex lock

	rc = pthread_mutex_lock(&mutex);  //lock the mutex

	//If there was no problem
	if(rc == 0)
	{
		cerr << "Producer " <<threadID << " inserted one item.  Total is now " << right << setw(2) << widget<< ".";

		//create temp widget to insert
		Widget p;
		p.producer = threadID;
		p.widgetNum = ctr;

		buffer.push(p); //insert the widget

		printQueue();

		cerr <<endl<<endl;

		widget++;
	}
	else //there was an issue
	{
		cerr << "Producer " << threadID << " Error in lock" <<endl<<endl;
		exit(-1);
	}

	rc = pthread_mutex_unlock(&mutex);  //Unlock the mutex

	//if there was a problem, quit
	if (rc != 0)
	{
		cerr << "Producer " << threadID << " Error in Unlock" <<endl<<endl;
		exit(-1);
	}
}

/***************************************************************
  Function: Remove()

  Use: This locks the mutex, takes a widget out of the buffer,
       and then removes the mutex lock.

  Arguments: threadID, an integer used to represent the thread's
		       id number.

  Returns: Nothing
***************************************************************/
void Remove(int threadID)
{
	int rc; //return code for mutex lock

	rc = pthread_mutex_lock(&mutex); //lock the mutex

	//If there is no problem
	if (rc == 0)
	{
		cerr << "Consumer " << threadID << " removed one item.   Total is now " << right << setw(2) << widget<<".";

		buffer.pop(); //remove the widget

		printQueue();

		cerr << endl << endl;

		widget--;
	}
	else //There was a problem
	{
		cerr << "Consumer " << threadID << ": Failure in Remove: lock" << endl<<endl;
		exit(-1);
	}

	rc = pthread_mutex_unlock(&mutex); //unlock the mutex

	if (rc != 0) //If error, exit
	{
		cerr << "Consumer : " << threadID << ": Failure in Remove: Unlock" << endl<<endl;
		exit(-1);
	}
}

/***************************************************************
  Function: Produce()

  Use: This runs each thread to insert the widgets into the buffer,
       utilizing semaphores along the way.

  Arguments: threadID, this is a pointer to represent the thread's
		       id number.

  Returns: Nothing
***************************************************************/
void *Produce(void *threadID)
{
	long temp = (long) threadID;	//convert the threadID to a long

	//loop for the number of iterations for produce
	for (int i = 0; i < p_STEPS; i++)
	{
		sem_wait(&notFull);
		Insert(temp, i); //Add a widget to the buffer
		sem_post(&notEmpty);

		sleep(1);//wait 1 sec
	}

	pthread_exit(NULL); //end thread
}

/***************************************************************
  Function: Consume()

  Use: This runs the threads to delete the widgets from the
       buffer, utilizing semaphores along the way.

  Arguments: threadID, this pointer represents the thread's id
		       number.

  Returns: Nothing
***************************************************************/
void *Consume(void *threadID)
{
	long temp = (long) threadID;	//convert to long

	//loop through the number of iterations for consume
	for (int i = 0; i < c_STEPS; i++)
	{
		sem_wait(&notEmpty);
		Remove(temp);	//Remove 1 widget from the buffer
		sem_post(&notFull);

		sleep(1); //wait 1 sec
	}


	pthread_exit(NULL);   //end thread
}

/***************************************************************
  Function: printQueue()

  Use: This is prints out the buffer (a queue of Widgets) by
       making a copy and going through one at a time, printing
       the Widget's producer number and its own widget number.

  Arguments: None

  Returns: Nothing
***************************************************************/
void printQueue()
{
	//Make  a copy queue of the buffer
	queue<Widget> q = buffer;


	//If the queue is empty
	if( q.size() == 0 )
	{
		cout<<"  Buffer is now empty.";
	}
	else //print the first q element, popping it after printing
	{
		cout<<"  Buffer now contains:  ";

		while( !q.empty() )
		{
			cout<<"P"<< q.front().producer <<"W"<< q.front().widgetNum <<" ";
			q.pop();
		}
	}
}
