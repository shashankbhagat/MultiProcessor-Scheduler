// File Name: Scheduler.cpp
// Author: Shashank Bhagat
// Date: 12/01/2017
// Compiler Used: VC++
// Notes: This is a implementation file. Includes all the definitions of member functions in private and public section.

#include "Scheduler.h"
#include "Priority.h"
#include<iostream>
#include<fstream>
#include<string>
#include<time.h>
#include<algorithm>
#include<thread>
#include<chrono>
using namespace std;

Scheduler::Scheduler()
{
	CPU_Clock = 0;
	CPU_ProcessCntr = 0;
	IsPriorityJob = false;
	IsSJFjob = false;
	IsFCFSJob = false;
	ProcessCntr[0]=0;
	global_StavationTime = 5;
}

Scheduler::Scheduler(const int& no_of_CPU)
{
	CPU = new char[no_of_CPU];
	CPU_count = no_of_CPU;
	ProcessCntr = new int[no_of_CPU]();
	for (int i = 0; i < no_of_CPU; i++)
		ProcessCntr[i] = 0;
	ProcessVerifyCPU = new string* [no_of_CPU];
	for (int i = 0; i < no_of_CPU; i++)
		ProcessVerifyCPU[i] = new string[2];
	for (int i = 0; i < no_of_CPU; i++)
		CPU[i] = 'O';
	for (int i = 0; i < no_of_CPU; i++)
		tempCPUBursts.push_back(0);
	CPUUtilisation = new int[no_of_CPU];
	for (int i = 0; i < no_of_CPU; i++)
		CPUUtilisation[i] = 0;
}

Scheduler::~Scheduler()
{
}

void Scheduler::set_arrival_prob(int probability)
{
	prob_percentage = probability;
}

void Scheduler::gen_init_pool(string infile)
{
	try
	{
		ifstream input(infile);
		string line;
		while (!input.eof())
		{
			getline(input, line);
			filedata.push_back(line);
		}
		input.close();
	}
	catch (exception)
	{
		throw;
	}	
}

void Scheduler::initiateScheduler()
{
	
	//increment process counter for processes in CPU.
	for(int j=0;j<BusyCPUs.size();j++)
	{
		int i = -1;
		i = BusyCPUs[j];		
		ProcessCntr[i]++;
	}

	while (BusyCPUs.size() > 0)
	{
		int i = -1;
		i = BusyCPUs[BusyCPUs.size() - 1];
		traverseRecurringQueuesSleepTime();	//check for any process done with sleep time
	
		ComputeProcess();	//compute the PCB and calculate statistics	-	done

		IncrementRecurringQueuesSleepTime(i);	//increment recurring queue sleep time	-	done

		//increment queue starvation time for SJF and FCFS (R and NR)
		incrementStarvationTime();

		Upgrade_SJF_to_Priority();
		Upgrade_FCFS_to_SJF();
		//method to upgrade SJF to Priority	-	done
		//method to upgrade FCFS to SJF	-	done
		
		BusyCPUs.pop_back();
		CPU_Clock++;
	}
}

void Scheduler::DequeueJobToCPU()
{
	while (AvailableCPUs.size() > 0)
	{
		if (!ArePriorityQueuesEmpty())
		{
			//create PCB(base class pointer) and assign it the derived class address i.e. create list of PCB objects 
			//done
			DequeueJob((string)"Priority");
			//CPU[0] = 'X';
			BusyCPUs.push_back(AvailableCPUs[AvailableCPUs.size() - 1]);
		}
		else if (!AreSJFQueuesEmpty())
		{
			DequeueJob((string)"SJF");
			//CPU[0] = 'X';
			BusyCPUs.push_back(AvailableCPUs[AvailableCPUs.size() - 1]);
		}
		else if (!AreFCFSQueuesEmpty())
		{
			DequeueJob((string)"FCFS");
			//CPU[0] = 'X';
			BusyCPUs.push_back(AvailableCPUs[AvailableCPUs.size() - 1]);
		}
		
		AvailableCPUs.pop_back();
	}
	sort(BusyCPUs.begin(), BusyCPUs.end());
	initiateScheduler();
}

void Scheduler::Upgrade_SJF_to_Priority()
{
	int JobTransferCntr = 0;
	list<SJF>::iterator it;
	sortJobQueueStarvationwise(NRSJFJobQueue);
	for (it = NRSJFJobQueue.begin(); it != NRSJFJobQueue.end(); it++)
	{
		if (it->StarvationTime >= global_StavationTime)
		{
			generateJobUpgrade_ToPriority("Non-Recurring");
			JobTransferCntr++;
			//NRSJFJobQueue.pop_front();
		}
	}

	if (NRSJFJobQueue.size() > 0)
	{
		for (int i = 0; i < JobTransferCntr; i++)
			NRSJFJobQueue.pop_front();
	}

	sortJobQueueStarvationwise(RSJFJobQueue);
	JobTransferCntr = 0;
	for (it = RSJFJobQueue.begin(); it != RSJFJobQueue.end(); it++)
	{
		if (it->StarvationTime >= global_StavationTime)
		{
			generateJobUpgrade_ToPriority("Recurring");
			JobTransferCntr++;
			//RSJFJobQueue.pop_front();
		}
	}
	if (RSJFJobQueue.size() > 0)
	{
		for(int i=0;i<JobTransferCntr;i++)
			RSJFJobQueue.pop_front();
	}
}

void Scheduler::Upgrade_FCFS_to_SJF()
{
	list<FCFS>::iterator it;
	sortJobQueueStarvationwise(NRFCFSJobQueue);
	int JobTransferCnt = 0;
	for (it = NRFCFSJobQueue.begin(); it != NRFCFSJobQueue.end(); it++)
	{
		if (it->StarvationTime >= global_StavationTime)
		{
			generateJobUpgrade_ToSJF("Non-Recurring");
			JobTransferCnt++;
			//NRFCFSJobQueue.pop_front();
		}
	}

	if (NRFCFSJobQueue.size() > 0)
	{
		for (int i = 0; i < JobTransferCnt; i++)
			NRFCFSJobQueue.pop_front();
	}

	sortJobQueueStarvationwise(RFCFSJobQueue);
	JobTransferCnt = 0;
	for (it = RFCFSJobQueue.begin(); it != RFCFSJobQueue.end(); it++)
	{
		if (it->StarvationTime >= global_StavationTime)
		{
			generateJobUpgrade_ToSJF("Recurring");
			JobTransferCnt++;
			//RFCFSJobQueue.pop_front();
		}
	}

	if (RFCFSJobQueue.size() > 0)
	{
		for (int i = 0; i < JobTransferCnt; i++)
			RFCFSJobQueue.pop_front();
	}
}

void Scheduler::generateJobUpgrade_ToPriority(string type)
{
	int ProcessCreationProb, RecursiveProb, NonRecursiveProb, BurstTime;
	int random_number, random_sleep;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(0));
	random_sleep = (rand() % 10) + 1;
	
	
		//PCB *pcb;
		Priority pr;
				
			if (type == "Recurring")
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				srand(time(0));
				list<SJF>::iterator it;
				it = RSJFJobQueue.begin();
				pr.ID = it->ID;
				pr.ArrivalTimeR.push_back(it->ArrivalTimeR[it->ArrivalTimeR.size() - 1]);
				pr.BurstTimeR.push_back(it->BurstTimeR[it->BurstTimeR.size() - 1]);
				pr.set_Sleeptime(0);
				pr.PriorityNumber = (rand() % 10) + 1;

				random_number = rand() % 100 + 1;
				//RecurringPCB *rPCB;	//created because createPCB is common to 
				//rPCB = &pr;
				//rPCB->CreatePCB(Priority_BurstRange1, Priority_BurstRange2, random_number);			
				
				/*static_cast<Priority*>(rPCB)->PriorityNumber = (rand() % 10) + 1;	//static cast for accessing derived class member using base class pointer*/
				//pr.set_Sleeptime(random_sleep);
				/*static_cast<Priority*>(rPCB)->set_Sleeptime((rand() % 10) + 1);	//static cast for accessing derived class member using base class pointer*/

				////pcb = rPCB;
				//
				//Priority* ptr_pr;
				//ptr_pr = dynamic_cast<Priority*>(rPCB);
				//ptr_pr->recurring = 1;
				//ptr_pr->nonRecurring = 0;

				pr.recurring = 1;
				pr.nonRecurring = 0;
				PriorityQueue.push_back(pr);	//Process enqued
				RpriorityJobQueue.push_back(pr);	//Process enqued				
			}
			else
			{
				list<SJF>::iterator it;
				it = NRSJFJobQueue.begin();
				pr.ID = it->ID;
				pr.ArrivalTime = it->ArrivalTime;
				pr.BurstTime = it->BurstTime;
				pr.set_Sleeptime(0);

				/*random_number = rand() % 100 + 1;
				NonRecurringPCB *nrPCB;
				nrPCB = &pr;
				nrPCB->CreatePCB(Priority_BurstRange1, Priority_BurstRange2, random_number);
				pr.PriorityNumber = (rand() % 10) + 1;
				pr.set_Sleeptime(0);*/
				//static_cast<Priority*>(nrPCB)->PriorityNumber = (rand() % 10) + 1;	//static cast for accessing derived class member using base class pointer
				//static_cast<Priority*>(nrPCB)->set_Sleeptime(-9999);	//static cast for accessing derived class member using base class pointer

				//pcb = nrPCB;
				
				/*Priority* ptr_pr;
				ptr_pr = dynamic_cast<Priority*>(nrPCB);
				ptr_pr->recurring = 0;
				ptr_pr->nonRecurring = 1;*/

				pr.recurring = 0;
				pr.nonRecurring = 1;
				PriorityQueue.push_back(pr);	//Process enqued
				NRpriorityJobQueue.push_back(pr);	//Process enqued
			}
		
			ofstream outTofile;
			outTofile.open("output.txt", ios::app);

	//display the generated PCB's
	if (RpriorityJobQueue.size() > 0)
	{
		list<Priority>::iterator it_Priority;
		cout << endl << "Recurring Priority Jobs post upgrade" << endl;
		outTofile << endl << "Recurring Priority Jobs post upgrade" << endl;
		for (it_Priority = RpriorityJobQueue.begin(); it_Priority != RpriorityJobQueue.end(); it_Priority++)
		{
			cout << "ID: " << it_Priority->ID;
			outTofile << "ID: " << it_Priority->ID;
			cout << endl << "Priority Number: " << it_Priority->PriorityNumber;
			outTofile << endl << "Priority Number: " << it_Priority->PriorityNumber;
			cout << endl << "Arrival Time/s: ";
			outTofile << endl << "Arrival Time/s: ";
			for (int i = 0; i < it_Priority->ArrivalTimeR.size(); i++)
			{
				cout << it_Priority->ArrivalTimeR[i] << " \t";
				outTofile << it_Priority->ArrivalTimeR[i] << " \t";
			}
				

			cout << endl << "Burst Time/s: ";
			outTofile << endl << "Burst Time/s: ";
			for (int i = 0; i < it_Priority->BurstTimeR.size(); i++)
			{
				cout << it_Priority->BurstTimeR[i] << " \t";
				outTofile<< it_Priority->BurstTimeR[i] << " \t";
			}
				
			cout << endl << "Completion Time/s";
			outTofile<< endl << "Completion Time/s";
			for (int i = 0; i < it_Priority->CompletionTimeR.size(); i++)
			{
				cout << it_Priority->CompletionTimeR[i] << " \t";
				outTofile<< it_Priority->CompletionTimeR[i] << " \t";
			}
				
			cout << endl << "Waiting Time/s";
			outTofile<< endl << "Waiting Time/s";
			for (int i = 0; i < it_Priority->WaitingTimeR.size(); i++)
			{
				cout << it_Priority->WaitingTimeR[i] << " \t";
				outTofile<< it_Priority->WaitingTimeR[i] << " \t";
			}
				
			cout << endl << "Turn Around Time/s";
			outTofile<< endl << "Turn Around Time/s";
			for (int i = 0; i < it_Priority->TurnAroundTimeR.size(); i++)
			{
				cout << it_Priority->TurnAroundTimeR[i] << " \t";
				outTofile<< it_Priority->TurnAroundTimeR[i] << " \t";
			}				
		}

	}

	if (NRpriorityJobQueue.size() > 0)
	{
		list<Priority>::iterator it;
		cout << endl << "Non-Recurring Priority Jobs post upgrade" << endl;
		outTofile<< endl << "Non-Recurring Priority Jobs post upgrade" << endl;
		cout << "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring" << endl;
		outTofile<< "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring" << endl;
		for (it = NRpriorityJobQueue.begin(); it != NRpriorityJobQueue.end(); it++)
		{
			cout << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->PriorityNumber << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << endl;
			outTofile<< it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->PriorityNumber << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << endl;
		}
	}
}

void Scheduler::generateJobUpgrade_ToSJF(string type)
{
	ofstream outTofile;
	outTofile.open("output.txt", ios::app);
	int ProcessCreationProb, RecursiveProb, NonRecursiveProb, BurstTime;
	int random_number, random_sleep;

	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(0));
	random_sleep = (rand() % 10) + 1;

	//PCB *pcb;
	SJF sjf;
	if (type == "Recurring")
	{
		list<FCFS>::iterator it;
		it = RFCFSJobQueue.begin();
		sjf.ID = it->ID;
		sjf.ArrivalTimeR.push_back(it->ArrivalTimeR[it->ArrivalTimeR.size() - 1]);
		sjf.BurstTimeR.push_back(it->BurstTimeR[it->BurstTimeR.size() - 1]);
		sjf.set_Sleeptime(0);
		sjf.StarvationTime = 0;

			//random_number = rand() % 100 + 1;
			//RecurringPCB *rPCB;
			//rPCB = &sjf;
			//rPCB->CreatePCB(SJF_BurstRange1, SJF_BurstRange2, random_number);
			////pcb = rPCB;
			////pcb = &sjf;
			////pcb->nonRecurring = 0;
			////pcb->recurring = 1;
			//SJF* ptr_sjf;
			//ptr_sjf = dynamic_cast<SJF*>(rPCB);
			//ptr_sjf->recurring = 1;
			//ptr_sjf->nonRecurring = 0;

			sjf.recurring = 1;
			sjf.nonRecurring = 0;
			SJFQueue.push_back(sjf);	//Process enqued		

			RSJFJobQueue.push_back(sjf);	//Process enqued
	}
	else
	{
		list<FCFS>::iterator it;
		it = NRFCFSJobQueue.begin();
		sjf.ID = it->ID;
		sjf.ArrivalTime = it->ArrivalTime;
		sjf.BurstTime = it->BurstTime;
		sjf.set_Sleeptime(0);
		sjf.StarvationTime = 0;

		//random_number = rand() % 100 + 1;
		//NonRecurringPCB *nrPCB;
		//nrPCB = &sjf;
		//nrPCB->CreatePCB(SJF_BurstRange1, SJF_BurstRange2, random_number);
		////pcb = nrPCB;
		////pcb = &sjf;
		////pcb->nonRecurring = 1;
		////pcb->recurring = 0;
		//SJF* ptr_sjf;
		//ptr_sjf = dynamic_cast<SJF*>(nrPCB);
		//ptr_sjf->recurring = 0;
		//ptr_sjf->nonRecurring = 1;
		//sjf.set_Sleeptime(0);

		sjf.recurring = 0;
		sjf.nonRecurring = 1;
		SJFQueue.push_back(sjf);	//Process enqued
		NRSJFJobQueue.push_back(sjf);	//Process enqued
	}
	//display the generated PCB's
	if (RSJFJobQueue.size() > 0)
	{
		list<SJF>::iterator it_SJF;
		cout << endl << "Recurring SJF Jobs post upgrade" << endl;
		outTofile<< endl << "Recurring SJF Jobs post upgrade" << endl;

		for (it_SJF = RSJFJobQueue.begin(); it_SJF != RSJFJobQueue.end(); it_SJF++)
		{
			cout << "ID: " << it_SJF->ID;
			outTofile<< "ID: " << it_SJF->ID;
			cout << endl << "Arrival Time/s: ";
			outTofile<< endl << "Arrival Time/s: ";
			for (int i = 0; i < it_SJF->ArrivalTimeR.size(); i++)
			{
				cout << it_SJF->ArrivalTimeR[i] << " \t";
				outTofile<< it_SJF->ArrivalTimeR[i] << " \t";
			}
				
			cout << endl << "Burst Time/s: ";
			outTofile<< endl << "Burst Time/s: ";
			for (int i = 0; i < it_SJF->BurstTimeR.size(); i++)
			{
				cout << it_SJF->BurstTimeR[i] << " \t";
				outTofile<< it_SJF->BurstTimeR[i] << " \t";
			}
				
			cout << endl << "Completion Time/s";
			outTofile<< endl << "Completion Time/s";
			for (int i = 0; i < it_SJF->CompletionTimeR.size(); i++)
			{
				cout << it_SJF->CompletionTimeR[i] << " \t";
				outTofile<< it_SJF->CompletionTimeR[i] << " \t";
			}
				
			cout << endl << "Waiting Time/s";
			outTofile<< endl << "Waiting Time/s";
			for (int i = 0; i < it_SJF->WaitingTimeR.size(); i++)
			{
				cout << it_SJF->WaitingTimeR[i] << " \t";
				outTofile<< it_SJF->WaitingTimeR[i] << " \t";
			}
				
			cout << endl << "Turn Around Time/s";
			outTofile << endl << "Turn Around Time/s";
			for (int i = 0; i < it_SJF->TurnAroundTimeR.size(); i++)
			{
				cout << it_SJF->TurnAroundTimeR[i] << " \t";
				outTofile<< it_SJF->TurnAroundTimeR[i] << " \t";
			}				
		}
	}

	if (NRSJFJobQueue.size() > 0)
	{
		list<SJF>::iterator it;
		cout << "Non-Recurring SJF Jobs post upgrade" << endl;
		cout << "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring" << endl;
		outTofile<< "Non-Recurring SJF Jobs post upgrade" << endl;
		outTofile<< "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring" << endl;
		for (it = NRSJFJobQueue.begin(); it != NRSJFJobQueue.end(); it++)
		{
			cout << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << endl;
			outTofile << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << endl;
		}
	}
}

void Scheduler::ComputeProcess()
{
	ofstream outTofile;
	outTofile.open("output.txt", ios::app);

	for (int j = 0; j<BusyCPUs.size(); j++)
	{		
		int i = BusyCPUs[j];
		CPUUtilisation[i] = CPUUtilisation[i] + 1;
	}
	//find which CPU has which process and accordingly do the comparison - for muultiprocessor.
	cout << endl << "Computing...." << endl;
	outTofile << endl << "Computing...." << endl;
	int i = -1;
	if (BusyCPUs.size() > 0)
	{
		i = BusyCPUs[BusyCPUs.size()-1];
		//AvailableCPUs.pop_back();		
	}
	if (i == -1)
	{
		cout << endl << "Problem in Computation!!!" << endl;
		outTofile<< endl << "Problem in Computation!!!" << endl;
		return;
	}
	cout << "CPU:" << i << " active" << endl;
	outTofile<< "CPU:" << i << " active" << endl;
	if (ProcessVerifyCPU[i][0] == "NRPriority" && NRpriorityJobQueue.size()>0)
	{
		list<Priority>::iterator it;
		it = NRpriorityJobQueue.begin();

		tempCPUBursts[i] = it->BurstTime - ProcessCntr[i];			//use i instead of 0 i.e. CPU used

		if (it->BurstTime == ProcessCntr[i])
		{
			CPU[i] = 'O';
			ProcessCntr[i] = -1;
			it->set_CompletionNR(CPU_Clock + it->ArrivalTime+1);
			it->set_TATNR(it->CompletionTime - it->ArrivalTime + 1);
			it->set_WaitNR(it->TurnAroundTime - it->BurstTime);
			NonRecurringPriorityBackup.push_back(*it);	//this list to be used in >> operator for outputting details.

			//display information 
			if (NRpriorityJobQueue.size() > 0)
			{
				list<Priority>::iterator it;
				it = NRpriorityJobQueue.begin();
				cout << "Non-Recurring Priority Jobs" << endl << "CPU:" << it->CPU << endl;
				cout << "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
				outTofile<< "Non-Recurring Priority Jobs" << endl << "CPU:" << it->CPU << endl;
				outTofile<< "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
				for (it = NRpriorityJobQueue.begin(); it != NRpriorityJobQueue.end(); it++)
				{
					cout << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->PriorityNumber << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->CompletionTime << "\t" << it->WaitingTime << "\t" << it->TurnAroundTime << endl;
				}
			}

			if (!NRpriorityJobQueue.empty())
				NRpriorityJobQueue.pop_front();

		}		
	}
	else if (ProcessVerifyCPU[i][0] == "NRSJF" && NRSJFJobQueue.size()>0)
	{
		list<SJF>::iterator it;
		it = NRSJFJobQueue.begin();

		tempCPUBursts[i] = it->BurstTime - ProcessCntr[i];			//use i instead of 0 i.e. CPU used

		if (it->BurstTime == ProcessCntr[i])
		{
			CPU[i] = 'O';
			ProcessCntr[i] = -1;
			it->set_CompletionNR(CPU_Clock + it->ArrivalTime+1);
			it->set_TATNR(it->CompletionTime - it->ArrivalTime + 1);
			it->set_WaitNR(it->TurnAroundTime - it->BurstTime);
			NonRecurringSJFBackup.push_back(*it);

			if (NRSJFJobQueue.size() > 0)
			{
				list<SJF>::iterator it;
				it = NRSJFJobQueue.begin();
				cout << "Non-Recurring SJF Jobs" << endl << "CPU:" << it->CPU << endl;
				cout << "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
				outTofile << "Non-Recurring SJF Jobs" << endl << "CPU:" << it->CPU << endl;
				outTofile<< "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
				for (it = NRSJFJobQueue.begin(); it != NRSJFJobQueue.end(); it++)
				{
					cout << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->CompletionTime << "\t" << it->WaitingTime << "\t" << it->TurnAroundTime << endl;
					outTofile<< it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->CompletionTime << "\t" << it->WaitingTime << "\t" << it->TurnAroundTime << endl;
				}
			}

			if (!NRSJFJobQueue.empty())
				NRSJFJobQueue.pop_front();
		}
	}
	else if (ProcessVerifyCPU[i][0] == "NRFCFS" && NRFCFSJobQueue.size()>0)
	{
		list<FCFS>::iterator it;
		it = NRFCFSJobQueue.begin();

		tempCPUBursts[i] = it->BurstTime - ProcessCntr[i];			//use i instead of 0 i.e. CPU used

		if (it->BurstTime == ProcessCntr[i])
		{
			CPU[i] = 'O';
			ProcessCntr[i] = -1;
			it->set_CompletionNR(CPU_Clock + it->ArrivalTime+1);
			it->set_TATNR(it->CompletionTime - it->ArrivalTime + 1);
			it->set_WaitNR(it->TurnAroundTime - it->BurstTime);
			NonRecurringFCFSBackup.push_back(*it);

			if (NRFCFSJobQueue.size() > 0)
			{
				list<FCFS>::iterator it;
				it = NRFCFSJobQueue.begin();
				cout << "Non-Recurring FCFS Jobs" << endl << "CPU:" << it->CPU << endl;
				cout << "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
				outTofile << "Non-Recurring FCFS Jobs" << endl << "CPU:" << it->CPU << endl;
				outTofile<< "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
				for (it = NRFCFSJobQueue.begin(); it != NRFCFSJobQueue.end(); it++)
				{
					cout << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->CompletionTime << "\t" << it->WaitingTime << "\t" << it->TurnAroundTime << endl;
					outTofile << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->CompletionTime << "\t" << it->WaitingTime << "\t" << it->TurnAroundTime << endl;
				}
			}

			if (!NRFCFSJobQueue.empty())
				NRFCFSJobQueue.pop_front();
		}
	}
	else if (ProcessVerifyCPU[i][0] == "RPriority" && RpriorityJobQueue.size()>0)
	{
		list<Priority>::iterator it;
		it = RpriorityJobQueue.begin();

		tempCPUBursts[i] = it->BurstTimeR[it->BurstTimeR.size() - 1] - ProcessCntr[i];			//use i instead of 0 i.e. CPU used

		if (it->BurstTimeR[it->BurstTimeR.size()-1] == ProcessCntr[i])
		{
			CPU[i] = 'O';
			ProcessCntr[i] = -1;
			it->set_CompletionR(CPU_Clock + it->ArrivalTimeR[it->ArrivalTimeR.size() - 1] + 1);
			it->set_TATR(it->CompletionTimeR[it->CompletionTimeR.size() - 1] - it->ArrivalTimeR[it->ArrivalTimeR.size() - 1] + 1);
			it->set_WaitR(it->TurnAroundTimeR[it->TurnAroundTimeR.size() - 1] - it->BurstTimeR[it->BurstTimeR.size() - 1]);
			RecurringPriorityBackup.push_back(*it);	//this list to be used in << operator for outputting details.

			//display information 
			if (RpriorityJobQueue.size() > 0)
			{
				list<Priority>::iterator it_Priority;
				it_Priority = RpriorityJobQueue.begin();
				cout << endl << "Recurring Priority Jobs" << endl << "CPU:" << it_Priority->CPU << endl;
				outTofile << endl << "Recurring Priority Jobs" << endl << "CPU:" << it_Priority->CPU << endl;
	
				for (it_Priority = RpriorityJobQueue.begin(); it_Priority != RpriorityJobQueue.end(); it_Priority++)
				{
					cout << endl << "ID: " << it_Priority->ID;
					cout << endl << "Priority Number: " << it_Priority->PriorityNumber;
					cout << endl << "Arrival Time/s: ";
					outTofile<< endl << "ID: " << it_Priority->ID;
					outTofile<< endl << "Priority Number: " << it_Priority->PriorityNumber;
					outTofile<< endl << "Arrival Time/s: ";
					for (int i = 0; i < it_Priority->ArrivalTimeR.size(); i++)
					{
						cout << it_Priority->ArrivalTimeR[i] << " \t";
						outTofile<< it_Priority->ArrivalTimeR[i] << " \t";
					}
						
					cout << endl << "Burst Time/s: ";
					outTofile<< endl << "Burst Time/s: ";
					for (int i = 0; i < it_Priority->BurstTimeR.size(); i++)
					{
						cout << it_Priority->BurstTimeR[i] << " \t";
						outTofile<< it_Priority->BurstTimeR[i] << " \t";
					}
						
					cout << endl << "Completion Time/s: ";
					outTofile<< endl << "Completion Time/s: ";
					for (int i = 0; i < it_Priority->CompletionTimeR.size(); i++)
					{
						cout << it_Priority->CompletionTimeR[i] << " \t";
						outTofile<< it_Priority->CompletionTimeR[i] << " \t";
					}
						
					cout << endl << "Waiting Time/s: ";
					outTofile<< endl << "Waiting Time/s: ";
					for (int i = 0; i < it_Priority->WaitingTimeR.size(); i++)
					{
						cout << it_Priority->WaitingTimeR[i] << " \t";
						outTofile<< it_Priority->WaitingTimeR[i] << " \t";
					}
						
					cout << endl << "Turn Around Time/s: ";
					outTofile<< endl << "Turn Around Time/s: ";
					for (int i = 0; i < it_Priority->TurnAroundTimeR.size(); i++)
					{
						cout << it_Priority->TurnAroundTimeR[i] << " \t";
						outTofile<< it_Priority->TurnAroundTimeR[i] << " \t";
					}						
				}
				cout << endl;
				outTofile << endl;
			}

			if (!RpriorityJobQueue.empty())
				RpriorityJobQueue.pop_front();
		}
	}
	else if (ProcessVerifyCPU[i][0] == "RSJF" && RSJFJobQueue.size()>0)
	{
		list<SJF>::iterator it;
		it = RSJFJobQueue.begin();

		tempCPUBursts[i] = it->BurstTimeR[it->BurstTimeR.size() - 1] - ProcessCntr[i];			//use i instead of 0 i.e. CPU used

		if (it->BurstTimeR[it->BurstTimeR.size() - 1] == ProcessCntr[i])
		{
			CPU[i] = 'O';
			ProcessCntr[i] = -1;
			it->set_CompletionR(CPU_Clock + it->ArrivalTimeR[it->ArrivalTimeR.size() - 1]+1);
			it->set_TATR(it->CompletionTimeR[it->CompletionTimeR.size() - 1] - it->ArrivalTimeR[it->ArrivalTimeR.size() - 1] + 1);
			it->set_WaitR(it->TurnAroundTimeR[it->TurnAroundTimeR.size() - 1] - it->BurstTimeR[it->BurstTimeR.size() - 1]);
			RecurringSJFBackup.push_back(*it);	//this list to be used in << operator for outputting details.

														//display information 
			if (RSJFJobQueue.size() > 0)
			{
				list<SJF>::iterator it_SJF;
				it_SJF = RSJFJobQueue.begin();
				cout << endl << "Recurring SJF Jobs" << endl << "CPU:" << it_SJF->CPU << endl;
				outTofile << endl << "Recurring SJF Jobs" << endl << "CPU:" << it_SJF->CPU << endl;

				for (it_SJF = RSJFJobQueue.begin(); it_SJF != RSJFJobQueue.end(); it_SJF++)
				{
					cout << endl << "ID: " << it_SJF->ID;
					cout << endl << "Arrival Time/s: ";
					outTofile<< endl << "ID: " << it_SJF->ID;
					outTofile<< endl << "Arrival Time/s: ";
					for (int i = 0; i < it_SJF->ArrivalTimeR.size(); i++)
					{
						cout << it_SJF->ArrivalTimeR[i] << " \t";
						outTofile<< it_SJF->ArrivalTimeR[i] << " \t";
					}
						
					cout << endl << "Burst Time/s: ";
					outTofile<< endl << "Burst Time/s: ";
					for (int i = 0; i < it_SJF->BurstTimeR.size(); i++)
					{
						cout << it_SJF->BurstTimeR[i] << " \t";
						outTofile<< it_SJF->BurstTimeR[i] << " \t";
					}
						
					cout << endl << "Completion Time/s: ";
					outTofile<< endl << "Completion Time/s: ";
					for (int i = 0; i < it_SJF->CompletionTimeR.size(); i++)
					{
						cout << it_SJF->CompletionTimeR[i] << " \t";
						outTofile<< it_SJF->CompletionTimeR[i] << " \t";
					}
						
					cout << endl << "Waiting Time/s: ";
					outTofile<< endl << "Waiting Time/s: ";
					for (int i = 0; i < it_SJF->WaitingTimeR.size(); i++)
					{
						cout << it_SJF->WaitingTimeR[i] << " \t";
						outTofile<< it_SJF->WaitingTimeR[i] << " \t";
					}
						
					cout << endl << "Turn Around Time/s: ";
					outTofile<< endl << "Turn Around Time/s: ";
					for (int i = 0; i < it_SJF->TurnAroundTimeR.size(); i++)
					{
						cout << it_SJF->TurnAroundTimeR[i] << " \t";
						outTofile<< it_SJF->TurnAroundTimeR[i] << " \t";
					}
						
				}
				cout << endl;
				outTofile<< endl;
			}

			if (!RSJFJobQueue.empty())
				RSJFJobQueue.pop_front();
		}
	}
	else if (ProcessVerifyCPU[i][0] == "RFCFS" && RFCFSJobQueue.size()>0)
	{
		list<FCFS>::iterator it;
		it = RFCFSJobQueue.begin();

		tempCPUBursts[i] = it->BurstTimeR[it->BurstTimeR.size() - 1] - ProcessCntr[i];			//use i instead of 0 i.e. CPU used

		if (it->BurstTimeR[it->BurstTimeR.size() - 1] == ProcessCntr[i])
		{
			CPU[i] = 'O';
			ProcessCntr[i] = -1;
			it->set_CompletionR(CPU_Clock + it->ArrivalTimeR[it->ArrivalTimeR.size() - 1] + 1);
			it->set_TATR(it->CompletionTimeR[it->CompletionTimeR.size() - 1] - it->ArrivalTimeR[it->ArrivalTimeR.size() - 1] + 1);
			it->set_WaitR(it->TurnAroundTimeR[it->TurnAroundTimeR.size() - 1] - it->BurstTimeR[it->BurstTimeR.size() - 1]);
			RecurringFCFSBackup.push_back(*it);	//this list to be used in << operator for outputting details.

														//display information 
			if (RFCFSJobQueue.size() > 0)
			{
				list<FCFS>::iterator it_FCFS;
				it_FCFS = RFCFSJobQueue.begin();
				cout << endl << "Recurring FCFS Jobs" << endl << "CPU:" << it_FCFS->CPU << endl;
				outTofile<< endl << "Recurring FCFS Jobs" << endl << "CPU:" << it_FCFS->CPU << endl;
				for (it_FCFS = RFCFSJobQueue.begin(); it_FCFS != RFCFSJobQueue.end(); it_FCFS++)
				{
					cout << endl << "ID: " << it_FCFS->ID;
					cout << endl << "Arrival Time/s: ";
					outTofile<< endl << "ID: " << it_FCFS->ID;
					outTofile<< endl << "Arrival Time/s: ";
					for (int i = 0; i < it_FCFS->ArrivalTimeR.size(); i++)
					{
						cout << it_FCFS->ArrivalTimeR[i] << " \t";
						outTofile<< it_FCFS->ArrivalTimeR[i] << " \t";
					}
						
					cout << endl << "Burst Time/s: ";
					outTofile<< endl << "Burst Time/s: ";
					for (int i = 0; i < it_FCFS->BurstTimeR.size(); i++)
					{
						cout << it_FCFS->BurstTimeR[i] << " \t";
						outTofile<< it_FCFS->BurstTimeR[i] << " \t";
					}
						
					cout << endl << "Completion Time/s:";
					outTofile<< endl << "Completion Time/s:";
					for (int i = 0; i < it_FCFS->CompletionTimeR.size(); i++)
					{
						cout << it_FCFS->CompletionTimeR[i] << " \t";
						outTofile<< it_FCFS->CompletionTimeR[i] << " \t";
					}
						
					cout << endl << "Waiting Time/s:";
					outTofile<< endl << "Waiting Time/s:";
					for (int i = 0; i < it_FCFS->WaitingTimeR.size(); i++)
					{
						cout << it_FCFS->WaitingTimeR[i] << " \t";
						outTofile<< it_FCFS->WaitingTimeR[i] << " \t";
					}
						
					cout << endl << "Turn Around Time/s:";
					outTofile<< endl << "Turn Around Time/s:";
					for (int i = 0; i < it_FCFS->TurnAroundTimeR.size(); i++)
					{
						cout << it_FCFS->TurnAroundTimeR[i] << " \t";
						outTofile<< it_FCFS->TurnAroundTimeR[i] << " \t";
					}						
				}
				cout << endl;
				outTofile<< endl;
			}

			if (!RFCFSJobQueue.empty())
				RFCFSJobQueue.pop_front();
		}
	}
}

void Scheduler::traverseRecurringQueuesSleepTime()
{
	//check for any process done with sleep time
	int JobRemove = 0;
	if (PrioritySleepqueue.size() > 0)
	{
		
		sortJobQueueSleepTimeWise(PrioritySleepqueue);
		list<Priority>::iterator it;
		for (it = PrioritySleepqueue.begin(); it != PrioritySleepqueue.end(); it++)
		{
			if (temp_Sleeptime == it->get_SleepTime() && it->processed==true)	//if sleep time is done then transfer to priority queue.
			{
				it->CreatePCBRecurring(Priority_BurstRange1, Priority_BurstRange2, it->ID, CPU_Clock);
				RpriorityJobQueue.push_back(*it);
				RecurringPriorityBackup.push_back(*it);
				//RecurringPriorityBackup.pop_front();
				JobRemove++;
				
				//PCB *pcb;
				Priority pr;
				//pcb = &pr;				
				RecurringPCB* rPCB;
				rPCB = &pr;
				Priority* ptr_pr;
				ptr_pr = dynamic_cast<Priority*>(rPCB);
				ptr_pr->recurring = 1;
				ptr_pr->nonRecurring = 0;

				pr.recurring = 1;
				pr.nonRecurring = 0;
				PriorityQueue.push_back(pr);
			}
		}
		for (int i = 0; i < JobRemove; i++)
			PrioritySleepqueue.pop_front();
	}

	JobRemove = 0;
	if (SJFSleepQueue.size() > 0)
	{		
		sortJobQueueSleepTimeWise(SJFSleepQueue);
		list<SJF>::iterator it;
		for (it = SJFSleepQueue.begin(); it != SJFSleepQueue.end(); it++)
		{
			if (it->SleepTime_Counter == it->get_SleepTime() &&  it->processed==true)	//if sleep time is done then transfer to SJF queue.
			{
				it->CreatePCBRecurring(SJF_BurstRange1, SJF_BurstRange2, it->ID, CPU_Clock);
				RSJFJobQueue.push_back(*it);
				RecurringSJFBackup.push_back(*it);
				//RecurringSJFBackup.pop_front();
				JobRemove++;

				//PCB *pcb;
				SJF sjf_obj;
				//pcb = &sjf_obj;
				RecurringPCB* rPCB;
				rPCB = &sjf_obj;
				SJF* ptr_sjf;
				ptr_sjf = dynamic_cast<SJF*>(rPCB);
				ptr_sjf->recurring = 1;
				ptr_sjf->nonRecurring = 0;
				
				sjf_obj.recurring = 1;
				sjf_obj.nonRecurring = 0;
				SJFQueue.push_back(sjf_obj);	//Process enqued
			}
		}

		for (int i = 0; i < JobRemove; i++)
			SJFSleepQueue.pop_front();
	}

	JobRemove = 0;
	if (FCFSSleepQueue.size() > 0)
	{		
		sortJobQueueSleepTimeWise(FCFSSleepQueue);
		list<FCFS>::iterator it;
		for (it = FCFSSleepQueue.begin(); it != FCFSSleepQueue.end(); it++)
		{
			if (it->SleepTime_Counter == it->get_SleepTime() && it->processed==true)	//if sleep time is done then transfer to FCFS queue.
			{
				it->CreatePCBRecurring(FCFS_BurstRange1, FCFS_BurstRange2, it->ID, CPU_Clock);
				RFCFSJobQueue.push_back(*it);
				RecurringFCFSBackup.push_back(*it);
				//RecurringFCFSBackup.pop_front();
				JobRemove++;

				PCB *pcb;
				FCFS fcfs_obj;
				pcb = &fcfs_obj;
				RecurringPCB* rPCB;
				rPCB = &fcfs_obj;
				FCFS* ptr_fcfs;
				ptr_fcfs = dynamic_cast<FCFS*>(rPCB);
				ptr_fcfs->recurring = 1;
				ptr_fcfs->nonRecurring = 0;
				
				fcfs_obj.recurring = 1;
				fcfs_obj.nonRecurring = 0;
				FCFSQueue.push_back(fcfs_obj);	//Process enqued
			}
		}
		for (int i = 0; i < JobRemove; i++)
			FCFSSleepQueue.pop_front();
	}
}

void Scheduler::sortJobQueueSleepTimeWise(list<Priority>& jobQueue)
{
	jobQueue.sort(compare_jobs_ST_Priority);
}

void Scheduler::sortJobQueueSleepTimeWise(list<SJF>& jobQueue)
{
	jobQueue.sort(compare_jobs_ST_SJF);
}

void Scheduler::sortJobQueueSleepTimeWise(list<FCFS>& jobQueue)
{
	jobQueue.sort(compare_jobs_ST_FCFS);
}

void Scheduler::sortJobQueueStarvationwise(list<SJF>& jobQueue)
{
	jobQueue.sort(compare_jobs_Starvation_SJF);
}

void Scheduler::sortJobQueueStarvationwise(list<FCFS>& jobQueue)
{
	jobQueue.sort(compare_jobs_Starvation_FCFS);
}

bool Scheduler::compare_jobs_Starvation_SJF(SJF &lhs, SJF &rhs)
{
	return lhs.StarvationTime < rhs.StarvationTime;
}

bool Scheduler::compare_jobs_Starvation_FCFS(FCFS &lhs, FCFS &rhs)
{
	return lhs.StarvationTime < rhs.StarvationTime;
}

bool Scheduler::compare_jobs_ST_Priority(Priority &lhs, Priority &rhs)
{
	return lhs.SleepTime_Counter<rhs.SleepTime_Counter;
}

bool Scheduler::compare_jobs_ST_SJF(SJF &lhs, SJF &rhs)
{
	return lhs.SleepTime_Counter<rhs.SleepTime_Counter;
}

bool Scheduler::compare_jobs_ST_FCFS(FCFS &lhs, FCFS &rhs)
{
	return lhs.SleepTime_Counter<rhs.SleepTime_Counter;
}

void Scheduler::IncrementRecurringQueuesSleepTime(const int &i)
{//paass CPU instace i.e. i
	//increment recurring queue sleep time. Sleep time would increment in backup queue only.
	if (PrioritySleepqueue.size()>0)
	{
		list<Priority>::iterator it;
		for (it = PrioritySleepqueue.begin(); it != PrioritySleepqueue.end(); it++)
		{
			if (it->BurstTimeR[it->BurstTimeR.size() - 1] == ProcessCntr[i])
				it->processed = true;
			if (it->processed == true)
				it->SleepTime_Counter++;
		}
	}
	if (SJFSleepQueue.size() > 0)
	{
		list<SJF>::iterator it;
		for (it = SJFSleepQueue.begin(); it != SJFSleepQueue.end(); it++)
		{
			if (it->BurstTimeR[it->BurstTimeR.size() - 1] == ProcessCntr[i])
				it->processed = true;
			if (it->processed == true)
				it->SleepTime_Counter++;
		}
	}
	if (FCFSSleepQueue.size() > 0)
	{
		list<FCFS>::iterator it;
		for (it = FCFSSleepQueue.begin(); it != FCFSSleepQueue.end(); it++)
		{
			if (it->BurstTimeR[it->BurstTimeR.size() - 1] == ProcessCntr[i])
				it->processed = true;
			if (it->processed == true)
				it->SleepTime_Counter++;
		}
	}
}

void Scheduler::incrementStarvationTime()
{
	if (NRSJFJobQueue.size() > 0)
	{
		list<SJF>::iterator it;
		for (it = NRSJFJobQueue.begin(); it != NRSJFJobQueue.end(); it++)
		{
			it->StarvationTime++;
		}
	}

	if (RSJFJobQueue.size() > 0)
	{
		list<SJF>::iterator it;
		for (it = RSJFJobQueue.begin(); it != RSJFJobQueue.end(); it++)
		{
			it->StarvationTime++;
		}
	}

	if (NRFCFSJobQueue.size() > 0)
	{
		list<FCFS>::iterator it;
		for (it = NRFCFSJobQueue.begin(); it != NRFCFSJobQueue.end(); it++)
		{
			it->StarvationTime++;
		}
	}

	if (RFCFSJobQueue.size() > 0)
	{
		list<FCFS>::iterator it;
		for (it = RFCFSJobQueue.begin(); it != RFCFSJobQueue.end(); it++)
		{
			it->StarvationTime++;
		}
	}
}

void Scheduler::DequeueJob(string& type)
{
	if (type == "Priority")
	{
		list<Priority>::iterator it;
		it = PriorityQueue.begin();
		
		if ((it)->recurring == 1)
		{
			DequeueRecurringJob(type);
		}
		else if ((it)->nonRecurring == 1)
		{
			DequeueNonRecurringJob(type);
		}		
		PriorityQueue.pop_front();
	}
	else if (type == "SJF")
	{
		list<SJF>::iterator it;
		it = SJFQueue.begin();

		if ((it)->recurring == 1)
		{
			DequeueRecurringJob(type);
		}
		else if ((it)->nonRecurring == 1)
		{
			DequeueNonRecurringJob(type);
		}
		SJFQueue.pop_front();
	}
	else if (type == "FCFS")
	{
		list<FCFS>::iterator it;
		it = FCFSQueue.begin();

		if ((it)->recurring == 1)
		{
			DequeueRecurringJob(type);
		}
		else if ((it)->nonRecurring == 1)
		{
			DequeueNonRecurringJob(type);
		}
		FCFSQueue.pop_front();
	}

	
}

void Scheduler::DequeueNonRecurringJob(string& type)
{
	ofstream outTofile;
	outTofile.open("output.txt", ios::app);
	int i = -1;
	if (AvailableCPUs.size() > 0)
	{
		i = AvailableCPUs[AvailableCPUs.size() - 1];
		//AvailableCPUs.pop_back();
	}
	cout << endl << "CPU:" << i << " active" << endl;
	outTofile<< endl << "CPU:" << i << " active" << endl;

	if (type == "Priority")
	{
		sortJobQueuePriorityWise(NRpriorityJobQueue);
		CPU[i] = 'X';
		list<Priority>::iterator it;
		it = NRpriorityJobQueue.begin();
		it->CPU = i;
		cout << "Non Recurring Priority Job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
		outTofile<< "Non Recurring Priority Job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
		//NRpriorityJobQueue.pop_front();
		ProcessVerifyCPU[i][0] = "NRPriority";
		ProcessVerifyCPU[i][1] = to_string(it->ID);
	}
	else if (type == "SJF")
	{
		sortJobQueueSJFWise(NRSJFJobQueue);
		CPU[i] = 'X';
		list<SJF>::iterator it;
		it = NRSJFJobQueue.begin();
		it->CPU = i;
		cout << "Non Recurring SJF Job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
		outTofile<< "Non Recurring SJF Job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
		//NRSJFJobQueue.pop_front();
		ProcessVerifyCPU[i][0] = "NRSJF";
		ProcessVerifyCPU[i][1] = to_string(it->ID);
	}
	else if (type == "FCFS")
	{
		CPU[i] = 'X';
		list<FCFS>::iterator it;
		it = NRFCFSJobQueue.begin();
		it->CPU = i;
		cout << "Non Recurring FCFS Job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
		outTofile<< "Non Recurring FCFS Job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
		//NRFCFSJobQueue.pop_front();
		ProcessVerifyCPU[i][0] = "NRFCFS";
		ProcessVerifyCPU[i][1] = to_string(it->ID);
	}
}
	
void Scheduler::DequeueRecurringJob(string& type)
{
	ofstream outTofile;
	outTofile.open("output.txt", ios::app);
	int i = -1;
	if (AvailableCPUs.size() > 0)
	{
		i = AvailableCPUs[AvailableCPUs.size() - 1];
		//AvailableCPUs.pop_back();
	}
	cout << endl << "CPU:" << i << " active" << endl;
	outTofile << endl << "CPU:" << i << " active" << endl;
		
		if (type == "Priority")
		{
			sortJobQueuePriorityWise(RpriorityJobQueue);
			CPU[i] = 'X';
			list<Priority>::iterator it;
			it = RpriorityJobQueue.begin();
			it->CPU = i;
			cout << "Recurring Priority Job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
			outTofile<< "Recurring Priority Job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
			it->processed = false;
			SleepRecurringJob(RpriorityJobQueue);	//transfer recurring job to a backup list until sleep time is over.
			//RpriorityJobQueue.pop_front();
			ProcessVerifyCPU[i][0] = "RPriority";
			ProcessVerifyCPU[i][1] = to_string(it->ID);
		}
		else if (type == "SJF")
		{
			sortJobQueueSJFWise(RSJFJobQueue);
			CPU[i] = 'X';
			list<SJF>::iterator it;
			it = RSJFJobQueue.begin();
			it->CPU = i;
			cout << "Recurring SJF job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
			outTofile<< "Recurring SJF job in CPU:" << it->CPU << "\tProcess ID:" << it->ID << " dequeued" << endl;
			SleepRecurringJob(RSJFJobQueue);	//transfer recurring job to a backup list until sleep time is over.
			ProcessVerifyCPU[i][0] = "RSJF";
			ProcessVerifyCPU[i][1] = to_string(it->ID);
		}
		else if (type == "FCFS")	//no sorting needed.
		{
			
			list<FCFS>::iterator it;
			it = RFCFSJobQueue.begin();
			it->CPU = i;			
			cout << "Recurring FCFS job in CPU:" << it->CPU << "\tPRocess ID:" << it->ID << " dequeued" << endl;
			outTofile<< "Recurring FCFS job in CPU:" << it->CPU << "\tPRocess ID:" << it->ID << " dequeued" << endl;
			CPU[i] = 'X';
			SleepRecurringJob(RFCFSJobQueue);	//transfer recurring job to a backup list until sleep time is over.
			ProcessVerifyCPU[i][0] = "RFCFS";
			ProcessVerifyCPU[i][1] = to_string(it->ID);
		}	
}

void Scheduler::SleepRecurringJob(list<Priority> RecurringJobs)
{
	list<Priority>::iterator it;
	it = RecurringJobs.begin();
	PrioritySleepqueue.push_back(*it);
}

void Scheduler::SleepRecurringJob(list<SJF> RecurringJobs)
{
	list<SJF>::iterator it;
	it = RecurringJobs.begin();
	SJFSleepQueue.push_back(*it);
}

void Scheduler::SleepRecurringJob(list<FCFS> RecurringJobs)
{
	list<FCFS>::iterator it;
	it = RecurringJobs.begin();
	FCFSSleepQueue.push_back(*it);
}

int Scheduler::AreCPUAvailable()
{
	AvailableCPU_count = 0;
	for (int i = 0; i < CPU_count; i++)
	{
		if (CPU[i] == 'O')
		{
			cout << endl << "CPU " << (i + 1) << "is available" << endl;
			
			AvailableCPU_count++;
		}			
	}
	return AvailableCPU_count;
}

int Scheduler::findAvailableCPU()
{	
	for (int i = 0; i < AvailableCPU_count; i++)
	{
		if (CPU[i] == 'O')
			return i;
	}
	return -1;
}

void Scheduler::backupRecurringJob(list<Priority> RecurringJobs)
{
	list<Priority>::iterator it;
	it = RecurringJobs.begin();
	RecurringPriorityBackup.push_back(*it);
}

void Scheduler::backupRecurringJob(list<SJF> RecurringJobs)
{
	list<SJF>::iterator it;
	it = RecurringJobs.begin();
	RecurringSJFBackup.push_back(*it);
}

void Scheduler::backupRecurringJob(list<FCFS> RecurringJobs)
{
	list<FCFS>::iterator it;
	it = RecurringJobs.begin();
	RecurringFCFSBackup.push_back(*it);
}

void Scheduler::sortJobQueueSJFWise(list<SJF>& jobQueue)
{
	jobQueue.sort(compare_Sjobs);
}

bool Scheduler::compare_Sjobs(SJF& lhs, SJF& rhs)
{
	return (lhs.BurstTime < rhs.BurstTime);
}


void Scheduler::sortJobQueuePriorityWise(list<Priority>& jobQueue)
{
	jobQueue.sort(compare_Pjobs);
}


bool Scheduler::compare_Pjobs(Priority& lhs, Priority& rhs)
{
	return (lhs.PriorityNumber < rhs.PriorityNumber);
}


bool Scheduler::ArePriorityQueuesEmpty()
{
	if (PriorityQueue.size()==0)
		return true;
	else
		return false;
}

bool Scheduler::AreSJFQueuesEmpty()
{
	if (SJFQueue.size()==0)
		return true;
	else
		return false;
}

bool Scheduler::AreFCFSQueuesEmpty()
{
	if (FCFSQueue.size()==0)
		return true;
	else
		return false;
}

void Scheduler::PCB_Generator() {		//PCB generate fine
	
	string type;
	list<string>::iterator it;
	it = filedata.begin();
	
	set_PCBCount(*it++);
	filedata.pop_front();
	temp_Sleeptime = get_Time(*it++);
	filedata.pop_front();
	global_StavationTime = get_Time(*it++);
	filedata.pop_front();

	/*Generate PCB's*/
	for (; it!=filedata.end(); it++)
	{
		type = getType(*it);
		if (type == "Pr")
		{
			generatePriorityJobs(*it);
			//filedata.pop_front();		//check this
		}
		else if (type == "SJF")
		{
			generateSJFJobs(*it);
			filedata.pop_front();
		}
		else if (type == "FCFS")
		{
			generateFCFSJobs(*it);
			filedata.pop_front();
		}
	}
}

void Scheduler::generateFCFSJobs(string& line)
{
	ofstream outTofile;
	outTofile.open("output.txt", ios::app);
	int ProcessCreationProb, RecursiveProb, NonRecursiveProb, BurstTime;
	int random_number, random_sleep;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(0));
	random_sleep = (rand() % 10) + 1;
	decodeInput(line, ProcessCreationProb, RecursiveProb, NonRecursiveProb, FCFS_BurstRange1, FCFS_BurstRange2);
	for (int i = 1; i < 4; i++)
	{
		FCFS fcfs;
		//PCB *pcb;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		srand(time(0));
		random_number = rand() % 100 + 1;
		if (random_number <= ProcessCreationProb)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			srand(time(0));
			random_number = rand() % 100 + 1;
			if (random_number <= RecursiveProb)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				srand(time(NULL));
				random_number = rand() % 100 + 1;
				RecurringPCB *rPCB;
				rPCB = &fcfs;
				rPCB->CreatePCB(FCFS_BurstRange1, FCFS_BurstRange2, random_number);
				
				//pcb = rPCB;
				//pcb = &fcfs;
				//pcb->recurring = 1;
				//pcb->nonRecurring = 0;
				PCB* pcb;
				pcb = rPCB;
				rPCB = dynamic_cast<RecurringPCB*>(pcb);
				FCFS* ptr_fcfs;
				ptr_fcfs = dynamic_cast<FCFS*>(rPCB);
				ptr_fcfs->recurring = 1;
				ptr_fcfs->nonRecurring = 0;
				
				fcfs.recurring = 1;
				fcfs.nonRecurring = 0;

				FCFSQueue.push_back(fcfs);	//Process enqued
				fcfs.set_Sleeptime(random_sleep);
				
				RFCFSJobQueue.push_back(fcfs);	//Process enqued
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				srand(time(0));
				random_number = rand() % 100 + 1;				
				NonRecurringPCB *nrPCB;
				nrPCB = &fcfs;
				nrPCB->CreatePCB(FCFS_BurstRange1, FCFS_BurstRange2, random_number);
				//pcb = nrPCB;
				//pcb = &fcfs;
				//pcb->nonRecurring = 1;
				//pcb->recurring = 0;
				FCFS* ptr_fcfs;
				PCB* pcb;
				pcb = nrPCB;
				nrPCB = dynamic_cast<NonRecurringPCB*>(pcb);
				ptr_fcfs = dynamic_cast<FCFS*>(nrPCB);
				ptr_fcfs->recurring = 0;
				ptr_fcfs->nonRecurring = 1;
				fcfs.set_Sleeptime(0);
				
				fcfs.recurring = 0; 
				fcfs.nonRecurring = 1;
				FCFSQueue.push_back(fcfs);	//Process enqued
				NRFCFSJobQueue.push_back(fcfs);	//Process enqued
			}
		}
		else
		{
			cout << "FCFS process not created." << endl;
			outTofile<< "FCFS process not created." << endl;
		}
	}

	//display the generated PCB's
	if (RFCFSJobQueue.size() > 0)
	{
		list<FCFS>::iterator it;
		cout << "Recurring FCFS Jobs" << endl;
		cout << "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring" << endl;
		outTofile<< "Recurring FCFS Jobs" << endl;
		outTofile<< "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring" << endl;
		for (it = RFCFSJobQueue.begin(); it != RFCFSJobQueue.end(); it++)
		{
			cout << it->ID << "\t" << it->ArrivalTimeR[0] << "\t" << it->BurstTimeR[0] << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << endl;
			outTofile<< it->ID << "\t" << it->ArrivalTimeR[0] << "\t" << it->BurstTimeR[0] << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << endl;
		}
	}

	if (NRFCFSJobQueue.size() > 0)
	{
		list<FCFS>::iterator it;
		cout << "Non-Recurring FCFS Jobs" << endl;
		cout << "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring" << endl;
		outTofile<< "Non-Recurring FCFS Jobs" << endl;
		outTofile<< "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring" << endl;
		for (it = NRFCFSJobQueue.begin(); it != NRFCFSJobQueue.end(); it++)
		{
			cout << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << endl;			
			outTofile<< it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << endl;
		}
	}
}

void Scheduler::generateSJFJobs(string& line)
{
	ofstream outTofile;
	outTofile.open("output.txt", ios::app);
	int ProcessCreationProb, RecursiveProb, NonRecursiveProb, BurstTime;
	int random_number, random_sleep;
	srand(time(0));
	random_sleep = (rand() % 10) + 1;
	decodeInput(line, ProcessCreationProb, RecursiveProb, NonRecursiveProb, SJF_BurstRange1, SJF_BurstRange2);
	for (int i = 1; i < 4; i++)
	{
		//PCB *pcb;
		SJF sjf;
		
		srand(time(0));
		random_number = rand() % 100 + 1;
		if (random_number <= ProcessCreationProb)
		{
			srand(time(NULL));
			random_number = rand() % 100 + 1;
			if (random_number <= RecursiveProb)
			{
				srand(time(0));
				random_number = rand() % 100 + 1;
				RecurringPCB *rPCB;
				rPCB = &sjf;
				rPCB->CreatePCB(SJF_BurstRange1, SJF_BurstRange2, random_number);
				//pcb = rPCB;
				//pcb = &sjf;
				//pcb->nonRecurring = 0;
				//pcb->recurring = 1;
				SJF* ptr_sjf;
				PCB* pcb;
				pcb = rPCB;
				rPCB = dynamic_cast<RecurringPCB*>(pcb);
				ptr_sjf = dynamic_cast<SJF*>(rPCB);
				ptr_sjf->recurring = 1;
				ptr_sjf->nonRecurring = 0;
				
				sjf.nonRecurring = 0;
				sjf.recurring = 1;

				SJFQueue.push_back(sjf);	//Process enqued
				sjf.set_Sleeptime(random_sleep);
				
				RSJFJobQueue.push_back(sjf);	//Process enqued
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				srand(time(NULL));
				random_number = rand() % 100 + 1;
				NonRecurringPCB *nrPCB;
				nrPCB = &sjf;
				nrPCB->CreatePCB(SJF_BurstRange1, SJF_BurstRange2, random_number);
				//pcb = nrPCB;
				//pcb = &sjf;
				//pcb->nonRecurring = 1;
				//pcb->recurring = 0;
				SJF* ptr_sjf;
				PCB* pcb;
				pcb = nrPCB;
				nrPCB = dynamic_cast<NonRecurringPCB*>(pcb);
				ptr_sjf = dynamic_cast<SJF*>(nrPCB);
				ptr_sjf->recurring = 0;
				ptr_sjf->nonRecurring = 1;
				sjf.set_Sleeptime(0);

				sjf.nonRecurring = 1;
				sjf.recurring = 0;
				SJFQueue.push_back(sjf);	//Process enqued
				NRSJFJobQueue.push_back(sjf);	//Process enqued
			}
		}
		else
		{
			cout << "SJF process not created." << endl;
			outTofile<< "SJF process not created." << endl;
		}
	}


	//display the generated PCB's
	if (RSJFJobQueue.size() > 0)
	{
		list<SJF>::iterator it;
		cout << "Recurring SJF Jobs" << endl;
		cout << "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring" << endl;
		outTofile<< "Recurring SJF Jobs" << endl;
		outTofile<< "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring" << endl;
		for (it = RSJFJobQueue.begin(); it != RSJFJobQueue.end(); it++)
		{
			cout << it->ID << "\t" << it->ArrivalTimeR[0] << "\t" << it->BurstTimeR[0] << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << endl;
			outTofile<< it->ID << "\t" << it->ArrivalTimeR[0] << "\t" << it->BurstTimeR[0] << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << endl;
		}
	}

	if (NRSJFJobQueue.size() > 0)
	{
		list<SJF>::iterator it;
		cout << "Non-Recurring SJF Jobs" << endl;
		cout << "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring" << endl;
		outTofile<< "Non-Recurring SJF Jobs" << endl;
		outTofile<< "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring" << endl;
		for (it = NRSJFJobQueue.begin(); it != NRSJFJobQueue.end(); it++)
		{
			cout << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << endl;
			outTofile<< it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->StarvationTime << "\t" << it->get_SleepTime() << "\t" << it->recurring << "/" << it->nonRecurring << endl;
		}
	}
}

void Scheduler::generatePriorityJobs(string& line)
{	
	ofstream outTofile;
	outTofile.open("output.txt", ios::app);
	int ProcessCreationProb, RecursiveProb, NonRecursiveProb, BurstTime;
	int random_number,random_sleep;
	decodeInput(line, ProcessCreationProb, RecursiveProb, NonRecursiveProb, Priority_BurstRange1, Priority_BurstRange2);	//decode successful
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(0));
	random_sleep = (rand() % 10) + 1;
	for (int i = 1; i < 4; i++)
	{
		//PCB *pcb;
		Priority pr;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		srand(time(0));
		random_number = rand() % 100 + 1;
		if (random_number <= ProcessCreationProb)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			srand(time(NULL));
			random_number = rand() % 100 + 1;
			if (random_number <= RecursiveProb)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				srand(time(0));
				random_number = rand() % 100 + 1;
				RecurringPCB *rPCB;	//created because createPCB is common to 
				rPCB = &pr;
				rPCB->CreatePCB(Priority_BurstRange1, Priority_BurstRange2,random_number);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				srand(time(0));
				pr.PriorityNumber = (rand() % 10) + 1;
				/*static_cast<Priority*>(rPCB)->PriorityNumber = (rand() % 10) + 1;	//static cast for accessing derived class member using base class pointer*/
				pr.set_Sleeptime(random_sleep);
				/*static_cast<Priority*>(rPCB)->set_Sleeptime((rand() % 10) + 1);	//static cast for accessing derived class member using base class pointer*/
				
				//pcb = rPCB;
				//pcb = &pr;
				//pcb->recurring = 1;
				//pcb->nonRecurring = 0;
				
				Priority* ptr_pr;
				PCB* pcb;
				pcb = rPCB;
				rPCB = dynamic_cast<RecurringPCB*>(pcb);
				ptr_pr = dynamic_cast<Priority*>(rPCB);
				ptr_pr->recurring = 1;
				ptr_pr->nonRecurring = 0;

				pr.recurring = 1;
				pr.nonRecurring = 0;
				PriorityQueue.push_back(pr);	//Process enqued
				
				RpriorityJobQueue.push_back(pr);	//Process enqued				
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				srand(time(NULL));
				random_number = rand() % 100 + 1;
				NonRecurringPCB *nrPCB;
				nrPCB = &pr;
				nrPCB->CreatePCB(Priority_BurstRange1, Priority_BurstRange2,random_number);
				std::this_thread::sleep_for(std::chrono::seconds(1));
				srand(time(0));
				pr.PriorityNumber = (rand() % 10) + 1;
				pr.set_Sleeptime(0);
				//static_cast<Priority*>(nrPCB)->PriorityNumber = (rand() % 10) + 1;	//static cast for accessing derived class member using base class pointer
				//static_cast<Priority*>(nrPCB)->set_Sleeptime(-9999);	//static cast for accessing derived class member using base class pointer
				
				//pcb = nrPCB;
				//pcb = &pr;
				//pcb->nonRecurring = 1;
				//pcb->recurring = 0;
				Priority* ptr_pr;
				PCB* pcb;
				pcb = nrPCB;
				nrPCB = dynamic_cast<NonRecurringPCB*>(pcb);
				ptr_pr = dynamic_cast<Priority*>(nrPCB);
				
				ptr_pr->recurring = 0;
				ptr_pr->nonRecurring = 1;

				pr.recurring = 0;
				pr.nonRecurring = 1;
				PriorityQueue.push_back(pr);	//Process enqued
				NRpriorityJobQueue.push_back(pr);	//Process enqued
			}
		}
		else
		{
			cout << "Priority process not created." << endl;
			outTofile<< "Priority process not created." << endl;
		}		
	}

	//display the generated PCB's
	if (RpriorityJobQueue.size() > 0)
	{
		list<Priority>::iterator it;
		cout << "Recurring Priority Jobs" << endl;
		cout << "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring" << endl;
		outTofile<< "Recurring Priority Jobs" << endl;
		outTofile<< "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring" << endl;
		for (it = RpriorityJobQueue.begin(); it != RpriorityJobQueue.end(); it++)
		{
			cout << it->ID << "\t" << it->ArrivalTimeR[0] << "\t" << it->BurstTimeR[0] << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->PriorityNumber << "\t" << it->get_SleepTime() << endl;			
			outTofile<< it->ID << "\t" << it->ArrivalTimeR[0] << "\t" << it->BurstTimeR[0] << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->PriorityNumber << "\t" << it->get_SleepTime() << endl;
		}
	}

	if (NRpriorityJobQueue.size() > 0)
	{
		list<Priority>::iterator it;
		cout << "Non-Recurring Priority Jobs" << endl;
		cout << "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring" << endl;
		outTofile<< "Non-Recurring Priority Jobs" << endl;
		outTofile<< "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring" << endl;
		for (it = NRpriorityJobQueue.begin();it!=NRpriorityJobQueue.end();it++)
		{	
			cout << it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->PriorityNumber << "\t" << it->get_SleepTime() << endl;
			outTofile<< it->ID << "\t" << it->ArrivalTime << "\t" << it->BurstTime << "\t" << it->recurring << "/" << it->nonRecurring << "\t" << it->PriorityNumber << "\t" << it->get_SleepTime() << endl;
		}
	}
}

void Scheduler::decodeInput(string& line, int& ProcessCreationProb, int& RecursiveProb, int& NonRecursiveProb, int& BurstRange1, int& BurstRange2)
{
	//Pr 15 R 1 NR 99 B 1 3
	lrtrim(line);
	line = line.substr(line.find(' '),line.length());
	lrtrim(line);
	ProcessCreationProb = stoi(line.substr(0, line.find(' ')));
	line = line.substr(line.find(' '), line.length());
	lrtrim(line);
	line = line.substr(line.find(' '), line.length());
	lrtrim(line);
	RecursiveProb = stoi(line.substr(0, line.find(' ')));
	line = line.substr(line.find(' '), line.length());
	lrtrim(line);
	line = line.substr(line.find(' '), line.length());
	lrtrim(line);
	NonRecursiveProb = stoi(line.substr(0, line.find(' ')));
	line = line.substr(line.find(' '), line.length());
	lrtrim(line);
	line = line.substr(line.find(' '), line.length());
	lrtrim(line);
	BurstRange1 = stoi(line.substr(0, line.find(' ')));
	line = line.substr(line.find(' '), line.length());
	lrtrim(line);
	rtrim(line);
	BurstRange2 = stoi(line.substr(0));		
}

string Scheduler::getType(string& line)
{
	lrtrim(line);
	string type = line.substr(0, line.find(' '));
	rtrim(type);
	return type;
}

void Scheduler::set_PCBCount(string& cnt)
{
	PCB_count = stoi(cnt);
}

int Scheduler::get_Time(string& val1)
{
	string temp;
	reverse(val1.begin(), val1.end());
	lrtrim(val1);
	temp = val1.substr(0, val1.find(' '));
	reverse(temp.begin(), temp.end());
	int timer1 = stoi(temp);
	val1 = val1.substr(val1.find(' '),val1.length());
	lrtrim(val1);
	temp = val1.substr(0, val1.find(' '));
	reverse(temp.begin(), temp.end());
	int timer2 = stoi(temp);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	srand(time(0));
	
	int timer = (rand() % (timer1 - timer2)) + timer2;
	
	return timer;
}


void Scheduler::lrtrim(string & line)
{
	int first = line.find_first_not_of('\t');
	line = line.substr(first);
	first = line.find_first_not_of(' ');
	line = line.substr(first);
}

void Scheduler::rtrim(string &line)
{
	int cnt;
	int space = line.find(' ', 0);
	int tab = line.find('\t', 0);
	if (space == string::npos)
		cnt = 0;
	if (tab == string::npos)
		cnt = 0;
	else
		cnt = space < tab ? space : tab;
	line = line.substr(0, cnt - 1);
}

void Scheduler::display()
{
	ofstream outToFile;
	outToFile.open("output.txt", ios::app);
	list<Priority>::iterator it_Priority;
	cout << "Final Statistics:" << endl;
	cout << "Display Non-Recurring Priority Jobs" << endl;
	cout << "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
	outToFile<< "Final Statistics:" << endl;
	outToFile<< "Display Non-Recurring Priority Jobs" << endl;
	outToFile<< "ID \t Arrival Time \t Burst Time \t Priority Number \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
	for (it_Priority = NonRecurringPriorityBackup.begin(); it_Priority != NonRecurringPriorityBackup.end(); it_Priority++)
	{
		cout << it_Priority->ID << "\t" << it_Priority->ArrivalTime << "\t" << it_Priority->BurstTime << "\t" << it_Priority->PriorityNumber << "\t" << it_Priority->get_SleepTime() << "\t" << it_Priority->recurring << "/" << it_Priority->nonRecurring << "\t" << it_Priority->CompletionTime << "\t" << it_Priority->WaitingTime << "\t" << it_Priority->TurnAroundTime << endl;
		outToFile<< it_Priority->ID << "\t" << it_Priority->ArrivalTime << "\t" << it_Priority->BurstTime << "\t" << it_Priority->PriorityNumber << "\t" << it_Priority->get_SleepTime() << "\t" << it_Priority->recurring << "/" << it_Priority->nonRecurring << "\t" << it_Priority->CompletionTime << "\t" << it_Priority->WaitingTime << "\t" << it_Priority->TurnAroundTime << endl;
	}

	list<SJF>::iterator it_SJF;
	cout << "Display Non-Recurring SJF Jobs" << endl;
	cout << "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
	outToFile<< "Display Non-Recurring SJF Jobs" << endl;
	outToFile<< "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
	for (it_SJF = NonRecurringSJFBackup.begin(); it_SJF != NonRecurringSJFBackup.end(); it_SJF++)
	{
		cout << it_SJF->ID << "\t" << it_SJF->ArrivalTime << "\t" << it_SJF->BurstTime << "\t" << it_SJF->StarvationTime << "\t" << it_SJF->get_SleepTime() << "\t" << it_SJF->recurring << "/" << it_SJF->nonRecurring << "\t" << it_SJF->CompletionTime << "\t" << it_SJF->WaitingTime << "\t" << it_SJF->TurnAroundTime << endl;
		outToFile<< it_SJF->ID << "\t" << it_SJF->ArrivalTime << "\t" << it_SJF->BurstTime << "\t" << it_SJF->StarvationTime << "\t" << it_SJF->get_SleepTime() << "\t" << it_SJF->recurring << "/" << it_SJF->nonRecurring << "\t" << it_SJF->CompletionTime << "\t" << it_SJF->WaitingTime << "\t" << it_SJF->TurnAroundTime << endl;
	}

	list<FCFS>::iterator it_FCFS;
	cout << "Display Non-Recurring FCFS Jobs" << endl;
	cout << "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
	outToFile<< "Display Non-Recurring FCFS Jobs" << endl;
	outToFile<< "ID \t Arrival Time \t Burst Time \t Starvation Time \t Sleep Time \t Recurring/Non-Recurring \t Completion Time \t Waiting Time \t Turn Around Time" << endl;
	for (it_FCFS = NonRecurringFCFSBackup.begin(); it_FCFS != NonRecurringFCFSBackup.end(); it_FCFS++)
	{
		cout << it_FCFS->ID << "\t" << it_FCFS->ArrivalTime << "\t" << it_FCFS->BurstTime << "\t" << it_FCFS->StarvationTime << "\t" << it_FCFS->get_SleepTime() << "\t" << it_FCFS->recurring << "/" << it_FCFS->nonRecurring << "\t" << it_FCFS->CompletionTime << "\t" << it_FCFS->WaitingTime << "\t" << it_FCFS->TurnAroundTime << endl;
		outToFile<< it_FCFS->ID << "\t" << it_FCFS->ArrivalTime << "\t" << it_FCFS->BurstTime << "\t" << it_FCFS->StarvationTime << "\t" << it_FCFS->get_SleepTime() << "\t" << it_FCFS->recurring << "/" << it_FCFS->nonRecurring << "\t" << it_FCFS->CompletionTime << "\t" << it_FCFS->WaitingTime << "\t" << it_FCFS->TurnAroundTime << endl;
	}

	cout << "Display Recurring Priority Jobs" << endl;
	outToFile<< "Display Recurring Priority Jobs" << endl;
	for (it_Priority = RecurringPriorityBackup.begin(); it_Priority != RecurringPriorityBackup.end(); it_Priority++)
	{
		cout << "ID: " << it_Priority->ID;
		cout << endl << "Priority Number: " << it_Priority->PriorityNumber;
		cout << endl << "Arrival Time/s: ";
		outToFile<< "ID: " << it_Priority->ID;
		outToFile<< endl << "Priority Number: " << it_Priority->PriorityNumber;
		outToFile<< endl << "Arrival Time/s: ";
		for (int i = 0; i < it_Priority->ArrivalTimeR.size(); i++)
		{
			cout << it_Priority->ArrivalTimeR[i] << " \t";
			outToFile<< it_Priority->ArrivalTimeR[i] << " \t";
		}
			
		cout << endl << "Burst Time/s: ";
		outToFile<< endl << "Burst Time/s: ";
		for (int i = 0; i < it_Priority->BurstTimeR.size(); i++)
		{
			cout << it_Priority->BurstTimeR[i] << " \t";
			outToFile<< it_Priority->BurstTimeR[i] << " \t";
		}
			
		cout << endl << "Completion Time/s";
		outToFile<< endl << "Completion Time/s";
		for (int i = 0; i < it_Priority->CompletionTimeR.size(); i++)
		{
			cout << it_Priority->CompletionTimeR[i] << " \t";
			outToFile<< it_Priority->CompletionTimeR[i] << " \t";
		}
			
		cout << endl << "Waiting Time/s";
		outToFile<< endl << "Waiting Time/s";
		for (int i = 0; i < it_Priority->WaitingTimeR.size(); i++)
		{
			cout << it_Priority->WaitingTimeR[i] << " \t";
			outToFile<< it_Priority->WaitingTimeR[i] << " \t";
		}
			
		cout << endl << "Turn Around Time/s";
		outToFile<< endl << "Turn Around Time/s";
		for (int i = 0; i < it_Priority->TurnAroundTimeR.size(); i++)
		{
			cout << it_Priority->TurnAroundTimeR[i] << " \t";
			outToFile<< it_Priority->TurnAroundTimeR[i] << " \t";
		}
			
	}
	cout << endl;
	outToFile<< endl;
	//print max starvation time and max sleep time as well
	cout << "Display Recurring SJF Jobs" << endl;
	outToFile<< "Display Recurring SJF Jobs" << endl;
	for (it_SJF = RecurringSJFBackup.begin(); it_SJF != RecurringSJFBackup.end(); it_SJF++)
	{
		cout << "ID: " << it_SJF->ID;		
		cout << endl << "Arrival Time/s: ";
		outToFile<< "ID: " << it_SJF->ID;
		outToFile<< endl << "Arrival Time/s: ";
		for (int i = 0; i < it_SJF->ArrivalTimeR.size(); i++)
		{
			cout << it_SJF->ArrivalTimeR[i] << " \t";
			outToFile<< it_SJF->ArrivalTimeR[i] << " \t";
		}
			
		cout << endl << "Burst Time/s: ";
		outToFile<< endl << "Burst Time/s: ";
		for (int i = 0; i < it_SJF->BurstTimeR.size(); i++)
		{
			cout << it_SJF->BurstTimeR[i] << " \t";
			outToFile<< it_SJF->BurstTimeR[i] << " \t";
		}
			
		cout << endl << "Completion Time/s";
		outToFile<< endl << "Completion Time/s";
		for (int i = 0; i < it_SJF->CompletionTimeR.size(); i++)
		{
			cout << it_SJF->CompletionTimeR[i] << " \t";
			outToFile<< it_SJF->CompletionTimeR[i] << " \t";
		}
			
		cout << endl << "Waiting Time/s";
		outToFile<< endl << "Waiting Time/s";
		for (int i = 0; i < it_SJF->WaitingTimeR.size(); i++)
		{
			cout << it_SJF->WaitingTimeR[i] << " \t";
			outToFile<< it_SJF->WaitingTimeR[i] << " \t";
		}
			
		cout << endl << "Turn Around Time/s";
		outToFile<< endl << "Turn Around Time/s";
		for (int i = 0; i < it_SJF->TurnAroundTimeR.size(); i++)
		{
			cout << it_SJF->TurnAroundTimeR[i] << " \t";
			outToFile<< it_SJF->TurnAroundTimeR[i] << " \t";
		}			
	}

	//print max starvation time and max sleep time as well
	cout << "Display Recurring FCFS Jobs" << endl;
	outToFile<< "Display Recurring FCFS Jobs" << endl;
	for (it_FCFS = RecurringFCFSBackup.begin(); it_FCFS != RecurringFCFSBackup.end(); it_FCFS++)
	{
		cout << "ID: " << it_FCFS->ID;
		cout << endl << "Arrival Time/s: ";
		outToFile<< "ID: " << it_FCFS->ID;
		outToFile<< endl << "Arrival Time/s: ";
		for (int i = 0; i < it_FCFS->ArrivalTimeR.size(); i++)
		{
			cout << it_FCFS->ArrivalTimeR[i] << " \t";
			outToFile<< it_FCFS->ArrivalTimeR[i] << " \t";
		}
			
		cout << endl << "Burst Time/s: ";
		outToFile<< endl << "Burst Time/s: ";
		for (int i = 0; i < it_FCFS->BurstTimeR.size(); i++)
		{
			cout << it_FCFS->BurstTimeR[i] << " \t";
			outToFile<< it_FCFS->BurstTimeR[i] << " \t";
		}
			
		cout << endl << "Completion Time/s";
		outToFile<< endl << "Completion Time/s";
		for (int i = 0; i < it_FCFS->CompletionTimeR.size(); i++)
		{
			cout << it_FCFS->CompletionTimeR[i] << " \t";
			outToFile<< it_FCFS->CompletionTimeR[i] << " \t";
		}
			
		cout << endl << "Waiting Time/s";
		outToFile<< endl << "Waiting Time/s";
		for (int i = 0; i < it_FCFS->WaitingTimeR.size(); i++)
		{
			cout << it_FCFS->WaitingTimeR[i] << " \t";
			outToFile<< it_FCFS->WaitingTimeR[i] << " \t";
		}
			
		cout << endl << "Turn Around Time/s";
		outToFile<< endl << "Turn Around Time/s";
		for (int i = 0; i < it_FCFS->TurnAroundTimeR.size(); i++)
		{
			cout << it_FCFS->TurnAroundTimeR[i] << " \t";
			outToFile<< it_FCFS->TurnAroundTimeR[i] << " \t";
		}
			
	}
	cout << endl << "CPU Utilisation" << endl;
	outToFile<< endl << "CPU Utilisation" << endl;
	int total = 0;
	for (int i = 0; i < CPU_count; i++)
		total += CPUUtilisation[i];
	//cout << "Total:" << total << endl;
	for (int i = 0; i < CPU_count; i++)
	{
		//cout << "CPU utilisation:" << CPUUtilisation[i] << endl;
		cout << "CPU " << i << ":" << ((double)CPUUtilisation[i]/total)*100 << "% utilised" << endl;
		outToFile<< "CPU " << i << ":" << ((double)CPUUtilisation[i] / total) * 100 << "% utilised" << endl;
	}
}

vector<int> Scheduler::sendCPUBurst()
{		
	return tempCPUBursts;
}