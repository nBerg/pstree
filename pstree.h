#ifndef _PSTREE_H_
#define _PSTREE_H_

struct proc_struct {
	char proc_name[64];
	int pid;
	int ppid;

	//Something for children
};

#endif
