/**************************************************************
 *
 * Startup code for the Quartz Darwin X Server
 *
 **************************************************************/
/*
 * Copyright (c) 2001-2003 Torrey T. Lyons. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE ABOVE LISTED COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name(s) of the above copyright
 * holders shall not be used in advertising or otherwise to promote the sale,
 * use or other dealings in this Software without prior written authorization.
 */
/* $XFree86: xc/programs/Xserver/hw/darwin/quartz/quartzStartup.c,v 1.2 2003/01/15 02:34:06 torrey Exp $ */

#include <fcntl.h>
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>
#include "quartzCommon.h"
#include "darwin.h"
#include "opaque.h"
#include "micmap.h"

int NSApplicationMain(int argc, char *argv[]);

char **envpGlobal;      // argcGlobal and argvGlobal
                        // are from dix/globals.c

// GLX bundle function pointers
typedef void (*GlxExtensionInitPtr)(void); 
static GlxExtensionInitPtr GlxExtensionInit = NULL;

typedef void (*GlxWrapInitVisualsPtr)(miInitVisualsProcPtr *);
static GlxWrapInitVisualsPtr GlxWrapInitVisuals = NULL;


/*
 * DarwinHandleGUI
 *  This function is called first from main(). The first time
 *  it is called we start the Mac OS X front end. The front end
 *  will call main() again from another thread to run the X
 *  server. On the second call this function loads the user
 *  preferences set by the Mac OS X front end.
 */
void DarwinHandleGUI(
    int         argc,
    char        *argv[],
    char        *envp[] )
{
    static Bool been_here = FALSE;
    int         main_exit, i;
    int         fd[2];

    if (been_here) {
        QuartzReadPreferences();
        return;
    }
    been_here = TRUE;

    // Make a pipe to pass events
    assert( pipe(fd) == 0 );
    darwinEventFD = fd[0];
    quartzEventWriteFD = fd[1];
    fcntl(darwinEventFD, F_SETFL, O_NONBLOCK);

    // Store command line arguments to pass back to main()
    argcGlobal = argc;
    argvGlobal = argv;
    envpGlobal = envp;

    // Determine if we need to start X clients
    // and what display mode to use
    quartzStartClients = 1;
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-nostartx")) {
            quartzStartClients = 0;    
        } else if (!strcmp( argv[i], "-fullscreen")) {
            quartzRootless = 0;
        } else if (!strcmp( argv[i], "-rootless")) {
            quartzRootless = 1;
        }
    }

    quartz = TRUE;
    main_exit = NSApplicationMain(argc, argv);
    exit(main_exit);
}


/*
 * LoadGlxBundle
 *  The Quartz mode X server needs to dynamically load the appropriate
 *  bundle before initializing GLX.
 */
static void LoadGlxBundle(void)
{
    CFBundleRef mainBundle;
    CFStringRef bundleName;
    CFURLRef    bundleURL;
    CFBundleRef glxBundle;

    // Get the main bundle for the application
    mainBundle = CFBundleGetMainBundle();

    // Choose the bundle to load
    ErrorF("Loading GLX bundle ");
    if (quartzUseAGL) {
        bundleName = CFSTR("glxAGL.bundle");
        ErrorF("glxAGL.bundle (using Apple's OpenGL)\n");
    } else {
        bundleName = CFSTR("glxMesa.bundle");
        ErrorF("glxMesa.bundle (using Mesa)\n");
    }

    // Look for the appropriate GLX bundle in the main bundle by name
    bundleURL = CFBundleCopyResourceURL(mainBundle, bundleName,
                                        NULL, NULL);
    if (!bundleURL) {
        FatalError("Could not find GLX bundle.");
    }

    // Make a bundle instance using the URLRef
    glxBundle = CFBundleCreate(kCFAllocatorDefault, bundleURL);

    if (!CFBundleLoadExecutable(glxBundle)) {
        FatalError("Could not load GLX bundle.");
    }

    // Find the GLX init functions
    GlxExtensionInit = (void *) CFBundleGetFunctionPointerForName(
                                glxBundle, CFSTR("GlxExtensionInit"));

    GlxWrapInitVisuals = (void *) CFBundleGetFunctionPointerForName(
                                glxBundle, CFSTR("GlxWrapInitVisuals"));

    if (!GlxExtensionInit || !GlxWrapInitVisuals) {
        FatalError("Could not initialize GLX bundle.");
    }

    // Release the CF objects
    CFRelease(mainBundle);
    CFRelease(bundleURL);
}


/*
 * DarwinGlxExtensionInit
 *  Initialize the GLX extension.
 */
void DarwinGlxExtensionInit(void)
{
    if (!GlxExtensionInit)
        LoadGlxBundle();

    GlxExtensionInit();
}


/*
 * DarwinGlxWrapInitVisuals
 */
void DarwinGlxWrapInitVisuals(
    miInitVisualsProcPtr *procPtr)
{
    if (!GlxWrapInitVisuals)
        LoadGlxBundle();

    GlxWrapInitVisuals(procPtr);
}


int QuartzProcessArgument( int argc, char *argv[], int i )
{
    // fullscreen: CoreGraphics full-screen mode
    // rootless: Cocoa rootless mode
    // quartz: Default, either fullscreen or rootless

    if ( !strcmp( argv[i], "-fullscreen" ) ) {
        ErrorF( "Running full screen in parallel with Mac OS X Quartz window server.\n" );
#ifdef QUARTZ_SAFETY_DELAY
        ErrorF( "Quitting in %d seconds if no controller is found.\n",
                QUARTZ_SAFETY_DELAY );
#endif
        return 1;
    }

    if ( !strcmp( argv[i], "-rootless" ) ) {
        ErrorF( "Running rootless inside Mac OS X window server.\n" );
#ifdef QUARTZ_SAFETY_DELAY
        ErrorF( "Quitting in %d seconds if no controller is found.\n",
                QUARTZ_SAFETY_DELAY );
#endif
        return 1;
     }

    if ( !strcmp( argv[i], "-quartz" ) ) {
        ErrorF( "Running in parallel with Mac OS X Quartz window server.\n" );
#ifdef QUARTZ_SAFETY_DELAY
        ErrorF( "Quitting in %d seconds if no controller is found.\n",
                QUARTZ_SAFETY_DELAY );
#endif
        return 1;
    }

    // The Mac OS X front end uses this argument, which we just ignore here.
    if ( !strcmp( argv[i], "-nostartx" ) ) {
        return 1;
    }

    // This command line arg is passed when launched from the Aqua GUI.
    if ( !strncmp( argv[i], "-psn_", 5 ) ) {
        return 1;
    }

    return 0;
}