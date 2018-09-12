PRG=gnu.exe
OSTYPE := $(shell uname)
ifeq ($(OSTYPE),Linux)
CYGWIN=
else
CYGWIN=-Wl,--enable-auto-import
endif

GCC=g++
GCCFLAGS=-O2 -Wall -Wextra -std=c++03 -pedantic -Wold-style-cast -Woverloaded-virtual -Wsign-promo  -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder
DEFINE=

VALGRIND_OPTIONS=-q --leak-check=full
DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines

OBJECTS0=
DRIVER1=main_part1.cpp
DRIVER2=main_part2.cpp
DRIVER3=main_part3.cpp
DRIVER4=main_part4.cpp

part1: $(DRIVER1)
	$(GCC)  $(DRIVER1) $(OBJECTS0) $(GCCFLAGS) $(DEFINE) -lpthread -o $@.exe
part2: $(DRIVER2)
	$(GCC)  $(DRIVER2) $(OBJECTS0) $(GCCFLAGS) $(DEFINE) -lpthread -o $@.exe
part3: $(DRIVER3)
	$(GCC)  $(DRIVER3) $(OBJECTS0) $(GCCFLAGS) $(DEFINE) -lpthread -o $@.exe
part4: $(DRIVER4)
	$(GCC)  $(DRIVER4) $(OBJECTS0) $(GCCFLAGS) $(DEFINE) -lpthread -o $@.exe
0 1:
	@echo "should run in less than 2000 ms"
	./$(PRG) init$@ 10 final$@ &>/dev/null
	sleep 1
	draw final$@ > student-picture$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff student-picture$@ picture$@ $(DIFF_OPTIONS)
clean:
	rm -f *.exe *.o *.obj
