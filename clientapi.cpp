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
    mainteance_timer = new QTimer(this);
    QObject::connect(mainteance_timer, &QTimer::timeout, this, &ClientApi::mainteanceTimerTimeout);

    bme280_timer = new QTimer(this);
    QObject::connect(bme280_timer, &QTimer::timeout, this, &ClientApi::bme280TimerTimeout);
}


ClientApi::~ClientApi()
{
    delete mainteance_timer;
}

void ClientApi::getCpuTemperature()
{
    httpRequest(__temperature);
}

void ClientApi::getCpuVolts()
{
    httpRequest(__cpu_volts);
}

void ClientApi::getClocks()
{
    httpRequest(__clocks);
}

void ClientApi::getDisplays()
{
    httpRequest(__displays);
}

void ClientApi::getCpuUsage()
{
    httpRequest(__cpu_usage);
}

void ClientApi::getLoadAverage()
{
    httpRequest(__load_average);
}

void ClientApi::getVirtualMemory()
{
    httpRequest(__virtual_memory);
}

void ClientApi::getDiskUsage()
{
    httpRequest(__disk_usage);
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

void ClientApi::bme280TimerTimeout()
{
    getBme280();
}


void ClientApi::parseReceiveData(const QJsonObject *m_json_object)
{
    if(m_json_object->keys().contains("Error message"))
    {
        const QString error_json = m_json_object->value("Error message").toString();
        _emit(ClientApi_onErrorMessageOccured(error_json.toStdString()));
    }

    else if(m_json_object->keys().contains("Full"))
    {
        const QJsonObject mainteance_json = m_json_object->value("Full").toObject();
        if(mainteance_json.isEmpty())
            _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));

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
        const QJsonObject bme280_json = m_json_object->value("BME280").toObject();
        if(bme280_json.isEmpty())
            _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));

        else compareBME280Data(&bme280_json);
    }
    else if(m_json_object->keys().contains("GPS"))
    {
        const QJsonObject gps_json = m_json_object->value("GPS").toObject();
        if(gps_json.isEmpty())
            _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));

        else compareGPSData(&gps_json);
    }
    else
    {
        if (m_json_object->keys().contains("cpu temperature"))
        {
            const float _cpu_temp = static_cast<float>(m_json_object->value("cpu temperature").toDouble());
            _emit(ClientApi_onCpuTemperatureChanged(_cpu_temp));
        }
        else if (m_json_object->keys().contains("cpu volts"))
        {
            const float _cpu_volts = static_cast<float>(m_json_object->value("cpu volts").toDouble());
            _emit(ClientApi_onCpuVoltsChanged(_cpu_volts));
        }
        else if (m_json_object->keys().contains("Cpu usage"))
        {
            const QJsonObject m_cpu_usage = m_json_object->value("Cpu usage").toObject();
            const float _cpu_usage = static_cast<float>(m_cpu_usage.value("Cpu usage").toDouble());
            _emit(ClientApi_onCpuUsageChanged(_cpu_usage));
        }
        else if (m_json_object->keys().contains("clocks")) compareClocksData(m_json_object);
        else if (m_json_object->keys().contains("displays")) compareDisplaysData(m_json_object);
        else if (m_json_object->keys().contains("Load average")) compareLoadAvgData(m_json_object);
        else if (m_json_object->keys().contains("Virtual memory")) compareVirtualMemoryData(m_json_object);
        else if (m_json_object->keys().contains("Disk usage")) compareDiskUsageData(m_json_object);
        else if (m_json_object->keys().contains("Time")) compareServerTimeData(m_json_object);

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

bool ClientApi::strToBool(const QString value)
{
    if(value == "on") return true;
    else return false;
}

void ClientApi::compareBME280Data(const QJsonObject * m_json_object)
{
    const float _temperature = static_cast<float>(m_json_object->value("temperature").toDouble());
    const float _humidity = static_cast<float>(m_json_object->value("humidity").toDouble());
    const float _pressure = static_cast<float>(m_json_object->value("pressure").toDouble());

    qDebug()<<"temperatura: "<<_temperature << ", wilgotnosc: " << _humidity << ", cisnienie: " << _pressure;

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

void ClientApi::compareGPSData(const QJsonObject * gps_json)
{
    const double _longtitude = gps_json->value("longtitude").toDouble();
    const double _latitude = gps_json->value("latitude").toDouble();
    const double _altitude = gps_json->value("altitude").toDouble();

    if(!compareValues(gps.longtitude, _longtitude, double(0.0000001)))
    {
        gps.longtitude = _longtitude;
        _emit(ClientApi_onLongtitudeChanged(gps.longtitude));
    }
    if(!compareValues(gps.latitude, _latitude, double(0.0000001)))
    {
        gps.latitude = _latitude;
        _emit(ClientApi_onLatitudeChanged(gps.latitude));
    }
    if(!compareValues(gps.altitude, _altitude, double(0.0000001)))
    {
        gps.altitude = _altitude;
        _emit(ClientApi_onAltitudeChanged(gps.altitude));
    }
}

void ClientApi::compareCpuData(const QJsonObject* cpu_json)
{
    const QJsonObject m_cpu_usage = cpu_json->value("Cpu usage").toObject();
    if(m_cpu_usage.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }

    const float _cpu_temperature =    static_cast<float>(cpu_json->value("cpu temperature").toDouble());
    const float _cpu_volts =          static_cast<float>(cpu_json->value("cpu volts").toDouble());
    const float _cpu_usage =          static_cast<float>(m_cpu_usage.value("Cpu usage").toDouble());

    if(!compareValues(mainteance.cpu_temperature,_cpu_temperature,0.01f))
    {
        mainteance.cpu_temperature = _cpu_temperature;
        _emit(ClientApi_onCpuTemperatureChanged(mainteance.cpu_temperature));
    }
    if(!compareValues(mainteance.cpu_volts,_cpu_volts,0.01f))
    {
        mainteance.cpu_volts = _cpu_volts;
        _emit(ClientApi_onCpuVoltsChanged(mainteance.cpu_volts));
    }
    if(!compareValues(mainteance.cpu_usage,_cpu_usage,0.01f))
    {
        mainteance.cpu_usage = _cpu_usage;
        _emit(ClientApi_onCpuUsageChanged(mainteance.cpu_usage));
    }
}

void ClientApi::compareClocksData(const QJsonObject* clock_json)
{
    const QJsonObject m_clocks = clock_json->value("clocks").toObject();
    //    qDebug()<<m_clocks;

    if(m_clocks.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }

    const uint32_t _arm = static_cast<uint32_t>(m_clocks.value("ARM cores").toInt());
    const uint32_t _vc4 = static_cast<uint32_t>(m_clocks.value("VC4 scaler cores").toInt());
    const uint32_t _isp = static_cast<uint32_t>(m_clocks.value("Image Signal Processor").toInt());
    const uint32_t _3d = static_cast<uint32_t>(m_clocks.value("3D block").toInt());
    const uint32_t _uart = static_cast<uint32_t>(m_clocks.value("UART").toInt());
    const uint32_t _pwm = static_cast<uint32_t>(m_clocks.value("pwm").toInt());
    const uint32_t _emmc = static_cast<uint32_t>(m_clocks.value("emmc").toInt());
    const uint32_t _pix = static_cast<uint32_t>(m_clocks.value("Pixel valve").toInt());
    const uint32_t _ave = static_cast<uint32_t>(m_clocks.value("Analogue video encoder").toInt());
    const uint32_t _hdmi = static_cast<uint32_t>(m_clocks.value("HDMI").toInt());
    const uint32_t _dpi = static_cast<uint32_t>(m_clocks.value("Display Peripheral Interface").toInt());

    if(mainteance.clocks.ARM_cores != _arm)
    {
        mainteance.clocks.ARM_cores = _arm;
        _emit(ClientApi_onClockArmCoresChanged(mainteance.clocks.ARM_cores));
    }
    if(mainteance.clocks.VC4 != _vc4)
    {
        mainteance.clocks.VC4 = _vc4;
        _emit(ClientApi_onClockVC4Changed(mainteance.clocks.VC4));
    }
    if(mainteance.clocks.ISP != _isp)
    {
        mainteance.clocks.ISP = _isp;
        _emit(ClientApi_onClockISPChanged(mainteance.clocks.ISP));
    }
    if(mainteance.clocks.block_3D != _3d)
    {
        mainteance.clocks.block_3D = _3d;
        _emit(ClientApi_onClockBlock3DChanged(mainteance.clocks.block_3D));
    }
    if(mainteance.clocks.UART != _uart)
    {
        mainteance.clocks.UART = _uart;
        _emit(ClientApi_onClockUARTChanged(mainteance.clocks.UART));
    }
    if(mainteance.clocks.PWM != _pwm)
    {
        mainteance.clocks.PWM = _pwm;
        _emit(ClientApi_onClockPWMChanged(mainteance.clocks.PWM));
    }
    if(mainteance.clocks.EMMC != _emmc)
    {
        mainteance.clocks.EMMC = _emmc;
        _emit(ClientApi_onClockEMMCChanged(mainteance.clocks.EMMC));
    }
    if(mainteance.clocks.Pixel != _pix)
    {
        mainteance.clocks.Pixel = _pix;
        _emit(ClientApi_onClockPixelChanged(mainteance.clocks.Pixel));
    }
    if(mainteance.clocks.AVE != _ave)
    {
        mainteance.clocks.AVE = _ave;
        _emit(ClientApi_onClockAVEChanged(mainteance.clocks.AVE));
    }
    if(mainteance.clocks.HDMI != _hdmi)
    {
        mainteance.clocks.HDMI = _hdmi;
        _emit(ClientApi_onClockHDMIChanged(mainteance.clocks.HDMI));
    }
    if(mainteance.clocks.DPI != _dpi)
    {
        mainteance.clocks.DPI = _dpi;
        _emit(ClientApi_onClockDPIChanged(mainteance.clocks.DPI));
    }
}

void ClientApi::compareDisplaysData(const QJsonObject* display_json)
{
    const QJsonObject m_displays = display_json->value("displays").toObject();
    //    qDebug()<<m_displays;
    if(m_displays.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }

    const bool _composite = strToBool(m_displays.value("Composite").toString());
    const bool  _main_lcd = strToBool(m_displays.value("MainLCD").toString());
    const bool _secondary_lcd = strToBool(m_displays.value("SecondaryLCD").toString());
    const bool _hdmi0 = strToBool(m_displays.value("HDMI0").toString());
    const bool _hdmi1 = strToBool(m_displays.value("HDMI1").toString());

    if(mainteance.displays.Composite != _composite )
    {
        mainteance.displays.Composite = _composite ;
        _emit(ClientApi_onDisplaysCompositeChanged(mainteance.displays.Composite));
    }
    if(mainteance.displays.MainLCD != _main_lcd )
    {
        mainteance.displays.MainLCD = _main_lcd ;
        _emit(ClientApi_onDisplaysMainLcdChanged(mainteance.displays.MainLCD));
    }
    if(mainteance.displays.SecondaryLCD != _secondary_lcd )
    {
        mainteance.displays.SecondaryLCD = _secondary_lcd ;
        _emit(ClientApi_onDisplaysSecondaryLcdChanged(mainteance.displays.SecondaryLCD));
    }
    if(mainteance.displays.HDMI0 != _hdmi0)
    {
        mainteance.displays.HDMI0 = _hdmi0;
        _emit(ClientApi_onDisplaysHDMI0Changed(mainteance.displays.HDMI0));
    }
    if(mainteance.displays.HDMI1 != _hdmi1)
    {
        mainteance.displays.HDMI1 = _hdmi1;
        _emit(ClientApi_onDisplaysHDMI0Changed(mainteance.displays.HDMI1));
    }

}

void ClientApi::compareLoadAvgData(const QJsonObject* load_avg_json)
{
    const QJsonObject m_load_avg_obj = load_avg_json->value("Load average").toObject();
    if(m_load_avg_obj.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }
    const QJsonArray m_load_avg_array = m_load_avg_obj.value("Load average").toArray();
    if(m_load_avg_array.size()!= 3)
    {
        //TODO
        return;
    }

    const float _l1 = static_cast<float>(m_load_avg_array.at(LA_L1).toDouble());
    const float _l2 = static_cast<float>(m_load_avg_array.at(LA_L2).toDouble());
    const float _l3 = static_cast<float>(m_load_avg_array.at(LA_L3).toDouble());

    if(!compareValues(mainteance.load_average.L1,_l1,0.01f))
    {
        mainteance.load_average.L1 = _l1;
        _emit(ClientApi_onLoadAvgL1Changed(mainteance.load_average.L1));
    }
    if(!compareValues(mainteance.load_average.L2,_l2,0.01f))
    {
        mainteance.load_average.L2 = _l2;
        _emit(ClientApi_onLoadAvgL2Changed(mainteance.load_average.L2));
    }
    if(!compareValues(mainteance.load_average.L3,_l3 ,0.01f))
    {
        mainteance.load_average.L3 = _l3 ;
        _emit(ClientApi_onLoadAvgL3Changed(mainteance.load_average.L3));
    }
}

void ClientApi::compareDiskUsageData(const QJsonObject* disk_usage_json)
{
    const QJsonObject m_disk_usage_obj = disk_usage_json->value("Disk usage").toObject();
    if(m_disk_usage_obj.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }
    const QJsonArray m_disk_usage_array = m_disk_usage_obj.value("Disk usage").toArray();
    if(m_disk_usage_array.size()!= 4)
    {
        //TODO
        return;
    }

    const uint64_t _total = static_cast<uint64_t>(m_disk_usage_array.at(DISK_TOTAL).toDouble());
    const uint64_t _used = static_cast<uint64_t>(m_disk_usage_array.at(DISK_USED).toDouble());
    const uint64_t _free = static_cast<uint64_t>(m_disk_usage_array.at(DISK_FREE).toDouble());
    const float _percent = static_cast<float>(m_disk_usage_array.at(DISK_PERCENT).toDouble());

    if(mainteance.disk_usage.total != _total)
    {
        mainteance.disk_usage.total = _total;
        _emit(ClientApi_onDiskUsageTotalChanged(mainteance.disk_usage.total));
    }
    if(mainteance.disk_usage.used != _used )
    {
        mainteance.disk_usage.used = _used ;
        _emit(ClientApi_onDiskUsageUsedChanged(mainteance.disk_usage.used));
    }
    if(mainteance.disk_usage.free != _free)
    {
        mainteance.disk_usage.free = _free;
        _emit(ClientApi_onDiskUsageFreeChanged(mainteance.disk_usage.free));
    }
    if(!compareValues(mainteance.disk_usage.percent,_percent ,0.01f))
    {
        mainteance.disk_usage.percent = _percent ;
        _emit(ClientApi_onDiskUsagePercentChanged(mainteance.disk_usage.percent));
    }
}

void ClientApi::compareVirtualMemoryData(const QJsonObject* virtual_memory_json)
{
    const QJsonObject m_virtual_memory_obj = virtual_memory_json->value("Virtual memory").toObject();
    if(m_virtual_memory_obj.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }
    const QJsonArray m_virtual_memory_array = m_virtual_memory_obj.value("Virtual memory").toArray();
    if(m_virtual_memory_array.size()!= 11)
    {
        //TODO
        return;
    }

    const uint64_t _total = static_cast<uint64_t>(m_virtual_memory_array.at(VM_TOTAL).toDouble());
    const uint64_t _available = static_cast<uint64_t>(m_virtual_memory_array.at(VM_AVAILABLE).toDouble());
    const uint64_t _used = static_cast<uint64_t>(m_virtual_memory_array.at(VM_USED).toDouble());
    const uint64_t _free = static_cast<uint64_t>(m_virtual_memory_array.at(VM_FREE).toDouble());
    const uint64_t _active = static_cast<uint64_t>(m_virtual_memory_array.at(VM_ACTIVE).toDouble());
    const uint64_t _inactive = static_cast<uint64_t>(m_virtual_memory_array.at(VM_INACTIVE).toDouble());
    const uint64_t _buffers = static_cast<uint64_t>(m_virtual_memory_array.at(VM_BUFFERS).toDouble());
    const uint64_t _cached = static_cast<uint64_t>(m_virtual_memory_array.at(VM_CACHED).toDouble());
    const uint64_t _shared = static_cast<uint64_t>(m_virtual_memory_array.at(VM_SHARED).toDouble());
    const uint64_t _slab = static_cast<uint64_t>(m_virtual_memory_array.at(VM_SLAB).toDouble());
    const uint64_t _wired = static_cast<uint64_t>(m_virtual_memory_array.at(VM_WIRED).toDouble());


    if(mainteance.virtual_memory.total != _total)
    {
        mainteance.virtual_memory.total = _total;
        _emit(ClientApi_onVirtualMemoryTotalChanged(mainteance.virtual_memory.total));
    }
    if(mainteance.virtual_memory.available != _available )
    {
        mainteance.virtual_memory.available = _available ;
        _emit(ClientApi_onVirtualMemoryAvailableChanged(mainteance.virtual_memory.available));
    }
    if(mainteance.virtual_memory.used != _used )
    {
        mainteance.virtual_memory.used = _used ;
        _emit(ClientApi_onVirtualMemoryUsedChanged(mainteance.virtual_memory.used));
    }
    if(mainteance.virtual_memory.free != _free )
    {
        mainteance.virtual_memory.free = _free ;
        _emit(ClientApi_onVirtualMemoryFreeChanged(mainteance.virtual_memory.free));
    }
    if(mainteance.virtual_memory.active != _active )
    {
        mainteance.virtual_memory.active = _active ;
        _emit(ClientApi_onVirtualMemoryActiveChanged(mainteance.virtual_memory.active));
    }
    if(mainteance.virtual_memory.inactive != _inactive )
    {
        mainteance.virtual_memory.inactive = _inactive ;
        _emit(ClientApi_onVirtualMemoryInactiveChanged(mainteance.virtual_memory.inactive));
    }
    if(mainteance.virtual_memory.buffers != _buffers )
    {
        mainteance.virtual_memory.buffers = _buffers ;
        _emit(ClientApi_onVirtualMemoryBuffersChanged(mainteance.virtual_memory.buffers));
    }
    if(mainteance.virtual_memory.cached != _cached )
    {
        mainteance.virtual_memory.cached = _cached ;
        _emit(ClientApi_onVirtualMemoryCachedChanged(mainteance.virtual_memory.cached));
    }
    if(mainteance.virtual_memory.shared != _shared )
    {
        mainteance.virtual_memory.shared = _shared ;
        _emit(ClientApi_onVirtualMemorySharedChanged(mainteance.virtual_memory.shared));
    }
    if(mainteance.virtual_memory.slab != _slab )
    {
        mainteance.virtual_memory.slab = _slab ;
        _emit(ClientApi_onVirtualMemorySlabChanged(mainteance.virtual_memory.slab));
    }
    if(mainteance.virtual_memory.wired != _wired )
    {
        mainteance.virtual_memory.wired = _wired ;
        _emit(ClientApi_onVirtualMemoryWiredChanged(mainteance.virtual_memory.wired));
    }
}

void ClientApi::compareServerTimeData(const QJsonObject *server_time_json)
{
    const QJsonObject m_server_time_obj = server_time_json->value("Time").toObject();
    if(m_server_time_obj.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }
    else
    {
        const QString server_time = m_server_time_obj.value("Server time").toString();
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

    const QString answer = reply->readAll();
    if(answer == "") return;
    const QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8(),&parse_error);
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
                _emit(ClientApi_onRawJSON(doc));
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

void ClientApi::getBme280()
{
    httpRequest(__full_bme280);
}

void ClientApi::mainteanceTimerTimeout()
{
    getMainteance();
}

bool ClientApi::startMainteanceTimer(uint time)
{
    if(time>=200)
    {
        mainteance_timer->start(time);
        return true;
    }
    else return false;
}

bool ClientApi::stopMainteanceTimer()
{
    if(mainteance_timer->isActive())
    {
        mainteance_timer->stop();
        return true;
    }
    else return false;
}

bool ClientApi::startBme280Timer(uint time)
{
    if(time>=200)
    {
        bme280_timer->start(time);
        return true;
    }
    else return false;
}

bool ClientApi::stopBme280Timer()
{
    if(bme280_timer->isActive())
    {
        bme280_timer->stop();
        return true;
    }
    else return false;
}

void ClientApi::httpRequest(const QString url_string)
{
    const QString api_address = QString::fromStdString(httpApiAddress())+url_string;
    request.setUrl(api_address);
    manager->get(request);
}
