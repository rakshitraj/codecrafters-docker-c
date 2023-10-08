#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>


// Usage: your_docker.sh run <image> <command> <arg1> <arg2> ...
int main(int argc, char *argv[]) {
	// Disable output buffering
	setbuf(stdout, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// Decalare pipe file descriptors for output and error
	int fdOut[2];
	int fdErr[2];

	if (pipe(fdOut) == -1 || pipe(fdErr) == -1) {
		printf("Error initializing pipe.");
		return 2;
	}
	
	char *command = argv[3];
	int child_pid = fork();
	if (child_pid == -1) {
	    printf("Error forking!");
	    return 1;
	}
	
	if (child_pid == 0) {
		// In the child process
		// Replace current program with calling program
		close(fdOut[0]);
		close(fdErr[0]);

		fdOut[1] = dup(1);
		fdErr[1] = dup(2);

		execv(command, &argv[3]);
	} else {
		// We're in parent
		int len;
		char buffer_out[1024];
		char buffer_err[1024];

		close(fdOut[1]);
		close(fdErr[1]);

		// Read output buffer
		if((len = read(fdOut[0], buffer_out, 1024)) == -1) {
			printf("Error reading output...");
			return -1;
		}
		write(1, buffer_out, len);

		// Read output buffer
		if((len = read(fdErr[0], buffer_err, 1024)) == -1) {
			printf("Error reading err...");
			return -1;
		}
		write(2, buffer_err, len);

		close(fdOut[0]);
		close(fdErr[0]);

		wait(NULL);
	    printf("Child terminated");
	}

	return 0;
}
