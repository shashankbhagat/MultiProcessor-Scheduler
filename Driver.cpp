#include "Scheduler.h"
#include<iostream>
#include "PCB.h"
#include "Priority.h"
#include "SJF.h"
#include "FCFS.h"
#include<string>
#include"CPUEvent.h"
#include<fstream>
using namespace std;
int main()
{
	ofstream outTofile;
	outTofile.open("output.txt",ios::app);
	string infile;
	int no_of_CPUs, CPU_clock=0;
	cout << "Number of CPU's: ";
	cin >> no_of_CPUs;
	outTofile << "Number of CPU's: " << no_of_CPUs;
	Scheduler sch(no_of_CPUs);
	CPUEvent eventObj(no_of_CPUs);

	infile = "input.txt";
	sch.gen_init_pool(infile);
	sch.PCB_Generator();

	do
	{
		cout << endl << "CPU Clock: " << CPU_clock << endl;
		outTofile << endl << "CPU Clock: " << CPU_clock << endl;
		eventObj.run(sch);
		//display every clock events occurred
		CPU_clock++;
	} while (CPU_clock < 60);

	sch.display();	//final statistics.

	//sch.initiateScheduler();

	system("PAUSE");
	return 0;
}