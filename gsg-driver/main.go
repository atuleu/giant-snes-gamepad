package main

// #cgo pkg-config: libusb-1.0
// #include <libusb.h>
import "C"

import "fmt"

func Run() error {
	usbErr := C.libusb_init(nil)
	if usbErr != 0 {
		return fmt.Errorf("libusb_init() returned error : %s", C.libusb_error_name(usbErr))
	}
	defer C.libusb_exit(nil)

	gamepads, err := ListAllGamepads()
	if err != nil {
		return err
	}

	if len(gamepads) == 0 {
		return fmt.Errorf("No Giant Snes Gamepad device found")
	}

	return nil
}

func main() {
	err := Run()
	if err != nil {
		fmt.Println("Got error : ", err)
	}
}
