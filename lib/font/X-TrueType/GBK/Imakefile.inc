XCOMM $XFree86$

#define ModuleName GBK
MODULENAME = ModuleName
MODULESRCDIR = $(XTTSOURCEDIR)/$(MODULENAME)

SRCS =  main.c GBKtoUCS2.c
OBJS =  main.o GBKtoUCS2.o

LinkSourceFile(main.c,$(MODULESRCDIR))
LinkSourceFile(GBKtoUCS2.c,$(MODULESRCDIR))
