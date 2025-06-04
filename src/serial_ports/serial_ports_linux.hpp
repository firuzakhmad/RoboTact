#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <system_error>
#include "../core/assert.hpp"

namespace RoboTact::SerialPort {

namespace {
    bool is_serial_device(const std::string& path) {
        struct stat st;
        if (stat(path.c_str(), &st) != 0) return false;
        if (!S_ISCHR(st.st_mode)) return false;

        int fd = open(path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd < 0) return false;

        // Verify it's actually a serial port
        struct termios tty;
        bool is_serial = (tcgetattr(fd, &tty) == 0);
        close(fd);
        return is_serial;
    }

    std::string read_sysfs_file(const std::string& path) {
        std::ifstream file(path);
        if (!file) return "";
        std::string content;
        std::getline(file, content);
        return content;
    }

    void populate_metadata(SerialPortInfo& info) {
        // Extract driver information
        info.driver = read_sysfs_file(info.sysfs_path + "/device/driver/module/driver");

        // Check for USB devices
        std::string uevent_path = info.sysfs_path + "/device/../uevent";
        std::string uevent_content = read_sysfs_file(uevent_path);

        if (uevent_content.find("DRIVER=usb") != std::string::npos) {
            info.is_usb = true;
            info.manufacturer = read_sysfs_file(info.sysfs_path + "/device/../manufacturer");
            info.product = read_sysfs_file(info.sysfs_path + "/device/../product");
        }

        // Check for Bluetooth devices
        if (info.device_path.find("rfcomm") != std::string::npos) {
            info.is_bluetooth = true;
        }

        // Check for serial console
        info.is_serial_console = (info.device_path == "/dev/console" || 
                                info.device_path == "/dev/ttyS0" || 
                                info.device_path == "/dev/ttyAMA0");
    }
}

std::vector<SerialPortInfo> list_serial_ports_linux() {
    std::vector<SerialPortInfo> ports;
    const std::string sysfs_tty_path = "/sys/class/tty";

    DIR* dir = opendir(sysfs_tty_path.c_str());
    if (!dir) {
        throw std::system_error(errno, std::system_category(), 
                              "Failed to open " + sysfs_tty_path);
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] == '.') continue;

        SerialPortInfo info;
        info.sysfs_path = sysfs_tty_path + "/" + entry->d_name;
        info.device_path = "/dev/" + std::string(entry->d_name);

        if (!is_serial_device(info.device_path)) continue;

        try {
            populate_metadata(info);
            ports.push_back(std::move(info));
        } catch (...) {
            // Skip devices with metadata errors
            continue;
        }
    }

    closedir(dir);
    return ports;
}
} // namespace RoboTact::SerialPort