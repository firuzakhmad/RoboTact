#include "../core/assert.hpp"

#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>

#include <vector>
#include <string>

#include "serial_ports.hpp"

namespace RoboTact::SerialPort 
{
std::vector<SerialPortInfo> list_serial_ports_mac() 
{
    std::vector<SerialPortInfo> ports;

    CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOSerialBSDServiceValue);
    RA_ASSERT(matchingDict && "Failed to create IOService matching dictionary");

    io_iterator_t iter = 0;
    kern_return_t result = IOServiceGetMatchingServices(kIOMainPortDefault, matchingDict, &iter);
    RA_ASSERT(result == KERN_SUCCESS, "IOServiceGetMatchingServices failed with error: ", result);

    io_object_t service = 0;
    while ((service = IOIteratorNext(iter))) 
    {
        CFStringRef device_path = static_cast<CFStringRef>(IORegistryEntryCreateCFProperty(
            service,
            CFSTR(kIOCalloutDeviceKey),
            kCFAllocatorDefault,
            0
        ));

        if (!device_path) 
        {
            LOG_WARNING("Skipping device: no callout path");
            IOObjectRelease(service);
            continue;
        }

        char path[PATH_MAX];
        if (CFStringGetCString(device_path, path, PATH_MAX, kCFStringEncodingUTF8)) 
        {
            SerialPortInfo info;
            info.device_path = path;
            info.is_usb = true; // Optional guess for macOS
            ports.emplace_back(std::move(info));
        } 
        else 
        {
            LOG_ERROR("Failed to convert CFString to C-string");
        }

        CFRelease(device_path);
        IOObjectRelease(service);
    }

    IOObjectRelease(iter);
    return ports;
}
} // namespace RoboTact::SerialPort
