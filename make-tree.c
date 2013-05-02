#include <stdio.h>

static void usage() 
{
	printf("Usage: make-tree --width <width> --depth <depth>\n");
}

int main (int argc, char **argv) 
{
	int width, depth;
	int i, j;
	int pid;

	if (argc < 5) {
		usage();
		return -1;
	}

	//TODO put in a bit of error checking
	if (strcmp(argv[1], "--width") == 0) {
		width = atoi(argv[2]);
	} else {
		usage();
		return -1;
	}

	if (strcmp(argv[3], "--depth") == 0) {
		depth = atoi(argv[4]);
	} else {
		usage();
		return -1;
	}

	for (i = 0; i < width; i++) {

        	pid = fork();

        	if (pid == 0) {
            		// child

            		if (--depth <= 0) break;
	            	else i = -1;
        	}
	}

	while(1);

}
