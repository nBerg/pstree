#include <stdio.h>
#include <stdlib.h>

#include "pstree.h"

int get_procs(struct proc_struct **procs) {
	
	DIR *proc_dir;
	struct dirent *dir;
	char pid[MAX_PROCS][MAX_PID_LENGTH + 1];

	int i, num_procs = 0;

	// Find all pids in /proc
	proc_dir = open_dir("/proc");
	if (proc_dir == NULL) {
		perror ("Error opening the /proc directory");
		return -1;
	}

	while ((dir = readdir(proc_dir)) != NULL) {
		if (!fnmatch("[0-9]*", dir->d_name, 0)) {
			snprintf(pids[i], sizeof(pid[i]), "%s", dir->d_name);
			++i;
			++num_procs;
		}
	}

	closedir(proc_dir);

	// Fill in a proc_struct for each process
}


int main(int argc, char *argv[])
{
	struct proc_struct *procs[MAX_PROCS];
	int num_procs;

	// Get cmd line options

	// Get list of processes
	if ((num_procs = get_procs(procs)) < 0) {
		return EXIT_FAILURE;
	}

	// Print Tree

	return EXIT_SUCCESS;
}
