#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>

#include "pstree.h"

void init_proc_struct(struct proc_struct *p) {
	p->child = NULL;
	p->sibling = NULL;
}

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
	for (i = 0; i < num_procs; i++) {
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
		init_proc_struct(procs[i]);

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

struct proc_struct *get_pid(int pid, struct proc_struct **procs, int num_procs) {
	int i;
	for (i = 0; i < num_procs; i++) {
		if (procs[i]->pid == pid) return procs[i];
	}

	return NULL;
}

int make_parent_child(struct proc_struct **procs, int num_procs) {
	int i;
	struct proc_struct *parent, *sibling;

	for (i = 0; i < num_procs; i++) {
		parent = get_pid(procs[i]->ppid, procs, num_procs);

		if (parent == NULL) {
			if (procs[i]->ppid == 0) {
				//root processes
				continue;
			}
			printf("Error creating parent/child relationships");
			return -1;
		}

		if (parent->child == NULL) {
			// Add as a child of the parent
			parent->child = procs[i];
		} else {
			// Find a sibling and add as a sibling
			sibling = get_pid(parent->child->pid, procs, num_procs);

			while (sibling->sibling != NULL) {
				sibling = get_pid(sibling->sibling->pid, procs, num_procs);
			}

			sibling->sibling = procs[i];
		}
	}

	return 0;
}

void print_tree(struct proc_struct **procs, int num_procs) {
	int i;
	for (i = 0; i < num_procs; i++) {
		printf("PID: %d\n", procs[i]->pid);
	}	
}

int main(int argc, char *argv[])
{
	struct proc_struct *procs[MAX_PROCS];
	int num_procs;

	// Get cmd line options TODO

	// Get list of processes
	if ((num_procs = get_procs(procs)) < 0) {
		return EXIT_FAILURE;
	}

	// Create parent/child relationships
	if (make_parent_child(procs, num_procs) < 0) {
		return EXIT_FAILURE;
	}

	// Print Tree
//	print_tree(procs, num_procs);

	// TODO: Free procs

	return EXIT_SUCCESS;
}
