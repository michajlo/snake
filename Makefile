all:
	gcc -Wall -lpthread snake.c list.c -o snake

clean:
	rm -rf *.o snake
