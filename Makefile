build:
	gcc -O3 -c agp.c -o bin/agp.out
	ar rcs bin/libagp.a bin/agp.out
	gcc -O3 -c -fPIC agp.c -o bin/agp.out
	gcc -shared -o bin/libagp.so bin/agp.out
dev:
	gcc -g -c agp.c -o bin/agp.out
	ar rcs bin/libagp.a bin/agp.out
	gcc -g -c -fPIC agp.c -o bin/agp.out
	gcc -shared -o bin/libagp.so bin/agp.out
run:
	gcc api.c -o api.out -g
	./api.out
