GCC=g++
GCCFLAGS=-O3 -Wall -Wextra -std=c++11 -latomic -mcx16

VALGRIND_OPTIONS=-q --leak-check=full
DIFFLAGS=--strip-trailing-cr -y --suppress-common-lines

OBJECTS0=
DRIVER0=driver.cpp
PRG=gcc0.exe

OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

gcc0:
	$(GCC) -o $(PRG) $(CYGWIN) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS) -pthread
0 1 2 3:
	@echo "running test$@"
	./$(PRG) $@ >studentout$@
	diff out$@ studentout$@ $(DIFFLAGS)
4 5 6 7:
	@echo "running test$@"
	./$(PRG) $@ >studentout$@
	diff out$@ studentout$@ $(DIFFLAGS)
mem0 mem1:
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
clean:
	rm -f *.exe *.o *.obj
