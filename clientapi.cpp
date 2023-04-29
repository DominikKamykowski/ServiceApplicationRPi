#include "clientapi.h"
#include <future>

#define _emit(x) for (uint8_t i = 0; i < listenersVector.size(); i++) \
    listenersVector.at(i)->x

ClientApi::ClientApi(std::string api_address, int port)
{
    apiAddress = api_address;
    apiPort = port;
    mainteance = {};
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &ClientApi::timerTimeout);

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
    QObject::connect(timer, &QTimer::timeout, this, &ClientApi::timerTimeout);

    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, &QNetworkAccessManager::finished,this,&ClientApi::managerFinished);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));
}

ClientApi::~ClientApi()
{
    delete timer;
}

void ClientApi::getCpuTemperature()
{
}

void ClientApi::getCpuVolts()
{
}

void ClientApi::getClocks()
{
//    Clocks_t clocks{
//    _json["ARM cores"].get<uint32_t>(),
//    _json["VC4 scaler cores"].get<uint32_t>(),
//    _json["Image Signal Processor"].get<uint32_t>(),
//    _json["3D block"].get<uint32_t>(),
//    _json["UART"].get<uint32_t>(),
//    _json["pwm"].get<uint32_t>(),
//    _json["emmc"].get<uint32_t>(),
//    _json["Pixel valve"].get<uint32_t>(),
//    _json["Analogue video encoder"].get<uint32_t>(),
//    _json["HDMI"].get<uint32_t>(),
//    _json["Display Peripheral Interface"].get<uint32_t>()
//    };
//    return clocks;
}

void ClientApi::getDisplays()
{
//    Displays_t displays{
//    strToBool(_json["MainLCD"].get<std::string>()),
//    strToBool(_json["SecondaryLCD"].get<std::string>()),
//    strToBool(_json["HDMI0"].get<std::string>()),
//    strToBool(_json["Composite"].get<std::string>()),
//    strToBool(_json["HDMI1"].get<std::string>())
//    };
//    return displays;
}

void ClientApi::getCpuUsage()
{
}

void ClientApi::getLoadAverage()
{
//    LoadAvg_t load
//    {
//        _data.at(0), _data.at(1), _data.at(2)
//    };
//    return load;
}

void ClientApi::getVirtualMemory()
{
//    VirtualMemory_t memory{
//        _data.at(0), _data.at(1), _data.at(2), _data.at(3), _data.at(4), _data.at(5), _data.at(6),
//        _data.at(7), _data.at(8), _data.at(9), _data.at(10)
//    };

//    return memory;
}

void ClientApi::getDiskUsage()
{
//    DiskUsage_t disks {static_cast<uint32_t>(disk_usage.at(0)),
//                       static_cast<uint32_t>(disk_usage.at(1)),
//                       static_cast<uint32_t>(disk_usage.at(2)),
//                       disk_usage.at(3)};

//    return disks;
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


void ClientApi::parseReceiveData(QJsonObject *m_json_object)
{
    if(m_json_object->keys().contains("Full"))
    {
        QJsonObject mainteance_json = m_json_object->value("Full").toObject();
        compareCpuData(&mainteance_json);
        compareClocksData(&mainteance_json);
        compareDisplaysData(&mainteance_json);
        compareLoadAvgData(&mainteance_json);
        compareDiskUsageData(&mainteance_json);
        compareVirtualMemoryData(&mainteance_json);
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

bool ClientApi::strToBool(QString value)
{
    if(value == "on") return true;
    else return false;
}

void ClientApi::compareCpuData(QJsonObject* cpu_json)
{
    QJsonObject m_cpu_usage = cpu_json->value("Cpu usage").toObject();

    if(!compareValues(mainteance.cpu_temperature,static_cast<float>(cpu_json->value("cpu temperature").toDouble()),0.01f))
    {
        mainteance.cpu_temperature = static_cast<float>(cpu_json->value("cpu temperature").toDouble());
        _emit(ClientApi_onCpuTemperatureChanged(mainteance.cpu_temperature));
    }
    if(!compareValues(mainteance.cpu_volts,static_cast<float>(cpu_json->value("cpu volts").toDouble()),0.01f))
    {
        mainteance.cpu_volts = static_cast<float>(cpu_json->value("cpu volts").toDouble());
        _emit(ClientApi_onCpuVoltsChanged(mainteance.cpu_volts));
    }
    if(!compareValues(mainteance.cpu_usage,static_cast<float>(m_cpu_usage.value("Cpu usage").toDouble()),0.01f))
    {
        mainteance.cpu_usage = static_cast<float>(m_cpu_usage.value("Cpu usage").toDouble());
        _emit(ClientApi_onCpuUsageChanged(mainteance.cpu_usage));
    }
}

void ClientApi::compareClocksData(QJsonObject* clock_json)
{
    QJsonObject m_clocks = clock_json->value("clocks").toObject();
//    qDebug()<<m_clocks;

    if(mainteance.clocks.ARM_cores != static_cast<uint32_t>(m_clocks.value("ARM cores").toInt()))
    {
        mainteance.clocks.ARM_cores = static_cast<uint32_t>(m_clocks.value("ARM cores").toInt());
        _emit(ClientApi_onClockArmCoresChanged(mainteance.clocks.ARM_cores));
    }
    if(mainteance.clocks.VC4 != static_cast<uint32_t>(m_clocks.value("VC4 scaler cores").toInt()))
    {
        mainteance.clocks.VC4 = static_cast<uint32_t>(m_clocks.value("VC4 scaler cores").toInt());
        _emit(ClientApi_onClockVC4Changed(mainteance.clocks.VC4));
    }
    if(mainteance.clocks.ISP != static_cast<uint32_t>(m_clocks.value("Image Signal Processor").toInt()))
    {
        mainteance.clocks.ISP = static_cast<uint32_t>(m_clocks.value("Image Signal Processor").toInt());
        _emit(ClientApi_onClockISPChanged(mainteance.clocks.ISP));
    }
    if(mainteance.clocks.block_3D != static_cast<uint32_t>(m_clocks.value("3D block").toInt()))
    {
        mainteance.clocks.block_3D = static_cast<uint32_t>(m_clocks.value("3D block").toInt());
        _emit(ClientApi_onClockBlock3DChanged(mainteance.clocks.block_3D));
    }
    if(mainteance.clocks.UART != static_cast<uint32_t>(m_clocks.value("UART").toInt()))
    {
        mainteance.clocks.UART = static_cast<uint32_t>(m_clocks.value("UART").toInt());
        _emit(ClientApi_onClockUARTChanged(mainteance.clocks.UART));
    }
    if(mainteance.clocks.PWM != static_cast<uint32_t>(m_clocks.value("pwm").toInt()))
    {
        mainteance.clocks.PWM = static_cast<uint32_t>(m_clocks.value("pwm").toInt());
        _emit(ClientApi_onClockPWMChanged(mainteance.clocks.PWM));
    }
    if(mainteance.clocks.EMMC != static_cast<uint32_t>(m_clocks.value("emmc").toInt()))
    {
        mainteance.clocks.EMMC = static_cast<uint32_t>(m_clocks.value("emmc").toInt());
        _emit(ClientApi_onClockEMMCChanged(mainteance.clocks.EMMC));
    }
    if(mainteance.clocks.Pixel != static_cast<uint32_t>(m_clocks.value("Pixel valve").toInt()))
    {
        mainteance.clocks.Pixel = static_cast<uint32_t>(m_clocks.value("Pixel valve").toInt());
        _emit(ClientApi_onClockPixelChanged(mainteance.clocks.Pixel));
    }
    if(mainteance.clocks.AVE != static_cast<uint32_t>(m_clocks.value("Analogue video encoder").toInt()))
    {
        mainteance.clocks.AVE = static_cast<uint32_t>(m_clocks.value("Analogue video encoder").toInt());
        _emit(ClientApi_onClockAVEChanged(mainteance.clocks.AVE));
    }
    if(mainteance.clocks.HDMI != static_cast<uint32_t>(m_clocks.value("HDMI").toInt()))
    {
        mainteance.clocks.HDMI = static_cast<uint32_t>(m_clocks.value("HDMI").toInt());
        _emit(ClientApi_onClockHDMIChanged(mainteance.clocks.HDMI));
    }
    if(mainteance.clocks.DPI != static_cast<uint32_t>(m_clocks.value("Display Peripheral Interface").toInt()))
    {
        mainteance.clocks.DPI = static_cast<uint32_t>(m_clocks.value("Display Peripheral Interface").toInt());
        _emit(ClientApi_onClockDPIChanged(mainteance.clocks.DPI));
    }
}

void ClientApi::compareDisplaysData(QJsonObject* display_json)
{
    QJsonObject m_displays = display_json->value("displays").toObject();
//    qDebug()<<m_displays;

    if(mainteance.displays.Composite != strToBool(m_displays.value("Composite").toString()))
    {
        mainteance.displays.Composite = strToBool(m_displays.value("Composite").toString());
        _emit(ClientApi_onDisplaysCompositeChanged(mainteance.displays.Composite));
    }
    if(mainteance.displays.MainLCD != strToBool(m_displays.value("MainLCD").toString()))
    {
        mainteance.displays.MainLCD = strToBool(m_displays.value("MainLCD").toString());
        _emit(ClientApi_onDisplaysMainLcdChanged(mainteance.displays.MainLCD));
    }
    if(mainteance.displays.SecondaryLCD != strToBool(m_displays.value("SecondaryLCD").toString()))
    {
        mainteance.displays.SecondaryLCD = strToBool(m_displays.value("SecondaryLCD").toString());
        _emit(ClientApi_onDisplaysSecondaryLcdChanged(mainteance.displays.SecondaryLCD));
    }
    if(mainteance.displays.HDMI0 != strToBool(m_displays.value("HDMI0").toString()))
    {
        mainteance.displays.HDMI0 = strToBool(m_displays.value("HDMI0").toString());
        _emit(ClientApi_onDisplaysHDMI0Changed(mainteance.displays.HDMI0));
    }
    if(mainteance.displays.HDMI1 != strToBool(m_displays.value("HDMI1").toString()))
    {
        mainteance.displays.HDMI1 = strToBool(m_displays.value("HDMI1").toString());
        _emit(ClientApi_onDisplaysHDMI0Changed(mainteance.displays.HDMI1));
    }

}

void ClientApi::compareLoadAvgData(QJsonObject* load_avg_json)
{
    QJsonObject m_load_avg_obj = load_avg_json->value("Load average").toObject();
    QJsonArray m_load_avg_array = m_load_avg_obj.value("Load average").toArray();
//    qDebug()<<m_load_avg_array;

    if(!compareValues(mainteance.load_average.L1,static_cast<float>(m_load_avg_array.at(0).toDouble()),0.01f))
    {
        mainteance.load_average.L1 = static_cast<float>(m_load_avg_array.at(0).toDouble());
        _emit(ClientApi_onLoadAvgL1Changed(mainteance.load_average.L1));
    }
    if(!compareValues(mainteance.load_average.L2,static_cast<float>(m_load_avg_array.at(1).toDouble()),0.01f))
    {
        mainteance.load_average.L2 = static_cast<float>(m_load_avg_array.at(1).toDouble());
        _emit(ClientApi_onLoadAvgL2Changed(mainteance.load_average.L2));
    }
    if(!compareValues(mainteance.load_average.L3,static_cast<float>(m_load_avg_array.at(2).toDouble()),0.01f))
    {
        mainteance.load_average.L3 = static_cast<float>(m_load_avg_array.at(2).toDouble());
        _emit(ClientApi_onLoadAvgL3Changed(mainteance.load_average.L3));
    }
}

void ClientApi::compareDiskUsageData(QJsonObject* disk_usage_json)
{
    QJsonObject m_disk_usage_obj = disk_usage_json->value("Disk usage").toObject();
    QJsonArray m_disk_usage_array = m_disk_usage_obj.value("Disk usage").toArray();
    if(mainteance.disk_usage.total != static_cast<uint64_t>(m_disk_usage_array.at(0).toDouble()))
    {

        mainteance.disk_usage.total = static_cast<uint64_t>(m_disk_usage_array.at(0).toDouble());
        _emit(ClientApi_onDiskUsageTotalChanged(mainteance.disk_usage.total));
    }
    if(mainteance.disk_usage.used != static_cast<uint64_t>(m_disk_usage_array.at(1).toDouble()))
    {
        mainteance.disk_usage.used = static_cast<uint64_t>(m_disk_usage_array.at(1).toDouble());
        _emit(ClientApi_onDiskUsageUsedChanged(mainteance.disk_usage.used));
    }
    if(mainteance.disk_usage.free != static_cast<uint64_t>(m_disk_usage_array.at(2).toDouble()))
    {
        mainteance.disk_usage.free = static_cast<uint64_t>(m_disk_usage_array.at(2).toDouble());
        _emit(ClientApi_onDiskUsageFreeChanged(mainteance.disk_usage.free));
    }
    if(!compareValues(mainteance.disk_usage.percent,static_cast<float>(m_disk_usage_array.at(3).toDouble()),0.01f))
    {
        mainteance.disk_usage.percent = static_cast<float>(m_disk_usage_array.at(3).toDouble());
        _emit(ClientApi_onDiskUsagePercentChanged(mainteance.disk_usage.percent));
    }
}

void ClientApi::compareVirtualMemoryData(QJsonObject* virtual_memory_json)
{
    QJsonObject m_virtual_memory_obj = virtual_memory_json->value("Virtual memory").toObject();
    QJsonArray m_virtual_memory_array = m_virtual_memory_obj.value("Virtual memory").toArray();

    if(mainteance.virtual_memory.total != static_cast<uint64_t>(m_virtual_memory_array.at(0).toDouble()))
    {
        mainteance.virtual_memory.total = static_cast<uint64_t>(m_virtual_memory_array.at(0).toDouble());
        _emit(ClientApi_onVirtualMemoryTotalChanged(mainteance.virtual_memory.total));
    }
    if(mainteance.virtual_memory.available != static_cast<uint64_t>(m_virtual_memory_array.at(1).toDouble()))
    {
        mainteance.virtual_memory.available = static_cast<uint64_t>(m_virtual_memory_array.at(1).toDouble());
        _emit(ClientApi_onVirtualMemoryAvailableChanged(mainteance.virtual_memory.available));
    }
    if(mainteance.virtual_memory.used != static_cast<uint64_t>(m_virtual_memory_array.at(2).toDouble()))
    {
        mainteance.virtual_memory.used = static_cast<uint64_t>(m_virtual_memory_array.at(2).toDouble());
        _emit(ClientApi_onVirtualMemoryUsedChanged(mainteance.virtual_memory.used));
    }
    if(mainteance.virtual_memory.free != static_cast<uint64_t>(m_virtual_memory_array.at(3).toDouble()))
    {
        mainteance.virtual_memory.free = static_cast<uint64_t>(m_virtual_memory_array.at(3).toDouble());
        _emit(ClientApi_onVirtualMemoryFreeChanged(mainteance.virtual_memory.free));
    }
    if(mainteance.virtual_memory.active != static_cast<uint64_t>(m_virtual_memory_array.at(4).toDouble()))
    {
        mainteance.virtual_memory.active = static_cast<uint64_t>(m_virtual_memory_array.at(4).toDouble());
        _emit(ClientApi_onVirtualMemoryActiveChanged(mainteance.virtual_memory.active));
    }
    if(mainteance.virtual_memory.inactive != static_cast<uint64_t>(m_virtual_memory_array.at(5).toDouble()))
    {
        mainteance.virtual_memory.inactive = static_cast<uint64_t>(m_virtual_memory_array.at(5).toDouble());
        _emit(ClientApi_onVirtualMemoryInactiveChanged(mainteance.virtual_memory.inactive));
    }
    if(mainteance.virtual_memory.buffers != static_cast<uint64_t>(m_virtual_memory_array.at(6).toDouble()))
    {
        mainteance.virtual_memory.buffers = static_cast<uint64_t>(m_virtual_memory_array.at(6).toDouble());
        _emit(ClientApi_onVirtualMemoryBuffersChanged(mainteance.virtual_memory.buffers));
    }
    if(mainteance.virtual_memory.cached != static_cast<uint64_t>(m_virtual_memory_array.at(7).toDouble()))
    {
        mainteance.virtual_memory.cached = static_cast<uint64_t>(m_virtual_memory_array.at(7).toDouble());
        _emit(ClientApi_onVirtualMemoryCachedChanged(mainteance.virtual_memory.cached));
    }
    if(mainteance.virtual_memory.shared != static_cast<uint64_t>(m_virtual_memory_array.at(8).toDouble()))
    {
        mainteance.virtual_memory.shared = static_cast<uint64_t>(m_virtual_memory_array.at(8).toDouble());
        _emit(ClientApi_onVirtualMemorySharedChanged(mainteance.virtual_memory.shared));
    }
    if(mainteance.virtual_memory.slab != static_cast<uint64_t>(m_virtual_memory_array.at(9).toDouble()))
    {
        mainteance.virtual_memory.slab = static_cast<uint64_t>(m_virtual_memory_array.at(9).toDouble());
        _emit(ClientApi_onVirtualMemorySlabChanged(mainteance.virtual_memory.slab));
    }
    if(mainteance.virtual_memory.wired != static_cast<uint64_t>(m_virtual_memory_array.at(10).toDouble()))
    {
        mainteance.virtual_memory.wired = static_cast<uint64_t>(m_virtual_memory_array.at(10).toDouble());
        _emit(ClientApi_onVirtualMemoryWiredChanged(mainteance.virtual_memory.wired));
    }
}

void ClientApi::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
    if(!doc.isEmpty())
    {
        QJsonObject obj = doc.object();
        parseReceiveData(&obj);
    }
}

void ClientApi::getMainteance()
{
    httpRequest(__full_mainteance);
}

void ClientApi::timerTimeout()
{
    getMainteance();
}

bool ClientApi::startTimer(uint time)
{
    if(time>=200)
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

void ClientApi::httpRequest(QString url_string)
{
    QString api_address = QString::fromStdString(httpApiAddress())+url_string;
    request.setUrl(api_address);
    manager->get(request);
}
