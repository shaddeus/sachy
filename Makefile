#
#    Chess
#    Pavel Tvrdik, tvrdipa1@fel.cvut.cz
#    X36TJC, ZS 2010 - 2011
# 

CXX = g++
LD = g++
LIB = -lncurses
CFLAGS = -g3 -pg -O0 -Wall -pedantic $(LIB)
OBJS = chess.o dat_struct.o figurky.o main.o sach_global.o
OUT = chess.out

all: $(OUT)

$(OUT): $(OBJS)
	$(LD) $(CFLAGS) -o $(OUT) $(OBJS)
	#strip --strip-unneeded $(OUT)

chess.o: chess.cpp chess.h dat_struct.h figurky.h sach_global.h
	$(CXX) $(CFLAGS) -c -o chess.o chess.cpp

dat_struct.o: dat_struct.cpp dat_struct.h
	$(CXX) $(CFLAGS) -c -o dat_struct.o dat_struct.cpp

figurky.o: figurky.cpp figurky.h dat_struct.h sach_global.h chess.h
	$(CXX) $(CFLAGS) -c -o figurky.o figurky.cpp

main.o: main.cpp dat_struct.h figurky.h sach_global.h chess.h
	$(CXX) $(CFLAGS) -c -o main.o main.cpp

sach_global.o: sach_global.cpp sach_global.h dat_struct.h figurky.h chess.h
	$(CXX) $(CFLAGS) -c -o sach_global.o sach_global.cpp

valgrind: $(OUT)
	valgrind --log-file=log.valgrind --leak-check=full --tool=memcheck --time-stamp=yes --leak-resolution=high --num-callers=40 ./$(OUT)

clean:
	rm -f $(OBJS) $(OUT)

