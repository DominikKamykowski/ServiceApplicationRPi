#ifndef CLIENTAPI_H
#define CLIENTAPI_H
#include<vector>
#include <iostream>
#include <ExternalLibs/json.hpp>

class ClientApi
{
public:
    ClientApi(std::string api_address, int port);
    ClientApi(std::string api_address);
    ~ClientApi();

    typedef struct Clocks_t
    {
        uint32_t ARM_cores = 0;
        uint32_t VC4 = 0;
        uint32_t ISP = 0;
        uint32_t block_3D = 0;
        uint32_t UART = 0;
        uint32_t PWM = 0;
        uint32_t EMMC = 0;
        uint32_t Pixel = 0;
        uint32_t AVE = 0;
        uint32_t HDMI = 0;
        uint32_t DPI = 0;
    }Clocks_t;

    typedef struct Displays_t
    {
        bool MainLCD = false;
        bool SecondaryLCD = false;
        bool HDMI0 = false;
        bool Composite = false;
        bool HDMI1 = false;
    }Displays_t;

    typedef struct LoadAvg_t
    {
        float L1 = 0;
        float L2 = 0;
        float L3 = 0;
    }LoadAvg_t;

    typedef struct VirtualMemory_t
    {

        uint64_t total = 0;       //: total physical memory (exclusive swap).
        uint64_t available = 0;   //: the memory that can be given instantly to processes without the system going into swap. This is calculated by summing different memory values depending on the platform and it is supposed to be used to monitor actual memory usage in a cross platform fashion.
        uint64_t used = 0;        //: memory used, calculated differently depending on the platform and designed for informational purposes only. total - free does not necessarily match used.
        uint64_t free = 0;        //: memory not being used at all (zeroed) that is readily available; note that this doesn’t reflect the actual memory available (use available instead). total - used does not necessarily match free.
        uint64_t active = 0;      //(UNIX): memory currently in use or very recently used, and so it is in RAM.
        uint64_t inactive = 0;    //(UNIX)memory that is marked as not used.
        uint64_t buffers = 0;     //(Linux, BSD): cache for things like file system metadata.
        uint64_t cached = 0;      //(Linux, BSD): cache for various things.
        uint64_t shared = 0;      //(Linux, BSD): memory that may be simultaneously accessed by multiple processes.
        uint64_t slab = 0;        //(Linux): in-kernel data structures cache.
        uint64_t wired = 0;       //(BSD, macOS): memory that is marked to always stay in RAM. It is never moved to disk.

    }VirtualMemory_t;

    typedef struct Users_t
    {

        std::string name = "";           //: the name of the user.
        std::string terminal = "";       //: the tty or pseudo-tty associated with the user, if any, else None.
        std::string host = "";           //: the host name associated with the entry, if any.
        uint64_t started = 0;            //: the creation time as a floating point number expressed in seconds since the epoch.
        uint32_t pid = 0;                //: the PID of the login process (like sshd, tmux, gdm-session-worker, …). On Windows and OpenBSD this is always set to None.

    }Users_t;

    // Getters

    float getTemperature();
    float getCpuVolts();
    Clocks_t getClocks();
    Displays_t getDisplays();
    float getCpuUsage();
    LoadAvg_t getLoadAverage();
    VirtualMemory_t getVirtualMemory();
    std::vector<uint8_t> getDiskUsage(std::string disk_label);
    std::vector<uint8_t> getNetConnections();
    Users_t getUsers();
    std::vector<uint8_t> getDateTime();



private:
    std::string apiAddress = "";
    int apiPort = 0;
    std::vector<uint8_t>parseReceiveData(std::string);
    std::vector<std::string> split(std::string, std::string);
    std::string httpApiAddress();
    nlohmann::json jsonFromVc(std::string);
    bool strToBool(std::string);

}


#define __temperature       "/mainteance/temperature"
#define __cpu_volts         "/mainteance/volts"
#define __clocks            "/mainteance/clock"
#define __displays          "/mainteance/display"
#define __cpu_usage         "/mainteance/cpu"
#define __load_average      "/mainteance/load"
#define __virtual_memory    "/mainteance/virtual_memory"
#define __disk_usage        "/mainteance/disk_usage"
#define __net_connections   "/mainteance/net_connections"
#define __users             "/mainteance/users"
#define __datetime          "/mainteance/detetime"


;

#endif // CLIENTAPI_H
