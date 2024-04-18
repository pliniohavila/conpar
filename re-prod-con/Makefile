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

run1:
	./main arquivos_teste/teste2-conteudo.txt Ipsum

run2:
	./main arquivos_teste/teste3-conteudo.txt Jude

%: %.c
	$(CC) $(FLAGS) $(THREAD) $< -o $@ 

clean: 
	rm main