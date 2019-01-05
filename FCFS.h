// File Name: FCFS.h
// Author: Shashank Bhagat
// Date: 11/13/2017
// Compiler Used: VC++
// Notes: This is a derived class header file. Includes all the declaration of the variables and member functions in private and public section.
#pragma once
#ifndef FCFS
#include "RecurringPCB.h"
#include "NonRecurringPCB.h"
#include <list>
using namespace std;

class FCFS :
	public RecurringPCB,public NonRecurringPCB
{
private:
	void Upgrade_to_SJF();
	void find_FCFSjob();

public:
	list<FCFS> FCFSProcess;
	FCFS();
	FCFS(string);
	~FCFS();
	int StarvationTime;
	void set_Sleeptime(const int&);
	int get_SleepTime();

protected:

};
#endif // !FCFS