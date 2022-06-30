#include <stdio.h>
#include<fcntl.h> 
#include<errno.h> 
#include <string.h>
#include <unistd.h>
#include<sys/wait.h>
#include <stdlib.h>

#define MAX_PATH_LEN 1024

void execute(char** argv, int argc, char* input_fd, char* output_fd, int append);
void change_directory(char* path);
void my_link(char* existing, char* new);
void my_remove(char* path);
void full_path(char* in_path, char* out_path);
void call_executable(char** argv, char* input_fd, char* output_fd, int append);