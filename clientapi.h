#ifndef CLIENTAPI_H
#define CLIENTAPI_H
#include<vector>
#include <iostream>

class ClientApi
{
public:
    ClientApi(std::string api_address, int port);
    ClientApi(std::string api_address);
    ~ClientApi();

    typedef struct Mainteance_t
    {
        float temperature;
        float cpu_volts;

    }Mainteance;

    // Getters

    std::vector<uint8_t> getTemperature();
    std::vector<uint8_t> getCpuVolts();
    std::vector<uint8_t> getClocks();
    std::vector<uint8_t> getDisplays();
    std::vector<uint8_t> getCpuUsage();
    std::vector<uint8_t> getLoadAverage();
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
