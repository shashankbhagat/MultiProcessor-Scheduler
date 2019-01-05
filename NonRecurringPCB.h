// File Name: NonRecurringPCB.h
// Author: Shashank Bhagat
// Date: 11/13/2017
// Compiler Used: VC++
// Notes: This is a derived class header file. Includes all the declaration of the variables and member functions in private and public section.
#pragma once
#ifndef NONRECURRINGPCB
#include "PCB.h"
class NonRecurringPCB :
	virtual public PCB
{
private:

public:
	int ArrivalTime;
	int BurstTime;
	int CompletionTime;
	int TurnAroundTime;
	int WaitingTime;
	NonRecurringPCB();
	~NonRecurringPCB();
	void CreatePCB(int&,int&,int&);
	void set_CompletionNR(const int&);
	void set_WaitNR(const int&);
	void set_TATNR(const int&);
};
#endif // !NONRECURRINGPCB