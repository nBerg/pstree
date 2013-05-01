#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <string.h>

#include "pstree.h"

static struct proc_struct *procs[MAX_PROCS];
static int num_procs = 0;

static void init_proc_struct(struct proc_struct *p) {
	p->child = NULL;
	p->sibling = NULL;
}

static int create_root() {
	struct proc_struct *root = malloc(sizeof(struct proc_struct));
	if (root == NULL) {
		perror("Error with malloc");
		return -1;
	}

	init_proc_struct(root);
	root->pid = 0;

	procs[0] = root;
	++num_procs;

	return 0;
}

static int get_pids(char pids[MAX_PROCS][MAX_PID_LENGTH + 1]) 
{
	DIR *proc_dir;
	struct dirent *dir;
	int i = 1;  //pids[0] is a placeholder for the root process

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
	return 0;
}

static int read_stats(char *pid, struct proc_struct *p)
{
	FILE *proc;
	char buf[256];
	char state;

	snprintf(buf, sizeof(buf), "/proc/%s/stat", pid);
	proc = fopen(buf, "r");

	if (proc) {
		if (4 != fscanf(proc, "%d %s %c %d", &p->pid, 
						p->proc_name, 
						&state, 
						&p->ppid)) {
			perror("Error reading from stat file");
			return -1;
		}
		fclose(proc);
	}

	return 0;
}

static int get_procs() {
	
	char pids[MAX_PROCS][MAX_PID_LENGTH + 1];
	int i;

	if (get_pids(pids) < 0) return -1;

	if (create_root() < 0) return -1;

	// Fill in a proc_struct for each process
	for (i = 1; i < num_procs; i++) {
		FILE *stat_file;
		char path[20], state, *name;
		int length;

		procs[i] = malloc(sizeof(struct proc_struct));
		if (procs[i] == NULL) {
			perror("Error with malloc");
			return -1;
		}
		init_proc_struct(procs[i]);

		if (read_stats(pids[i], procs[i]) < 0) return -1;

		//Fix proc_name
		name = procs[i]->proc_name;
		name++;
		length = strlen(name);
		name[length - 1] = 0;

		strncpy(procs[i]->proc_name, name, sizeof(procs[i]->proc_name));
	}

	return num_procs;
}

static struct proc_struct *get_pid(int pid) {
	int i;
	for (i = 0; i < num_procs; i++) {
		if (procs[i]->pid == pid) return procs[i];
	}

	return NULL;
}

static int make_parent_child() {
	int i;
	struct proc_struct *parent, *sibling;

	for (i = 1; i < num_procs; i++) {
		parent = get_pid(procs[i]->ppid);

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
			sibling = get_pid(parent->child->pid);

			while (sibling->sibling != NULL) {
				sibling = get_pid(sibling->sibling->pid);
			}

			sibling->sibling = procs[i];
		}
	}

	return 0;
}

static void print_tree(struct proc_struct *root, int depth, int first) {
	int local_depth = depth;

	// print this process
	if (root->pid != 0) {
		while (local_depth-- > 0) printf("\t");
		printf("%s (%d)\n", root->proc_name, root->pid);
	}

	// print children
	if (root->child != NULL) {
		print_tree(root->child, depth + 1, 0);
	}

	// print siblings
	if (root->sibling != NULL && !first) {
		print_tree(root->sibling, depth, 0);
	}
}

int main(int argc, char *argv[])
{
	int i, start_pid = 0, depth, input;
	struct proc_struct *root;

	// Get cmd line options
	if (argc > 1) {
		input = atoi(argv[1]);	
		if (input == 0) {
			printf("Error: Invalid input\n");
			return EXIT_FAILURE;
		} else {
			start_pid = input;
		}
	}

	// Get list of processes
	if ((num_procs = get_procs()) < 0) {
		return EXIT_FAILURE;
	}

	// Create parent/child relationships
	if (make_parent_child() < 0) {
		return EXIT_FAILURE;
	}

	// Print Tree
	if (start_pid == 0) depth = -1;
	else depth = 0;

	if((root = get_pid(start_pid)) == NULL) {
		printf("Error: PID %d could not be found\n", start_pid);
		return EXIT_FAILURE;
	}

	print_tree(root, depth, 1);

	for (i = 0; i < num_procs; i++) {
		free(procs[i]);
	}

	return EXIT_SUCCESS;
}
