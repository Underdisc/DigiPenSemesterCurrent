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

#=MICROSOFT COMPILER DEFINITION=================================================
prgx86="Program Files (x86)"
mvs="Microsoft Visual Studio"
winkit="Windows Kits"
vsdir=$(prgx86)/$(mvs)
versiondir=2017/Community/VC/Tools/MSVC/14.10.25017
bindir=$(vsdir)/$(versiondir)/bin/HostX86/x86

invoke_vsdir=/cygdrive/c/$(bindir)
invoke_cl=$(invoke_vsdir)/cl.exe
invoke_link=$(invoke_vsdir)/link.exe

vs_include=$(vsdir)/$(versiondir)/include
winkit_include=$(prgx86)/$(winkit)/10/Include/10.0.15063.0/ucrt
inc_a=/I/$(vs_include)
inc_b=/I/$(winkit_include)

vs_lib=C:/$(vsdir)/$(versiondir)/lib/x86
winkit_lib_um=C:/$(prgx86)/$(winkit)/10/Lib/10.0.15063.0/um/x86
winkit_lib_ucrt=C:/$(prgx86)/$(winkit)/10/Lib/10.0.15063.0/ucrt/x86
lib_a=/LIBPATH:$(vs_lib)
lib_b=/LIBPATH:$(winkit_lib_um)
lib_c=/LIBPATH:$(winkit_lib_ucrt)

clc=$(invoke_cl) /nologo /EHsc /c $(inc_a) $(inc_b)
cll=$(invoke_link) /NOLOGO $(lib_a) $(lib_b) $(lib_c)

#=BUILD=========================================================================
CC=$(clc)
LINK=$(cll)

CC_FLAGS=/I../inc/
LIBS=../lib/portaudio_x86.lib
SRCDIR=


OBJS=portaudio_output_demo.obj
EXE=test.exe

#=TARGETS=======================================================================

$(EXE) : $(OBJS) $(EXTERNALOBJS)
	$(GT)
	$(BOLD)
	@$(LINK) $(LIBS) $(OBJS) /OUT:$(EXE)
	@echo $(EXE)
	$(RESET)

%.obj : $(SRCDIR)%.cpp
	$(BT)
	$(BOLD)
	@$(CC) $(CC_FLAGS) $<
	$(RESET)

clean :
	$(RT)
	rm $(EXE) $(OBJS)
	$(RESET)
