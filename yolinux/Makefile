CC 		= gcc
FLAGS	= -Wall -Wextra -Werror
SANIT 	= -fsanitize=thread
DBG   	= -g3
THREAD	= -pthread

all:
	$(CC) $(FLAGS) main.c -o main

sanit:
	$(CC) $(FLAGS) $(SANIT) race.c -o race

run:
	./main

%: %.c
	$(CC) $(FLAGS) $(THREAD) $< -o $@ 

clean: 
	rm main