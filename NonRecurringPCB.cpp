// File Name: NonRecurringPCB.cpp
// Author: Shashank Bhagat
// Date: 12/01/2017
// Compiler Used: VC++
// Notes: This is a implementation file. Includes all the definitions of member functions in private and public section.

#include "NonRecurringPCB.h"
#include<time.h>
#include<string>
#include<thread>
#include<chrono>
using namespace std;

NonRecurringPCB::NonRecurringPCB()
{
}


NonRecurringPCB::~NonRecurringPCB()
{
}

void NonRecurringPCB::CreatePCB(int& burstTime1, int& burstTime2, int& ID)
{	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(0));	
	this->BurstTime = (rand() % (burstTime2 - burstTime1)) + burstTime1; //Burst Time	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(NULL));
	this->ArrivalTime = (rand() % 50) + 1;	//Arrival Time	
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(0));
	this->ID = (rand() % 100) + 1;	//Process ID
	ID = this->ID;
}

void NonRecurringPCB::set_CompletionNR(const int& time)
{
	this->CompletionTime = time;
}

void NonRecurringPCB::set_TATNR(const int& time)
{
	this->TurnAroundTime = time;
}

void NonRecurringPCB::set_WaitNR(const int& time)
{
	this->WaitingTime = time;
}