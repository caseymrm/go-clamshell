#include "clamshell.h"

#include <dispatch/dispatch.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOMessage.h>
#include <IOKit/pwr_mgt/IOPM.h>

// goClamshellChanged is implemented in Go via cgo //export.
extern void goClamshellChanged(bool closed);

static io_service_t          gRootDomain   = MACH_PORT_NULL;
static IONotificationPortRef gNotifyPort   = NULL;
static io_object_t           gNotification = MACH_PORT_NULL;

static io_service_t copy_root_domain(void) {
	return IOServiceGetMatchingService(kIOMainPortDefault,
	                                   IOServiceMatching("IOPMrootDomain"));
}

bool clamshell_state(void) {
	io_service_t rd = copy_root_domain();
	if (rd == MACH_PORT_NULL) {
		return false;
	}
	CFTypeRef val = IORegistryEntryCreateCFProperty(
	    rd, CFSTR(kAppleClamshellStateKey), kCFAllocatorDefault, 0);
	IOObjectRelease(rd);
	if (val == NULL) {
		return false;
	}
	bool closed = false;
	if (CFGetTypeID(val) == CFBooleanGetTypeID()) {
		closed = CFBooleanGetValue((CFBooleanRef)val);
	}
	CFRelease(val);
	return closed;
}

static void clamshell_callback(void *refcon, io_service_t service,
                               natural_t messageType, void *messageArgument) {
	(void)refcon;
	(void)service;
	if (messageType != kIOPMMessageClamshellStateChange) {
		return;
	}
	uintptr_t arg    = (uintptr_t)messageArgument;
	bool      closed = (arg & kClamshellStateBit) != 0;
	goClamshellChanged(closed);
}

void clamshell_subscribe(void) {
	if (gNotifyPort != NULL) {
		return; // already subscribed
	}
	gRootDomain = copy_root_domain();
	if (gRootDomain == MACH_PORT_NULL) {
		return;
	}
	gNotifyPort = IONotificationPortCreate(kIOMainPortDefault);
	if (gNotifyPort == NULL) {
		IOObjectRelease(gRootDomain);
		gRootDomain = MACH_PORT_NULL;
		return;
	}
	// Deliver callbacks via the dispatch main queue so this works without the
	// caller owning the current thread's CFRunLoop — NSApplication and
	// dispatch_main both drive the main queue.
	IONotificationPortSetDispatchQueue(gNotifyPort, dispatch_get_main_queue());
	IOServiceAddInterestNotification(gNotifyPort, gRootDomain,
	                                 kIOGeneralInterest, clamshell_callback,
	                                 NULL, &gNotification);
}

void clamshell_run_main_loop(void) {
	dispatch_main();
}
