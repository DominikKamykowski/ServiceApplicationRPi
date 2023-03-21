#include "clientapi.h"
#include <http.hpp>

ClientApi::ClientApi(std::string api_address, int port)
{
    apiAddress = api_address;
    apiPort = port;
}

ClientApi::ClientApi(std::string api_address)
{
    std::vector<std::string> data = {};
    data = split(api_address,":");
    apiAddress = data.at(0);
    apiPort = std::stoi(data.at(1));
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

        http::Request request{httpApiAddress() + url};

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

std::vector<std::string> ClientApi::split(std::string sentence, std::string splitter)
{
    size_t pos_start = 0, pos_end, delim_len = splitter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = sentence.find(splitter, pos_start)) != std::string::npos)
    {
        token = sentence.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (sentence.substr (pos_start));
    return res;
}

std::string ClientApi::httpApiAddress()
{
    return "http://" + apiAddress + ":" + std::to_string(apiPort);
}
