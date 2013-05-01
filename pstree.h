#ifndef _PSTREE_H_
#define _PSTREE_H_

// /proc/sys/kernel/pid_max
// TODO: Dont hardcode this in
#define MAX_PROCS 32768
#define MAX_PID_LENGTH 5

struct proc_struct {
	char proc_name[256];
	int pid;
	int ppid;
	struct proc_struct *child;
	struct proc_struct *sibling;
};

#endif
