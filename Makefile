all:
	gcc api.c -o api.out
dev:
	gcc api.c -o api.out -g
run:
	gcc api.c -o api.out -g
	./api.out
