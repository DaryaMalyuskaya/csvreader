all: csvreader

csvreader: main.o table.o parse.o evaluate.o util.o
	gcc -Wall -Wextra -std=c11 -O2 -o csvreader main.o table.o parse.o evaluate.o util.o

main.o:  main.c  table.h parse.h evaluate.h
table.o: table.c table.h util.h
parse.o: parse.c parse.h table.h util.h
evaluate.o: evaluate.c evaluate.h table.h util.h
util.o:  util.c  util.h

clean:
	rm -f csvreader main.o table.o parse.o evaluate.o util.o