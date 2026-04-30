echo "test 1: test_main.c"
gcc -O3 test_main.c -o test_main.out ../agp.c
./test_main.out

echo " "
echo "test 2: ./test_main -lol lmao"
gcc -O3 test_main.c -o test_main.out ../agp.c
./test_main.out -lol lmao
