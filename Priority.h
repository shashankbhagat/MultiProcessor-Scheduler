// File Name: Priority.h
// Author: Shashank Bhagat
// Date: 11/13/2017
// Compiler Used: VC++
// Notes: This is a derived class header file. Includes all the declaration of the variables and member functions in private and public section.
#pragma once
#ifndef PRIORITY
#include "RecurringPCB.h"
#include "NonRecurringPCB.h"
#include<list>
using namespace std;

class Priority :
	public RecurringPCB, public NonRecurringPCB
{
private:
	
	int Find_Pjob(); //find Priority Job
	void SortPriorityQueue();

public:
	int PriorityNumber;
	list<Priority> PriorityProcess;
	Priority();
	Priority(string);
	~Priority();
	void set_Sleeptime(const int&);
	int get_SleepTime();
	
};
#endif // !PRIORITY