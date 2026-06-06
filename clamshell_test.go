package clamshell

import "testing"

// TestGetClamshellState exercises the synchronous IOKit query path. It does
// not assert the value (we don't know whether the test machine is a laptop
// with the lid open or a desktop with no clamshell at all) — it only proves
// the IOKit framework is linked and the property lookup doesn't crash.
func TestGetClamshellState(t *testing.T) {
	closed := GetClamshellState()
	t.Logf("clamshell currently closed: %v", closed)
}
