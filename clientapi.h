#ifndef CLIENTAPI_H
#define CLIENTAPI_H
#include<vector>
#include <iostream>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <cmath>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class ClientApiEventListener;

class ClientApi : public QObject
{
    Q_OBJECT

    typedef std::vector<ClientApiEventListener *> ClientApiEventListeners_t;
public:
    ClientApi(std::string api_address, int port);
    ClientApi(std::string api_address);
    ~ClientApi();

    //! \brief Porownanie dwoch liczb (double, float).
    //! \param Liczba 1.
    //! \param Liczba 2.
    //! \param Epsilon - dokladnosc porownania.
    template <typename T>
    bool compareValues(T a, T b, T epsilon)
    {
        return(fabs(a - b) <= epsilon);
    }


    typedef struct Clocks_t
    {
        uint32_t ARM_cores = 0;
        uint32_t VC4 = 0;
        uint32_t ISP = 0;
        uint32_t block_3D = 0;
        uint32_t UART = 0;
        uint32_t PWM = 0;
        uint32_t EMMC = 0;
        uint32_t Pixel = 0;
        uint32_t AVE = 0;
        uint32_t HDMI = 0;
        uint32_t DPI = 0;
    }Clocks_t;

    typedef struct Displays_t
    {
        bool MainLCD = false;
        bool SecondaryLCD = false;
        bool HDMI0 = false;
        bool Composite = false;
        bool HDMI1 = false;
    }Displays_t;

    typedef struct LoadAvg_t
    {
        float L1 = 0;
        float L2 = 0;
        float L3 = 0;
    }LoadAvg_t;


    enum class VIRTUAL_MEMORY{
        TOTAL = 0,
        AVAILABLE = 1,
        USED = 2,
        FREE = 3,
        ACTIVE = 4,
        INACTIVE = 5,
        BUFFERS = 6,
        CACHED = 7,
        SHARED = 8,
        SLAB = 9,
        WIRED = 10
    };

    typedef struct VirtualMemory_t
    {

        uint64_t total = 0;       //: total physical memory (exclusive swap).
        uint64_t available = 0;   //: the memory that can be given instantly to processes without the system going into swap. This is calculated by summing different memory values depending on the platform and it is supposed to be used to monitor actual memory usage in a cross platform fashion.
        uint64_t used = 0;        //: memory used, calculated differently depending on the platform and designed for informational purposes only. total - free does not necessarily match used.
        uint64_t free = 0;        //: memory not being used at all (zeroed) that is readily available; note that this doesn’t reflect the actual memory available (use available instead). total - used does not necessarily match free.
        uint64_t active = 0;      //(UNIX): memory currently in use or very recently used, and so it is in RAM.
        uint64_t inactive = 0;    //(UNIX)memory that is marked as not used.
        uint64_t buffers = 0;     //(Linux, BSD): cache for things like file system metadata.
        uint64_t cached = 0;      //(Linux, BSD): cache for various things.
        uint64_t shared = 0;      //(Linux, BSD): memory that may be simultaneously accessed by multiple processes.
        uint64_t slab = 0;        //(Linux): in-kernel data structures cache.
        uint64_t wired = 0;       //(BSD, macOS): memory that is marked to always stay in RAM. It is never moved to disk.

    }VirtualMemory_t;

    typedef struct Users_t
    {

        std::string name = "";           //: the name of the user.
        std::string terminal = "";       //: the tty or pseudo-tty associated with the user, if any, else None.
        std::string host = "";           //: the host name associated with the entry, if any.
        uint64_t started = 0;            //: the creation time as a floating point number expressed in seconds since the epoch.
        uint32_t pid = 0;                //: the PID of the login process (like sshd, tmux, gdm-session-worker, …). On Windows and OpenBSD this is always set to None.

    }Users_t;

    typedef struct DiskUsage_t
    {
        uint64_t total = 0;
        uint64_t used = 0;
        uint64_t free = 0;
        float percent = 0;
        // total=21378641920, used=4809781248, free=15482871808, percent=22.5
    }DiskUsage_t;


    typedef struct Mainteance_t
    {
        float cpu_temperature;
        float cpu_volts;
        Clocks_t clocks;
        Displays_t displays;
        float cpu_usage;
        LoadAvg_t load_average;
        DiskUsage_t disk_usage;
        VirtualMemory_t virtual_memory;

    }Mainteance_t;

    // --------------------------------------- Getters ------------------------------------------
    // --------------------------Mainteance
    void getCpuTemperature();
    void getCpuVolts();
    void getClocks();
    void getDisplays();
    void getCpuUsage();
    void getLoadAverage();
    void getVirtualMemory();
    void getDiskUsage();
    void getUsers();
    void getMainteance();

    // --------------------------Sensors

    void getExtermalTemperature();
    void getHumidity();
    void getPressure();
    void getBME280AllData();





    void addEventListener(ClientApiEventListener * listener);
    void removeEventListener(ClientApiEventListener * listener);

    void timerTimeout();
    bool startTimer(uint time);
    bool stopTimer();

    void httpRequest(QString);

private:
    ClientApiEventListeners_t listenersVector;
    ClientApiEventListener * mlistener = nullptr;

    QNetworkAccessManager *manager = nullptr;
    QNetworkRequest request;

    std::string apiAddress = "";
    int apiPort = 0;

    void parseReceiveData(QJsonObject*);
    std::vector<std::string> split(std::string, std::string);
    std::string httpApiAddress();
    bool strToBool(QString);
    void configureNetworkManager();
    void configureTimers();

    QTimer *timer = nullptr;

    void compareCpuData(QJsonObject*);
    void compareClocksData(QJsonObject*);
    void compareDisplaysData(QJsonObject*);
    void compareLoadAvgData(QJsonObject*);
    void compareDiskUsageData(QJsonObject*);
    void compareVirtualMemoryData(QJsonObject*);
    void compareServerTimeData(QJsonObject*);

    Mainteance_t mainteance;

public slots:
    void managerFinished(QNetworkReply *reply);

}

//Url defines
#define __temperature       "/mainteance/temperature"
#define __cpu_volts         "/mainteance/volts"
#define __clocks            "/mainteance/clock"
#define __displays          "/mainteance/display"
#define __cpu_usage         "/mainteance/cpu"
#define __load_average      "/mainteance/load"
#define __virtual_memory    "/mainteance/virtual_memory"
#define __disk_usage        "/mainteance/disk_usage"
#define __net_connections   "/mainteance/net_connections"
#define __users             "/mainteance/users"
#define __datetime          "/mainteance/detetime"
#define __full_mainteance   "/mainteance/mainteance"


// API Json Keys
#define __FullMainteanceKey "Full"
#define __FullBME280All     "BME280All"



;

class ClientApiEventListener{
public:
    virtual ~ClientApiEventListener(){};

    virtual void ClientApi_onJsonParseError(std::string){};
    virtual void ClientApi_onJsonObjectNull(std::string){};

    virtual void ClientApi_onCpuTemperatureChanged(float){};
    virtual void ClientApi_onCpuVoltsChanged(float){};
    virtual void ClientApi_onCpuUsageChanged(float){};

    virtual void ClientApi_onClockArmCoresChanged(uint32_t){};
    virtual void ClientApi_onClockVC4Changed(uint32_t){};
    virtual void ClientApi_onClockISPChanged(uint32_t){};
    virtual void ClientApi_onClockBlock3DChanged(uint32_t){};
    virtual void ClientApi_onClockUARTChanged(uint32_t){};
    virtual void ClientApi_onClockPWMChanged(uint32_t){};
    virtual void ClientApi_onClockEMMCChanged(uint32_t){};
    virtual void ClientApi_onClockPixelChanged(uint32_t){};
    virtual void ClientApi_onClockAVEChanged(uint32_t){};
    virtual void ClientApi_onClockHDMIChanged(uint32_t){};
    virtual void ClientApi_onClockDPIChanged(uint32_t){};

    virtual void ClientApi_onDisplaysMainLcdChanged(bool){};
    virtual void ClientApi_onDisplaysSecondaryLcdChanged(bool){};
    virtual void ClientApi_onDisplaysHDMI0Changed(bool){};
    virtual void ClientApi_onDisplaysCompositeChanged(bool){};
    virtual void ClientApi_onDisplaysHDMI1Changed(bool){};

    virtual void ClientApi_onLoadAvgL1Changed(float){};
    virtual void ClientApi_onLoadAvgL2Changed(float){};
    virtual void ClientApi_onLoadAvgL3Changed(float){};

    virtual void ClientApi_onVirtualMemoryTotalChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemoryAvailableChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemoryUsedChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemoryFreeChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemoryActiveChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemoryInactiveChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemoryBuffersChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemoryCachedChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemorySharedChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemorySlabChanged(uint64_t){};
    virtual void ClientApi_onVirtualMemoryWiredChanged(uint64_t){};

    virtual void ClientApi_onDiskUsageTotalChanged(uint64_t){};
    virtual void ClientApi_onDiskUsageUsedChanged(uint64_t){};
    virtual void ClientApi_onDiskUsageFreeChanged(uint64_t){};
    virtual void ClientApi_onDiskUsagePercentChanged(float){};

    virtual void ClientApi_onServerTimeChanged(std::string){};

};




#endif // CLIENTAPI_H
