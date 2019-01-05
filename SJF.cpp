// File Name: SJF.cpp
// Author: Shashank Bhagat
// Date: 12/01/2017
// Compiler Used: VC++
// Notes: This is a implementation file. Includes all the definitions of member functions in private and public section.

#include "SJF.h"
#include<iostream>
using namespace std;


SJF::SJF()
{
}

SJF::SJF(string type)
{
	if (type == "Recurring")
		cout << "Recurring SJF";
	if (type == "NonRecurring")
		cout << "Non-recurring SJF";
}

SJF::~SJF()
{
}

void SJF::set_Sleeptime(const int &sleepTime)
{
	this->SleepTime = sleepTime;
}

int SJF::get_SleepTime()
{
	return this->SleepTime;
}