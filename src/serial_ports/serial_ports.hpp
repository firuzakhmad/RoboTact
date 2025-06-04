#ifndef SERIAL_PORT_HPP
#define SERIAL_PORT_HPP

#include <vector>
#include <string>

namespace RoboTact::SerialPort 
{
	/*
	 * @brief Lists all available serial ports on the system
	 * @return Vector of strings with paths to available serial ports
	 * *throws std::runtime_error using custom assert if platform-specific 
	 * 	implementation fails
	 */
	struct SerialPortInfo 
	{
	    std::string device_path;    // e.g. "/dev/ttyUSB0"
	    std::string sysfs_path;     // e.g. "/sys/class/tty/ttyUSB0"
	    std::string driver;         // Driver name if available
	    std::string manufacturer;   // USB manufacturer if available
	    std::string product;        // USB product description if available
	    bool is_usb = false;
	    bool is_bluetooth = false;
	    bool is_serial_console = false;
	};

	// Platform-agnostic function
	std::vector<SerialPortInfo> list_serial_ports();
} // namespace RoboTact::SerialPort {

#endif // SERIAL_PORT_HPP
