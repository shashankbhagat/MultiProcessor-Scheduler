// File Name: CPUEvent.cpp
// Author: Shashank Bhagat
// Date: 12/01/2017
// Compiler Used: VC++
// Notes: This is a implementation file. Includes all the definitions of member functions in private and public section.

#include "CPUEvent.h"
#include<algorithm>
#include<iostream>
#include<fstream>
using namespace std;

CPUEvent::CPUEvent()
{
}


CPUEvent::~CPUEvent()
{
}

CPUEvent::CPUEvent(const int& no_of_CPU)
{
	CPU = new char[no_of_CPU];
	minTime = 9999999;
	no_of_CPUs = no_of_CPU;
	start = true;
}

void CPUEvent::run(Scheduler &sch)
{
	ofstream outTofile;
	outTofile.open("output.txt", ios::app);
	findMinTime(sch);
	if (minTime > 0 && start==false)	//CPU is not available
	{
		cout << "CPU busy";
		outTofile<< "CPU busy";
		sch.initiateScheduler();
	}		
	else
		sch.DequeueJobToCPU();
	start = false;
}

void CPUEvent::findMinTime(Scheduler &sch)
{
	MinBurstTime = sch.sendCPUBurst();
	if (MinBurstTime.size() > 0)
		minTime = *min_element(MinBurstTime.begin(), MinBurstTime.end());
	else
		minTime = 0;
	
	CPU = sch.CPU;	//status of CPU
	sch.AvailableCPUs.clear();
	sch.BusyCPUs.clear();
	for (int i = 0; i < no_of_CPUs; i++)
	{
		if (CPU[i] == 'O')
			sch.AvailableCPUs.push_back(i);
		else if(CPU[i] == 'X')
			sch.BusyCPUs.push_back(i);
	}
}