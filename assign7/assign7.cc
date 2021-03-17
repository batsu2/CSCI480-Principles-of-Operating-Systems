/****************************************************************

 CSCI 480 - ASSIGNMENT 7 - SPRING 2020

 PROGRAMMER:  Bryan Butz

 SECTION:  Section 1

 TA:  Joshua Boley

 zID:  z1836033

 DUE DATE:  4/23/2020

 PURPOSE:  This program will maintain a File Allocation Table and
	   simulate "FAT-12" on a small disk. Each block will be 512
	   bytes in size. With FAT-12, we have 4096 blocks, thus the
	   the disk is only 2 MB in size. It will read an input file,
	   determine the desired operation, and perform that operation,
           on the FAT.

*****************************************************************/

#define BLOCKSIZE 512
#define MAXENTRIES 12
#define HOWOFTEN 5


#include<iostream>
#include<iomanip>
#include<cstring>
#include<fstream>
#include<cmath>
#include<cstdlib>
#include<vector>

using namespace std;



struct Entry
{
	int startPos;
	string fileName;
	int size;

	Entry(int x, string y, int z)
	{
		startPos = x;
		fileName = y;
		size = z;
	}
};


bool newFile(string, int);
bool rename(string, string);
bool modify(string, int);
bool delFile(string);
bool copy(string, string);
void checkFAT();
Entry* findFileWithName(string name);
int findEmptySpot(int initalPos);
void printFAT();
void printDirectory();


vector<Entry*> dir;
short FAT[4096];
const char* fileName = "data7.txt";



int main(int argc, char ** argv)
{
	FAT[0] = -1;
	FAT[4095] = -1;
	dir.push_back(new Entry(0, ".", 512));
	dir.push_back(new Entry(-1, "..", 0));


	//open file
	ifstream inFile;
	inFile.open(fileName);


	//exit if open failed
	if (!inFile)
	{
		cerr << "unable to open input file";
		exit(-1);
	}

	cout << "Beginning of the FAT simulation\n";

	//read line
	char inputLine[256];

	//basic variables for multi-use
	int count = 0;
	bool end = false;
	string nameHolder1;
	string nameHolder2;
	int sizeHolder;


	//main event loop
	while (inFile && !end)
	{
		//print every HOWOFTEN
		if (count % HOWOFTEN == 0)
		{
			printDirectory();
			printFAT();
		}

		//clear previous input
		memset(inputLine, '\0', 256);

		//read the input line
		inFile.getline(inputLine, 256);


		//Read function character and act accordingly
		switch(inputLine[0])
		{
			case 'C':
				cout << "Transaction:  Copy a file\n";

				strtok(inputLine, " \n\r");
				nameHolder1 = (string) (strtok(nullptr, " \n\r"));
				nameHolder2 = (string) (strtok(nullptr, " \n\r"));


				if (copy(nameHolder1, nameHolder2))
				{
					cout << "Successfully copied an existing file, " << nameHolder1 << ", to a new file, " << nameHolder2 << endl;
				}

				break;

			case 'D':
				cout << "Transaction:  Delete a file\n";
				strtok(inputLine, " \n\r");
				nameHolder1 = (string) (strtok(nullptr, " \n\r"));

				if (delFile(nameHolder1))
				{
					cout << "Successfully deleted a file, " << nameHolder1 << endl;
				}

				break;

			case 'N':
				cout << "Transaction:  Add a new file\n";

				strtok(inputLine, " \n\r");
				nameHolder1 = (string) (strtok(nullptr, " \n\r"));
				sizeHolder = atoi(strtok(nullptr, " \n\r"));

				if (newFile(nameHolder1, sizeHolder))
				{
					cout << "Successfuly added a new file, " << nameHolder1 << ", of size " << sizeHolder << endl;
				}

				break;

			case 'M':
				cout << "Transaction:  Modify a file\n";

				strtok(inputLine, " \n\r");
				nameHolder1 = (string) (strtok(nullptr, " \n\r"));
				sizeHolder = atoi(strtok(nullptr, " \n\r"));

				if (modify(nameHolder1, sizeHolder))
				{
					cout << "Successfully modified a file, " << nameHolder1 << endl;
				}

				break;

			case 'R':
				cout << "Transaction:  Rename a file\n";

				strtok(inputLine, " \n\r");
				nameHolder1 = (string) (strtok(nullptr, " \n\r"));
				nameHolder2 = (string) (strtok(nullptr, " \n\r"));

				if (rename(nameHolder1, nameHolder2))
				{
					cout << "Successfully changed the file name " << nameHolder1 << " to " << nameHolder2 << endl;
				}

				break;

			case '?':
				end = true;
				break;

			default:
				break;
		}

		count++;
	}

	//final print
	cout << "\nEnd of the FAT simulation\n";
	printDirectory();
	printFAT();

	return 0;
}


/***************************************************************
  Function: printDirectory()

  Use: This prints everything in the current directory

  Arguments: None

  Returns: Nothing
***************************************************************/
void printDirectory()
{
	cout << "\nDirectory Listing\n";
	int sizeCount = 0;
	int clustCount;

	//iterate through every element in "dir"
	for (vector<Entry*>::iterator it = dir.begin(); it != dir.end(); it++)
	{
		sizeCount += (*it)->size;

		cout << "File name: " << setw(23) << left << (*it)->fileName;
		cout << "File Size: " << setw(5) << right << (*it)->size << endl;

		cout << "Cluster(s) in use:  ";

		if ((*it)->startPos != -1)
		{
			clustCount = 1;

			if (FAT[(*it)->startPos] == -1)
			{
				cout << setw(6) << (*it)->startPos << endl;
			}
			else
			{
				cout << setw(6) << (*it)->startPos;

				for(int pos = (*it)->startPos; FAT[pos] != -1; pos = FAT[pos])
				{
					if (clustCount % 12 == 0)
						cout << endl << "                    ";


					cout << setw(6) << FAT[pos];
					clustCount++;

				}

				cout << endl;
			}
		}
		else
		{
			cout << "(none)\n";
		}
	}

	//final results
	cout << "Files:  " << dir.size();
	cout << "  Total Size:  " << sizeCount;
	cout << " bytes\n";

}


/***************************************************************
  Function: printFAT()

  Use: This prints the FAT up to its 240th block

  Arguments: None

  Returns: Nothing
***************************************************************/
void printFAT()
{
	cout << "\nContents of the File Allocation Table";
	int lineno = 0, lineCount = 0;

	while (lineCount < 240)
	{
		//print line Numbers
		if (lineCount % 12 == 0)
		{
			//for the proper output format
			if (lineCount == 0)
				cout << "\n#" << setw(3) << setfill('0') << lineno;
			else
				cout << " \n#" << setw(3) << setfill('0') << lineno;


			lineno += 12;
			cout << "-" << setw(3) << setfill('0') << lineno-1 << " ";
		}

		//print each block content
		cout << setw(6) << setfill(' ') << FAT[lineCount];
		lineCount++;
	}

	cout << setfill(' ') << " " << endl << endl;
}



/***************************************************************
  Function: newFile

  Use: This creates a new file in the directory and fills the
       corresponding FAT.

  Arguments: nameHolder, a string used to represent the name of
                         the new file
	     sizeHolder, a string used to represent the size of the
                     new file

  Returns: A boolean value of true if successful or false if
	   unsucessfull.
***************************************************************/
bool newFile(string nameHolder, int sizeHolder)
{
	bool rv = true;
	int currentSpot = 0;
	int nextSpot = -1;

	//check if file exsists
	Entry * fileNameHolder = findFileWithName(nameHolder);



	if (fileNameHolder == nullptr)
	{
		if (sizeHolder != 0)
		{
			//find the first spot available
			currentSpot = findEmptySpot(0);

			//create a new entry in dir
			dir.push_back(new Entry(currentSpot, nameHolder, sizeHolder));

			sizeHolder -= BLOCKSIZE;

			//allocate multiple blocks to satisfy size
			while (sizeHolder > 0)
			{
				nextSpot = findEmptySpot(currentSpot+1);

				FAT[currentSpot] = nextSpot;
				currentSpot = nextSpot;

				sizeHolder -= BLOCKSIZE;
			}


			FAT[currentSpot] = -1;
		}
		else
		{
			dir.push_back(new Entry(-1, nameHolder, 0));
		}
	}
	else
	{
		cout << "File already exsists!\n";
		rv = true;
	}

	checkFAT();
	return rv;
}


/***************************************************************
  Function: modify

  Use: This modifies an existing file, used to change the filesize

  Arguments: name, a string used to represent the name of
                         the file to modify
	     size, a string used to represent the size of the file

  Returns: A boolean value of true if successful or false if
	   unsuccessful.
***************************************************************/
bool modify(string name, int size)
{
	Entry * file = findFileWithName(name);

	if (file == nullptr)
	{
		cout << "Error!  The file name, " << name << ", was not found.\n";
		return false;
	}

	//tempname has spaces, and since a file shouldn't have
	//spces, there'll be no rename collisions
	string tempname = (file->fileName) + "     ";

	string oldname = file->fileName;

	//create new file, delete old file, rename file back
	newFile(tempname, size);
	delFile(oldname);
	rename(tempname, oldname);

	return true;
}


/***************************************************************
  Function: rename

  Use: This renames a file to a desired new name.

  Arguments: name1, a string used to represent the filename to
                    be renamed
             name2, a string used to represent the desired new
                    filename

  Returns: A boolean value of true if successful or false if
	   unsuccessful
***************************************************************/
bool rename(string name1, string name2)
{
	Entry* file1 = findFileWithName(name1);

	if (file1 == nullptr)
	{
		cout << "Error!  The old file name, " << name1 << ", was not found.\n";
		return false;
	}

	Entry* file2 = findFileWithName(name2);

	if (file2 != nullptr)
	{
		cout << "File already exsists!\n";
		return false;
	}

	file1->fileName = name2;
	return true;
}


/***************************************************************
  Function: copy

  Use: This copies a file with a given filename to another

  Arguments: name1, a string used to represent the name of the
                    desired fiel to copy
	     name2, a string used to represent the name of the
                    desired copy

  Returns: A boolean value of true if successful or false if
	   unsuccessful.
***************************************************************/
bool copy(string name1, string name2)
{
	Entry* file1 = findFileWithName(name1);

	//error check
	if (file1 == nullptr)
	{
		cout << "Error!  The old file name, " << name1 << ", was not found.\n";
		return false;
	}

	Entry* file2 = findFileWithName(name2);

	//error check
	if (file2 != nullptr)
	{
		cout << "Error!  The new file name, " << name2 << ", already exists.\n";
		return false;
	}

	newFile(name2, file1->size);
	return true;
}


/***************************************************************
  Function: delFile

  Use: This deletes a file from the directory and removes its
       place in the corresponding FAT entry

  Arguments: name, a string used to represent the name of the file
                   to be deleted

  Returns: A boolean value of true if successful or false if
	  unsuccessful.
***************************************************************/
bool delFile(string name)
{
	Entry* file1 = findFileWithName(name);

	if (file1 == nullptr)
	{
		cout << "Error!  The file name, " << name << ", was not found.\n";
		return false;
	}

	int pos = file1->startPos;
	int nextPos;

	//if startPos = -1, then the size = 0
	if (pos != -1)
	{
		//iterate through this file's FAT data
		while (FAT[pos] != -1)
		{
			nextPos = FAT[pos];
			FAT[pos] = 0;
			pos = nextPos;
		}

		//replace final -1 to 0
		FAT[pos] = 0;
	}

	//erase the specific file entry
	bool stopCondition = false;

	for (vector<Entry*>::iterator it = dir.begin(); it != dir.end() && !stopCondition; it++)
	{
		if ((*it)->fileName == name)
		{
			dir.erase(it);
			stopCondition = true;
		}
	}



	//update FAT state
	checkFAT();

	return true;
}


/***************************************************************
  Function: checkFAT()

  Use: This checks to see if the current directory size is
       enough.

  Arguments: None

  Returns: Nothing
***************************************************************/
void checkFAT()
{
	//shrink
	if (((float)dir.front()->size / (float)BLOCKSIZE) > ceil((float)dir.size() / (float)MAXENTRIES))
	{
		int prevPos = 0;
		int curPos = 0;


		while (FAT[curPos] != -1)
		{
			prevPos = curPos;
			curPos = FAT[curPos];
		}

		FAT[curPos] = 0;
		FAT[prevPos] = -1;

		dir.front()->size = (int) ((float)BLOCKSIZE * ceil((float)dir.size() / (float)MAXENTRIES));
	}
	//grow
	else if (((float) dir[0]->size / (float) BLOCKSIZE) < ceil( (float) dir.size() / (float) MAXENTRIES))
	{
		int curPos = 0;
		int nextPos;


		while (FAT[curPos] != -1)
		{
			curPos = FAT[curPos];
		}

		nextPos = findEmptySpot(curPos);

		FAT[curPos] = nextPos;
		FAT[nextPos] = -1;

		dir.front()->size = (int) ((float)BLOCKSIZE * ceil((float)dir.size() / (float)MAXENTRIES));
	}
}


/***************************************************************
  Function: findEmptySpot()

  Use: This finds a '0' in the FAT following initialPos

  Arguments: initialPost, the starting position for the search

  Returns: The position of the next zero or -1 if the FAT is full
***************************************************************/
int findEmptySpot(int initalPos)
{
	int rv = -1;

	for(bool found = false; !found && initalPos < 4096; initalPos++)
	{
		if (FAT[initalPos] == 0)
		{
			found = true;
			rv = initalPos;
		}
	}

	return rv;
}


/***************************************************************
  Function: findFileWithName()

  Use: This finds a file in a directory with the same name

  Arguments: name, a string representing the name to look up

  Returns: A pointer to the Entry in dir with the specified name
***************************************************************/
Entry* findFileWithName(string name)
{
	Entry* rv = nullptr;
	bool stopCondition = false;

	for (vector<Entry*>::iterator it = dir.begin(); it != dir.end() && !stopCondition; it++)
	{
		if ((*it)->fileName == name )
		{
			rv = *it;
			stopCondition = true;
		}
	}

	return rv;
}
