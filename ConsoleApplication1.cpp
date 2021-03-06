// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
//#include <unistd.h>
#include <fstream>
#include <string>
#include <sys/types.h>
//#include <sys/shm.h>
//#include <semaphore.h>
//#include <sys/sem.h>
//#include <sys.ipc.h>


using namespace std;

void generateEmptyMemory(int,int,int);
void fifo(string[],int,int,int);
void lifo(string[], int, int,int);
void lruX(string[], int, int, int,int);
void ldf(string[], int, int,int);
void optX(string[], int, int, int,int);
void workingset(string[], int, int, int, int,int);

class pageFrame
{
public:
	int sizeFrame;
	int pageNumber;
	int processOccupied;
	string addressSpace;
	int timeEntered;
};
class segmentedTable
{
public:
	int arraySize;
};
int main()
{
	//remember to make this dynamic
	ifstream infile("C:\\Users\\JP\\Downloads\\hw3input.txt");
	string line;
	
	int input_from_file[8];
	string input_from_file_stringline_needs_to_be_delimited_on_space[1000];
	int count = 0;
	int countstr = 0;
	while (getline(infile, line))
	{
		if (count < 8)
		{
			input_from_file[count] = stoi(line);
			count++;
		}
		else
		{
			input_from_file_stringline_needs_to_be_delimited_on_space[countstr] = line;
			countstr++;
		}
		if (count > 8 || countstr > 999)
			break;
	}
	


	



	int total_number_of_page_frames = input_from_file[0];//in main memory
	int segment_length = input_from_file[1];//in number of pages
	int page_size = input_from_file[2];//in bytes
	int number_of_page_frames_per_process = input_from_file[3]; //or window size for working set
	int x_lru_opt = input_from_file[4];//used only int he LRU-X or OPT-X
	int min = input_from_file[5];
	int max = input_from_file[6];
	int processes = input_from_file[7];
	cout << "Total number of page frames: " << total_number_of_page_frames << endl;
	cout << "Maximum segment length: " << segment_length << endl;
	cout << "Page size in bytes: " << page_size << endl;
	cout << "Number of page frames per process or window size: " << number_of_page_frames_per_process << endl;
	cout << "X in LRU-X or OPT-X: " << x_lru_opt << endl;
	cout << "Min: " << min << endl;
	cout << "Max: " << max << endl;
	cout << "Number of processes to be created: " << processes << endl;


	cout << "BEGIN EXTRACT" << endl;
	string requests[2000];
	int doubleCount = 0;
	int endProcessCount = 0;
	
	for (int dCount = 0; dCount < 1000; dCount++)
	{

		if (input_from_file_stringline_needs_to_be_delimited_on_space[dCount].find(" ") < 4)
		{
			requests[doubleCount] = input_from_file_stringline_needs_to_be_delimited_on_space[dCount].substr(0, 3);
			requests[doubleCount + 1] = input_from_file_stringline_needs_to_be_delimited_on_space[dCount].substr(4);
			doubleCount += 2;
		}
		else
		{
			break;
		}
	}
	cout << "THE TOTAL NUMBER OF EXTRACTIONS: " << doubleCount << endl;
	

	for (int x = 0; x < (doubleCount / 2); x++)
	{
		cout << "Request from process: " << requests[(x * 2)] << " For logical address: " << requests[((x * 2) + 1)] << endl;
	}
	int requestSize = doubleCount;
	
	int choice = 0;
	cout << "Which algorithm to use?\n 1. FIFO\n 2. LIFO\n 3. LRU-X\n 4. LDF\n 5. OPT-X\n 6. Working Set\n 7. All\n";
	cin >> choice;
	switch (choice)
	{
	case 1:
		fifo(requests, total_number_of_page_frames, page_size, requestSize);
		break;
	case 2:
		lifo(requests, total_number_of_page_frames, page_size, requestSize);
		break;
	case 3:
		lruX(requests, total_number_of_page_frames, page_size, x_lru_opt, requestSize);
		break;
	case 4:
		ldf(requests, total_number_of_page_frames, page_size, requestSize);
		break;
	case 5:
		optX(requests, total_number_of_page_frames, page_size, x_lru_opt, requestSize);
		break;
	case 6:
		workingset(requests, total_number_of_page_frames, page_size, min, max, requestSize);
		break;
	case 7:
		fifo(requests, total_number_of_page_frames, page_size, requestSize);
		lifo(requests, total_number_of_page_frames, page_size, requestSize);
		lruX(requests, total_number_of_page_frames, page_size, x_lru_opt, requestSize);
		ldf(requests, total_number_of_page_frames, page_size, requestSize);
		optX(requests, total_number_of_page_frames, page_size, x_lru_opt, requestSize);
		workingset(requests, total_number_of_page_frames, page_size, min, max, requestSize);
		break;
	default:
		cout << "Error, defaulted" << endl;
		break;
	}


	
	cin.get();
	cin.get();
	
	//this goes in child i think, because the -1 in input strings will terminate the process
	//detectpagefault() searches for page fault, if found will start new process STARTDISK(r/w, memadd, diskadd) which has semaphore so that it can only write one at a time
	//writes to new frame index

	//remember as they come in to populate the memory_frames table, which would be -1 otherwise(?)



    return 0;
}
void generateEmptyMemory(int total_number_of_page_frames, int page_size)
{
	pageFrame *frames = new pageFrame[total_number_of_page_frames];
	for (int x = 0; x < total_number_of_page_frames; x++)
	{
		frames[x].pageNumber = x;
		frames[x].sizeFrame = page_size;
		frames[x].addressSpace = "";
		frames[x].processOccupied = 0;
		frames[x].timeEntered = -1;
	} 
}
void fifo(string requests[], int total_number_of_page_frames, int page_size, int requestSize)
{

	cout << "Generating memory of size " << total_number_of_page_frames << " page frames with a size of " << page_size << " bytes." << endl;
	pageFrame *frames = new pageFrame[total_number_of_page_frames];
	for (int x = 0; x < total_number_of_page_frames; x++)
	{
		frames[x].pageNumber = x;
		frames[x].sizeFrame = page_size;
		frames[x].addressSpace = "";
		frames[x].processOccupied = 0;
		frames[x].timeEntered = -1;
	}
	//will have to add the faults for specific processes
	int totalfaults = 0;
	cout << "We will now solve for " << requestSize << " requests." << endl;

	for (int y = 0; y < (requestSize/2); y++)
	{
		cout << "Searching for spot for " << requests[y * 2] << " " << requests[(y * 2) + 1] << endl;
		//add it to main memory if not there
		for (int x = 0; x < total_number_of_page_frames; ++x)
		{
			cout << "Checking page frame number " << x + 1 << " out of " << total_number_of_page_frames << " for spot." << endl;
			if (requests[(y * 2) + 1] == "-1")
			{
				cout << "Process " << requests[y * 2] << " has terminated." << endl;;
				break;
			}
			cout << "Checking page frame number " << x+1 << " out of " << total_number_of_page_frames << " for spot." << endl;
		
			if (frames[x].addressSpace == "")
			{
				//this is an empty space on frames[x] add the info to it
				frames[x].processOccupied = stoi(requests[y * 2]);
				frames[x].addressSpace = requests[(y * 2) + 1];
				frames[x].timeEntered = y;
				cout << "Address: " << requests[(y * 2) + 1] << " added to main memory." << endl;
				break;
				//on to the next request
			}
			else if (frames[x].addressSpace == requests[(y * 2) + 1])
			{
				//this address is already in memory
				cout << "Address already in memory." << endl;
				break;
			}
			else if (x == total_number_of_page_frames - 1)
			{
				//none of the pages could catch this address, it goes to disk, fault counter increases
				cout << "FAULT" << endl;
				//replace the one with the lowest timeEntered value
				int lowest = 9999;
				int idofframe;
				for (int replaceCount = 0; replaceCount < total_number_of_page_frames; replaceCount++)
				{
					if (frames[replaceCount].timeEntered < lowest)
					{
						lowest = frames[replaceCount].timeEntered;
						idofframe = replaceCount;
					}

				}
				cout << "The frame " << frames[idofframe].pageNumber << " was the oldest process, it will be replaced by " << requests[(y * 2) + 1] <<
					" from process " << requests[y * 2] << "." << endl;
				frames[idofframe].processOccupied = stoi(requests[y * 2]);
				frames[idofframe].addressSpace = requests[(y * 2) + 1];
				frames[idofframe].timeEntered = y;
				totalfaults += 1;
				break;
			}
			else
			{
				cout << "The address does not fit here, or is not found yet." << endl;
			}
		}
		cout << "Addressing resolved, checking next request." << endl;
	}
	cout << "FIFO had " << totalfaults << " total faults." << endl;
	for (int xy = 0; xy < total_number_of_page_frames; xy++)
	{
		cout << "This is page: " << xy << " address: " << frames[xy].addressSpace << ". Placed by process " << frames[xy].processOccupied << "." << endl;
	}
	//create the processes
	// seg#|page#|offset seg table -> segment with x pages will point to a specific frame
	/*
	long key = 1283987;
	int id = semget(key, 1, 0666 | IPC_CREAT);

	pid_t pid;
	for (int x = 0; x < processes; x++)
	{
	pid = fork();
	if (pid = 0)
	{
	//wait
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	//child process
	generateMemory(total_number_of_page_frames, page_size)
	//signal
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	exit(0);
	}
	else
	{
	//parent
	}
	}
	*/
}
void lifo(string requests[], int total_number_of_page_frames, int page_size, int requestSize)
{


	cout << "Generating memory of size " << total_number_of_page_frames << " page frames with a size of " << page_size << " bytes." << endl;
	pageFrame *frames = new pageFrame[total_number_of_page_frames];
	for (int x = 0; x < total_number_of_page_frames; x++)
	{
		frames[x].pageNumber = x;
		frames[x].sizeFrame = page_size;
		frames[x].addressSpace = "";
		frames[x].processOccupied = 0;
		frames[x].timeEntered = -1;
	}
	//will have to add the faults for specific processes
	int totalfaults = 0;
	cout << "We will now solve for " << requestSize << " requests." << endl;

	for (int y = 0; y < (requestSize / 2); y++)
	{

		cout << "Searching for spot for " << requests[y * 2] << " " << requests[(y * 2) + 1] << endl;
		//add it to main memory if not there
		for (int x = 0; x < total_number_of_page_frames; ++x)
		{			
			if (requests[(y * 2) + 1] == "-1")
			{
				cout << "Process " << requests[y * 2] << " has terminated." << endl;;
				break;
			}
			cout << "Checking page frame number " << x + 1 << " out of " << total_number_of_page_frames << " for spot." << endl;

			if (frames[x].addressSpace == "")
			{
				//this is an empty space on frames[x] add the info to it
				frames[x].processOccupied = stoi(requests[y * 2]);
				frames[x].addressSpace = requests[(y * 2) + 1];
				frames[x].timeEntered = y;
				cout << "Address: " << requests[(y * 2) + 1] << " added to main memory." << endl;
				break;
				//on to the next request
			}
			else if (frames[x].addressSpace == requests[(y * 2) + 1])
			{
				//this address is already in memory
				cout << "Address already in memory." << endl;
				break;
			}
			else if (x == total_number_of_page_frames - 1)
			{
				//none of the pages could catch this address, it goes to disk, fault counter increases
				cout << "FAULT" << endl;
				//replace the one with the lowest timeEntered value
				int highest = 0;
				int idofframe;
				for (int replaceCount = 0; replaceCount < total_number_of_page_frames; replaceCount++)
				{
					if (frames[replaceCount].timeEntered > highest)
					{
						highest = frames[replaceCount].timeEntered;
						idofframe = replaceCount;
					}

				}
				cout << "The frame " << frames[idofframe].pageNumber << " was the youngest process, it will be replaced by " << requests[(y * 2) + 1] <<
					" from process " << requests[y * 2] << "." << endl;
				frames[idofframe].processOccupied = stoi(requests[y * 2]);
				frames[idofframe].addressSpace = requests[(y * 2) + 1];
				frames[idofframe].timeEntered = y;
				totalfaults += 1;
				break;
			}
			else
			{
				cout << "The address does not fit here, or is not found yet." << endl;
			}
		}
		cout << "Addressing resolved." << endl;
	}
	cout << "LIFO had " << totalfaults << " total faults." << endl;
	for (int xy = 0; xy < total_number_of_page_frames; xy++)
	{
		cout << "This is page: " << xy << " address: " << frames[xy].addressSpace << ". Placed by process " << frames[xy].processOccupied << "." << endl;
	}
	//create the processes
	// seg#|page#|offset seg table -> segment with x pages will point to a specific frame
	/*
	long key = 1283987;
	int id = semget(key, 1, 0666 | IPC_CREAT);

	pid_t pid;
	for (int x = 0; x < processes; x++)
	{
	pid = fork();
	if (pid = 0)
	{
	//wait
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	//child process
	generateMemory(total_number_of_page_frames, page_size)
	//signal
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	exit(0);
	}
	else
	{
	//parent
	}
	}
	*/
}
void lruX(string requests[], int total_number_of_page_frames, int page_size, int xlru, int requestSize)
{


	cout << "Generating memory of size " << total_number_of_page_frames << " page frames with a size of " << page_size << " bytes." << endl;
	pageFrame *frames = new pageFrame[total_number_of_page_frames];
	for (int x = 0; x < total_number_of_page_frames; x++)
	{
		frames[x].pageNumber = x;
		frames[x].sizeFrame = page_size;
		frames[x].addressSpace = "";
		frames[x].processOccupied = 0;
		frames[x].timeEntered = -1;
	}
	//will have to add the faults for specific processes
	int totalfaults = 0;
	cout << "We will now solve for " << requestSize << " requests." << endl;
	string *lru = new string[xlru];
	for (int y = 0; y < (requestSize / 2); y++)
	{
		cout << "Searching for spot for " << requests[y * 2] << " " << requests[(y * 2) + 1] << endl;
		//add it to main memory if not there
		for (int x = 0; x < total_number_of_page_frames; ++x)
		{
			cout << "Checking page frame number " << x + 1 << " out of " << total_number_of_page_frames << " for spot." << endl;
			if (requests[(y * 2) + 1] == "-1")
			{
				cout << "Process " << requests[y * 2] << " has terminated." << endl;;
				break;
			}
			cout << "Checking page frame number " << x + 1 << " out of " << total_number_of_page_frames << " for spot." << endl;

			if (frames[x].addressSpace == "")
			{
				//this is an empty space on frames[x] add the info to it
				frames[x].processOccupied = stoi(requests[y * 2]);
				frames[x].addressSpace = requests[(y * 2) + 1];
				frames[x].timeEntered = y;
				cout << "Address: " << requests[(y * 2) + 1] << " added to main memory." << endl;
				break;
				//on to the next request
			}
			else if (frames[x].addressSpace == requests[(y * 2) + 1])
			{
				//this address is already in memory
				cout << "Address already in memory." << endl;
				//update to have higher time for LRU
				frames[x].timeEntered = y;
				break;
			}
			else if (x == total_number_of_page_frames - 1)
			{
				//none of the pages could catch this address, it goes to disk, fault counter increases
				cout << "FAULT" << endl;
				//find the values with lowest times (meaning least recently used). the ones that were hit but not replaced were given higher times
				//store in lru string array of size xlru
				int lowest = 9999;
				int idofframe;
				for (int replaceCount = 0; replaceCount < total_number_of_page_frames; replaceCount++)
				{
					if (frames[replaceCount].timeEntered < lowest)
					{
						lowest = frames[replaceCount].timeEntered;
						idofframe = replaceCount;
					}

				}
				cout << "The frame " << frames[idofframe].pageNumber << " was the oldest process, it will be replaced by " << requests[(y * 2) + 1] <<
					" from process " << requests[y * 2] << "." << endl;
				frames[idofframe].processOccupied = stoi(requests[y * 2]);
				frames[idofframe].addressSpace = requests[(y * 2) + 1];
				frames[idofframe].timeEntered = y;
				totalfaults += 1;
				break;
			}
			else
			{
				cout << "The address does not fit here, or is not found yet." << endl;
			}
		}
		cout << "Addressing resolved, checking next request." << endl;
	}
	cout << "LRU-X had " << totalfaults << " total faults." << endl;
	for (int xy = 0; xy < total_number_of_page_frames; xy++)
	{
		cout << "This is page: " << xy << " address: " << frames[xy].addressSpace << ". Placed by process " << frames[xy].processOccupied << "." << endl;
	}
	//create the processes
	// seg#|page#|offset seg table -> segment with x pages will point to a specific frame
	/*
	long key = 1283987;
	int id = semget(key, 1, 0666 | IPC_CREAT);

	pid_t pid;
	for (int x = 0; x < processes; x++)
	{
	pid = fork();
	if (pid = 0)
	{
	//wait
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	//child process
	generateMemory(total_number_of_page_frames, page_size)
	//signal
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	exit(0);
	}
	else
	{
	//parent
	}
	}
	*/
}
void ldf(string requests[], int total_number_of_page_frames, int page_size, int requestSize)
{



	cout << "Generating memory of size " << total_number_of_page_frames << " page frames with a size of " << page_size << " bytes." << endl;
	pageFrame *frames = new pageFrame[total_number_of_page_frames];
	for (int x = 0; x < total_number_of_page_frames; x++)
	{
		frames[x].pageNumber = x;
		frames[x].sizeFrame = page_size;
		frames[x].addressSpace = "";
		frames[x].processOccupied = 0;
		frames[x].timeEntered = -1;
	}
	//will have to add the faults for specific processes
	int totalfaults = 0;
	cout << "We will now solve for " << requestSize << " requests." << endl;

	for (int y = 0; y < (requestSize / 2); y++)
	{
		cout << "Searching for spot for " << requests[y * 2] << " " << requests[(y * 2) + 1] << endl;
		//add it to main memory if not there
		for (int x = 0; x < total_number_of_page_frames; ++x)
		{
			cout << "Checking page frame number " << x + 1 << " out of " << total_number_of_page_frames << " for spot." << endl;

			if (frames[x].addressSpace == "")
			{
				//this is an empty space on frames[x] add the info to it
				frames[x].processOccupied = stoi(requests[y * 2]);
				frames[x].addressSpace = requests[(y * 2) + 1];
				frames[x].timeEntered = y;
				cout << "Address: " << requests[(y * 2) + 1] << " added to main memory." << endl;
				break;
				//on to the next request
			}
			else if (frames[x].addressSpace == requests[(y * 2) + 1])
			{
				//this address is already in memory
				cout << "Address already in memory." << endl;
				break;
			}
			else if (x == total_number_of_page_frames - 1)
			{
				//none of the pages could catch this address, it goes to disk, fault counter increases
				cout << "FAULT" << endl;
				totalfaults += 1;
				break;
			}
			else
			{
				cout << "The address does not fit here, or is not found yet." << endl;
			}
		}
		cout << "Addressing resolved, checking next request." << endl;
	}
	cout << "LDF had " << totalfaults << " total faults." << endl;
	for (int xy = 0; xy < total_number_of_page_frames; xy++)
	{
		cout << "This is page: " << xy << " address: " << frames[xy].addressSpace << endl;
	}
	//create the processes
	// seg#|page#|offset seg table -> segment with x pages will point to a specific frame
	/*
	long key = 1283987;
	int id = semget(key, 1, 0666 | IPC_CREAT);

	pid_t pid;
	for (int x = 0; x < processes; x++)
	{
	pid = fork();
	if (pid = 0)
	{
	//wait
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	//child process
	generateMemory(total_number_of_page_frames, page_size)
	//signal
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	exit(0);
	}
	else
	{
	//parent
	}
	}
	*/
	
}
void optX(string requests[], int total_number_of_page_frames, int page_size, int x, int requestSize)
{



	cout << "Generating memory of size " << total_number_of_page_frames << " page frames with a size of " << page_size << " bytes." << endl;
	pageFrame *frames = new pageFrame[total_number_of_page_frames];
	for (int x = 0; x < total_number_of_page_frames; x++)
	{
		frames[x].pageNumber = x;
		frames[x].sizeFrame = page_size;
		frames[x].addressSpace = "";
		frames[x].processOccupied = 0;
		frames[x].timeEntered = -1;
	}
	//will have to add the faults for specific processes
	int totalfaults = 0;
	cout << "We will now solve for " << requestSize << " requests." << endl;

	for (int y = 0; y < (requestSize / 2); y++)
	{
		cout << "Searching for spot for " << requests[y * 2] << " " << requests[(y * 2) + 1] << endl;
		//add it to main memory if not there
		for (int x = 0; x < total_number_of_page_frames; ++x)
		{
			cout << "Checking page frame number " << x + 1 << " out of " << total_number_of_page_frames << " for spot." << endl;

			if (frames[x].addressSpace == "")
			{
				//this is an empty space on frames[x] add the info to it
				frames[x].processOccupied = stoi(requests[y * 2]);
				frames[x].addressSpace = requests[(y * 2) + 1];
				frames[x].timeEntered = y;
				cout << "Address: " << requests[(y * 2) + 1] << " added to main memory." << endl;
				break;
				//on to the next request
			}
			else if (frames[x].addressSpace == requests[(y * 2) + 1])
			{
				//this address is already in memory
				cout << "Address already in memory." << endl;
				break;
			}
			else if (x == total_number_of_page_frames - 1)
			{
				//none of the pages could catch this address, it goes to disk, fault counter increases
				cout << "FAULT" << endl;
				totalfaults += 1;
				break;
			}
			else
			{
				cout << "The address does not fit here, or is not found yet." << endl;
			}
		}
		cout << "Addressing resolved, checking next request." << endl;
	}
	cout << "OPT-X had " << totalfaults << " total faults." << endl;
	for (int xy = 0; xy < total_number_of_page_frames; xy++)
	{
		cout << "This is page: " << xy << " address: " << frames[xy].addressSpace << endl;
	}
	//create the processes
	// seg#|page#|offset seg table -> segment with x pages will point to a specific frame
	/*
	long key = 1283987;
	int id = semget(key, 1, 0666 | IPC_CREAT);

	pid_t pid;
	for (int x = 0; x < processes; x++)
	{
	pid = fork();
	if (pid = 0)
	{
	//wait
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	//child process
	generateMemory(total_number_of_page_frames, page_size)
	//signal
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	exit(0);
	}
	else
	{
	//parent
	}
	}
	*/
}
void workingset(string requests[], int total_number_of_page_frames, int page_size, int min, int max, int requestSize)
{



	cout << "Generating memory of size " << total_number_of_page_frames << " page frames with a size of " << page_size << " bytes." << endl;
	pageFrame *frames = new pageFrame[total_number_of_page_frames];
	for (int x = 0; x < total_number_of_page_frames; x++)
	{
		frames[x].pageNumber = x;
		frames[x].sizeFrame = page_size;
		frames[x].addressSpace = "";
		frames[x].processOccupied = 0;
		frames[x].timeEntered = -1;
	}
	//will have to add the faults for specific processes
	int totalfaults = 0;
	cout << "We will now solve for " << requestSize << " requests." << endl;

	for (int y = 0; y < (requestSize / 2); y++)
	{
		cout << "Searching for spot for " << requests[y * 2] << " " << requests[(y * 2) + 1] << endl;
		//add it to main memory if not there
		for (int x = 0; x < total_number_of_page_frames; ++x)
		{
			cout << "Checking page frame number " << x + 1 << " out of " << total_number_of_page_frames << " for spot." << endl;

			if (frames[x].addressSpace == "")
			{
				//this is an empty space on frames[x] add the info to it
				frames[x].processOccupied = stoi(requests[y * 2]);
				frames[x].addressSpace = requests[(y * 2) + 1];
				frames[x].timeEntered = y;
				cout << "Address: " << requests[(y * 2) + 1] << " added to main memory." << endl;
				break;
				//on to the next request
			}
			else if (frames[x].addressSpace == requests[(y * 2) + 1])
			{
				//this address is already in memory
				cout << "Address already in memory." << endl;
				break;
			}
			else if (x == total_number_of_page_frames - 1)
			{
				//none of the pages could catch this address, it goes to disk, fault counter increases
				cout << "FAULT" << endl;
				totalfaults += 1;
				break;
			}
			else
			{
				cout << "The address does not fit here, or is not found yet." << endl;
			}
		}
		cout << "Addressing resolved, checking next request." << endl;
	}
	cout << "Working Set had " << totalfaults << " total faults." << endl;
	for (int xy = 0; xy < total_number_of_page_frames; xy++)
	{
		cout << "This is page: " << xy << " address: " << frames[xy].addressSpace << endl;
	}
	//create the processes
	// seg#|page#|offset seg table -> segment with x pages will point to a specific frame
	/*
	long key = 1283987;
	int id = semget(key, 1, 0666 | IPC_CREAT);

	pid_t pid;
	for (int x = 0; x < processes; x++)
	{
	pid = fork();
	if (pid = 0)
	{
	//wait
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	//child process
	generateMemory(total_number_of_page_frames, page_size)
	//signal
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	semop(sid, &sb, 1);
	exit(0);
	}
	else
	{
	//parent
	}
	}
	*/
}