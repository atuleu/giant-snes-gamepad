package main

// #cgo pkg-config: libusb-1.0
// #include <libusb.h>
import "C"

import (
	"fmt"
	"log"
	"reflect"
	"runtime"
	"unsafe"
)

type Gamepad struct {
	dev    *C.libusb_device
	handle *C.libusb_device_handle
}

func newGamepad(dev *C.libusb_device) (Gamepad, error) {
	var handle *C.libusb_device_handle

	if usbErr := C.libusb_open(dev, &handle); usbErr != 0 {
		return Gamepad{}, fmt.Errorf("libsusb_open() returned : %s", C.libusb_error_name(usbErr))
	}

	res := Gamepad{dev: dev, handle: handle}

	runtime.SetFinalizer(res, C.libusb_close(res.handle))

	return res, nil

}

func ListAllGamepads() ([]Gamepad, error) {
	var res []Gamepad

	var list **C.libusb_device
	deviceListSize := C.libusb_get_device_list(nil, &list)
	if deviceListSize < 0 {
		return nil, fmt.Errorf("libusb_get_Device_list() returned error : %s",
			C.libusb_error_name(C.int(deviceListSize)))
	}
	//defer list deallocation
	defer C.libusb_free_device_list(list, 0)

	var slice []*C.libusb_device
	sliceHeader := (*reflect.SliceHeader)((unsafe.Pointer(&slice)))
	sliceHeader.Cap = int(deviceListSize)
	sliceHeader.Len = int(deviceListSize)
	sliceHeader.Data = uintptr(unsafe.Pointer(list))

	for i := 0; i < int(deviceListSize); i++ {
		var desc C.struct_libusb_device_descriptor
		usbErr := C.libusb_get_device_descriptor(slice[i], &desc)
		if usbErr != 0 {
			return nil, fmt.Errorf("libusb_get_device_descriptor() returned error : ",
				C.libusb_error_name(usbErr))
		}

		if desc.idVendor != 0x03eb || desc.idProduct != 0x2040 || desc.bcdDevice != 0x6942 {
			log.Printf("Ignoring device {vid: 0x%04x pid: 0x%04x bcd: 0x%04x}\n",
				desc.idVendor, desc.idProduct, desc.bcdDevice)
			C.libusb_unref_device(slice[i])
			continue
		}

		newGamepad, err := newGamepad(slice[i])
		if err != nil {
			return res, err
		}

		res = append(res, newGamepad)

	}

	return res, nil
}

func (g *Gamepad) ReadAllCells() ([]int16, error) {
	return nil, fmt.Errorf("Not yet implemented")
}
