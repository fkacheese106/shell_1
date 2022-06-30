#include "executor.h"

/*
	Attempts to execute the command specified by the tokens in argv

	@argv - the tokens of the input command
	@argc - the length of argv, i.e. number of tokens
	@input_fd - file descriptor for input
	@output_fd - file descriptor for output
	@append - boolean representing whether to append or truncate the output
				is meaningless if output_fd is the empty string, i.e. we use STDOUT

	returns - 0 if we should again prompt for input in our main REPL, regardless
				of whether the command was successful
			  11 something happened such that we should exit the shell
*/
void execute(char** argv, int argc, char* input_fd, char* output_fd, int append) {

	// First we check for built-in commands
	if (strcmp(argv[0], "cd") == 0) {
		if (argc == 1) {
			fprintf(stderr, "%s\n", "cd: syntax error");
		} else {
			change_directory(argv[1]);
		}
		return;
	}

	if (strcmp(argv[0], "ln") == 0) {
		if (argc < 3) {
			fprintf(stderr, "%s\n", "ln: syntax error");
		} else {
			my_link(argv[1], argv[2]);
		}
		return;		
	}

	if (strcmp(argv[0], "rm") == 0) {
		if (argc == 1) {
			fprintf(stderr, "%s\n", "rm: syntax error");
		} else {
			my_remove(argv[1]);
		}
		return;
	}

	if (strcmp(argv[0], "exit") == 0) {
		exit(0);
	} 

	/* If the input is not one of the built-in commands, then we will try to parse
	it as a call to an executable */
	call_executable(argv, input_fd, output_fd, append);
}

/*
	Attempts to change the current directory to that specified by path

	@path - the path, absolute or relative, which we will attempt to change to
*/
void change_directory(char* path) {
	char abs_path[MAX_PATH_LEN];
	full_path(path, abs_path);
	if (chdir(abs_path) != 0) {
		perror("cd");
	}
}


/*
	Attempts to link the file/directory specified by existing to the address specified by new

	@existing - the path, absolute or relative, to the existing file / directory to be linked
	@new - the path/name of the new link to the file
*/
void my_link(char* existing, char* new) {
	char abs_existing[MAX_PATH_LEN];
	full_path(existing, abs_existing);
	char abs_new[MAX_PATH_LEN];
	full_path(new, abs_new);
	if (link(abs_existing, abs_new) != 0) {
		perror("ln");
	}
}


/*
	Attempts to remove the file/directory specified by path

	@path - path to the file/directory to be removed
*/
void my_remove(char* path) {
	char abs_path[MAX_PATH_LEN];
	full_path(path, abs_path);
	if (unlink(abs_path) != 0) {
		perror("rm");
		printf("%s\n", abs_path);
	}
}


/*
	Given a possibly relative path, determines the full path to it

	@in_path - a path, possibly relative

	returns - an absolute path, either by returning the exact input path
				or by appending "./" to the beginning of the input path
*/

void full_path(char* in_path, char* out_path) {
	if (in_path[0] == '/') {
		// We have a absolute path 
		strcpy(out_path, in_path);
	} else {
		// We will treat the input as a relative path
		out_path[0] = '.';
		out_path[1] = '/';
		strcpy(out_path + 2, in_path);
	}
}

/*
	Attempts to call the executable specified by argv[0]

	@argv - the input array from the command line
	@argc - the length of argv
	@input_fd - file descriptor for input
	@output_fd - file descriptor for output

*/
void call_executable(char** argv, char* input_fd, char* output_fd, int append) {
	char abs_path[MAX_PATH_LEN];
	full_path(argv[0], abs_path);

	// Logic for getting the last part of the input path
	/* We don't want to mutate the full path to the string, since
		we need to pass it into execv. Thus, we call strtok over
		a copy of the full path. */
	char copy_of_full_path[MAX_PATH_LEN];
	strcpy(copy_of_full_path, abs_path);
	// Call strtok until we get the last non-null token
	char* current_arg = strtok(copy_of_full_path, "/");
	char* next_arg;
	while ((next_arg = strtok(NULL, "/")) != NULL) {
		current_arg = next_arg;
	}
	argv[0] = current_arg;

	// Fork and execute the command
	if (fork() == 0) {
		// Try the IO redirection
		if (input_fd[0] != '\0') {
			if (close(0) == -1) {
				perror("close");
				exit(1);
			}
			if (open(input_fd, O_RDONLY | O_CREAT, S_IRWXU) == -1) {
				perror("open");
				exit(1);
			};
		}
		if (output_fd[0] != '\0') {
			if (close(1) == -1) {
				perror("close");
				exit(1);
			}
			if (append) {
				if (open(output_fd, O_WRONLY | O_CREAT, S_IRWXU) == -1) {
					perror("open");
					exit(1);
				}
				if (lseek(1, 0, SEEK_END) == -1) {
					perror("lseek");
					exit(1);
				}
			} else {
				if (open(output_fd, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU) == -1) {
					perror("open");
					exit(1);
				}
			}
		}

		execv(abs_path, argv);
		perror("exec.v");
		exit(1);
	}

	wait(NULL);
}