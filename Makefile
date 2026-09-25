all:
	gcc -std=gnu99 -Wall -Wextra -Werror -pedantic main.c -o proj2 -pthread -lrt
	gcc doe.c -o doe
clean:
	rm proj2 doe proj2.out
