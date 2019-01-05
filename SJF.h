// File Name: SJF.h
// Author: Shashank Bhagat
// Date: 11/13/2017
// Compiler Used: VC++
// Notes: This is a derived class header file. Includes all the declaration of the variables and member functions in private and public section.
#pragma once
#ifndef SJF
#include "RecurringPCB.h"
#include "NonRecurringPCB.h"
#include<list>
using namespace std;

class SJF :
	public RecurringPCB,public NonRecurringPCB
{
private:
	void Upgrade_to_Priority();
	void SortProcessQueue(list<SJF>);

public:
	list<SJF> SJFProcess;	
	SJF();
	SJF(string);
	~SJF();
	void find_Sjob();
	int StarvationTime;
	void set_Sleeptime(const int&);
	int get_SleepTime();

protected:
};
#endif // !SJF