CC 		= gcc
FLAGS	= -Wall -Wextra -Werror
SANIT 	= -fsanitize=address
DBG   	= -g3
THREAD	= -pthread

all:
	$(CC) $(FLAGS) $(THREAD) main.c -o main

sanit:
	$(CC) $(FLAGS) $(SANIT) main.c -o main

run:
	./main ./arquivos_test/teste1-conteudo.txt a

%: %.c
	$(CC) $(FLAGS) $(THREAD) $< -o $@ 

clean: 
	rm main