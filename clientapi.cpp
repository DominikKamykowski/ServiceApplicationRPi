#include "clientapi.h"
#include <ExternalLibs/http.hpp>

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
    Clocks_t clocks;
    clocks.ARM_cores =  _json["ARM cores"].get<uint32_t>();
    clocks.VC4 =        _json["VC4 scaler cores"].get<uint32_t>();
    clocks.ISP =        _json["Image Signal Processor"].get<uint32_t>();
    clocks.block_3D =   _json["3D block"].get<uint32_t>();
    clocks.UART =       _json["UART"].get<uint32_t>();
    clocks.PWM =        _json["pwm"].get<uint32_t>();
    clocks.EMMC =       _json["emmc"].get<uint32_t>();
    clocks.Pixel =      _json["Pixel valve"].get<uint32_t>();
    clocks.AVE =        _json["Analogue video encoder"].get<uint32_t>();
    clocks.HDMI =       _json["HDMI"].get<uint32_t>();
    clocks.DPI =        _json["Display Peripheral Interface"].get<uint32_t>();
    return clocks;
}

ClientApi::Displays_t ClientApi::getDisplays()
{
    nlohmann::json _json = jsonFromVc(__displays);
    Displays_t displays;
    displays.MainLCD = strToBool(_json["MainLCD"].get<std::string>());
    displays.SecondaryLCD = strToBool(_json["SecondaryLCD"].get<std::string>());
    displays.HDMI0 = strToBool(_json["HDMI0"].get<std::string>());
    displays.Composite = strToBool(_json["Composite"].get<std::string>());
    displays.HDMI1 = strToBool(_json["HDMI1"].get<std::string>());
    return displays;
}

float ClientApi::getCpuUsage()
{
    return jsonFromVc(__cpu_usage)["Cpu usage"].get<float>();
}

std::vector<float> ClientApi::getLoadAverage()
{
    return jsonFromVc(__load_average)["Load average"].get<std::vector<float>>();
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
