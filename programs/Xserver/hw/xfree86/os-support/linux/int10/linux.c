/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/linux/int10/linux.c,v 1.28 2002/09/16 18:06:14 eich Exp $ */
/*
 * linux specific part of the int10 module
 * Copyright 1999 Egbert Eich
 */
#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86_ansic.h"
#include "xf86Pci.h"
#include "compiler.h"
#define _INT10_PRIVATE
#include "xf86int10.h"
#ifdef __sparc__
#define DEV_MEM "/dev/fb"
#else
#define DEV_MEM "/dev/mem"
#endif
#ifndef XFree86LOADER
#include <sys/mman.h>
#ifndef MAP_FAILED
#define MAP_FAILED ((void *)-1)
#endif
#endif
#define ALLOC_ENTRIES(x) ((V_RAM / x) - 1)
#define SHMERRORPTR (pointer)(-1)

static int counter = 0;
static unsigned long int10Generation = 0;

static CARD8 read_b(xf86Int10InfoPtr pInt, int addr);
static CARD16 read_w(xf86Int10InfoPtr pInt, int addr);
static CARD32 read_l(xf86Int10InfoPtr pInt, int addr);
static void write_b(xf86Int10InfoPtr pInt, int addr, CARD8 val);
static void write_w(xf86Int10InfoPtr pInt, int addr, CARD16 val);
static void write_l(xf86Int10InfoPtr pInt, int addr, CARD32 val);

int10MemRec linuxMem = {
    read_b,
    read_w,
    read_l,
    write_b,
    write_w,
    write_l
};

typedef struct {
    int lowMem;
    int highMem;
    char* base;
    char* base_high;
    int screen;
    char* alloc;
} linuxInt10Priv;

#if defined DoSubModules

typedef enum {
    INT10_NOT_LOADED,
    INT10_LOADED_VM86,
    INT10_LOADED_X86EMU,
    INT10_LOAD_FAILED
} Int10LinuxSubModuleState;

static Int10LinuxSubModuleState loadedSubModule = INT10_NOT_LOADED;

static Int10LinuxSubModuleState int10LinuxLoadSubModule(ScrnInfoPtr pScrn);

#endif /* DoSubModules */

xf86Int10InfoPtr
xf86InitInt10(int entityIndex)
{
    return xf86ExtendedInitInt10(entityIndex, 0);
}

xf86Int10InfoPtr
xf86ExtendedInitInt10(int entityIndex, int Flags)
{
    xf86Int10InfoPtr pInt = NULL;
    CARD8 *bios_base;
    int screen;
    int fd;
    static void* vidMem = NULL;
    static void* sysMem = NULL;
    void* vMem = NULL;
    void *options = NULL;
    int low_mem;
    int high_mem = -1;
    char *base = SHMERRORPTR;
    char *base_high = SHMERRORPTR;
    int pagesize; 
    memType cs;
    legacyVGARec vga;
    xf86int10BiosLocation bios;
    Bool videoBiosMapped = FALSE;
    
    if (int10Generation != serverGeneration) {
	counter = 0;
	int10Generation = serverGeneration;
    }

    screen = (xf86FindScreenForEntity(entityIndex))->scrnIndex;

    options = xf86HandleInt10Options(xf86Screens[screen],entityIndex);

    if (int10skip(options)) {
	xfree(options);
	return NULL;
    }

#if defined DoSubModules
    if (loadedSubModule == INT10_NOT_LOADED) 
	loadedSubModule = int10LinuxLoadSubModule(xf86Screens[screen]);

    if (loadedSubModule == INT10_LOAD_FAILED)
	return NULL;
#endif

    if ((!vidMem) || (!sysMem)) {
	if ((fd = open(DEV_MEM, O_RDWR, 0)) >= 0) {
	    if (!sysMem) {
#ifdef DEBUG
		ErrorF("Mapping sys bios area\n");
#endif
		if ((sysMem = mmap((void *)(SYS_BIOS), BIOS_SIZE,
				   PROT_READ | PROT_WRITE | PROT_EXEC,
				   MAP_SHARED | MAP_FIXED, fd, SYS_BIOS))
		    == MAP_FAILED) {
		    xf86DrvMsg(screen, X_ERROR, "Cannot map SYS BIOS\n");
		    close(fd);
		    goto error0;
		}
	    }
	    if (!vidMem) {
#ifdef DEBUG
		ErrorF("Mapping VRAM area\n");
#endif
		if ((vidMem = mmap((void *)(V_RAM), VRAM_SIZE,
				   PROT_READ | PROT_WRITE | PROT_EXEC,
				   MAP_SHARED | MAP_FIXED, fd, V_RAM))
		    == MAP_FAILED) {
		    xf86DrvMsg(screen, X_ERROR, "Cannot map V_RAM\n");
		    close(fd);
		    goto error0;
		}
	    }
	    close(fd);
	} else {
	    xf86DrvMsg(screen, X_ERROR, "Cannot open %s\n", DEV_MEM);
	    goto error0;
	}
    }

    pInt = (xf86Int10InfoPtr)xnfcalloc(1, sizeof(xf86Int10InfoRec));
    pInt->scrnIndex = screen;
    pInt->entityIndex = entityIndex;
    if (!xf86Int10ExecSetup(pInt))
	goto error0;
    pInt->mem = &linuxMem;
    pagesize = getpagesize();
    pInt->private = (pointer)xnfcalloc(1, sizeof(linuxInt10Priv));
    ((linuxInt10Priv*)pInt->private)->screen = screen;
    ((linuxInt10Priv*)pInt->private)->alloc =
	(pointer)xnfcalloc(1, ALLOC_ENTRIES(pagesize));

    if (!xf86IsEntityPrimary(entityIndex)) {
#ifdef DEBUG
	ErrorF("Mapping high memory area\n");
#endif
	if ((high_mem = shmget(counter++, HIGH_MEM_SIZE,
			       IPC_CREAT | SHM_R | SHM_W)) == -1) {
	    if (errno == ENOSYS)
		xf86DrvMsg(screen, X_ERROR, "shmget error\n Please reconfigure"
			   " your kernel to include System V IPC support\n");
	    goto error1;
	}
    } else {
#ifdef DEBUG
	ErrorF("Mapping Video BIOS\n");
#endif
	videoBiosMapped = TRUE;
	if ((fd = open(DEV_MEM, O_RDWR, 0)) >= 0) {
	    if ((vMem = mmap((void *)(V_BIOS), SYS_BIOS - V_BIOS,
			     PROT_READ | PROT_WRITE | PROT_EXEC,
			     MAP_SHARED | MAP_FIXED, fd, V_BIOS))
		== MAP_FAILED) {
		xf86DrvMsg(screen, X_ERROR, "Cannot map V_BIOS\n");
		close(fd);
		goto error1;
	    }
	    close (fd);
	} else
	    goto error1;
    }
    ((linuxInt10Priv*)pInt->private)->highMem = high_mem;
    
#ifdef DEBUG
    ErrorF("Mapping 640kB area\n");
#endif
    if ((low_mem = shmget(counter++, V_RAM,
			      IPC_CREAT | SHM_R | SHM_W)) == -1)
	goto error2;

    ((linuxInt10Priv*)pInt->private)->lowMem = low_mem;
    base = shmat(low_mem, 0, 0);
    if (base == SHMERRORPTR) goto error4;
    ((linuxInt10Priv *)pInt->private)->base = base;
    if (high_mem > -1) {
	base_high = shmat(high_mem, 0, 0);
	if (base_high == SHMERRORPTR) goto error4;
	((linuxInt10Priv*)pInt->private)->base_high = base_high;
    } else
	((linuxInt10Priv*)pInt->private)->base_high = NULL;

    MapCurrentInt10(pInt);
    Int10Current = pInt;

#ifdef DEBUG
    ErrorF("Mapping int area\n");
#endif
    if (xf86ReadBIOS(0, 0, (unsigned char *)0, LOW_PAGE_SIZE) < 0) {
	xf86DrvMsg(screen, X_ERROR, "Cannot read int vect\n");
	goto error3;
    }
#ifdef DEBUG
    ErrorF("done\n");
#endif
    /*
     * Read in everything between V_BIOS and SYS_BIOS as some system BIOSes
     * have executable code there.  Note that xf86ReadBIOS() can only bring in
     * 64K bytes at a time.
     */
    if (!videoBiosMapped) {
	(void)memset((pointer)V_BIOS, 0, SYS_BIOS - V_BIOS);
#ifdef DEBUG
	ErrorF("Reading BIOS\n");
#endif
	for (cs = V_BIOS;  cs < SYS_BIOS;  cs += V_BIOS_SIZE)
	    if (xf86ReadBIOS(cs, 0, (pointer)cs, V_BIOS_SIZE) < V_BIOS_SIZE)
		xf86DrvMsg(screen, X_WARNING,
			   "Unable to retrieve all of segment 0x%06X.\n", cs);
#ifdef DEBUG
	ErrorF("done\n");
#endif
    }
    
    xf86int10ParseBiosLocation(options,&bios);

    if (xf86IsEntityPrimary(entityIndex) 
	&& !(initPrimary(options))) {
	if (bios.bus == BUS_ISA && bios.location.legacy) {
	    xf86DrvMsg(screen, X_CONFIG,
		       "Overriding BIOS location: 0x%lx\n",
		       bios.location.legacy);
	    cs = bios.location.legacy >> 4;
	    bios_base = (unsigned char *)(cs << 4);
	    if (!int10_check_bios(screen, cs, bios_base)) {
		xf86DrvMsg(screen, X_ERROR,
			   "No V_BIOS at specified address 0x%x\n",cs << 4);
		goto error3;
	    }
	} else {
	    if (bios.bus == BUS_PCI) {
		xf86DrvMsg(screen, X_WARNING,
			   "Option BiosLocation for primary device ignored: "
			   "It points to PCI.\n");
		xf86DrvMsg(screen, X_WARNING,
			   "You must set Option InitPrimary also\n");
	    }

	    cs = ((CARD16*)0)[(0x10<<1) + 1];

	    bios_base = (unsigned char *)(cs << 4);

	    if (!int10_check_bios(screen, cs, bios_base)) {
		cs = ((CARD16*)0)[(0x42 << 1) + 1];
		bios_base = (unsigned char *)(cs << 4);
		if (!int10_check_bios(screen, cs, bios_base)) {
		    cs = V_BIOS >> 4;
		    bios_base = (unsigned char *)(cs << 4);
		    if (!int10_check_bios(screen, cs, bios_base)) {
			xf86DrvMsg(screen, X_ERROR, "No V_BIOS found\n");
			goto error3;
		    }
		}
	    }
	}

	xf86DrvMsg(screen, X_INFO, "Primary V_BIOS segment is: 0x%x\n", cs);

	pInt->BIOSseg = cs;
	set_return_trap(pInt);
#ifdef _PC	
	pInt->Flags = Flags & (SET_BIOS_SCRATCH | RESTORE_BIOS_SCRATCH);
	if (! (pInt->Flags & SET_BIOS_SCRATCH))
	    pInt->Flags &= ~RESTORE_BIOS_SCRATCH;
  	xf86Int10SaveRestoreBIOSVars(pInt, TRUE);
#endif
    } else {
        EntityInfoPtr pEnt = xf86GetEntityInfo(pInt->entityIndex);
	BusType location_type;

	if (bios.bus != BUS_NONE) {
	    switch (location_type = bios.bus) {
	    case BUS_PCI:
		xf86DrvMsg(screen,X_CONFIG,"Overriding bios location: "
			   "PCI:%i:%i%i\n",bios.location.pci.bus,
			   bios.location.pci.dev,bios.location.pci.func);
		break;
	    case BUS_ISA:
		if (bios.location.legacy)
		    xf86DrvMsg(screen,X_CONFIG,"Overriding bios location: "
			       "Legacy:0x%x\n",bios.location.legacy);
		else
		    xf86DrvMsg(screen,X_CONFIG,"Overriding bios location: "
			       "Legacy\n");
		break;
	    default:
		break;
	    }
	} else
	    location_type = pEnt->location.type;

	switch (location_type) {
	case BUS_PCI:
	{
	    int pci_entity;
	    
	    if (bios.bus == BUS_PCI)
		pci_entity = xf86GetPciEntity(bios.location.pci.bus,
					      bios.location.pci.dev,
					      bios.location.pci.func);
	    else 
		pci_entity = pInt->entityIndex;
	    if (!mapPciRom(pci_entity, (unsigned char *)(V_BIOS))) {
	        xf86DrvMsg(screen, X_ERROR, "Cannot read V_BIOS\n");
		goto error3;
	    }
	    pInt->BIOSseg = V_BIOS >> 4;
	    break;
	}
	case BUS_ISA:
	    if (bios.bus == BUS_ISA && bios.location.legacy) {
		cs = bios.location.legacy >> 4;
		bios_base = (unsigned char *)(cs << 4);
		if (!int10_check_bios(screen, cs, bios_base)) {
		    xf86DrvMsg(screen,X_ERROR,"No V_BIOS found "
			       "on override address 0x%x\n",bios_base);
		    goto error3;
		}
	    } else {
		cs = ((CARD16*)0)[(0x10<<1)+1];
		bios_base = (unsigned char *)(cs << 4);
		
		if (!int10_check_bios(screen, cs, bios_base)) {
		    cs = ((CARD16*)0)[(0x42<<1)+1];
		    bios_base = (unsigned char *)(cs << 4);
		    if (!int10_check_bios(screen, cs, bios_base)) {
			cs = V_BIOS >> 4;
			bios_base = (unsigned char *)(cs << 4);
			if (!int10_check_bios(screen, cs, bios_base)) {
			    xf86DrvMsg(screen,X_ERROR,"No V_BIOS found\n");
			    goto error3;
			}
		    }
		}
	    }
	    xf86DrvMsg(screen,X_INFO,"Primary V_BIOS segment is: 0x%x\n",cs);
	    pInt->BIOSseg = cs;
	    break;
	default:
	    goto error3;
	}
	xfree(pEnt);
	pInt->num = 0xe6;
	reset_int_vect(pInt);
	set_return_trap(pInt);
	LockLegacyVGA(pInt, &vga);
	xf86ExecX86int10(pInt);
	UnlockLegacyVGA(pInt, &vga);
    }
#ifdef DEBUG
    dprint(0xc0000, 0x20);
#endif

    xfree(options);
    return pInt;

error4:
    xf86DrvMsg(screen, X_ERROR, "shmat() call retruned errno %d\n", errno);
error3:
    if (base_high)
	shmdt(base_high);
    shmdt(base);
    shmdt(0);
    if (base_high)
	shmdt((char*)HIGH_MEM);
    shmctl(low_mem, IPC_RMID, NULL);
    Int10Current = NULL;
error2:
    if (high_mem > -1)
	shmctl(high_mem, IPC_RMID,NULL);
error1:
    if (vMem)
	munmap(vMem, SYS_BIOS - V_BIOS);
    xfree(((linuxInt10Priv*)pInt->private)->alloc);
    xfree(pInt->private);
error0:
    xfree(options);
    xfree(pInt);
    return NULL;
}

Bool
MapCurrentInt10(xf86Int10InfoPtr pInt)
{
    pointer addr;
    int fd = -1;
    
    if (Int10Current) {
	shmdt(0);
	if (((linuxInt10Priv*)Int10Current->private)->highMem >= 0)
	    shmdt((char*)HIGH_MEM);
	else
	    munmap((pointer)V_BIOS, (SYS_BIOS - V_BIOS));
    }
    addr = shmat(((linuxInt10Priv*)pInt->private)->lowMem, (char*)1, SHM_RND);
    if (addr == SHMERRORPTR) {
	xf86DrvMsg(pInt->scrnIndex, X_ERROR, "Cannot shmat() low memory\n");
	return FALSE;
    }
    
    if (((linuxInt10Priv*)pInt->private)->highMem >= 0) {
	addr = shmat(((linuxInt10Priv*)pInt->private)->highMem,
		     (char*)HIGH_MEM, 0);
	if (addr == SHMERRORPTR) {
	    xf86DrvMsg(pInt->scrnIndex, X_ERROR,
		       "Cannot shmat() high memory\n");
	    return FALSE;
	}
    } else {
	if ((fd = open(DEV_MEM, O_RDWR, 0)) >= 0) {
	    if (mmap((void *)(V_BIOS), SYS_BIOS - V_BIOS,
			     PROT_READ | PROT_WRITE | PROT_EXEC,
			     MAP_SHARED | MAP_FIXED, fd, V_BIOS)
		== MAP_FAILED) {
		xf86DrvMsg(pInt->scrnIndex, X_ERROR, "Cannot map V_BIOS\n");
		close (fd);
		return FALSE;
	    }
	} else {
	    xf86DrvMsg(pInt->scrnIndex, X_ERROR, "Cannot open %s\n",DEV_MEM);
	    return FALSE;
	}
	close (fd);
    }
    
    return TRUE;
}

void
xf86FreeInt10(xf86Int10InfoPtr pInt)
{
    if (!pInt)
	return;

#ifdef _PC
    xf86Int10SaveRestoreBIOSVars(pInt, FALSE); 
#endif
    if (Int10Current == pInt) {
	shmdt(0);
	if (((linuxInt10Priv*)pInt->private)->highMem >= 0)
	    shmdt((char*)HIGH_MEM);
	else
	    munmap((pointer)V_BIOS, (SYS_BIOS - V_BIOS));
	Int10Current = NULL;
    }
    
    if (((linuxInt10Priv*)pInt->private)->base_high)
	shmdt(((linuxInt10Priv*)pInt->private)->base_high);
    shmdt(((linuxInt10Priv*)pInt->private)->base);
    shmctl(((linuxInt10Priv*)pInt->private)->lowMem, IPC_RMID, NULL);
    if (((linuxInt10Priv*)pInt->private)->highMem >= 0)
	shmctl(((linuxInt10Priv*)pInt->private)->highMem, IPC_RMID, NULL);
    xfree(((linuxInt10Priv*)pInt->private)->alloc);
    xfree(pInt->private);
    xfree(pInt);
}

void *
xf86Int10AllocPages(xf86Int10InfoPtr pInt, int num, int *off)
{
    int pagesize = getpagesize();
    int num_pages = ALLOC_ENTRIES(pagesize);
    int i, j;

    for (i = 0; i < (num_pages - num); i++) {
	if (((linuxInt10Priv*)pInt->private)->alloc[i] == 0) {
	    for (j = i; j < (num + i); j++)
		if ((((linuxInt10Priv*)pInt->private)->alloc[j] != 0))
		    break;
	    if (j == (num + i))
		break;
	    else
		i = i + num;
	}
    }
    if (i == (num_pages - num))
	return NULL;

    for (j = i; j < (i + num); j++)
	((linuxInt10Priv*)pInt->private)->alloc[j] = 1;

    *off = (i + 1) * pagesize;

    return ((linuxInt10Priv*)pInt->private)->base + ((i + 1) * pagesize);
}

void
xf86Int10FreePages(xf86Int10InfoPtr pInt, void *pbase, int num)
{
    int pagesize = getpagesize();
    int first = (((unsigned long)pbase
		 - (unsigned long)((linuxInt10Priv*)pInt->private)->base)
	/ pagesize) - 1;
    int i;

    for (i = first; i < (first + num); i++)
	((linuxInt10Priv*)pInt->private)->alloc[i] = 0;
}

static CARD8
read_b(xf86Int10InfoPtr pInt, int addr)
{
    return *((CARD8 *)(memType)addr);
}

static CARD16
read_w(xf86Int10InfoPtr pInt, int addr)
{
    return *((CARD16 *)(memType)addr);
}

static CARD32
read_l(xf86Int10InfoPtr pInt, int addr)
{
    return *((CARD32 *)(memType)addr);
}

static void
write_b(xf86Int10InfoPtr pInt, int addr, CARD8 val)
{
    *((CARD8 *)(memType)addr) = val;
}

static void
write_w(xf86Int10InfoPtr pInt, int addr, CARD16 val)
{
    *((CARD16 *)(memType)addr) = val;
}

static
void write_l(xf86Int10InfoPtr pInt, int addr, CARD32 val)
{
    *((CARD32 *)(memType) addr) = val;
}

pointer
xf86int10Addr(xf86Int10InfoPtr pInt, CARD32 addr)
{
    if (addr < V_RAM)
	return ((linuxInt10Priv*)pInt->private)->base + addr;
    else if (addr < V_BIOS)
	return (pointer)(memType)addr;
    else if (addr < SYS_BIOS) {
	if (((linuxInt10Priv*)pInt->private)->base_high)
	    return (pointer)(((linuxInt10Priv*)pInt->private)->base_high
			     - V_BIOS + addr);
	else
	    return (pointer) (memType)addr;
    } else
	return (pointer) (memType)addr;
}

#if defined DoSubModules

static Bool
vm86_tst(void)
{
    int __res;

#ifdef __PIC__
    /* When compiling with -fPIC, we can't use asm constraint "b" because
       %ebx is already taken by gcc. */
    __asm__ __volatile__("pushl %%ebx\n\t"
			 "movl %2,%%ebx\n\t"
			 "movl %1,%%eax\n\t"
			 "int $0x80\n\t"
			 "popl %%ebx"
			 :"=a" (__res)
			 :"n" ((int)113), "r" (NULL));
#else
    __asm__ __volatile__("int $0x80\n\t"
			 :"=a" (__res):"a" ((int)113),
			 "b" ((struct vm86_struct *)NULL));
#endif

    if (__res < 0 && __res == -ENOSYS) 
	return FALSE;

    return TRUE;
}

static Int10LinuxSubModuleState
int10LinuxLoadSubModule(ScrnInfoPtr pScrn)
{
    if (vm86_tst()) {
	if (xf86LoadSubModule(pScrn,"vm86"))
	    return INT10_LOADED_VM86;
    } 
    if (xf86LoadSubModule(pScrn,"x86emu"))
	return INT10_LOADED_X86EMU;

    return INT10_LOAD_FAILED;
}

#endif /* DoSubModules */
