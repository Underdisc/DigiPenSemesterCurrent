#=COLOUR========================================================================

#-TEXT--------------------------------------------------------------------------
RT=@tput setaf 1
GT=@tput setaf 2
YT=@tput setaf 3
BT=@tput setaf 4
PT=@tput setaf 5
CT=@tput setaf 6
WT=@tput setaf 7

#-BACKGROUND--------------------------------------------------------------------
RB=@tput setab 1
GB=@tput setab 2
YB=@tput setab 3
BB=@tput setab 4
PB=@tput setab 5
CB=@tput setab 6
WB=@tput setab 7

#-STYLE-------------------------------------------------------------------------
BOLD=@tput bold
UNDERLINE=@tput smul
RESET=@tput sgr0

cc=g++

all : fft.exe dft.exe

dft.exe : ../src/Complex.cc ../src/utility.cc ../src/main_dft.cc
	$(BT)
	$(BOLD)
	$(cc) ../src/Complex.cc ../src/utility.cc ../src/main_dft.cc -o dft.exe
	$(RESET)

fft.exe : ../src/Complex.cc ../src/utility.cc ../src/main_fft.cc
	$(BT)
	$(BOLD)
	$(cc) ../src/Complex.cc ../src/utility.cc ../src/main_fft.cc -o fft.exe
	$(RESET)

clean :
	$(RT)
	rm *.exe 
	$(RESET)
