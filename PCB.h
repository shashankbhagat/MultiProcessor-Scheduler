// File Name: PCB.h
// Author: Shashank Bhagat
// Date: 11/13/2017
// Compiler Used: VC++
// Notes: This is a Base class header file. Includes all the declaration of the variables and member functions in private and public section.
#pragma once
#ifndef PCB
class PCB
{
private:

public:
	int ID, recurring, nonRecurring, Process_Clock, CPU;
	virtual void test() {};		//to make this class polymorphic
	//virtual void set_Wait(int&) = 0;
	//virtual void set_TAT(int&) = 0; //Turn Around Time
	//virtual void set_Completion(int&) = 0;
	
	PCB();
	~PCB();

protected:
	
};
#endif // !PCB
