// File Name: RecurringPCB.cpp
// Author: Shashank Bhagat
// Date: 12/01/2017
// Compiler Used: VC++
// Notes: This is a implementation file. Includes all the definitions of member functions in private and public section.

#include "RecurringPCB.h"
#include<time.h>
#include<string>
#include<thread>
#include<chrono>
using namespace std;

RecurringPCB::RecurringPCB()
{
}


RecurringPCB::~RecurringPCB()
{
}

void RecurringPCB::CreatePCB(int& burstTime1, int& burstTime2,int& ID)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	int tempVal;
	srand(time(0));
	tempVal = (rand() % (burstTime2 - burstTime1)) + burstTime1; //Burst Time
	this->BurstTimeR.push_back(tempVal);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(NULL));
	tempVal = (rand() % 50) + 1;	//Arrival Time
	this->ArrivalTimeR.push_back(tempVal);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(0));
	this->ID = (rand() % 100) + 1;	//Process ID	
	this->SleepTime = 0;	//Sleep Time
	ID = this->ID;
}

void RecurringPCB::CreatePCBRecurring(int& burstTime1, int& burstTime2, int& ID, int& CPU_time)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	int tempVal;
	srand(time(0));
	tempVal = (rand() % (burstTime2 - burstTime1)) + burstTime1; //Burst Time
	this->BurstTimeR.push_back(tempVal);	
	this->ArrivalTimeR.push_back(CPU_time);
	this->ID = ID;	//ID
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(NULL));
	this->SleepTime = (rand() % 10) + 1;	//Sleep Time
}

void RecurringPCB::set_CompletionR(const int& time)
{
	this->CompletionTimeR.push_back(time);
}

void RecurringPCB::set_TATR(const int& time)
{
	this->TurnAroundTimeR.push_back(time);
}

void RecurringPCB::set_WaitR(const int& time)
{
	this->WaitingTimeR.push_back(time);
}
