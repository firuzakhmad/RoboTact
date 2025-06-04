#include "../core/assert.hpp"

#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <initguid.h>
#include <tchar.h>
#include <vector>
#include <string>

#include "serial_ports.hpp"

// Required for USB device information
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);

namespace RoboTact::SerialPort 
{
std::vector<SerialPortInfo> list_serial_ports_windows() 
{
    std::vector<SerialPortInfo> ports;

    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(
        &GUID_DEVINTERFACE_COMPORT,
        nullptr,
        nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
    );
    RA_ASSERT(deviceInfoSet != INVALID_HANDLE_VALUE, "SetupDiGetClassDevs failed");

    SP_DEVINFO_DATA deviceInfoData = {0};
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); i++) 
    {
        SerialPortInfo info;
        
        // Get the port name (COMx)
        HKEY hDeviceKey = SetupDiOpenDevRegKey(
            deviceInfoSet,
            &deviceInfoData,
            DICS_FLAG_GLOBAL,
            0,
            DIREG_DEV,
            KEY_READ
        );
        
        if (hDeviceKey != INVALID_HANDLE_VALUE) 
        {
            TCHAR portName[256];
            DWORD size = sizeof(portName);
            DWORD type = 0;
            
            if (RegQueryValueEx(hDeviceKey, _T("PortName"), nullptr, &type, (LPBYTE)portName, &size) == ERROR_SUCCESS && type == REG_SZ) 
            {
                // Convert to std::string (assuming Unicode build)
                #ifdef UNICODE
                int size_needed = WideCharToMultiByte(CP_UTF8, 0, portName, -1, nullptr, 0, nullptr, nullptr);
                std::string portNameStr(size_needed, 0);
                WideCharToMultiByte(CP_UTF8, 0, portName, -1, &portNameStr[0], size_needed, nullptr, nullptr);
                portNameStr.pop_back(); // Remove null terminator
                #else
                std::string portNameStr(portName);
                #endif
                
                info.device_path = "\\\\.\\" + portNameStr;
            }
            RegCloseKey(hDeviceKey);
        }

        // Get friendly name
        TCHAR friendlyName[256];
        if (SetupDiGetDeviceRegistryProperty(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_FRIENDLYNAME,
            nullptr,
            (PBYTE)friendlyName,
            sizeof(friendlyName),
            nullptr)) 
        {
            #ifdef UNICODE
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, friendlyName, -1, nullptr, 0, nullptr, nullptr);
            std::string friendlyNameStr(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, friendlyName, -1, &friendlyNameStr[0], size_needed, nullptr, nullptr);
            friendlyNameStr.pop_back();
            #else
            std::string friendlyNameStr(friendlyName);
            #endif
            
            // Extract COM port from friendly name if we didn't get it earlier
            if (info.device_path.empty()) {
                size_t comPos = friendlyNameStr.find("(COM");
                if (comPos != std::string::npos) {
                    size_t endPos = friendlyNameStr.find(")", comPos);
                    if (endPos != std::string::npos) {
                        std::string comPort = friendlyNameStr.substr(comPos + 1, endPos - comPos - 1);
                        info.device_path = "\\\\.\\" + comPort;
                    }
                }
            }
        }

        // Get manufacturer and product info
        TCHAR manufacturer[256];
        if (SetupDiGetDeviceRegistryProperty(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_MFG,
            nullptr,
            (PBYTE)manufacturer,
            sizeof(manufacturer),
            nullptr)) 
        {
            #ifdef UNICODE
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, manufacturer, -1, nullptr, 0, nullptr, nullptr);
            std::string manufacturerStr(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, manufacturer, -1, &manufacturerStr[0], size_needed, nullptr, nullptr);
            manufacturerStr.pop_back();
            info.manufacturer = manufacturerStr;
            #else
            info.manufacturer = manufacturer;
            #endif
        }

        // Check if it's a Bluetooth device
        TCHAR service[256];
        if (SetupDiGetDeviceRegistryProperty(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_SERVICE,
            nullptr,
            (PBYTE)service,
            sizeof(service),
            nullptr)) 
        {
            #ifdef UNICODE
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, service, -1, nullptr, 0, nullptr, nullptr);
            std::string serviceStr(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, service, -1, &serviceStr[0], size_needed, nullptr, nullptr);
            serviceStr.pop_back();
            info.is_bluetooth = (serviceStr.find("BTH") != std::string::npos);
            #else
            info.is_bluetooth = (strstr(service, "BTH") != nullptr);
            #endif
        }

        // Assume USB if not Bluetooth (this is an approximation)
        info.is_usb = !info.is_bluetooth;

        // Get device instance ID for sysfs_path equivalent
        TCHAR instanceId[MAX_DEVICE_ID_LEN];
        if (CM_Get_Device_ID(deviceInfoData.DevInst, instanceId, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS) 
        {
            #ifdef UNICODE
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, instanceId, -1, nullptr, 0, nullptr, nullptr);
            std::string instanceIdStr(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, instanceId, -1, &instanceIdStr[0], size_needed, nullptr, nullptr);
            instanceIdStr.pop_back();
            info.sysfs_path = "\\\\?\\" + instanceIdStr;
            #else
            info.sysfs_path = "\\\\?\\" + std::string(instanceId);
            #endif
        }

        if (!info.device_path.empty()) {
            ports.emplace_back(std::move(info));
        }
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return ports;
}

} // namespace RoboTact::SerialPort