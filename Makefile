all:
	gcc api.c -o api.out
build:
	gcc api.c -o api.out -g
run:
	gcc api.c -o api.out
	./api.out
