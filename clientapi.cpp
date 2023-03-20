#include "clientapi.h"
#include <http.hpp>

ClientApi::ClientApi(std::string api_address)
{
    apiAddress = "http://" + api_address;
}

ClientApi::~ClientApi()
{

}

std::vector<uint8_t> ClientApi::getTemperature()
{
    return parseReceiveData(__temperature);
}

std::vector<uint8_t> ClientApi::getCpuVolts()
{
    return parseReceiveData(__cpu_volts);
}

std::vector<uint8_t> ClientApi::getDisplays()
{
    return parseReceiveData(__displays);
}

std::vector<uint8_t> ClientApi::getCpuUsage()
{
    return parseReceiveData(__cpu_usage);
}

std::vector<uint8_t> ClientApi::getLoadAverage()
{
    return parseReceiveData(__load_average);
}

std::vector<uint8_t> ClientApi::getVirtualMemory()
{
    return parseReceiveData(__virtual_memory);
}

//std::vector<uint8_t> ClientApi::getDiskUsage(std::string disk_label)
//{
////    return parseReceiveData(__cpu_volts);
//}

std::vector<uint8_t> ClientApi::getNetConnections()
{
    return parseReceiveData(__net_connections);
}

std::vector<uint8_t> ClientApi::getUsers()
{
    return parseReceiveData(__users);
}

std::vector<uint8_t> ClientApi::getDateTime()
{
    return parseReceiveData(__datetime);
}

std::vector<uint8_t> ClientApi::parseReceiveData(std::string url)
{
    try
    {
        http::Request request{apiAddress + url};

        const auto response = request.send("GET");
        return response.body;

    }
    catch (const std::exception& e)
    {
        std::vector<uint8_t> _data = {};
        std::cerr << "Initialize reqiest failed! Error: " << e.what();
        return _data;
    }
}

