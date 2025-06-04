#include "serial_ports.hpp"

#ifdef _WIN32
    #include "serial_ports_windows.hpp"
#elif defined(__APPLE__)
    #include "serial_ports_mac.hpp"
#else
    #include "serial_ports_linux.hpp"
#endif

namespace RoboTact::SerialPort {

std::vector<SerialPortInfo> list_serial_ports()
{
#ifdef _WIN32
    return list_serial_ports_windows();
#elif defined(__APPLE__)
    return list_serial_ports_mac();
#else
    return list_serial_ports_linux();
#endif
}

} // namespace RoboTact::SerialPort