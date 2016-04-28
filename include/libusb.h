/*
 * libusb pseudo-header
 *
 * libusb's API header is "usb.h", which is too generic a name
 * to include blindly. This header is filled in with the full
 * path at configure time and various apcupsd bits include this
 * when they need libusb's usb.h.
 */
#include "/usr/include/usb.h"
