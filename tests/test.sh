echo "test 1: test_main.c"
gcc -O3 test_main.c -o test_main ../agp.c
./test_main

echo " "
echo "test 2: ./test_main -lol lmao"
gcc -O3 test_main.c -o test_main ../agp.c
./test_main -lol lmao
