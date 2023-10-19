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
    // --------------------------------------- Constructors ------------------------------------------
    ClientApi(std::string api_address, int port);
    ClientApi(std::string api_address);
    ~ClientApi();


    // --------------------------------------- Templates ------------------------------------------
    //! \brief Porownanie dwoch liczb (double, float).
    //! \param Liczba 1.
    //! \param Liczba 2.
    //! \param Epsilon - dokladnosc porownania.
    template <typename T>
    bool compareValues(const T a,const  T b, T epsilon)
    {
        return(fabs(a - b) <= epsilon);
    }


    // --------------------------------------- Enums ------------------------------------------
    enum TIMERS
    {
        MAINTEANCE = 0,
        BME280,
        GPS
    };

    enum FIX_QUALITY{
        NO_FIX = 0,
        GPS_FIX,
        DIFFERENTIAL_GPS_FIX,
        PPS_FIX,
        REAL_TIME_KINEMATIC,
        FLOAT_RTK,
        ESTIMATED,
        MANUAL_INPUT_MODE,
        SIMULATION_MODE
    };

    enum FIX_TYPE{
        NO_FIX_TYPE = 1,
        FIX_2D,
        FIX_3D
    };

    enum VIRTUAL_MEMORY{
        VM_TOTAL = 0,
        VM_AVAILABLE = 1,
        VM_USED = 2,
        VM_FREE = 3,
        VM_ACTIVE = 4,
        VM_INACTIVE = 5,
        VM_BUFFERS = 6,
        VM_CACHED = 7,
        VM_SHARED = 8,
        VM_SLAB = 9,
        VM_WIRED = 10
    };

    enum DISK_USAGE{
        DISK_TOTAL = 0,
        DISK_USED = 1,
        DISK_FREE = 2,
        DISK_PERCENT = 3
    };

    enum LOAD_AVERAGE{
        LA_L1 = 0,
        LA_L2 = 1,
        LA_L3 = 2,
    };


    // --------------------------------------- Structures ------------------------------------------
    struct Clocks_t
    {
        uint32_t ARM_cores  = 0;
        uint32_t VC4        = 0;
        uint32_t ISP        = 0;
        uint32_t block_3D   = 0;
        uint32_t UART       = 0;
        uint32_t PWM        = 0;
        uint32_t EMMC       = 0;
        uint32_t Pixel      = 0;
        uint32_t AVE        = 0;
        uint32_t HDMI       = 0;
        uint32_t DPI        = 0;
    };

    struct Displays_t
    {
        bool MainLCD        = false;
        bool SecondaryLCD   = false;
        bool HDMI0          = false;
        bool Composite      = false;
        bool HDMI1          = false;
    };

    struct LoadAvg_t
    {
        float L1 = 0;
        float L2 = 0;
        float L3 = 0;
    };

    struct VirtualMemory_t
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
    };

    struct Users_t
    {
        std::string name = "";           //: the name of the user.
        std::string terminal = "";       //: the tty or pseudo-tty associated with the user, if any, else None.
        std::string host = "";           //: the host name associated with the entry, if any.
        uint64_t started = 0;            //: the creation time as a floating point number expressed in seconds since the epoch.
        uint32_t pid = 0;                //: the PID of the login process (like sshd, tmux, gdm-session-worker, …). On Windows and OpenBSD this is always set to None.
    };

    struct DiskUsage_t
    {
        uint64_t total = 0;
        uint64_t used = 0;
        uint64_t free = 0;
        float percent = 0;
        // total=21378641920, used=4809781248, free=15482871808, percent=22.5
    };


    struct Mainteance_t
    {
        float cpu_temperature = 0;
        float cpu_volts = 0;
        Clocks_t clocks = {};
        Displays_t displays = {};
        float cpu_usage = 0;
        LoadAvg_t load_average = {};
        DiskUsage_t disk_usage = {};
        VirtualMemory_t virtual_memory = {};
    };

    struct BME280_t
    {
        float temperature = 0;
        float humidity = 0;
        float pressure = 0;
    };

    struct GPS_Satelites_t
    {
        uint8_t satelites_number = 0;
        //TODO szczegoly satelit
    };

    struct GPS_Fix_t
    {
        FIX_QUALITY fix_quality;
        FIX_TYPE fix_quality_3d;
    };

    struct GPS_Coordinates_t
    {
        double longitude = 0;
        double latitude = 0;
        double altitude = 0;
    };

    struct GPS_CoordinatesPrecise_t
    {
        double longitude = 0;
        double latitude = 0;
    };

    struct GPS_t
    {
        std::string timestamp = "";
        GPS_Coordinates_t coordinates = {};
        GPS_CoordinatesPrecise_t precise = {};
        double speed = 0;
        GPS_Fix_t fix = {};
        GPS_Satelites_t satelites = {};
    };



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
    void getMainteance();

    // --------------------------Sensors
    void getBme280();
    void getGPS();

    // --------------------------------------- Listener ------------------------------------------
    void addEventListener(ClientApiEventListener * listener);
    void removeEventListener(ClientApiEventListener * listener);


    // --------------------------------------- Timers ------------------------------------------
    // --------------------------BME280
    void bme280TimerTimeout();

    // --------------------------GPS
    void gpsTimerTimeout();

    // --------------------------Mainteance
    void mainteanceTimerTimeout();

    bool stopTimer(TIMERS);
    bool startTimer(TIMERS,uint time);

    // --------------------------------------- HTTP Request ------------------------------------------
    void httpRequest(const QString);

private:
    ClientApiEventListeners_t listenersVector;
    ClientApiEventListener * mlistener = nullptr;


    // --------------------------------------- Network configure ------------------------------------------
    void configureNetworkManager();
    QNetworkAccessManager *manager = nullptr;
    QNetworkRequest request;
    std::string apiAddress = "";
    int apiPort = 0;
    std::string httpApiAddress();

    // --------------------------------------- Timers ------------------------------------------
    void configureTimers();
    QTimer *mainteance_timer = nullptr;
    QTimer *bme280_timer = nullptr;
    QTimer *gps_timer = nullptr;


    // --------------------------------------- Data analyze ------------------------------------------
    void parseReceiveData           (const QJsonObject);
    void compareBME280Data          (const QJsonObject);
    void compareGPSData             (const QJsonObject);
    void compareGPSPrecision        (const QJsonObject);
    void compareGPSFix              (const QJsonObject);
    void compareGPSSatelites        (const QJsonObject);
    void compareCpuData             (const QJsonObject);
    void compareClocksData          (const QJsonObject);
    void compareDisplaysData        (const QJsonObject);
    void compareLoadAvgData         (const QJsonObject);
    void compareDiskUsageData       (const QJsonObject);
    void compareVirtualMemoryData   (const QJsonObject);
    void compareServerTimeData      (const QJsonObject);

    // --------------------------------------- Structure initialize ------------------------------------------
    Mainteance_t mainteance {};
    BME280_t bme280 {};
    GPS_t gps {};


    // --------------------------------------- Flags ------------------------------------------
    bool gps_changed        = false;
    bool vt_changed         = false;
    bool disk_changed       = false;
    bool load_avg_changed   = false;
    bool displays_changed   = false;
    bool clocks_changed     = false;
    bool bme280_changed     = false;

    // --------------------------------------- Others ------------------------------------------

    bool strToBool(const QString);
    std::vector<std::string> split(std::string, std::string);


public slots:
    void managerFinished(QNetworkReply *reply);

}

// --------------------------------------- URL Defines ------------------------------------------
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
#define __full_bme280       "/sensors/bme280"
#define __gps               "/sensors/gps"

// --------------------------------------- API JSON keys ------------------------------------------
#define __FullMainteanceKey "Full"
#define __FullBME280        "BME280"
#define __FullGPS           "GPS"
#define __CpuTemp           "cpu temperature"
;
// --------------------------------------- Listener ------------------------------------------
class ClientApiEventListener{
public:
    virtual ~ClientApiEventListener(){};

    virtual void ClientApi_onJsonParseError(std::string){};
    virtual void ClientApi_onJsonObjectNull(std::string){};

    virtual void ClientApi_onCpuTemperatureChanged(float){};
    virtual void ClientApi_onCpuVoltsChanged(float){};
    virtual void ClientApi_onCpuUsageChanged(float){};

    virtual void ClientApi_onClocksChanged(ClientApi::Clocks_t){};

    virtual void ClientApi_onDisplayChanged(ClientApi::Displays_t){};

    virtual void ClientApi_onLoadAvgChanged(ClientApi::LoadAvg_t){};

    virtual void ClientApi_onVirtualMemoryChanged(ClientApi::VirtualMemory_t){};

    virtual void ClientApi_onDiskDataChanged(ClientApi::DiskUsage_t){};

    virtual void ClientApi_onServerTimeChanged(std::string){};

    virtual void ClientApi_onBME280TDataChanged(ClientApi::BME280_t){};

    virtual void ClientApi_onGPSDataChanged(ClientApi::GPS_t){};

    virtual void ClientApi_onErrorMessageOccured(std::string){};

    virtual void ClientApi_onRawJSON(QJsonDocument){};
};

#endif // CLIENTAPI_H
