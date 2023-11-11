CFLAGS = -lSDL2 -Wno-pointer-arith -g -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

IFLAGS = -I./headers/

all: a object/main.o object/stackfuncs.o object/textfuncs.o object/spu.o object/assembler.o object/utils.o # object/disassembler.o

a: object/main.o object/stackfuncs.o object/textfuncs.o object/spu.o object/assembler.o object/utils.o # object/disassembler.o
	g++ $(IFLAGS) $(CFLAGS) object/main.o object/stackfuncs.o object/textfuncs.o object/spu.o object/assembler.o object/utils.o # object/disassembler.o

object/main.o: src/main.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/main.cpp -o object/main.o

object/stackfuncs.o: src/stackfuncs.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/stackfuncs.cpp -o object/stackfuncs.o

object/textfuncs.o: src/textfuncs.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/textfuncs.cpp -o object/textfuncs.o

object/spu.o: src/spu.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/spu.cpp -o object/spu.o

object/assembler.o: src/assembler.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/assembler.cpp -o object/assembler.o

object/utils.o: src/utils.cpp
	g++ $(IFLAGS) $(CFLAGS) -c src/utils.cpp -o object/utils.o

# object/disassembler.o: src/disassembler.cpp
#	g++ $(IFLAGS) $(CFLAGS) -c src/disassembler.cpp -o object/utils.o

clean :
	rm a.out object/*.o 