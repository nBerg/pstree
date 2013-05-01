#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>

#include "pstree.h"

int get_procs(struct proc_struct **procs) {
	
	DIR *proc_dir;
	struct dirent *dir;
	char pids[MAX_PROCS][MAX_PID_LENGTH + 1];

	int i, num_procs = 0;

	// Find all pids in /proc
	proc_dir = opendir("/proc");
	if (proc_dir == NULL) {
		perror ("Error opening the /proc directory");
		return -1;
	}

	while ((dir = readdir(proc_dir)) != NULL) {
		if (!fnmatch("[0-9]*", dir->d_name, 0)) {
			snprintf(pids[i], sizeof(pids[i]), "%s", dir->d_name);
			++i;
			++num_procs;
		}
	}

	closedir(proc_dir);

	// Fill in a proc_struct for each process
	for (i = 1; i < num_procs; i++) {
		FILE *stat_file;
		char path[20], state;

		snprintf(path, sizeof(path), "%s%s%s", "/proc/", pids[i], 
								"/stat");
		if ((stat_file = fopen(path, "r")) == NULL) {
			// Error opening file, or process no longer exists
			continue;
		}

		procs[i] = malloc(sizeof(struct proc_struct));
		if (procs[i] == NULL) {
			perror("Error with malloc");
			return -1;
		}

		if (fscanf(stat_file, "%d %s %c %d", &procs[i]->pid,
						procs[i]->proc_name,
						&state,
						&procs[i]->ppid) != 4) {
			perror("Error reading stat_file");
			return -1;
		}

		fclose(stat_file);
	}

	return num_procs;
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
