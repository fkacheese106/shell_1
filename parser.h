#include <stdio.h>
#include<fcntl.h> 
#include<errno.h> 
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <stdlib.h>

#define BUF_LEN 1024
#define MAX_NUM_TOKENS 16
#define MAX_ARGC 16
#define MAX_PATH_LEN 1024

int read_input(char* args[MAX_ARGC], int* argc, char* input_fd, char* output_fd, int* append);
void get_tokens(char input_buffer[BUF_LEN], char** tokens, int* num_tokens);
int get_args(char** tokens, int num_tokens, char* argv[MAX_ARGC], int* argc, char* input_fd, char* output_fd, int* append);