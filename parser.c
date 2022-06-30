#include "parser.h"

/*
	Reads input from standard input, picks out IO redirection tokens,
	and stores the remaining tokens in the args array.

	@argv - the array where we store our input tokens
	@argc - length of initialized argv array
	@input_fd - path to the input file specified for the current command
	@output_fd - path to the input file specified for the current command
	@append - boolean representing whether we should append to the end of the output
				file rather than overwrite it

	@returns - 0 if there was at least one token read from the input,
				indicating that there might be a valid command to carry out.
			   1 if either there was an error in the input, such that the next
			  	logical step is to prompt for input again. For example, there were
			  	there was invalid input or the input was a blank line.
*/
int read_input(char* argv[MAX_ARGC], int* argc, char* input_fd, char* output_fd, int* append) {

	// Stores our input from the commmand line
	char input_buffer[BUF_LEN];
	memset(input_buffer, 0, BUF_LEN);

	// Stores our input tokens before we sort out the IO redirection tokens
	char* tokens[MAX_ARGC];
	memset(tokens, 0, sizeof(char*)*MAX_ARGC);
	int num_tokens;

	get_tokens(input_buffer,tokens, &num_tokens);
	if (num_tokens == 0) {
		// Input was an empty line
		return 1;
	}

	memset(argv, 0, sizeof(char*)*MAX_ARGC);
	return get_args(tokens, num_tokens, argv, argc, input_fd, output_fd, append);
}

/*
	Populates the tokens array with input tokens. Does not filter out
	IO redirection tokens like <, >, >>.

	@tokens - an array of strings to hold the input tokens
	@num_tokens - pointer to an integer which stores how many tokens
					we have read
*/
void get_tokens(char input_buffer[], char** tokens, int* num_tokens) {

	if (read(STDIN_FILENO, input_buffer, BUF_LEN) == 0) {
		// If we can't read from STDIN, we should probably exit
		exit(1);
	}

	char* current_token = strtok(input_buffer, " \t");
	if (current_token[0] == '\n' || input_buffer[0] == '\0') {
		/* First, we check the edge case that we have no tokens
	       because the user entered a blank line of input. */
		*num_tokens = 0;
	} else {
		/* We pulled the first token out of the while loop
		   since we need to call strtok first with 0 for STDIN
		   and thenceforth with NULL */
		tokens[0] = current_token;
		int i = 1; // Used to iterate over input tokens
		while ((current_token = strtok(NULL, " \t"))) {
			tokens[i++] = current_token;
		}
		*num_tokens = i;

		// Remove the \n character from the last token
		size_t last_token_len = strlen(tokens[i-1]);
		tokens[i-1][last_token_len - 1] = '\0';
	}
}

/*
	Extracts the IO redirection symbols from the tokens

	@tokens - the array of tokens which may contain <, >, >>
	@args - the array with which we will populate with our non-IO redirection tokens
	@input_fd - path to the current command's input file
	@output_fd - path to the current command's output file
*/
int get_args(char* tokens[], int num_tokens, char* argv[], int* argc, char* input_fd, char* output_fd, int* append) {

	// Here we set the default values of input_fd and output_fd
	memset(input_fd, 0, sizeof(char)*MAX_PATH_LEN);
	memset(output_fd, 0, sizeof(char)*MAX_PATH_LEN);

	// Boolean values representing whether we have yet encountered
	// an IO redirection symbol of that type
	int input_redirected = 0;
	int output_redirected = 0;

	int j = 0; // Index for iterating over output args array

	for (int i = 0; i < num_tokens; i++) {

		if (strcmp(tokens[i], "<") == 0) {
			if (input_redirected) {
				printf("%s\n", "syntax error: multiple input files");
				return 1;
			}
			input_redirected = 1;
			if (i + 1 == num_tokens) {
				printf("%s\n", "syntax error: no input file");
				return 1;
			}
			strcpy(input_fd, tokens[++i]);

		} else if (strcmp(tokens[i], ">") == 0) {
			if (output_redirected) {
				printf("%s\n", "syntax error: multiple output files");
				return 1;
			}
			output_redirected = 1;
			*append = 0;
			if (i + 1 == num_tokens) {
				printf("%s\n", "syntax error: no output file");
				return 1;
			}
			strcpy(output_fd, tokens[++i]);

		} else if (strcmp(tokens[i], ">>") == 0) {
			if (output_redirected) {
				printf("syntax error: multiple output files");
				return 1;
			}
			output_redirected = 1;
			*append = 1;
			if (i + 1 == num_tokens) {
				printf("%s\n", "syntax error: no output file");
				return 1;
			}
			strcpy(output_fd, tokens[++i]);

		} else {
			argv[j++] = tokens[i];
		}
	}

	*argc = j;
	return 0;
}