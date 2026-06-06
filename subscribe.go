package clamshell

/*
#include <stdbool.h>
*/
import "C"
import "sync"

var (
	subMu      sync.Mutex
	subscriber chan<- bool
)

func setSubscriber(ch chan<- bool) {
	subMu.Lock()
	subscriber = ch
	subMu.Unlock()
}

//export goClamshellChanged
func goClamshellChanged(closed C.bool) {
	subMu.Lock()
	ch := subscriber
	subMu.Unlock()
	if ch == nil {
		return
	}
	// Non-blocking send so a slow consumer never stalls the IOKit callback,
	// which runs on the CFRunLoop thread.
	select {
	case ch <- bool(closed):
	default:
	}
}
