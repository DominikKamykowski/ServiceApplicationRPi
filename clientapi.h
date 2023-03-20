#ifndef CLIENTAPI_H
#define CLIENTAPI_H
#include<vector>
#include <iostream>

class ClientApi
{
public:
    ClientApi();
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

};

#endif // CLIENTAPI_H
