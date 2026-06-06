# go-clamshell

Detect MacBook lid (clamshell) open/close events from Go, via IOKit push
notifications. No polling.

## Install

```sh
go get github.com/caseymrm/go-clamshell
```

## Usage

```go
package main

import (
    "fmt"

    "github.com/caseymrm/go-clamshell"
)

func main() {
    fmt.Printf("lid currently closed: %v\n", clamshell.GetClamshellState())

    ch := make(chan bool, 1)
    go func() {
        for closed := range ch {
            if closed {
                fmt.Println("lid closed")
            } else {
                fmt.Println("lid opened")
            }
        }
    }()
    clamshell.SubscribeClamshellChangesAndRun(ch)
}
```

If you already drive your own `CFRunLoop` (or are running an `NSApplication`,
e.g. via [menuet](https://github.com/caseymrm/menuet)), use
`SubscribeClamshellChanges` instead of the `…AndRun` variant.

## How it works

`IOServiceAddInterestNotification` on `IOPMrootDomain` with `kIOGeneralInterest`
delivers `kIOPMMessageClamshellStateChange` whenever
`AppleClamshellState` or `AppleClamshellCausesSleep` changes. Bit 0 of the
message argument is the closed/open state. See `<IOKit/pwr_mgt/IOPM.h>`.

## License

MIT.
