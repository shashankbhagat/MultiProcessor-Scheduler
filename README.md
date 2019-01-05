# MultiProcessor-Scheduler - C++

This project is about designing and simulating a clock-driven multi-processor scheduler in an object-oriented manner. The scheduler consists of a multi-level job queue where each level follows a different scheduling algorithm viz. Priority, Shortest Job First (SJF) and First-come-first-serve (FCFS). These queues will be enqueued with PCBs. The PCBs can further be classified into Recurring and non-Recurring. These PCBs are generated randomly, based on the probability data provided in an input file.
The goals of the project:
1.	Create a PCB hierarchy.
2.	The hierarchy created should be scalable and easily extendible.
3.	The scheduling of the PCB’s should be handled by multiple processors.
4.	Event driven mechanism should be incorporated.
5.	Object oriented design principles should also be incorporated in the project.

The PCB’s are generated dynamically and are scheduled into multiple processors simultaneously. The three types of queues incorporated are scheduled in a sequence viz. Priority, SJF and then FCFS. There is a high probability for the PCB’s to be starved in SJF and FCFS. To accomplish this, an PCB upgradation mechanism is also implemented. This mechanism would enable the PCB to be upgraded to  higher queue in the order.
The statistics are displayed per CPU clock instant as well as like a report at the end.

