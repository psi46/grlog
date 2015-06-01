# define compile command

CC = g++ -Wall -g
CFLAGS  =	-O3
LDFLAGS =	-s


grlog: grlog.o parser.o scanner.o ps.o error.o
	$(CC) $(LDFLAGS) -o grlog grlog.o parser.o scanner.o ps.o error.o

grlog.o: grlog.cpp scanner.h parser.h ps.h
	$(CC) $(CFLAGS)  -c -o grlog.o grlog.cpp

parser.o: parser.cpp parser.h
	$(CC) $(CFLAGS)  -c -o parser.o parser.cpp

scanner.o: scanner.cpp scanner.h
	$(CC) $(CFLAGS)  -c -o scanner.o scanner.cpp

ps.o: ps.cpp ps.h
	$(CC) $(CFLAGS)  -c -o ps.o ps.cpp

error.o: error.cpp error.h
	$(CC) $(CFLAGS)  -c -o error.o error.cpp


clean:
	rm -f *.o

