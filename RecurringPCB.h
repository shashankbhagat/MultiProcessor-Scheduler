// File Name: RecurringPCB.h
// Author: Shashank Bhagat
// Date: 11/13/2017
// Compiler Used: VC++
// Notes: This is a derived class header file. Includes all the declaration of the variables and member functions in private and public section.
#pragma once
#ifndef RECURRINGPCB
#include "PCB.h"
#include<vector>
using namespace std;
class RecurringPCB :
	virtual public PCB
{
private:
	int currentArrivalTime, currentBurstTime, currentCompletionTime, currentTurnAroundTime, currentWaitingTime;

public:
	vector<int> ArrivalTimeR;
	vector<int> BurstTimeR;
	vector<int> CompletionTimeR;
	vector<int> TurnAroundTimeR;
	vector<int> WaitingTimeR;	
	RecurringPCB();
	~RecurringPCB();

	void CreatePCB(int&,int&,int&);
	void CreatePCBRecurring(int&, int&, int&, int&);	//this function is for PCB's after sleep time
	int SleepTime_Counter;	
	void set_CompletionR(const int&);
	void set_WaitR(const int&);
	void set_TATR(const int&);
	bool processed;
protected:
	int SleepTime;
};
#endif // !RECURRINGPCB