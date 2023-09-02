++ = g++
FLAGS = -g -Wall

all: Test

Test: ufs.o mylibc.o Test.o
	$(++) $(FLAGS) ufs.o mylibc.o Test.o -o Test
mylibc.o: mylibc.o
	$(++) $(FLAGS) -c mylibc.cpp
ufs.o: ufs.cpp
	$(++) $(FLAGS) -c ufs.cpp
Test.o: Test.cpp
	$(++) $(FLAGS) -c Test.cpp
.PHONY: clean all

clean:
	rm -f *.o mylibc ufs Test