CC 		= gcc
FLAGS	= -Wall -Wextra -Werror
SANIT 	= -fsanitize=thread
DBG   	= -g3
THREAD	= -pthread

all:
	$(CC) $(FLAGS) $(DBG) $(THREAD) main.c -o main

sanit:
	$(CC) $(FLAGS) $(DBG) $(THREAD) $(SANIT) main.c -o main

run:
	./main arquivos_teste/teste1-conteudo.txt a

%: %.c
	$(CC) $(FLAGS) $(THREAD) $< -o $@ 

clean: 
	rm main