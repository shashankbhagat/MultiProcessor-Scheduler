// File Name: Priority.cpp
// Author: Shashank Bhagat
// Date: 12/01/2017
// Compiler Used: VC++
// Notes: This is a implementation file. Includes all the definitions of member functions in private and public section.

#include "Priority.h"
#include <iostream>
using namespace std;

Priority::Priority()
{
	
}

Priority::Priority(string type)
{
	if (type == "Recurring")
		cout << "Recurring Priority";
	if (type == "NonRecurring")
		cout << "Non-recurring Priority";
}

void Priority::set_Sleeptime(const int& sleep)
{
	this->SleepTime = sleep;
}

int Priority::get_SleepTime()
{
	return this->SleepTime;
}

Priority::~Priority()
{
}
