#ifndef _PSTREE_H_
#define _PSTREE_H_

// /proc/sys/kernel/pid_max
// TODO: Dont hardcode this in
int MAX_PROCS = 32768;
int MAX_PID_LENGTH = 5;

struct proc_struct {
	char proc_name[64];
	int pid;
	int ppid;

	//Something for children
};

#endif
