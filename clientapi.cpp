#include "clientapi.h"

#define _emit(x) for (uint8_t i = 0; i < listenersVector.size(); i++) \
    listenersVector.at(i)->x

ClientApi::ClientApi(std::string api_address, int port)
{
    apiAddress = api_address;
    apiPort = port;
    mainteance = {};

    configureTimers();
    configureNetworkManager();
}

ClientApi::ClientApi(std::string api_address)
{
    std::vector<std::string> data = {};
    data = split(api_address,":");
    apiAddress = data.at(0);
    apiPort = std::stoi(data.at(1));
    mainteance = {};

    configureTimers();
    configureNetworkManager();

}

void ClientApi::configureNetworkManager()
{
    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, &QNetworkAccessManager::finished,this,&ClientApi::managerFinished);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));
}

void ClientApi::configureTimers()
{
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &ClientApi::timerTimeout);
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

}

void ClientApi::getDisplays()
{

}

void ClientApi::getCpuUsage()
{
}

void ClientApi::getLoadAverage()
{

}

void ClientApi::getVirtualMemory()
{

}

void ClientApi::getDiskUsage()
{

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
        if(mainteance_json.isEmpty())
        {
            _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        }
        else
        {
            compareCpuData(&mainteance_json);
            compareClocksData(&mainteance_json);
            compareDisplaysData(&mainteance_json);
            compareLoadAvgData(&mainteance_json);
            compareDiskUsageData(&mainteance_json);
            compareVirtualMemoryData(&mainteance_json);
            compareServerTimeData(&mainteance_json);
        }
    }
    else if(m_json_object->keys().contains("BME280"))
    {
        QJsonObject bme280_json = m_json_object->value("BME280").toObject();
        if(bme280_json.isEmpty())
        {
            _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        }
        else
        {
            compareBME280Data(&bme280_json);
        }
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

void ClientApi::compareBME280Data(QJsonObject * m_json_object)
{
    float _temperature = static_cast<float>(m_json_object->value("temperature").toDouble());
    float _humidity = static_cast<float>(m_json_object->value("humidity").toDouble());
    float _pressure = static_cast<float>(m_json_object->value("pressure").toDouble());

    if(!compareValues(bme280.temperature, _temperature, 0.01f))
    {
        bme280.temperature = _temperature;
        _emit(ClientApi_onBME280TemperatureChanged(bme280.temperature));
    }
    if(!compareValues(bme280.humidity, _humidity, 0.01f))
    {
        bme280.humidity = _humidity;
        _emit(ClientApi_onBME280HumidityChanged(bme280.humidity));
    }
    if(!compareValues(bme280.pressure, _pressure, 0.01f))
    {
        bme280.pressure = _pressure;
        _emit(ClientApi_onBME280PressureChanged(bme280.pressure));
    }
}

void ClientApi::compareCpuData(QJsonObject* cpu_json)
{
    QJsonObject m_cpu_usage = cpu_json->value("Cpu usage").toObject();
    if(m_cpu_usage.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }

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

    if(m_clocks.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }

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
    if(m_displays.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }

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
    if(m_load_avg_obj.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }
    QJsonArray m_load_avg_array = m_load_avg_obj.value("Load average").toArray();
    if(m_load_avg_array.size()!= 3)
    {
        //TODO
        return;
    }
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
    if(m_disk_usage_obj.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }
    QJsonArray m_disk_usage_array = m_disk_usage_obj.value("Disk usage").toArray();
    if(m_disk_usage_array.size()!= 4)
    {
        //TODO
        return;
    }
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
    if(m_virtual_memory_obj.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }
    QJsonArray m_virtual_memory_array = m_virtual_memory_obj.value("Virtual memory").toArray();
    if(m_virtual_memory_array.size()!= 11)
    {
        //TODO
        return;
    }

    if(mainteance.virtual_memory.total != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::TOTAL)).toDouble()))
    {
        mainteance.virtual_memory.total = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::TOTAL)).toDouble());
        _emit(ClientApi_onVirtualMemoryTotalChanged(mainteance.virtual_memory.total));
    }
    if(mainteance.virtual_memory.available != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::AVAILABLE)).toDouble()))
    {
        mainteance.virtual_memory.available = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::AVAILABLE)).toDouble());
        _emit(ClientApi_onVirtualMemoryAvailableChanged(mainteance.virtual_memory.available));
    }
    if(mainteance.virtual_memory.used != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::USED)).toDouble()))
    {
        mainteance.virtual_memory.used = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::USED)).toDouble());
        _emit(ClientApi_onVirtualMemoryUsedChanged(mainteance.virtual_memory.used));
    }
    if(mainteance.virtual_memory.free != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::FREE)).toDouble()))
    {
        mainteance.virtual_memory.free = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::FREE)).toDouble());
        _emit(ClientApi_onVirtualMemoryFreeChanged(mainteance.virtual_memory.free));
    }
    if(mainteance.virtual_memory.active != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::ACTIVE)).toDouble()))
    {
        mainteance.virtual_memory.active = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::ACTIVE)).toDouble());
        _emit(ClientApi_onVirtualMemoryActiveChanged(mainteance.virtual_memory.active));
    }
    if(mainteance.virtual_memory.inactive != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::INACTIVE)).toDouble()))
    {
        mainteance.virtual_memory.inactive = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::INACTIVE)).toDouble());
        _emit(ClientApi_onVirtualMemoryInactiveChanged(mainteance.virtual_memory.inactive));
    }
    if(mainteance.virtual_memory.buffers != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::BUFFERS)).toDouble()))
    {
        mainteance.virtual_memory.buffers = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::BUFFERS)).toDouble());
        _emit(ClientApi_onVirtualMemoryBuffersChanged(mainteance.virtual_memory.buffers));
    }
    if(mainteance.virtual_memory.cached != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::CACHED)).toDouble()))
    {
        mainteance.virtual_memory.cached = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::CACHED)).toDouble());
        _emit(ClientApi_onVirtualMemoryCachedChanged(mainteance.virtual_memory.cached));
    }
    if(mainteance.virtual_memory.shared != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::SHARED)).toDouble()))
    {
        mainteance.virtual_memory.shared = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::SHARED)).toDouble());
        _emit(ClientApi_onVirtualMemorySharedChanged(mainteance.virtual_memory.shared));
    }
    if(mainteance.virtual_memory.slab != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::SLAB)).toDouble()))
    {
        mainteance.virtual_memory.slab = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::SLAB)).toDouble());
        _emit(ClientApi_onVirtualMemorySlabChanged(mainteance.virtual_memory.slab));
    }
    if(mainteance.virtual_memory.wired != static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::WIRED)).toDouble()))
    {
        mainteance.virtual_memory.wired = static_cast<uint64_t>(m_virtual_memory_array.at(static_cast<int>(VIRTUAL_MEMORY::WIRED)).toDouble());
        _emit(ClientApi_onVirtualMemoryWiredChanged(mainteance.virtual_memory.wired));
    }
}

void ClientApi::compareServerTimeData(QJsonObject *server_time_json)
{
    QJsonObject m_server_time_obj = server_time_json->value("Time").toObject();
    if(m_server_time_obj.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }
    else
    {
        QString server_time = m_server_time_obj.value("Server time").toString();
        _emit(ClientApi_onServerTimeChanged(server_time.toStdString()));
    }
}


void ClientApi::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }
    QJsonParseError parse_error;
    QString answer = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8(),&parse_error);
    if(parse_error.error == QJsonParseError::NoError)
    {
        if(!doc.isEmpty())
        {
            QJsonObject obj = doc.object();
            if(obj.isEmpty())
            {
                _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
                return;
            }
            else
            {
                parseReceiveData(&obj);
            }
        }
    }
    else
    {
        _emit(ClientApi_onJsonParseError(parse_error.errorString().toStdString()));
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
