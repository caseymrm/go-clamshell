#ifndef __CLAMSHELL_H__
#define __CLAMSHELL_H__

#include <stdbool.h>

// Returns true if the lid is currently closed.
bool clamshell_state(void);

// Registers a general-interest notification on IOPMrootDomain so that
// kIOPMMessageClamshellStateChange messages are delivered. Adds the notify
// port's CFRunLoopSource to the current run loop. Must be paired with a
// running CFRunLoop (e.g. via clamshell_run_main_loop or an NSApplication).
void clamshell_subscribe(void);

// Runs CFRunLoopRun forever. Use only if you don't have your own run loop.
void clamshell_run_main_loop(void);

#endif
