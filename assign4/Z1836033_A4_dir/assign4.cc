/****************************************************************

 CSCI 480 - ASSIGNMENT 4 - SPRING 2020

 PROGRAMMER:  Bryan Butz

 SECTION:  Section 1

 TA:  Joshua Boley

 zID:  z1836033

 DUE DATE:  3/24/2020

 PURPOSE:  This program acts as a simulator for virtual memory in-
	   side a computer. It takes 2 command line arguments.
	   (1) "F" or "L" for first-in-first-out sort method or
	   last-recently-used sort method respectively. (2) "D" or
	   "P" for direct paging or pre-paging respectively.
	   It takes in a file named "testfile.txt" and converts
	   the logical address and places it in a frame. If the
	   frame isnt available the program will find a free frame
	   based on the entered sort method.

*****************************************************************/


#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <queue>
#include <list>
#include <map>
#include <algorithm>
#include <iterator>
#include <unistd.h>

#define HOWOFTEN 10
#define SIZE 25
#define PAGE_SIZE 200

using namespace std;




int addressFile;

char input[1000];
char modifiedBit;
char srtType;

int logicalAddress, physicalAddress;
int i, j;

//Queue for FIFO
queue<int> frameQueue;

//LOGICAL MEMORY
int p;		// page-number: used as an index into a page table

//PHYSICAL MEMORY
int f; 			// frame-number: address of each page in physical memory
int frametable[15];
map<int, bool> isFrameFree;
list<int> frames;

// vars to handle page faults
int pagefault = 0;
int freeFrame = -1;
int inputLine = 1;






struct pgeEntry
{
	int frameNum;
	bool modBit;
};



//PAGE TABLE
pgeEntry pagetable[SIZE];


/***************************************************************
Function: printTable()

Use: This prints the contents of pagetable (the number of the
     page, number of the frame it's in, and whether or not the
     modified bit is set).

Arguments: None

Returns: Nothing
***************************************************************/
void printTable()
{
	cout<<"Page Number     Frame Number     Modified?\n";

	for( int n = 0; n < SIZE; n++ )
	{
		cout <<"    " <<right<<setw(2)<< n;
		cout <<"             " <<right<<setw(4)<<pagetable[n].frameNum;
		cout <<"             ";

		if( pagetable[n].modBit == true )
			cout<<right<<setw(3)<<"Yes"<<endl;
		else
			cout<<right<<setw(3)<<"No"<<endl;
	}

	inputLine = 1;
}


/***************************************************************
Function: nextFrame()

Use: This takes a list of frame numbers and returns the next
     frame to be used based on which sort technique is used.

Arguments: 1. frames, a reference to a list of integers to
	      represent the frame numbers.
           2. qType, a character to represent whether the
	      sort technique will be first-in-first-out or
	      least-recently-used

Returns: An integer to represent the next frame to be used.
***************************************************************/
int nextFrame( list<int>& frames, char qType )
{
	if( qType == 'F' )
	{
		int front = frames.front();

		frames.push_back(front);
		frames.pop_front();

		return front;
	}
	else if( qType == 'L' )
	{
		int back = frames.back();

		frames.push_front(back);
		frames.pop_back();

		return back;
	}


	return 101;
}




int main(int argc, char *argv[]) //argc = num of arguments
{				 //argv[0] = name of prog
				 //argv[1] = first argument
				 //argv[2] = second argument


	char *sortType = argv[1];

	//convert pointer to var to pass to function
	if( *sortType == 'F' )
		srtType = 'F';
	else if(*sortType == 'L')
		srtType = 'L';


	char *pagingType = argv[2];

	int d = 0;
	int k = 101;


        //Initialize the frame queue
        for( i = 100; i <= 300; i += 100 )
        {
	        for( j = 1; j <= 5; j++ )
		{
			k = i + j;

                        frames.push_back(k);
			frametable[d] = k;
			isFrameFree[k] = true;
			d++;
		}
	}



	cout<<"Entry start...\n";


	// Set up table depending on which paging technique is requested
	if( *pagingType == 'D' )
	{
		for( j = 0; j < SIZE; j++ )
		{
			pagetable[j].frameNum = -1;
			pagetable[j].modBit = false;
		}
	}
	else if( *pagingType == 'P' )
	{
		for( j = 0; j < SIZE; j++ )
                {

			if( j < 15 )
			{
                        	pagetable[j].frameNum = frametable[j];
				isFrameFree[ frametable[j] ] = false;
			}
			else
			{
				pagetable[j].frameNum = -1;
			}


                        pagetable[j].modBit = false;
                }
	}


	//READING LOGICAL ADDRESS FROM FILE
	addressFile = open("testfile.txt",O_RDONLY);


	if( addressFile != -1 )
	{
		char ch;

		while(read(addressFile, &ch, sizeof(char)) != 0)  // returns the number of bytes read and 0 for end of file
		{

			//Grab the access type(read or write)
			if( isalpha(ch) )
			{
				modifiedBit = ch;
				i++;
			}


			//Grab the logical address from input
			if( ch != '\n' )
			{
				input[i] = ch;
				i++;
			}
			else if( ch != ' ')
			{
				logicalAddress =atoi(input);


				//EXTRACT PAGE NUMBER

				p = logicalAddress / PAGE_SIZE;


				//Set Modified Bit & print message
				if( modifiedBit == 'W' && pagetable[p].frameNum != -1 )
				{
					pagetable[p].modBit = true;
					printf("Write page %i from frame %i on the disk\n", p, f );
				}
				else if (modifiedBit == 'W' && pagetable[p].frameNum == -1 )
				{
                                        f = nextFrame(frames, srtType);

                                        //ADDRESS TRANSLATION THROUGH PAGE TABLE

                                        //pagetable-hit, set frame number
                                        if( pagetable[p].frameNum == -1 && isFrameFree[f] == true )
                                        {
                                                pagetable[p].frameNum = f;
                                                isFrameFree[f] = false;
                                        }
                                        else  //pagetable-miss, page-fault
                                        {
                                                pagefault++;

                                                if( isFrameFree[f] == true )
                                                        pagetable[p].frameNum = f;
                                                else
                                                {
							f = nextFrame(frames, srtType);
                                                }

                                        }

					pagetable[p].modBit = true;
					printf("Write page %i from frame %i on the disk\n", p, f );
				}
				else if( modifiedBit == 'R' )
				{

	                                f = nextFrame(frames, srtType);

        	                        //ADDRESS TRANSLATION THROUGH PAGE TABLE

                	                //pagetable-hit, set frame number
                        	        if( pagetable[p].frameNum == -1 && isFrameFree[f] == true )
                                	{
                                        	pagetable[p].frameNum = f;
                                        	isFrameFree[f] = false;
                                	}
                                	else  //pagetable-miss, page-fault
                                	{
                                        	pagefault++;

                                        	if( isFrameFree[f] == true )
                                                	pagetable[p].frameNum = f;
                                        	else
                                        	{
							f = nextFrame(frames, srtType);
                                        	}

	                               	}

                                        pagetable[p].modBit = false;
                                        printf("Read page %i from disk into frame %i\n", p, f );
				}


				i = 0;
				inputLine++;

			}//end else


			//if 5th line, print page table
			if( (inputLine % HOWOFTEN) == 0 )
			{
				printTable();
			}


		}//end while



		printf("\nTotal Page Faults: %d\n",pagefault);
	}
	else
	{
		printf("Addresses File Does not exist!\n");
	}



	close(addressFile); //Close address file
	return 0;
}

