#include "clientapi.h"
#include <ExternalLibs/http.hpp>
#include <future>

#define _emit(x) for (uint8_t i = 0; i < listenersVector.size(); i++) \
    listenersVector.at(i)->x

ClientApi::ClientApi(std::string api_address, int port)
{
    apiAddress = api_address;
    apiPort = port;
    mainteance = {};
    timer = new QTimer(this);


    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, &QNetworkAccessManager::finished,this,&ClientApi::managerFinished);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));
}

ClientApi::ClientApi(std::string api_address)
{
    std::vector<std::string> data = {};
    data = split(api_address,":");
    apiAddress = data.at(0);
    apiPort = std::stoi(data.at(1));
    mainteance = {};
    timer = new QTimer(this);
    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, &QNetworkAccessManager::finished,this,&ClientApi::managerFinished);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));
}

ClientApi::~ClientApi()
{
    delete timer;
}

float ClientApi::getCpuTemperature()
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

ClientApi::DiskUsage_t ClientApi::getDiskUsage()
{
    std::vector<float> disk_usage = jsonFromVc(__disk_usage)["Disk usage"].get<std::vector<float>>();
    DiskUsage_t disks {static_cast<uint32_t>(disk_usage.at(0)),
                       static_cast<uint32_t>(disk_usage.at(1)),
                       static_cast<uint32_t>(disk_usage.at(2)),
                       disk_usage.at(3)};

    return disks;
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

void ClientApi::addEventListener(ClientApiEventListener *listener)
{
    assert(listener);
    mlistener = listener;

    assert(std::find(listenersVector.begin(), listenersVector.end(), listener) == listenersVector.end());

    listenersVector.push_back(listener);
}

void ClientApi::removeEventListener(ClientApiEventListener *listener)
{
    assert(listener);
    ClientApiEventListeners_t::iterator it = std::find(listenersVector.begin(), listenersVector.end(), listener);

    if (it == listenersVector.end()) return;
    listenersVector.erase(it);
}


std::vector<uint8_t> ClientApi::parseReceiveData(std::string url)
{
    //
    //
    //
    //
    //
    //
    //
    //
    //
    // DO ZMIANY NA RETURN QSTRING ORAZ QNETWORKACCESMANAGER
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

void ClientApi::compareCpuData()
{
    const float m_cpu_temperature = getCpuTemperature();
    if(compareValues(mainteance.cpu_temperature,m_cpu_temperature,0.01f))
    {
        mainteance.cpu_temperature = m_cpu_temperature;
        _emit(ClientApi_onCpuTemperatureChanged(mainteance.cpu_temperature));
    }
    const float m_cpu_volts = getCpuVolts();
    if(compareValues(mainteance.cpu_volts,m_cpu_volts,0.01f))
    {
        mainteance.cpu_volts = m_cpu_volts;
        _emit(ClientApi_onCpuVoltsChanged(mainteance.cpu_volts));
    }
    const float m_cpu_usage = getCpuUsage();
    if(compareValues(mainteance.cpu_usage,m_cpu_usage,0.01f))
    {
        mainteance.cpu_usage = m_cpu_usage;
        _emit(ClientApi_onCpuUsageChanged(mainteance.cpu_usage));
    }
}

void ClientApi::compareClocksData()
{
    const Clocks_t m_clocks = getClocks();
    if(mainteance.clocks.ARM_cores != m_clocks.ARM_cores)
    {
        mainteance.clocks.ARM_cores = m_clocks.ARM_cores;
        _emit(ClientApi_onClockArmCoresChanged(m_clocks.ARM_cores));
    }
    if(mainteance.clocks.VC4 != m_clocks.VC4)
    {
        mainteance.clocks.VC4 = m_clocks.VC4;
        _emit(ClientApi_onClockVC4Changed(m_clocks.VC4));
    }
    if(mainteance.clocks.ISP != m_clocks.ISP)
    {
        mainteance.clocks.ISP = m_clocks.ISP;
        _emit(ClientApi_onClockISPChanged(m_clocks.ISP));
    }
    if(mainteance.clocks.block_3D != m_clocks.block_3D)
    {
        mainteance.clocks.block_3D = m_clocks.block_3D;
        _emit(ClientApi_onClockBlock3DChanged(m_clocks.block_3D));
    }
    if(mainteance.clocks.UART != m_clocks.UART)
    {
        mainteance.clocks.UART = m_clocks.UART;
        _emit(ClientApi_onClockUARTChanged(m_clocks.UART));
    }
    if(mainteance.clocks.PWM != m_clocks.PWM)
    {
        mainteance.clocks.PWM = m_clocks.PWM;
        _emit(ClientApi_onClockPWMChanged(m_clocks.PWM));
    }
    if(mainteance.clocks.EMMC != m_clocks.EMMC)
    {
        mainteance.clocks.EMMC = m_clocks.EMMC;
        _emit(ClientApi_onClockEMMCChanged(m_clocks.EMMC));
    }
    if(mainteance.clocks.Pixel != m_clocks.Pixel)
    {
        mainteance.clocks.Pixel = m_clocks.Pixel;
        _emit(ClientApi_onClockPixelChanged(m_clocks.Pixel));
    }
    if(mainteance.clocks.AVE != m_clocks.AVE)
    {
        mainteance.clocks.AVE = m_clocks.AVE;
        _emit(ClientApi_onClockAVEChanged(m_clocks.AVE));
    }
    if(mainteance.clocks.HDMI != m_clocks.HDMI)
    {
        mainteance.clocks.HDMI = m_clocks.HDMI;
        _emit(ClientApi_onClockHDMIChanged(m_clocks.HDMI));
    }
    if(mainteance.clocks.DPI != m_clocks.DPI)
    {
        mainteance.clocks.DPI = m_clocks.DPI;
        _emit(ClientApi_onClockDPIChanged(m_clocks.DPI));
    }
}

void ClientApi::compareDisplaysData()
{
    const Displays_t m_displays = getDisplays();
    if(mainteance.displays.MainLCD != m_displays.MainLCD)
    {
        mainteance.displays.MainLCD = m_displays.MainLCD;
        _emit(ClientApi_onDisplaysMainLcdChanged(m_displays.MainLCD));
    }
    if(mainteance.displays.SecondaryLCD != m_displays.SecondaryLCD)
    {
        mainteance.displays.SecondaryLCD = m_displays.SecondaryLCD;
        _emit(ClientApi_onDisplaysSecondaryLcdChanged(m_displays.SecondaryLCD));
    }
    if(mainteance.displays.HDMI0 != m_displays.HDMI0)
    {
        mainteance.displays.HDMI0 = m_displays.HDMI0;
        _emit(ClientApi_onDisplaysHDMI0Changed(m_displays.HDMI0));
    }
    if(mainteance.displays.Composite != m_displays.Composite)
    {
        mainteance.displays.Composite = m_displays.Composite;
        _emit(ClientApi_onDisplaysCompositeChanged(m_displays.Composite));
    }
    if(mainteance.displays.HDMI1 != m_displays.HDMI1)
    {
        mainteance.displays.HDMI1 = m_displays.HDMI1;
        _emit(ClientApi_onDisplaysHDMI1Changed(m_displays.HDMI1));
    }
}

void ClientApi::compareLoadAvgData()
{
    const LoadAvg_t m_loadAvg = getLoadAverage();
    if(compareValues(mainteance.load_average.L1,m_loadAvg.L1,0.01f))
    {
        mainteance.load_average.L1 = m_loadAvg.L1;
        _emit(ClientApi_onLoadAvgL1Changed(m_loadAvg.L1));
    }
    if(compareValues(mainteance.load_average.L2,m_loadAvg.L2,0.01f))
    {
        mainteance.load_average.L2 = m_loadAvg.L2;
        _emit(ClientApi_onLoadAvgL2Changed(m_loadAvg.L2));
    }
    if(compareValues(mainteance.load_average.L3,m_loadAvg.L3,0.01f))
    {
        mainteance.load_average.L3 = m_loadAvg.L3;
        _emit(ClientApi_onLoadAvgL3Changed(m_loadAvg.L3));
    }
}

void ClientApi::compareDiskUsageData()
{
    const DiskUsage_t m_disk = getDiskUsage();
    if(mainteance.disk_usage.total != m_disk.total)
    {
        mainteance.disk_usage.total = m_disk.total;
        _emit(ClientApi_onDiskUsageTotalChanged(m_disk.total));
    }
    if(mainteance.disk_usage.used != m_disk.used)
    {
        mainteance.disk_usage.used = m_disk.used;
        _emit(ClientApi_onDiskUsageUsedChanged(m_disk.used));
    }
    if(mainteance.disk_usage.free != m_disk.free)
    {
        mainteance.disk_usage.free = m_disk.free;
        _emit(ClientApi_onDiskUsageFreeChanged(m_disk.free));
    }
    if(compareValues(mainteance.disk_usage.percent,m_disk.percent,0.01f))
    {
        mainteance.disk_usage.percent = m_disk.percent;
        _emit(ClientApi_onDiskUsagePercentChanged(m_disk.percent));
    }
}

void ClientApi::compareVirtualMemoryData()
{
    const VirtualMemory_t m_virtual = getVirtualMemory();
    if(mainteance.virtual_memory.total != m_virtual.total)
    {
        mainteance.virtual_memory.total = m_virtual.total;
        _emit(ClientApi_onVirtualMemoryTotalChanged(m_virtual.total));
    }
    if(mainteance.virtual_memory.available != m_virtual.available)
    {
        mainteance.virtual_memory.available = m_virtual.available;
        _emit(ClientApi_onVirtualMemoryAvailableChanged(m_virtual.available));
    }
    if(mainteance.virtual_memory.used != m_virtual.used)
    {
        mainteance.virtual_memory.used = m_virtual.used;
        _emit(ClientApi_onVirtualMemoryUsedChanged(m_virtual.used));
    }
    if(mainteance.virtual_memory.free != m_virtual.free)
    {
        mainteance.virtual_memory.free = m_virtual.free;
        _emit(ClientApi_onVirtualMemoryFreeChanged(m_virtual.free));
    }
    if(mainteance.virtual_memory.active != m_virtual.active)
    {
        mainteance.virtual_memory.active = m_virtual.active;
        _emit(ClientApi_onVirtualMemoryActiveChanged(m_virtual.active));
    }
    if(mainteance.virtual_memory.inactive != m_virtual.inactive)
    {
        mainteance.virtual_memory.inactive = m_virtual.inactive;
        _emit(ClientApi_onVirtualMemoryInactiveChanged(m_virtual.inactive));
    }
    if(mainteance.virtual_memory.buffers != m_virtual.buffers)
    {
        mainteance.virtual_memory.buffers = m_virtual.buffers;
        _emit(ClientApi_onVirtualMemoryBuffersChanged(m_virtual.buffers));
    }
    if(mainteance.virtual_memory.cached != m_virtual.cached)
    {
        mainteance.virtual_memory.cached = m_virtual.cached;
        _emit(ClientApi_onVirtualMemoryCachedChanged(m_virtual.cached));
    }
    if(mainteance.virtual_memory.shared != m_virtual.shared)
    {
        mainteance.virtual_memory.shared = m_virtual.shared;
        _emit(ClientApi_onVirtualMemorySharedChanged(m_virtual.shared));
    }
    if(mainteance.virtual_memory.slab != m_virtual.slab)
    {
        mainteance.virtual_memory.slab = m_virtual.slab;
        _emit(ClientApi_onVirtualMemorySlabChanged(m_virtual.slab));
    }
    if(mainteance.virtual_memory.wired != m_virtual.wired)
    {
        mainteance.virtual_memory.wired = m_virtual.wired;
        _emit(ClientApi_onVirtualMemoryWiredChanged(m_virtual.wired));
    }

}

void ClientApi::fillMainteanceData()
{
    compareCpuData();
    compareClocksData();
    compareDisplaysData();
    compareLoadAvgData();
    compareVirtualMemoryData();
    compareDiskUsageData();
}

void ClientApi::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    qDebug() << answer;
}

ClientApi::Mainteance_t ClientApi::getMainteance()
{
    fillMainteanceData();
    return mainteance;
}

void ClientApi::timerTimeout()
{
    fillMainteanceData();
}

bool ClientApi::startTimer(uint time)
{
    if(time>200)
    {
        timer->start(time);
        return true;
    }
    else return false;
}

bool ClientApi::stopTimer()
{
    if(timer->isActive())
    {
        timer->stop();
        return true;
    }
    else return false;
}

void ClientApi::httpRequest()
{
    std::string api_address = httpApiAddress()+__full_mainteance;
    QUrl url(QString::fromStdString(api_address));
    request.setUrl(url);
    manager->get(request);
}
