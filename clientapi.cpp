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
    try
    {
        http::Request request{apiAddress + __temperature};

        const auto response = request.send("GET");
//        std::cout << std::string{response.body.begin(), response.body.end()} << std::endl; // print the result
        return response.body;

    }
    catch (const std::exception& e)
    {
        std::vector<uint8_t> _data = {};
        std::cerr << "Initialize reqiest failed! Error: " << e.what();
        return _data;
    }
}

