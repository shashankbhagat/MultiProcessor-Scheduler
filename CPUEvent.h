// File Name: CPUevent.h
// Author: Shashank Bhagat
// Date: 12/14/2017
// Compiler Used: VC++
// Notes: This is a derived class header file. Includes all the declaration of the variables and member functions in private and public section.

#pragma once
#ifndef CPUEVENT
#include<vector>
#include"Scheduler.h"
using namespace std;
class CPUEvent
{
public:
	CPUEvent();
	~CPUEvent();
	CPUEvent(const int&);
	char *CPU;
	void run(Scheduler &);
	void processEvent();
	int minTime;
	void findMinTime(Scheduler &);
	vector<int> MinBurstTime;

private:
	int no_of_CPUs;
	bool start;
};
#endif // !CPUEVENT