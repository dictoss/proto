/************
 * device scan program used libudev and evdev.

  http://www.signal11.us/oss/udev/
 ************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

#include <libudev.h>

#if !defined ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof( (x)[0]) )
#endif

#define ETH_NAME_LOOPBACK "lo"
#define UDEV_DEVTYPE_WIRELESS "wlan"


struct modem_info {
    char name[128];
    unsigned short venderid;
    unsigned short productid;
    char ttypath_at[128];
};


/* modem info */
struct modem_info g_modemlist[] = {
    {"L-05A", 0x1004, 0x6124, "/dev/ttyACM0"},
    {"L-02C", 0x1004, 0x618f, "/dev/ttyUSB2"}
};


/* function */
int udev_lookup_device(const char *subsystem);

int udev_lookup_device_3gmodem(const struct modem_info *modemlist,
                               size_t modemlist_count,
                               struct modem_info *result);

int udev_lookup_device_ethernet(char* eth, size_t eth_size);

/*
 main
 */
int main(int argc, char *argv[])
{
    int ret;
    int exitcode = EXIT_SUCCESS;
    struct modem_info modem;
    char eth[128];

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

    ret = udev_lookup_device_3gmodem(g_modemlist,
                                     ARRAY_SIZE(g_modemlist),
                                     &modem);
    if(0 < ret){
        fprintf(stderr, "found modem\n");
        fprintf(stderr, "  %s\n", modem.name);
    }
    else{
        fprintf(stderr, "no modem\n");
    }

    ret = udev_lookup_device_ethernet(eth, sizeof(eth));
    if(0 < ret){
        fprintf(stderr, "found ethernet (ret=%d)\n", ret);
        fprintf(stderr, "  %s\n", eth);
    }
    else{
        fprintf(stderr, "no ethernet\n");
    }
    
	return exitcode;
}


int udev_lookup_device_3gmodem(const struct modem_info *modemlist,
                               size_t modemlist_count,
                               struct modem_info *result)
{
    /* 3g modem is tty subsystem */
    struct udev *udev = NULL;
    struct udev_enumerate *enumerate = NULL;
    struct udev_list_entry *devices = NULL, *dev_list_entry = NULL;
    struct udev_device *dev = NULL;
    const char *subsystem = "tty";
    int i = 0;
    unsigned short vender = 0;
    unsigned short product = 0;
    int found_count = 0;

    fprintf(stderr, "\nIN udev_lookup_device_3gmodem()\n");

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

        /* get venderid and productid */
        vender = (unsigned short)strtol(
            udev_device_get_sysattr_value(dev,"idVendor"), NULL, 16);
        product = (unsigned short)strtol(
            udev_device_get_sysattr_value(dev, "idProduct"), NULL, 16);
        fprintf(stderr, "DEBUG:  %x:%x\n", vender, product);

        for(i = 0; i < modemlist_count; i ++){
            if((modemlist[i].venderid == vender) && (modemlist[i].productid == product)){
                fprintf(stderr, "id match!!\n");
                *result = modemlist[i];
                found_count ++;
                break;
            }
        }

        udev_device_unref(dev);
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    udev_unref(udev);

    fprintf(stderr, "scan done udev.\n");

    return found_count;
}


int udev_lookup_device_ethernet(char* eth, size_t eth_size)
{
    /* ethernet is net subsystem */
    struct udev *udev = NULL;
    struct udev_enumerate *enumerate = NULL;
    struct udev_list_entry *devices = NULL, *dev_list_entry = NULL;
    struct udev_device *dev = NULL;
    const char *subsystem = "net";
    int found_count = 0;

    fprintf(stderr, "\nIN udev_lookup_device_ethernet()\n");

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
        const char *path, *sysname;
        const char *devtype;

        /* Get the filename of the /sys entry for the device
           and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        sysname = udev_device_get_sysname(dev);

        // wireless device return "wlan"
        devtype = udev_device_get_devtype(dev);

        fprintf(stderr, "Device Path: %s\n", path);
        fprintf(stderr, "Device SysName: %s\n", sysname);
        fprintf(stderr, "Device Type: %s\n", devtype);

        /* exclude loopback */
        if(0 == strcmp(sysname, ETH_NAME_LOOPBACK)){
            fprintf(stderr, "not wired ethernet(lo)\n");
            continue;
        }

        /* exclude wireless */
        if(devtype && (0 == strcmp(devtype, UDEV_DEVTYPE_WIRELESS))){
            fprintf(stderr, "not wired ethernet(wireless)\n");
            continue;
        }

        strcpy(eth, sysname);
        found_count ++;
        fprintf(stderr, " Ethernet Device: %s\n", eth);
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    udev_unref(udev);

    fprintf(stderr, "scan done udev.\n");

    return found_count;
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
