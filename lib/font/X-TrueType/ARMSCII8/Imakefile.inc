XCOMM $XFree86$

#define ModuleName ARMSCII8
MODULENAME = ModuleName
MODULESRCDIR= $(XTTSOURCEDIR)/$(MODULENAME)

SRCS =	main.c ARMSCII8toUCS2.c
OBJS =	main.o ARMSCII8toUCS2.o

LinkSourceFile(main.c,$(MODULESRCDIR))
LinkSourceFile(ARMSCII8toUCS2.c,$(MODULESRCDIR))