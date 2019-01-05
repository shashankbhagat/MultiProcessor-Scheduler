// File Name: FCFS.cpp
// Author: Shashank Bhagat
// Date: 12/01/2017
// Compiler Used: VC++
// Notes: This is a implementation file. Includes all the definitions of member functions in private and public section.

#include "FCFS.h"
#include <iostream>
using namespace std;

FCFS::FCFS()
{
}

FCFS::FCFS(string type)
{
	if (type == "Recurring")
		cout << "Recurring FCFS";
	if (type == "NonRecurring")
		cout << "Non-recurring FCFS";
}


FCFS::~FCFS()
{
}

void FCFS::set_Sleeptime(const int &sleepTime)
{
	this->SleepTime = sleepTime;
}

int FCFS::get_SleepTime()
{
	return this->SleepTime;
}