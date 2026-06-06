// Package clamshell reports MacBook lid state and pushes lid open/close
// events via an IOKit general-interest notification on IOPMrootDomain.
package clamshell

/*
#cgo LDFLAGS: -framework IOKit -framework CoreFoundation

#include <stdbool.h>
#include "clamshell.h"
*/
import "C"

// GetClamshellState returns true if the lid is currently closed.
func GetClamshellState() bool {
	return bool(C.clamshell_state())
}

// SubscribeClamshellChanges arranges for lid state changes to be delivered on
// the supplied channel. Events are not delivered until a CFRunLoop is running
// — call SubscribeClamshellChangesAndRun, run an NSApplication, or call
// CFRunLoopRun yourself. The bool sent on the channel is true when the lid
// has just closed and false when it has just opened.
//
// The current implementation supports a single subscriber; calling this
// twice replaces the previous channel.
func SubscribeClamshellChanges(channel chan<- bool) {
	setSubscriber(channel)
	C.clamshell_subscribe()
}

// SubscribeClamshellChangesAndRun is SubscribeClamshellChanges followed by
// running a CFRunLoop forever. Use only if you don't have your own run loop.
func SubscribeClamshellChangesAndRun(channel chan<- bool) {
	SubscribeClamshellChanges(channel)
	C.clamshell_run_main_loop()
}
