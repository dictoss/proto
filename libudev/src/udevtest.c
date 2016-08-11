/************
 * device scan program used libudev and evdev.

  http://www.signal11.us/oss/udev/
 ************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <libudev.h>


/* function */
int udev_lookup_device(const char *subsystem);

/*
 main
 */
int main(int argc, char *argv[])
{
    int exitcode = EXIT_SUCCESS;

    fprintf(stderr, "Test udev library.\n");

    /*
      found device

      ethernet
      wireless lan
      usb 3g modem      
     */
    
    udev_lookup_device("block");
    udev_lookup_device("net");
    udev_lookup_device("video4linux");
    udev_lookup_device("drm");
    udev_lookup_device("acpi");
    udev_lookup_device("tty");

    udev_lookup_device("usb");
    
	return exitcode;
}


int udev_lookup_device(const char *subsystem)
{
    struct udev *udev = NULL;
    struct udev_enumerate *enumerate = NULL;
    struct udev_list_entry *devices = NULL, *dev_list_entry = NULL;
    struct udev_device *dev = NULL;

    fprintf(stderr, "\nIN udev_lookup_device()\n");
    fprintf(stderr, "  subsystem: %s\n", subsystem);

    udev = udev_new();
    if (NULL == udev) {
        fprintf(stderr, "Can't create udev\n");
        return -1;
    }

    enumerate = udev_enumerate_new(udev);
    /* set scan subsystem */
    udev_enumerate_add_match_subsystem(enumerate, subsystem);    
    udev_enumerate_scan_devices(enumerate);    
    devices = udev_enumerate_get_list_entry(enumerate);

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char *path;

        /* Get the filename of the /sys entry for the device
           and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        /* usb_device_get_devnode() returns the path to the device node
           itself in /dev. */
        fprintf(stderr, "Device Node Path: %s\n", udev_device_get_devnode(dev));

        /* The device pointed to by dev contains information about
           the hidraw device. In order to get information about the
           USB device, get the parent device with the
           subsystem/devtype pair of "usb"/"usb_device". This will
           be several levels up the tree, but the function will find
           it.*/
        
        dev = udev_device_get_parent_with_subsystem_devtype(
            dev, "usb", "usb_device");
        if (!dev) {
            fprintf(stderr, "Unable to find parent usb device.\n");
            return -1;
        }
        
        /* From here, we can call get_sysattr_value() for each file
           in the device's /sys entry. The strings passed into these
           functions (idProduct, idVendor, serial, etc.) correspond
           directly to the files in the directory which represents
           the USB device. Note that USB strings are Unicode, UCS2
           encoded, but the strings returned from
           udev_device_get_sysattr_value() are UTF-8 encoded. */
        fprintf(stderr, "  VID/PID: %s %s\n",
               udev_device_get_sysattr_value(dev,"idVendor"),
               udev_device_get_sysattr_value(dev, "idProduct"));
        fprintf(stderr, "  %s\n  %s\n",
               udev_device_get_sysattr_value(dev,"manufacturer"),
               udev_device_get_sysattr_value(dev,"product"));
        fprintf(stderr, "  serial: %s\n",
               udev_device_get_sysattr_value(dev, "serial"));
        udev_device_unref(dev);
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    udev_unref(udev);

    fprintf(stderr, "scan done udev.\n");
    
    return 0;
}
