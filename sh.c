#include "parser.c"
#include "executor.c"

int main() {

	char* argv[MAX_ARGC];			// Array to store our arguments when we parse
	int argc;						// How many arguments we have							
	int append;						// Boolean to distinguish between > (append = 0) and >> (append = 1) IO tokens
	int error;						// Integer to store whether our parsing / execution went smoothly		
	char input_fd[MAX_PATH_LEN];	// File descriptors for our command's input
	char output_fd[MAX_PATH_LEN];	// File descriptors for our command's output

	/* Our core REPL loop. Repeatedly follows the pattern below;
		1. Reads input from STDIN and tries to parse it as a command
		2. Attempts to execute that command

	   The value of error determines how we should proceed after parsing.
	   	error = 0 : parsing went smoothly, so we attempt to execute the input command
	   	error = 1 : parsing failed for some reason, so we prompt for input again
	*/
	while(1) {

		#ifdef PROMPT 
			if (printf("33sh> ") < 0) { 
			    // If we can't even write to STDOUT, then we should probably exit.
			    perror("printf");
			    exit(1);
			} 
			if (fflush(stdout) == EOF) {
				// If we can't even flush to STDOUT, then we should probably exit.
				perror("printf");
				exit(1);
			}
		#endif

		// Get input from STDIN
		if ((error = read_input(argv, &argc, input_fd, output_fd, &append)) == -1) {
			break; // Exit the shell
		} else if (error == 1) {
			continue; // Skip to getting input again
		}

		/* We have a command which we can attempt to execute. 
			Note that, unline with parsing, we don't need to
			condition on the result of the parsing, since we should
			prompt for input whether or not the current command is
			successful (assuming the current command is not "exit", 
			in which case we call exit(0) inside the function. */
		execute(argv, argc, input_fd, output_fd, append);
	}
	return 0;
}