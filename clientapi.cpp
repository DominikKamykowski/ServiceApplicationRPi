    #include "clientapi.h"

#define _emit(x) for (uint8_t i = 0; i < listenersVector.size(); i++) \
    listenersVector.at(i)->x

// --------------------------------------- Constructors/ Descructor ------------------------------------------
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

ClientApi::~ClientApi()
{
    delete mainteance_timer;
    delete bme280_timer;
    delete gps_timer;
}

// --------------------------------------- Parsing received data ------------------------------------------
void ClientApi::parseReceiveData(const QJsonObject m_json_object)
{
    if(m_json_object.keys().contains("Error message"))
    {
        const QString error_json = m_json_object.value("Error message").toString();
        _emit(ClientApi_onErrorMessageOccured(error_json.toStdString()));
    }

    else if(m_json_object.keys().contains(__FullMainteanceKey))
    {
        const QJsonObject mainteance_json = m_json_object.value(__FullMainteanceKey).toObject();
        if(mainteance_json.isEmpty())
            _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));

        else
        {
            compareCpuData(mainteance_json);
            compareClocksData(mainteance_json);
            compareDisplaysData(mainteance_json);
            compareLoadAvgData(mainteance_json);
            compareDiskUsageData(mainteance_json);
            compareVirtualMemoryData(mainteance_json);
            compareServerTimeData(mainteance_json);
        }
    }
    else if(m_json_object.keys().contains(__FullBME280))
    {
        const QJsonObject bme280_json = m_json_object.value(__FullBME280).toObject();
        if(bme280_json.isEmpty())
            _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));

        else compareBME280Data(bme280_json);
    }
    else if(m_json_object.keys().contains(__FullGPS))
    {
        const QJsonObject gps_json = m_json_object.value(__FullGPS).toObject();
        if(gps_json.isEmpty())
            _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));

        else compareGPSData(gps_json);
    }
    else
    {
        if (m_json_object.keys().contains(__CpuTemp))
        {
            const float _cpu_temp = static_cast<float>(m_json_object.value(__CpuTemp).toDouble());
            _emit(ClientApi_onCpuTemperatureChanged(_cpu_temp));
        }
        else if (m_json_object.keys().contains("cpu volts"))
        {
            const float _cpu_volts = static_cast<float>(m_json_object.value("cpu volts").toDouble());
            _emit(ClientApi_onCpuVoltsChanged(_cpu_volts));
        }
        else if (m_json_object.keys().contains("Cpu usage"))
        {
            const QJsonObject m_cpu_usage = m_json_object.value("Cpu usage").toObject();
            const float _cpu_usage = static_cast<float>(m_cpu_usage.value("Cpu usage").toDouble());
            _emit(ClientApi_onCpuUsageChanged(_cpu_usage));
        }
        else if (m_json_object.keys().contains("clocks")) compareClocksData(m_json_object);
        else if (m_json_object.keys().contains("displays")) compareDisplaysData(m_json_object);
        else if (m_json_object.keys().contains("Load average")) compareLoadAvgData(m_json_object);
        else if (m_json_object.keys().contains("Virtual memory")) compareVirtualMemoryData(m_json_object);
        else if (m_json_object.keys().contains("Disk usage")) compareDiskUsageData(m_json_object);
        else if (m_json_object.keys().contains("Time")) compareServerTimeData(m_json_object);

    }
}


// --------------------------------------- Network ------------------------------------------
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
                parseReceiveData(obj);
            }
        }
    }
    else
    {
        _emit(ClientApi_onJsonParseError(parse_error.errorString().toStdString()));
    }

}

std::string ClientApi::httpApiAddress()
{
    return "http://" + apiAddress + ":" + std::to_string(apiPort);
}

void ClientApi::configureNetworkManager()
{
    manager = new QNetworkAccessManager(this);
    QObject::connect(manager, &QNetworkAccessManager::finished,this,&ClientApi::managerFinished);
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(managerFinished(QNetworkReply*)));
}


// --------------------------------------- Data compares ------------------------------------------
void ClientApi::compareServerTimeData(const QJsonObject server_time_json)
{
    const QJsonObject m_server_time_obj = server_time_json.value("Time").toObject();
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

void ClientApi::compareVirtualMemoryData(const QJsonObject virtual_memory_json)
{
    vt_changed = false;
    const QJsonObject m_virtual_memory_obj = virtual_memory_json.value("Virtual memory").toObject();
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
        vt_changed = true;
    }
    if(mainteance.virtual_memory.available != _available )
    {
        mainteance.virtual_memory.available = _available ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.used != _used )
    {
        mainteance.virtual_memory.used = _used ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.free != _free )
    {
        mainteance.virtual_memory.free = _free ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.active != _active )
    {
        mainteance.virtual_memory.active = _active ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.inactive != _inactive )
    {
        mainteance.virtual_memory.inactive = _inactive ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.buffers != _buffers )
    {
        mainteance.virtual_memory.buffers = _buffers ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.cached != _cached )
    {
        mainteance.virtual_memory.cached = _cached ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.shared != _shared )
    {
        mainteance.virtual_memory.shared = _shared ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.slab != _slab )
    {
        mainteance.virtual_memory.slab = _slab ;
        vt_changed = true;
    }
    if(mainteance.virtual_memory.wired != _wired )
    {
        mainteance.virtual_memory.wired = _wired ;
        vt_changed = true;
    }
    if(vt_changed)
    {
        _emit(ClientApi_onVirtualMemoryChanged(mainteance.virtual_memory));
    }
}

void ClientApi::compareDiskUsageData(const QJsonObject disk_usage_json)
{
    disk_changed = false;
    const QJsonObject m_disk_usage_obj = disk_usage_json.value("Disk usage").toObject();
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
        disk_changed = true;
    }
    if(mainteance.disk_usage.used != _used )
    {
        mainteance.disk_usage.used = _used ;
        disk_changed = true;
    }
    if(mainteance.disk_usage.free != _free)
    {
        mainteance.disk_usage.free = _free;
        disk_changed = true;
    }
    if(!compareValues(mainteance.disk_usage.percent,_percent ,0.01f))
    {
        mainteance.disk_usage.percent = _percent ;
        disk_changed = true;
    }

    if(disk_changed)
    {
        _emit(ClientApi_onDiskDataChanged(mainteance.disk_usage));
    }
}

void ClientApi::compareLoadAvgData(const QJsonObject load_avg_json)
{
    load_avg_changed = false;
    const QJsonObject m_load_avg_obj = load_avg_json.value("Load average").toObject();
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
        load_avg_changed = true;
    }
    if(!compareValues(mainteance.load_average.L2,_l2,0.01f))
    {
        mainteance.load_average.L2 = _l2;
        load_avg_changed = true;
    }
    if(!compareValues(mainteance.load_average.L3,_l3 ,0.01f))
    {
        mainteance.load_average.L3 = _l3;
        load_avg_changed = true;
    }
    if(load_avg_changed)
    {
        _emit(ClientApi_onLoadAvgChanged(mainteance.load_average));
    }
}

void ClientApi::compareDisplaysData(const QJsonObject display_json)
{
    displays_changed = false;
    const QJsonObject m_displays = display_json.value("displays").toObject();
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
        displays_changed = true;
    }
    if(mainteance.displays.MainLCD != _main_lcd )
    {
        mainteance.displays.MainLCD = _main_lcd ;
        displays_changed = true;
    }
    if(mainteance.displays.SecondaryLCD != _secondary_lcd )
    {
        mainteance.displays.SecondaryLCD = _secondary_lcd ;
        displays_changed = true;
    }
    if(mainteance.displays.HDMI0 != _hdmi0)
    {
        mainteance.displays.HDMI0 = _hdmi0;
        displays_changed = true;
    }
    if(mainteance.displays.HDMI1 != _hdmi1)
    {
        mainteance.displays.HDMI1 = _hdmi1;
        displays_changed = true;
    }
    if(displays_changed)
    {
        _emit(ClientApi_onDisplayChanged(mainteance.displays));
    }
}

void ClientApi::compareClocksData(const QJsonObject clock_json)
{
    clocks_changed = false;
    const QJsonObject m_clocks = clock_json.value("clocks").toObject();
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
        clocks_changed = true;
    }
    if(mainteance.clocks.VC4 != _vc4)
    {
        mainteance.clocks.VC4 = _vc4;
        clocks_changed = true;
    }
    if(mainteance.clocks.ISP != _isp)
    {
        mainteance.clocks.ISP = _isp;
        clocks_changed = true;
    }
    if(mainteance.clocks.block_3D != _3d)
    {
        mainteance.clocks.block_3D = _3d;
        clocks_changed = true;
    }
    if(mainteance.clocks.UART != _uart)
    {
        mainteance.clocks.UART = _uart;
        clocks_changed = true;
    }
    if(mainteance.clocks.PWM != _pwm)
    {
        mainteance.clocks.PWM = _pwm;
        clocks_changed = true;
    }
    if(mainteance.clocks.EMMC != _emmc)
    {
        mainteance.clocks.EMMC = _emmc;
        clocks_changed = true;
    }
    if(mainteance.clocks.Pixel != _pix)
    {
        mainteance.clocks.Pixel = _pix;
        clocks_changed = true;
    }
    if(mainteance.clocks.AVE != _ave)
    {
        mainteance.clocks.AVE = _ave;
        clocks_changed = true;
    }
    if(mainteance.clocks.HDMI != _hdmi)
    {
        mainteance.clocks.HDMI = _hdmi;
        clocks_changed = true;
    }
    if(mainteance.clocks.DPI != _dpi)
    {
        mainteance.clocks.DPI = _dpi;
        clocks_changed = true;
    }
    if(clocks_changed)
    {
        _emit(ClientApi_onClocksChanged(mainteance.clocks));
    }
}

void ClientApi::compareCpuData(const QJsonObject cpu_json)
{
    const QJsonObject m_cpu_usage = cpu_json.value("Cpu usage").toObject();
    if(m_cpu_usage.isEmpty())
    {
        _emit(ClientApi_onJsonObjectNull(Q_FUNC_INFO));
        return;
    }

    const float _cpu_temperature =    static_cast<float>(cpu_json.value("cpu temperature").toDouble());
    const float _cpu_volts =          static_cast<float>(cpu_json.value("cpu volts").toDouble());
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

void ClientApi::compareGPSData(const QJsonObject gps_json)
{
    gps_changed = false;
    const QString timestamp = gps_json.value("timestamp UTC").toString();
    if(!(timestamp == "") || !(timestamp == "Null") || !(timestamp == "null"))
    {
        if(gps.timestamp != timestamp.toStdString())
        {
            gps.timestamp = timestamp.toStdString();
            gps_changed = true;
        }
    }
    const QJsonObject coordinates = gps_json.value("coordinates").toObject();
    const double _longtitude = coordinates.value("longtitude").toDouble();
    const double _latitude = coordinates.value("latitude").toDouble();
    const double _altitude = coordinates.value("altitude").toDouble();

    if(!compareValues(gps.coordinates.longitude, _longtitude, double(0.0000001)))
    {
        gps.coordinates.longitude = _longtitude;
        gps_changed = true;
    }
    if(!compareValues(gps.coordinates.latitude, _latitude, double(0.0000001)))
    {
        gps.coordinates.latitude = _latitude;
        gps_changed = true;
    }
    if(!compareValues(gps.coordinates.altitude, _altitude, double(0.0000001)))
    {
        gps.coordinates.altitude = _altitude;
        gps_changed = true;
    }

    compareGPSPrecision(gps_json);

    const double _speed = gps_json.value("speed").toDouble();
    if(!compareValues(gps.speed, _speed, double(0.01)))
    {
        gps.speed = _speed;
        gps_changed = true;
    }
    compareGPSFix(gps_json);



    //emit
    if(gps_changed) _emit(ClientApi_onGPSDataChanged(gps));

}

void ClientApi::compareGPSPrecision(const QJsonObject precision)
{
    QJsonObject _precision = precision.value("coordinates precise").toObject();
    const double longiutde_prec = _precision.value("longtitude").toString().toDouble();
    const double latitude_prec = _precision.value("latitude").toString().toDouble();

    if(!compareValues(gps.precise.longitude, longiutde_prec, double(0.001)))
    {
        gps.precise.longitude = longiutde_prec;
        gps_changed = true;
    }
    if(!compareValues(gps.precise.latitude, latitude_prec, double(0.001)))
    {
        gps.precise.latitude = latitude_prec;
        gps_changed = true;
    }

}

void ClientApi::compareGPSFix(const QJsonObject fix)
{
    QJsonObject _fix = fix.value("fix data").toObject();
    const uint8_t _fix_quality = _fix.value("fix quality").toInt();
    const uint8_t _fix_quality_3d = _fix.value("fix quality 3d").toInt();
    if(gps.fix.fix_quality != _fix_quality || gps.fix.fix_quality_3d != _fix_quality_3d)
    {
        gps.fix.fix_quality = static_cast<FIX_QUALITY>(_fix_quality);
        gps.fix.fix_quality_3d = static_cast<FIX_TYPE>(_fix_quality_3d);
        gps_changed = true;
    }

}

void ClientApi::compareGPSSatelites(const QJsonObject sats)
{
    QJsonObject _sats = sats.value("satellites data").toObject();
    const uint8_t _sats_number = _sats.value("satelites number").toInt();
    if(gps.satelites.satelites_number != _sats_number)
    {
        gps.satelites.satelites_number = _sats_number;
        gps_changed = true;
    }

    //TODO szczegoly satelit
}

void ClientApi::compareBME280Data(const QJsonObject m_json_object)
{
    bme280_changed = false;
    const float _temperature = static_cast<float>(m_json_object.value("temperature").toDouble());
    const float _humidity = static_cast<float>(m_json_object.value("humidity").toDouble());
    const float _pressure = static_cast<float>(m_json_object.value("pressure").toDouble());
    if(!compareValues(bme280.temperature, _temperature, 0.01f))
    {
        bme280.temperature = _temperature;
        bme280_changed = true;
    }
    if(!compareValues(bme280.humidity, _humidity, 0.01f))
    {
        bme280.humidity = _humidity;
        bme280_changed = true;
    }
    if(!compareValues(bme280.pressure, _pressure, 0.01f))
    {
        bme280.pressure = _pressure;
        bme280_changed = true;
    }
    if(bme280_changed)
    {
        _emit(ClientApi_onBME280TDataChanged(bme280));
    }
}


// --------------------------------------- HTTP ------------------------------------------
void ClientApi::httpRequest(const QString url_string)
{
    const QString api_address = QString::fromStdString(httpApiAddress())+url_string;
    request.setUrl(api_address);
    manager->get(request);
}


// --------------------------------------- Timers ------------------------------------------
void ClientApi::mainteanceTimerTimeout()
{
    getMainteance();
}

bool ClientApi::stopTimer(TIMERS timer)
{
    QTimer* selected_timer = nullptr;

    switch (timer)
    {
    case TIMERS::MAINTEANCE:
        selected_timer = mainteance_timer;
        break;

    case TIMERS::BME280:
        selected_timer = bme280_timer;
        break;

    case TIMERS::GPS:
        selected_timer = gps_timer;
        break;
    }

    if (selected_timer && selected_timer->isActive())
    {
        selected_timer->stop();
        return true;
    }
    else return false;
}

bool ClientApi::startTimer(TIMERS timer, uint time)
{
    if(time< 200) return false;
    else{
        QTimer * selected_timer = nullptr;
        switch(timer)
        {
        case TIMERS::MAINTEANCE:
            selected_timer = mainteance_timer;
            break;
        case TIMERS::BME280:
            selected_timer = bme280_timer;
            break;
        case TIMERS::GPS:
            selected_timer = gps_timer;
            break;
        }
        if(selected_timer)
        {
            selected_timer->start(time);
            return true;
        }
        else return false;
    }
}

void ClientApi::bme280TimerTimeout()
{
    getBme280();
}

void ClientApi::gpsTimerTimeout()
{
    getGPS();
}

void ClientApi::configureTimers()
{
    mainteance_timer = new QTimer(this);
    QObject::connect(mainteance_timer, &QTimer::timeout, this, &ClientApi::mainteanceTimerTimeout);

    bme280_timer = new QTimer(this);
    QObject::connect(bme280_timer, &QTimer::timeout, this, &ClientApi::bme280TimerTimeout);

    gps_timer = new QTimer(this);
    QObject::connect(gps_timer, &QTimer::timeout, this, &ClientApi::gpsTimerTimeout);
}

// --------------------------------------- Other ------------------------------------------
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

bool ClientApi::strToBool(const QString value)
{
    if(value == "on") return true;
    else return false;
}

// --------------------------------------- Getters ------------------------------------------
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

void ClientApi::getMainteance()
{
    httpRequest(__full_mainteance);
}

void ClientApi::getBme280()
{
    httpRequest(__full_bme280);
}

void ClientApi::getGPS()
{
    httpRequest(__gps);
}

// --------------------------------------- Event Listener ------------------------------------------
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
