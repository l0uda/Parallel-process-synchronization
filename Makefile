all:
	gcc -std=gnu99 -Wall -Wextra -Werror -pedantic main.c -o proj2 -pthread -lrt
clean:
	rm proj2 proj2.out
