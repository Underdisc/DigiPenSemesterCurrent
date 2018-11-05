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

all : bitrev.exe genrandoms.exe dft.exe fft.exe fft2.exe 

bitrev.exe : ../src/Complex.cc ../src/utility.cc ../src/main_bitrev.cc
	$(BT)
	$(BOLD)
	$(cc) ../src/Complex.cc ../src/utility.cc ../src/main_bitrev.cc -o bitrev.exe
	$(RESET)


genrandoms.exe : ../src/Complex.cc ../src/utility.cc ../src/main_genrandoms.cc
	$(BT)
	$(BOLD)
	$(cc) ../src/Complex.cc ../src/utility.cc ../src/main_genrandoms.cc -o genrandoms.exe
	$(RESET)

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

fft2.exe : ../src/Complex.cc ../src/utility.cc ../src/main_fft2.cc
	$(BT)
	$(BOLD)
	$(cc) ../src/Complex.cc ../src/utility.cc ../src/main_fft2.cc -o fft2.exe
	$(RESET)

clean :
	$(RT)
	rm *.exe 
	$(RESET)
