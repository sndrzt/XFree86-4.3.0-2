XCOMM $XFree86$

#define ModuleName KSCJOHAB
MODULENAME = ModuleName
MODULESRCDIR= $(XTTSOURCEDIR)/$(MODULENAME)

SRCS =  main.c KSCJOHABtoUCS2.c
OBJS =  main.o KSCJOHABtoUCS2.o

LinkSourceFile(main.c,$(MODULESRCDIR))
LinkSourceFile(KSCJOHABtoUCS2.c,$(MODULESRCDIR))