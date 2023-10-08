#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>


// Usage: your_docker.sh run <image> <command> <arg1> <arg2> ...
int main(int argc, char *argv[]) {
	// Disable output buffering
	setbuf(stdout, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	// printf("Logs from your program will appear here!\n");

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
        close(fdOut[0]);  // Close read end of output pipe
        close(fdErr[0]);  // Close read end of error pipe

        dup2(fdOut[1], STDOUT_FILENO);  // Redirect standard output to write end of output pipe
        dup2(fdErr[1], STDERR_FILENO);  // Redirect standard error to write end of error pipe

        close(fdOut[1]);  // Close write end of output pipe, already duplicated to STDOUT
        close(fdErr[1]);  // Close write end of error pipe, already duplicated to STDERR

        execv(command, &argv[3]);

		// // If execv fails:
        // perror("Error executing command");
        // _exit(1);  // Use _exit in a child after fork (avoid flushing stdio buffers again)
	} else {
		// In the parent
        int len;
        char buffer_out[1024];
        char buffer_err[1024];

        close(fdOut[1]);  // Close write end of output pipe
        close(fdErr[1]);  // Close write end of error pipe

        while ((len = read(fdOut[0], buffer_out, sizeof(buffer_out))) > 0) {
            write(STDOUT_FILENO, buffer_out, len);
        }

        while ((len = read(fdErr[0], buffer_err, sizeof(buffer_err))) > 0) {
            write(STDERR_FILENO, buffer_err, len);
        }

        close(fdOut[0]);
        close(fdErr[0]);

        wait(NULL);
        // printf("Child terminated\n");
	}

	return 0;
}
