
debug:
	${CC}  main.c list.c -DDEBUG -ggdb3 -O0 -Og -Wall -Wextra -o debug

release:
	${CC}  main.c list.c -O3 -Wall -Wextra -Werror -o release
