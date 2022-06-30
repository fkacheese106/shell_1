CFLAGS = -g3 -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align -g
CFLAGS += -Winline -Wfloat-equal -Wnested-externs
CFLAGS += -pedantic -std=gnu99 -Werror

PROMPT = -DPROMPT
CC = gcc
EXECS = 33sh 33noprompt
C_FILES = sh.c parser.c parser.h executor.c executor.h

.PHONY: all clean

.SILENT:

33sh: $(C_FILES)	
	#TODO: compile your program, including the -DPROMPT macro
	$(CC) $(CFLAGS) $< -o $@ $(PROMPT)
33noprompt: $(C_FILES)
	#TODO: compile your program without the prompt macro
	$(CC) $(CFLAGS) $< -o $@
clean:
	#TODO: clean up any executable files that this Makefile has produced
	rm -f $(EXECS)

all: $(EXECS)

