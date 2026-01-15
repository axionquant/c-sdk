# Install dependencies on Ubuntu/Debian
# sudo apt-get install libcurl4-openssl-dev libcjson-dev

# Compile the library
gcc -c axion.c -o axion.o -lcurl -lcjson -fPIC
gcc -shared -o libaxion.so axion.o -lcurl -lcjson

# Compile your program
gcc -o test_program test_program.c -L. -laxion -lcurl -lcjson
