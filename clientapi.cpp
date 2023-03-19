#include "clientapi.h"

ClientApi::ClientApi()
{
    try
    {
        http::Request request{"http://192.168.1.25:8000/mainteance/virtual_memory"};

        const auto response = request.send("GET");
        std::cout << std::string{response.body.begin(), response.body.end()} << std::endl; // print the result
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }
}

ClientApi::~ClientApi()
{

}

