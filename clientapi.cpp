#include "clientapi.h"
#include <ExternalLibs/http.hpp>

ClientApi::ClientApi(std::string api_address, int port)
{
    apiAddress = api_address;
    apiPort = port;
    mainteance = {};
    fillMainteanceData();
}

ClientApi::ClientApi(std::string api_address)
{
    std::vector<std::string> data = {};
    data = split(api_address,":");
    apiAddress = data.at(0);
    apiPort = std::stoi(data.at(1));
    mainteance = {};
    fillMainteanceData();
}

ClientApi::~ClientApi()
{

}

float ClientApi::getTemperature()
{
    return jsonFromVc(__temperature)["cpu temperature"].get<float>();
}

float ClientApi::getCpuVolts()
{
    return jsonFromVc(__cpu_volts)["cpu volts"].get<float>();
}

ClientApi::Clocks_t ClientApi::getClocks()
{
    nlohmann::json _json = jsonFromVc(__clocks);
    Clocks_t clocks{
    _json["ARM cores"].get<uint32_t>(),
    _json["VC4 scaler cores"].get<uint32_t>(),
    _json["Image Signal Processor"].get<uint32_t>(),
    _json["3D block"].get<uint32_t>(),
    _json["UART"].get<uint32_t>(),
    _json["pwm"].get<uint32_t>(),
    _json["emmc"].get<uint32_t>(),
    _json["Pixel valve"].get<uint32_t>(),
    _json["Analogue video encoder"].get<uint32_t>(),
    _json["HDMI"].get<uint32_t>(),
    _json["Display Peripheral Interface"].get<uint32_t>()
    };
    return clocks;
}

ClientApi::Displays_t ClientApi::getDisplays()
{
    nlohmann::json _json = jsonFromVc(__displays);
    Displays_t displays{
    strToBool(_json["MainLCD"].get<std::string>()),
    strToBool(_json["SecondaryLCD"].get<std::string>()),
    strToBool(_json["HDMI0"].get<std::string>()),
    strToBool(_json["Composite"].get<std::string>()),
    strToBool(_json["HDMI1"].get<std::string>())
    };
    return displays;
}

float ClientApi::getCpuUsage()
{
    return jsonFromVc(__cpu_usage)["Cpu usage"].get<float>();
}

ClientApi::LoadAvg_t ClientApi::getLoadAverage()
{
    std::vector<float> _data = jsonFromVc(__load_average)["Load average"].get<std::vector<float>>();
    LoadAvg_t load
    {
        _data.at(0), _data.at(1), _data.at(2)
    };
    return load;
}

ClientApi::VirtualMemory_t ClientApi::getVirtualMemory()
{
    std::vector<uint64_t> _data = jsonFromVc(__virtual_memory)["Virtual memory"].get<std::vector<uint64_t>>();
    VirtualMemory_t memory{
        _data.at(0), _data.at(1), _data.at(2), _data.at(3), _data.at(4), _data.at(5), _data.at(6),
        _data.at(7), _data.at(8), _data.at(9), _data.at(10)
    };

    return memory;
}

std::vector<float> ClientApi::getDiskUsage()
{
    std::vector<float> disk_usage = jsonFromVc(__disk_usage)["Disk usage"].get<std::vector<float>>();
    return disk_usage;
}

//std::vector<uint8_t> ClientApi::getNetConnections()
//{
//    return parseReceiveData(__net_connections);
//}

ClientApi::Users_t ClientApi::getUsers()
{
    nlohmann::json users = jsonFromVc(__virtual_memory);
    Users_t user;
    return user;
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

nlohmann::json ClientApi::jsonFromVc(std::string value)
{
    nlohmann::json json = nlohmann::json::parse(parseReceiveData(value));
    return json;
}

bool ClientApi::strToBool(std::string value)
{
    if(value == "on") return true;
    else return false;
}

void ClientApi::fillMainteanceData()
{
    mainteance.temperature = getTemperature();
    mainteance.cpu_volts = getCpuVolts();
    mainteance.clocks = getClocks();
    mainteance.displays = getDisplays();
    mainteance.cpu_usage = getCpuUsage();
    mainteance.load_average = getLoadAverage();
    mainteance.virtual_memory = getVirtualMemory();
}

ClientApi::Mainteance_t ClientApi::getMainteance() const
{
    return mainteance;
}
