
IFLAGS = -I./headers/

all: a object/main.o object/stackfuncs.o object/textfuncs.o object/spu.o object/assembler.o

a: object/main.o object/stackfuncs.o object/textfuncs.o object/spu.o object/assembler.o
	g++ object/main.o object/stackfuncs.o object/textfuncs.o object/spu.o object/assembler.o -g

object/main.o: src/main.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/main.cpp -o object/main.o -g

object/stackfuncs.o: src/stackfuncs.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/stackfuncs.cpp -o object/stackfuncs.o -g 

object/textfuncs.o: src/textfuncs.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/textfuncs.cpp -o object/textfuncs.o -g 

object/spu.o: src/spu.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/spu.cpp -o object/spu.o -g 

object/assembler.o: src/assembler.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/assembler.cpp -o object/assembler.o -g 

	