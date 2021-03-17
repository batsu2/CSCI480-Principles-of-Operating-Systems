/****************************************************************

 CSCI 480 - ASSIGNMENT 6 - SPRING 2020

 PROGRAMMER:  Bryan Butz

 SECTION:  Section 1

 TA:  Joshua Boley

 zID:  z1836033

 DUE DATE:  4/13/2020

 PURPOSE:  This simulates the operations of an operating systems
	   memory managment system. It creates a linked list of blocks
	   sizes 1mb, 1mb, 1mb, 2mb, 8mb and manages the contents of
	   those blocks.

*****************************************************************/

#include <iostream>
#include <iomanip>
#include <cstring>
#include <climits>
#include <fstream>
#include <cstdlib>

using namespace std;

#define HOWOFTEN 6


struct Block;

void print();
void mergeAvail();
void terminate(string, bool firstRun = true);
void load(string, int, string);
void allocate(string, int, string);
void deallocate(string, string);
void deleteBlock(Block *, Block **);




struct Block
{
	Block(int, int);
	int address;
	int size;
	string ownpid;
	string blockpid;
	Block *prev;
	Block *next;
};


Block::Block(int x, int y)
{
	address = x;
	size = y;
}

//Available blocks
Block *Avail = nullptr;

//InUse blocks
Block *InUse = nullptr;


char fit = 'F';



int main( int argc, char ** argv )
{
	string alg;

	//Check if proper number of arguments are given
	if (argc != 2)
	{
		cerr << "wrong input, please provide either F or B as an inline argument\n";
		exit(-1);
	}


	//Check the arguments
	switch (argv[1][0])
	{
		case 'F':
			fit = 'F';
			alg = "First-Fit";
			break;

		case 'B':
			fit = 'B';
			alg = "Best-Fit";
			break;

		default:
			cerr << "ERROR - Please enter either F or B as an argument\n";
			exit(-1);
			break;
	}

	//pointer for iteration
	Block *blockIter;

	//Make linked-list of memory blocks

	//Make new block, size 1MB
	Avail = new Block(3*1024*1024, 1024*1024);

	//Make new block, size 1MB
	Avail->next = new Block(Avail->size + Avail->address, 1024*1024);
	Avail->prev = nullptr;
	blockIter = Avail->next;
	blockIter->prev = Avail;

	//Make new block, size 1MB
	blockIter->next = new Block(blockIter->size + blockIter->address, 1024*1024);
	blockIter->next->prev = blockIter;
	blockIter = blockIter->next;

	//Make new block, size 2MB
	blockIter->next = new Block(blockIter->size + blockIter->address, 2*1024*1024);
	blockIter->next->prev = blockIter;
	blockIter = blockIter->next;

	//Make new block, size 8MB
	blockIter->next = new Block(blockIter->size + blockIter->address, 8*1024*1024);
	blockIter->next->prev = blockIter;
	blockIter->next->next = nullptr;


	//open file
	ifstream inFile;
	inFile.open("data6.txt");

	if (!inFile)
	{
		cerr << "unable to open input file";
		exit(-1);
	}


	char inputLine[256];


	string own = "";
	string id = "";
	int size = 0;
	int count = 0;

	cout << "Simulation of Memory Management using the " << alg << " algorithm\n\n";
	cout << "Beginning of the run\n\n";


	//main loop
	while (inFile && inFile.peek() != '?')
	{
		if ((count % HOWOFTEN) == 0)
			print();


		//clear input
		memset(inputLine, '\0', 256);

		inFile.getline(inputLine, 256);

		//decide what to do
		switch(inputLine[0])
		{
			case 'L':
				strtok(inputLine, " \n\r");
				own = (string) strtok(nullptr, " \n\r");
				size = atoi(strtok(nullptr, " \n\r"));
				id = (string) strtok(nullptr, " \n\r");
				load(own, size, id);
				break;

			case 'A':
				strtok(inputLine, " \n\r");
				own = (string) strtok(nullptr, " \n\r");
				size = atoi(strtok(nullptr, " \n\r"));
				id = (string) strtok(nullptr, " \n\r");
				allocate(own, size, id);
				break;

			case 'D':
				strtok(inputLine, " \n\r");
				own = (string) strtok(nullptr, " \n\r");
				id = (string) strtok(nullptr, " \n\r");
				deallocate(own, id);
				break;

			case 'T':
				strtok(inputLine, " \n\r");
				own = (string) strtok(nullptr, " \n\r");
				terminate(own);
				break;

			default:
				break;
		}
	}


	cout << "\nEnd of the run\n\n";
	print();

	return 0;
}

/***************************************************************
  Function: load()

  Use: This loads a process into memory.

  Arguments: mem, this string is a process ID of a block of memory
	     size, this integer represents the size of requested
		   memory.
	     block, this string represents the block ID of the
		    memory block.

  Returns: Nothing
***************************************************************/
void load(string mem, int size, string block)
{
	Block *iter = Avail;
	Block *nextSpot = nullptr;
	int prevSize = INT_MAX;
	bool stop = false;

	cout << "Transaction: request to load process " << mem << ", block ID " << block << " using " << size << " bytes\n";

	if (fit == 'F')
	{
		//look for block of the needed size using First-Fit
		while (iter != nullptr && stop != true)
		{
			if (iter->size >= size)
				stop = true;
			else
				iter = iter->next;
		}
	}
	else
	{
		//look for block of the needed size using Best-Fit
		while (iter != nullptr)
		{
			if (iter->size >= size)
			{
				if (prevSize > iter->size)
				{
					prevSize = iter->size;
					nextSpot = iter;
				}
				iter = iter->next;
			}
			else
			{
				iter = iter->next;
			}
		}


		//obtain the best-fit
		if (nextSpot != nullptr)
		{
			stop = true;
			iter = nextSpot;
			nextSpot = nullptr;
		}
	}

	//if block wasn't found
	if (stop != true)
	{
		cerr << "Unable to comply as no block of adequate size is available\n\n";
	}
	else
	{
		cout << "Found a block of size " << iter->size << "\n";

		//make new block
		nextSpot = new Block(iter->address, size);

		//update sizes/address
		iter->address += size;
		iter->size -= size;

		if (iter->size <= 0)
		{
			deleteBlock(iter, &Avail);
		}

		iter = InUse;

		//get the last element in InUse
		if (iter != nullptr)
		{
			while (iter->next != nullptr)
				iter = iter->next;
		}

		//set defaults for block
		nextSpot->ownpid = mem;
		nextSpot->blockpid = block;
		nextSpot->next = nullptr;
		nextSpot->prev = nullptr;

		//decide where to put the block
		if (InUse == nullptr)
		{
			InUse = nextSpot;
		}
		else
		{
			iter->next = nextSpot;
			nextSpot->prev = iter;
		}

		cout << "Success in allocating a block\n\n";
	}
}

/***************************************************************
  Function: allocate()

  Use: This allocates a piece of memory for a process.

  Arguments: mem, this string represents the process ID of a
	          memory block
	     size, this integer represents the size of memory
                   that is requested.
	     block, this string represents the block ID of a
		    memory block.

  Returns: Nothing
***************************************************************/
void allocate(string mem, int size, string block)
{
	Block *iter = Avail;
	Block *nextSpot;
	int prevSize = INT_MAX;
	bool stop = false;

	cout << "Transaction: request to allocate " << size << " bytes for process " << mem << ", block ID: " << block << endl;


	if (fit == 'F')
	{
		//look for block of the needed size using First-Fit
		while (iter != nullptr && stop != true)
		{
			if (iter->size >= size)
				stop = true;
			else
				iter = iter->next;
		}
	}
	else
	{
		//look for block of the needed size using Best-Fit
		while (iter != nullptr)
		{
			if (iter->size >= size)
			{
				if (prevSize > iter->size)
				{
					prevSize = iter->size;
					nextSpot = iter;
				}

				iter = iter->next;
			}
			else
			{
				iter = iter->next;
			}
		}


		//get best fit
		if (nextSpot != nullptr)
		{
			stop = true;
			iter = nextSpot;
			nextSpot = nullptr;
		}
	}


	//error if we cant find the block
	if (stop != true)
	{
		cerr << "ERROR: No space found\n";
    }
	else
	{
		cout << "Found a block of size " << iter->size << "\n";

		//make a new block
		nextSpot = new Block(iter->address, size);

		//update size and address
		iter->address += size;
		iter->size -= size;

		if (iter->size <= 0)
		{
			deleteBlock(iter, &Avail);
		}

		iter = InUse;

		//get the last element in InUse
		if (iter != nullptr)
		{
			while (iter->next != nullptr)
				iter = iter->next;
		}

		//set defaults for block
		nextSpot->ownpid = mem;
		nextSpot->blockpid = block;
		nextSpot->next = nullptr;
		nextSpot->prev = nullptr;

		//where to put the block
		if (InUse == nullptr)
		{
			InUse = nextSpot;
		}
		else
		{
			iter->next = nextSpot;
			nextSpot->prev = iter;
		}

		cout << "Success in allocating a block\n\n";
	}
}

/***************************************************************
  Function: deallocate()

  Use: This deallocates a block of memory.

  Arguments: mem, this string represents the process ID of a
	          memory block.
             block, this string represents the block ID of the
                  memory block.

  Returns: Nothing
***************************************************************/
void deallocate(string mem, string block)
{
	cout << "Transaction: request to deallocate block ID " << block << " for process " << mem << "\n";

	Block *iter = InUse;
	Block *emplacer;
	bool found = false;

	//try and find the block of memory
	while (iter != nullptr && found != true)
	{
		if (iter->ownpid == mem && iter->blockpid == block)
			found = true;
		else
			iter = iter->next;
	}


	if (found == false)
	{
		cerr << "Unable to comply as the indicated block cannot be found.\n\n";
	}
	else
	{
		Block *prevHold = nullptr;

		//emplacer is a new block with copied contents
		emplacer = new Block(iter->address, iter->size);

		//take iter off InUse
		deleteBlock(iter, &InUse);


		iter = Avail;
		found = false;

		//find the spot to put emplacer
		while (iter != nullptr && found != true)
		{
			if (emplacer->address < iter->address)
			{
				found = true;
            }
			else
			{
				prevHold = iter;
				iter = iter->next;
			}
		}



		//end of block
		if (iter == nullptr)
		{
			//get iter to the end of Avail
			iter = Avail;

			while (iter->next != nullptr)
				iter = iter->next;


			//put address of block as next
			iter->next = emplacer;

			//set prev to second to last block
			emplacer->prev = iter;

			emplacer->next = nullptr;
		}
		else if (iter == Avail)
		{
			//shift 1st block to be second
			emplacer->next = Avail;
			Avail->prev = emplacer;
			emplacer->prev = nullptr;

			Avail = emplacer;
		}
		else
		{
			prevHold->next = emplacer;
			emplacer->next = iter;
			emplacer->prev = prevHold;
			iter->prev = emplacer;
		}


		//merge memory
		mergeAvail();
		cout << "Success in deallocating a block\n\n";
	}
}

/***************************************************************
  Function: terminate()

  Use: This terminates a process and frees up memory blocks to
       then be used.

  Arguments: mem, the process ID of the memory block
	     firstRun, this boolean value indicates if a step in
                       the recursive method is the first.

  Returns: Nothing
***************************************************************/
void terminate(string mem, bool firstRun)
{
	if (firstRun)
		cout << "Transaction: request to terminate process " << mem << "\n";


	Block *iter = InUse;
	Block *emplacer;
	bool found = false;

	//find a block with ownpid == mem
	while (iter != nullptr && found != true)
	{

		if (iter->ownpid == mem)
			found = true;
		else
			iter = iter->next;
	}



	if (found == false && firstRun == true)
	{
		cerr << "Unable to comply as the indicated block cannot be found.\n\n";
	}
	else if (found == true)
	{
		//previous holder
		Block *prevHold = nullptr;

		//emplacer is a new block with copied contents
		emplacer = new Block(iter->address, iter->size);

		emplacer->next = nullptr;
		emplacer->prev = nullptr;

		//take iter off InUse
		deleteBlock(iter, &InUse);

		//iter will be where to put emplacer
		iter = Avail;
		found = false;

		//find the spot to put emplacer
		while (iter != nullptr && found != true)
		{
			if (emplacer->address < iter->address)
				found = true;
			else
			{
				prevHold = iter;
				iter = iter->next;
			}
		}



		//end of block
		if (iter == nullptr)
		{
			//get iter back to the end of Avail
			iter = Avail;

			while (iter->next != nullptr)
				iter = iter->next;


			//put address of block as next(end)
			iter->next = emplacer;

			//set prev to second to last block
			emplacer->prev = iter;

			//at the end, there is no next
			emplacer->next = nullptr;
		}
		else if (iter == Avail)
		{

			//shift 1st block to be the second.
			emplacer->next = Avail;
			Avail->prev = emplacer;
			emplacer->prev = nullptr;

			Avail = emplacer;
		}
		else
		{
			prevHold->next = emplacer;
			emplacer->next = iter;
			emplacer->prev = prevHold;
			iter->prev = emplacer;
		}



		//recursive call until no more blocks of memory are found
		if (InUse != nullptr)
			terminate(mem, false);



		//1st call will merge
		if (firstRun)
		{
			mergeAvail();
			cout << "Success in terminating a process\n\n";
		}
	}
}

/***************************************************************
  Function: deleteBlock()

  Use: This deletes a Block in the linked list.

  Arguments: iter, a pointer to a Block that will be deleted
	     from, the address of a pointer that is the linked list
             starting point.

  Returns: Nothing
***************************************************************/
void deleteBlock(Block * iter, Block ** from)
{
	//start point
	if (iter->prev == nullptr && iter->next != nullptr)
	{
		*from = iter->next;
		iter->next->prev = nullptr;
	}
	//mid point
	else if (iter->prev != nullptr && iter->next != nullptr)
	{
		iter->prev->next = iter->next;
		iter->next->prev = iter->prev;
	}
	//start point
	else if (iter->prev == nullptr && iter->next == nullptr)
	{
		delete *from;
		*from = nullptr;
	}
	else
	{
		iter->prev->next = nullptr;
	}


	if (*from != nullptr)
		delete iter;
}

/***************************************************************
  Function: mergeAvail()

  Use: This merges the Blocks of memory that are available.

  Arguments: None

  Returns: Nothing
***************************************************************/
void mergeAvail()
{
	Block * iter = Avail;
	Block * hold;
	bool iterFlag = false;


	while (iter->next != nullptr)
	{
		if ((iter->size + iter->next->size) < 4*1024*1024)
		{
			iterFlag = true;
			cout << "Merging two blocks at " << iter->address << " and " << iter->next->address << "\n";

			iter->size += iter->next->size;
			hold = iter->next;
			iter->next = iter->next->next;

			delete hold;
		}

		iter = iter->next;
	}

	if (iterFlag)
		mergeAvail();
}

/***************************************************************
  Function: print()

  Use: This prints the contents of Avail and InUse.

  Arguments: None

  Returns: Nothing
***************************************************************/
void print()
{
	Block *iter = Avail;
	long int totalSize = 0;

	cout << "List of available blocks\n";

	if (iter == nullptr)
		cout << "(none)\n";

	//print Avail
	while (iter != nullptr)
	{
		cout << "Start Address = " << setw(8) << iter->address << " Size = " << setw(7) << iter->size << "\n";

		totalSize += iter->size;
		iter = iter->next;
	}


	cout << "Total size of the list = " << totalSize << "\n\n";


	iter = InUse;
	totalSize = 0;

	cout << "List of blocks in use\n";

	if (iter == nullptr)
		cout << "(none)\n";

	//print InUse
	while (iter != nullptr)
	{
		cout << "Start Address = " << setw(9) << iter->address << " Size = " << setw(8) << iter->size;
		cout << " Process ID = " << iter->ownpid << " Block ID = " << iter->blockpid << "\n";

		totalSize += iter->size;
		iter = iter->next;
	}

	cout << "Total size of the list = " << totalSize << "\n\n";
}

