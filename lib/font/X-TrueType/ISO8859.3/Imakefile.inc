XCOMM $XFree86$

#define ModuleName ISO8859_3
MODULENAME = ModuleName
MODULESRCDIR= $(XTTSOURCEDIR)/ISO8859.3

SRCS =  main.c ISO8859_3toUCS2.c
OBJS =  main.o ISO8859_3toUCS2.o

LinkSourceFile(main.c,$(MODULESRCDIR))
LinkSourceFile(ISO8859_3toUCS2.c,$(MODULESRCDIR))