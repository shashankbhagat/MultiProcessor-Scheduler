// File Name: Scheduler.h
// Author: Shashank Bhagat
// Date: 11/13/2017
// Compiler Used: VC++
// Notes: This is a header file. Includes all the declaration of the variables and member functions in private and public section.
#pragma once
#ifndef SCHEDULER
#include<list>
#include "Priority.h";
#include "SJF.h"
#include "FCFS.h"

using namespace std;

class Scheduler
{
public:
	void initiateScheduler();
	void set_arrival_prob(int);
	void gen_init_pool(string infile = "default.txt");
	//void 
	Scheduler();
	~Scheduler();
	Scheduler(const int&);
	int temp_Sleeptime;
	int temp_StarvationTime;
	void PCB_Generator();
	void display();
	vector<int> sendCPUBurst();
	void ComputeProcess();
	void DequeueJobToCPU();
	char *CPU;	/*X-Not Available	O-Available*/
	vector<int> AvailableCPUs;
	vector<int> BusyCPUs;
	int *CPUUtilisation;

private:
	
	int CPU_ProcessID, prob_percentage, PCB_count, CPU_Clock, CPU_ProcessCntr, Priority_BurstRange1, Priority_BurstRange2, SJF_BurstRange1, SJF_BurstRange2, FCFS_BurstRange1, FCFS_BurstRange2, global_StavationTime;
	int CPU_count, *ProcessCntr;
	string **ProcessVerifyCPU;
	vector<int> tempCPUBursts;
	const bool IsCPUAvailable();
	//void EnqueProcess(list<PCB*>);	//Processes already enqued during creation.
	void DequeueProcess();	
	
	
	int get_PCBCount();
	void set_PCBCount(string&);
	int get_Time(string&);
	
	void lrtrim(string & line);
	void rtrim(string & line);
	string getType(string&);
	list<string> filedata;
	void generatePriorityJobs(string&);
	void generateSJFJobs(string&);
	void generateFCFSJobs(string&);
	void decodeInput(string&, int&, int&, int&, int&, int&);
	list<Priority> RpriorityJobQueue;	//Recurring Priority list
	list<Priority> NRpriorityJobQueue;	//Non-Recurring Priority list
	list<SJF> RSJFJobQueue;	//Recurring SJF list
	list<SJF> NRSJFJobQueue;//Non-Recurring SJF list
	list<FCFS> RFCFSJobQueue;	//Recurring FCFS list
	list<FCFS> NRFCFSJobQueue;//Non-Recurring FCFS list
	list<SJF> SJFJobQueue;
	list<FCFS> FCFSJobQueue;
	list<Priority> PriorityQueue;
	list<SJF> SJFQueue;
	list<FCFS> FCFSQueue;
	bool ArePriorityQueuesEmpty();
	void DequeueJob(string&);
	void DequeueRecurringJob(string&);
	void DequeueNonRecurringJob(string&);
	void sortJobQueuePriorityWise(list<Priority>&);
	static bool compare_Pjobs(Priority&, Priority&);
	void sortJobQueueSJFWise(list<SJF>&);
	static bool compare_Sjobs(SJF&, SJF&);
	list<Priority> RecurringPriorityBackup;
	list<SJF> RecurringSJFBackup;
	list<FCFS> RecurringFCFSBackup;
	list<Priority> NonRecurringPriorityBackup;
	list<SJF> NonRecurringSJFBackup;
	list<FCFS> NonRecurringFCFSBackup;
	void backupRecurringJob(list<Priority>);
	void backupRecurringJob(list<SJF>);
	void backupRecurringJob(list<FCFS>);
	void traverseRecurringQueuesSleepTime();
	void IncrementRecurringQueuesSleepTime(const int&);
	void sortJobQueueSleepTimeWise(list<Priority>&);
	void sortJobQueueSleepTimeWise(list<SJF>&);
	void sortJobQueueSleepTimeWise(list<FCFS>&);
	static bool compare_jobs_ST_Priority(Priority&, Priority&);
	static bool compare_jobs_ST_SJF(SJF&, SJF&);
	static bool compare_jobs_ST_FCFS(FCFS&, FCFS&);
	bool AreSJFQueuesEmpty();
	bool AreFCFSQueuesEmpty();
	
	bool IsPriorityJob, IsSJFjob, IsFCFSJob;
	
	
	void Upgrade_SJF_to_Priority();
	void Upgrade_FCFS_to_SJF();
	void sortJobQueueStarvationwise(list<SJF>&);
	void sortJobQueueStarvationwise(list<FCFS>&);
	static bool compare_jobs_Starvation_SJF(SJF&, SJF&);
	static bool compare_jobs_Starvation_FCFS(FCFS&, FCFS&);
	void generateJobUpgrade_ToPriority(string);
	void generateJobUpgrade_ToSJF(string);
	void incrementStarvationTime();
	list<Priority>PrioritySleepqueue;
	list<SJF>SJFSleepQueue;
	list<FCFS>FCFSSleepQueue;
	void SleepRecurringJob(list<Priority>);
	void SleepRecurringJob(list<SJF>);
	void SleepRecurringJob(list<FCFS>);
	int AreCPUAvailable();
	int AvailableCPU_count;
	int findAvailableCPU();
};
#endif // !SCHEDULER