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

    typedef struct Clock
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

    typedef struct Display
    {
        bool MainLCD = false;
        bool SecondaryLCD = false;
        bool HDMI0 = false;
        bool Composite = false;
        bool HDMI1 = false;
    }Displays_t;



    // Getters

    float getTemperature();
    float getCpuVolts();
    Clocks_t getClocks();
    Displays_t getDisplays();
    float getCpuUsage();
    std::vector<float> getLoadAverage();
    std::vector<uint8_t> getVirtualMemory();
    std::vector<uint8_t> getDiskUsage(std::string disk_label);
    std::vector<uint8_t> getNetConnections();
    std::vector<uint8_t> getUsers();
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
