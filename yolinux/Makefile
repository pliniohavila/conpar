CC 		= gcc
FLAGS	= -Wall -Wextra -Werror
SANIT 	= -fsanitize=address
DBG   	= -g3
THREAD	= -pthread

all:
	$(CC) $(FLAGS) main.c -o main

sanit:
	$(CC) $(FLAGS) $(SANIT) main.c -o main

run:
	./main

%: %.c
	$(CC) $(FLAGS) $(THREAD) $< -o $@ 

clean: 
	rm main