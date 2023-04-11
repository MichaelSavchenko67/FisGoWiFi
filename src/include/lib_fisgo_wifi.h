#ifndef LIB_FISGO_WIFI_H_
#define LIB_FISGO_WIFI_H_

#include <string.h>
#include <unistd.h>

#include <mutex>
#include <vector>
#include <string>
#include <future>
#include <fstream>
#include <sstream>
#include <iostream>

#include "lib_fisgo_wifi_db.h"

using namespace std;

class Fisgo_Wifi
{
    enum WIFI_SEC_TYPE
    {
        WIFI_SEC_WPA  = 1,
        WIFI_SEC_WPA2 = 2,
        WIFI_SEC_WEP  = 3,
        WIFI_SEC_EAP  = 4,

        WIFI_SEC_ALL
    };

    enum CYR_SYMBOLS : uint64_t
    {
        CYR_SIZE    = 0x08,                 // Длина кириллического символа

        CYR_TEST_1  = 0x85c3064785c,        // этот код начинает кириллический символ
        CYR_TEST_2  = 0x85c3164785c,        // этот код начинает кириллический символ

        CYR_UP_1    = 0x3039785c3064785c,   // А
        CYR_UP_2    = 0x3139785c3064785c,   // Б
        CYR_UP_3    = 0x3239785c3064785c,   // В
        CYR_UP_4    = 0x3339785c3064785c,   // Г
        CYR_UP_5    = 0x3439785c3064785c,   // Д
        CYR_UP_6    = 0x3539785c3064785c,   // Е
        CYR_UP_7    = 0x3138785c3064785c,   // Ё
        CYR_UP_8    = 0x3639785c3064785c,   // Ж
        CYR_UP_9    = 0x3739785c3064785c,   // З
        CYR_UP_10   = 0x3839785c3064785c,   // И
        CYR_UP_11   = 0x3939785c3064785c,   // Й
        CYR_UP_12   = 0x6139785c3064785c,   // К
        CYR_UP_13   = 0x6239785c3064785c,   // Л
        CYR_UP_14   = 0x6339785c3064785c,   // М
        CYR_UP_15   = 0x6439785c3064785c,   // Н
        CYR_UP_16   = 0x6539785c3064785c,   // О
        CYR_UP_17   = 0x6639785c3064785c,   // П
        CYR_UP_18   = 0x3061785c3064785c,   // Р
        CYR_UP_19   = 0x3161785c3064785c,   // С
        CYR_UP_20   = 0x3261785c3064785c,   // Т
        CYR_UP_21   = 0x3361785c3064785c,   // У
        CYR_UP_22   = 0x3461785c3064785c,   // Ф
        CYR_UP_23   = 0x3561785c3064785c,   // Х
        CYR_UP_24   = 0x3661785c3064785c,   // Ц
        CYR_UP_25   = 0x3761785c3064785c,   // Ч
        CYR_UP_26   = 0x3861785c3064785c,   // Ш
        CYR_UP_27   = 0x3961785c3064785c,   // Щ
        CYR_UP_28   = 0x6161785c3064785c,   // Ъ
        CYR_UP_29   = 0x6261785c3064785c,   // Ы
        CYR_UP_30   = 0x6361785c3064785c,   // Ь
        CYR_UP_31   = 0x6461785c3064785c,   // Э
        CYR_UP_32   = 0x6561785c3064785c,   // Ю
        CYR_UP_33   = 0x6661785c3064785c,   // Я

        CYR_LOW_1   = 0x3062785c3064785c,   // а
        CYR_LOW_2   = 0x3162785c3064785c,   // б
        CYR_LOW_3   = 0x3262785c3064785c,   // в
        CYR_LOW_4   = 0x3362785c3064785c,   // г
        CYR_LOW_5   = 0x3462785c3064785c,   // д
        CYR_LOW_6   = 0x3562785c3064785c,   // е
        CYR_LOW_7   = 0x3139785c3164785c,   // ё
        CYR_LOW_8   = 0x3662785c3064785c,   // ж
        CYR_LOW_9   = 0x3762785c3064785c,   // з
        CYR_LOW_10  = 0x3862785c3064785c,   // и
        CYR_LOW_11  = 0x3962785c3064785c,   // й
        CYR_LOW_12  = 0x6162785c3064785c,   // к
        CYR_LOW_13  = 0x6262785c3064785c,   // л
        CYR_LOW_14  = 0x6362785c3064785c,   // м
        CYR_LOW_15  = 0x6462785c3064785c,   // н
        CYR_LOW_16  = 0x6562785c3064785c,   // о
        CYR_LOW_17  = 0x6662785c3064785c,   // п
        CYR_LOW_18  = 0x3038785c3164785c,   // р
        CYR_LOW_19  = 0x3138785c3164785c,   // с
        CYR_LOW_20  = 0x3238785c3164785c,   // т
        CYR_LOW_21  = 0x3338785c3164785c,   // у
        CYR_LOW_22  = 0x3438785c3164785c,   // ф
        CYR_LOW_23  = 0x3538785c3164785c,   // х
        CYR_LOW_24  = 0x3638785c3164785c,   // ц
        CYR_LOW_25  = 0x3738785c3164785c,   // ч
        CYR_LOW_26  = 0x3838785c3164785c,   // ш
        CYR_LOW_27  = 0x3938785c3164785c,   // щ
        CYR_LOW_28  = 0x6138785c3164785c,   // ъ
        CYR_LOW_29  = 0x6238785c3164785c,   // ы
        CYR_LOW_30  = 0x6338785c3164785c,   // ь
        CYR_LOW_31  = 0x6438785c3164785c,   // э
        CYR_LOW_32  = 0x6538785c3164785c,   // ю
        CYR_LOW_33  = 0x6638785c3164785c    // я
    };

public:
    enum WIFI_STATUS
    {
        WIFI_INTERFACE_DISABLED = 0,
        WIFI_SCANNING           = 1,
        WIFI_ASSOCIATING        = 2,
        WIFI_COMPLETED          = 3,
        WIFI_DISCONNECTED       = 4,
        WIFI_INITIALIZATION     = 5,

        WIFI_UNKNOWN_STATE
    };

    struct Wifi_Data
    {
        string  bssid;      // MAC-адрес сети
        int32_t freq;       // частота сети
        int32_t level;      // уровень сигнала
        string  flags;      // флаги
        string  ssid;       // название
        WIFI_SEC_TYPE sec;  // тип шифрования
    };

private:
    vector<Wifi_Data> wifi_networks;    // список сетей

    mutex wifi_mutex;   // mutex для доступа к wpa_supplicant

    WIFI_STATUS state = WIFI_INITIALIZATION;    // статус состояния

    string wpa_cli = "";    // Путь до wpa_cli
    string exp_ld  = "";

    /**
     * @brief create_cfg создание конфигурационного файла wpa_supplicant.conf
     * @param data структура с информацией о подключаемой сети
     * @param password пароль сети
     * @return true в случае успеха, иначе false
     */
    bool create_cfg(const Wifi_Data &data, string password);

    /**
     * @brief splitString разделение строки по символам
     * @param divisibleStr разделяемая строка
     * @param delimiter разделитель
     * @return вектор строк, если невозможно разделить - пустой вектор
     */
    vector<string> splitString(string divisibleStr, char delimiter);

    /**
     * @brief is_wpa_run проверка работы wpa_supplicant
     * @return
     */
    int8_t is_wpa_run();

    /**
     * @brief convert_cyrillic конвертер кириллицы из wpa_cli в IBM-866
     * @param ssid название сети
     * @return сконвертированная строка с названием
     */
    string convert_cyrillic(string ssid);

    /**
     * @brief status статус работы wi-fi
     * @return значение из enum WIFI_STATUS
     */
    WIFI_STATUS status();

public:
    Fisgo_Wifi();
    ~Fisgo_Wifi();

    /**
     * @brief savePassword
     * @return
     */
    bool savePassword(string bssid, string password);

    /**
     * @brief init запуск  wpa_supplicant с параметрами из wpa-supplicant.conf
     * @return true в случае запущенного wpa_supplicant, иначе false
     */
    bool init();

    /**
     * @brief deinit останов wpa_supplicant
     */
    void deinit();

    /**
     * @brief clear очистить БД
     * @return true в случае успеха, иначе false
     */
    bool clear();

    /**
     * @brief scan_net сканировать сети
     * @return true в случае успешного поиска, иначе false
     */
    bool scan_net();

    /**
     * @brief get_status статус работы wi-fi
     * @return значение из enum WIFI_STATUS
     */
    WIFI_STATUS get_status();

    /**
     * @brief request_status запрос текущего статуса (опрос wpa_cli)
     * @return значение из enum WIFI_STATUS
     */
    WIFI_STATUS request_status();

    /**
     * @brief create_def_cfg создание дефолтного конфига wpa_supplicant.conf
     * @return true в случае успешного создания, иначе false
     */
    bool create_def_cfg();

    /**
     * @brief is_ip_available проверка получения ip
     * @return true в случае установленного ip, иначе false
     */
    bool is_ip_available();

    /**
     * @brief connect подключение к сети wi-fi
     * @param idNet индекс выбранной сети из вектора найденных сетей
     * @param password пароль сети
     * @return true в случае успешного подключения к сети, иначе false
     */
    bool connect( uint8_t idNet, string password );

    /**
     * @brief request_ip попытка получения IP
     */
    void request_ip();

    /**
     * @brief turn_on поднять интерфейс wlan0
     */
    void turn_on();

    /**
     * @brief turn_off опустить интерфейс wlan0
     */
    void turn_off();

    /**
     * @brief getWifi_networks вернуть вектор найденных сетей
     * @return вектор структур с данными по каждой найденной сети
     */
    vector<Wifi_Data> getWifi_networks() const;

    /**
     * @brief getPassword получение пароля сети по BSSID
     * @param bssid MAC-адрес роутера
     * @return пароль сети или пустую строку
     */
    string getPassword(string bssid);
};

#endif  /* LIB_FISGO_WIFI_H_ */
