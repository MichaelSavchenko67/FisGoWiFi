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
        CYR_SIZE    = 0x08,                 // ����� ��ਫ���᪮�� ᨬ����

        CYR_TEST_1  = 0x85c3064785c,        // ��� ��� ��稭��� ��ਫ���᪨� ᨬ���
        CYR_TEST_2  = 0x85c3164785c,        // ��� ��� ��稭��� ��ਫ���᪨� ᨬ���

        CYR_UP_1    = 0x3039785c3064785c,   // �
        CYR_UP_2    = 0x3139785c3064785c,   // �
        CYR_UP_3    = 0x3239785c3064785c,   // �
        CYR_UP_4    = 0x3339785c3064785c,   // �
        CYR_UP_5    = 0x3439785c3064785c,   // �
        CYR_UP_6    = 0x3539785c3064785c,   // �
        CYR_UP_7    = 0x3138785c3064785c,   // �
        CYR_UP_8    = 0x3639785c3064785c,   // �
        CYR_UP_9    = 0x3739785c3064785c,   // �
        CYR_UP_10   = 0x3839785c3064785c,   // �
        CYR_UP_11   = 0x3939785c3064785c,   // �
        CYR_UP_12   = 0x6139785c3064785c,   // �
        CYR_UP_13   = 0x6239785c3064785c,   // �
        CYR_UP_14   = 0x6339785c3064785c,   // �
        CYR_UP_15   = 0x6439785c3064785c,   // �
        CYR_UP_16   = 0x6539785c3064785c,   // �
        CYR_UP_17   = 0x6639785c3064785c,   // �
        CYR_UP_18   = 0x3061785c3064785c,   // �
        CYR_UP_19   = 0x3161785c3064785c,   // �
        CYR_UP_20   = 0x3261785c3064785c,   // �
        CYR_UP_21   = 0x3361785c3064785c,   // �
        CYR_UP_22   = 0x3461785c3064785c,   // �
        CYR_UP_23   = 0x3561785c3064785c,   // �
        CYR_UP_24   = 0x3661785c3064785c,   // �
        CYR_UP_25   = 0x3761785c3064785c,   // �
        CYR_UP_26   = 0x3861785c3064785c,   // �
        CYR_UP_27   = 0x3961785c3064785c,   // �
        CYR_UP_28   = 0x6161785c3064785c,   // �
        CYR_UP_29   = 0x6261785c3064785c,   // �
        CYR_UP_30   = 0x6361785c3064785c,   // �
        CYR_UP_31   = 0x6461785c3064785c,   // �
        CYR_UP_32   = 0x6561785c3064785c,   // �
        CYR_UP_33   = 0x6661785c3064785c,   // �

        CYR_LOW_1   = 0x3062785c3064785c,   // �
        CYR_LOW_2   = 0x3162785c3064785c,   // �
        CYR_LOW_3   = 0x3262785c3064785c,   // �
        CYR_LOW_4   = 0x3362785c3064785c,   // �
        CYR_LOW_5   = 0x3462785c3064785c,   // �
        CYR_LOW_6   = 0x3562785c3064785c,   // �
        CYR_LOW_7   = 0x3139785c3164785c,   // �
        CYR_LOW_8   = 0x3662785c3064785c,   // �
        CYR_LOW_9   = 0x3762785c3064785c,   // �
        CYR_LOW_10  = 0x3862785c3064785c,   // �
        CYR_LOW_11  = 0x3962785c3064785c,   // �
        CYR_LOW_12  = 0x6162785c3064785c,   // �
        CYR_LOW_13  = 0x6262785c3064785c,   // �
        CYR_LOW_14  = 0x6362785c3064785c,   // �
        CYR_LOW_15  = 0x6462785c3064785c,   // �
        CYR_LOW_16  = 0x6562785c3064785c,   // �
        CYR_LOW_17  = 0x6662785c3064785c,   // �
        CYR_LOW_18  = 0x3038785c3164785c,   // �
        CYR_LOW_19  = 0x3138785c3164785c,   // �
        CYR_LOW_20  = 0x3238785c3164785c,   // �
        CYR_LOW_21  = 0x3338785c3164785c,   // �
        CYR_LOW_22  = 0x3438785c3164785c,   // �
        CYR_LOW_23  = 0x3538785c3164785c,   // �
        CYR_LOW_24  = 0x3638785c3164785c,   // �
        CYR_LOW_25  = 0x3738785c3164785c,   // �
        CYR_LOW_26  = 0x3838785c3164785c,   // �
        CYR_LOW_27  = 0x3938785c3164785c,   // �
        CYR_LOW_28  = 0x6138785c3164785c,   // �
        CYR_LOW_29  = 0x6238785c3164785c,   // �
        CYR_LOW_30  = 0x6338785c3164785c,   // �
        CYR_LOW_31  = 0x6438785c3164785c,   // �
        CYR_LOW_32  = 0x6538785c3164785c,   // �
        CYR_LOW_33  = 0x6638785c3164785c    // �
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
        string  bssid;      // MAC-���� ��
        int32_t freq;       // ���� ��
        int32_t level;      // �஢��� ᨣ����
        string  flags;      // 䫠��
        string  ssid;       // ��������
        WIFI_SEC_TYPE sec;  // ⨯ ��஢����
    };

private:
    vector<Wifi_Data> wifi_networks;    // ᯨ᮪ �⥩

    mutex wifi_mutex;   // mutex ��� ����㯠 � wpa_supplicant

    WIFI_STATUS state = WIFI_INITIALIZATION;    // ����� ���ﭨ�

    string wpa_cli = "";    // ���� �� wpa_cli
    string exp_ld  = "";

    /**
     * @brief create_cfg ᮧ����� ���䨣��樮����� 䠩�� wpa_supplicant.conf
     * @param data ������� � ���ଠ樥� � ������砥��� ��
     * @param password ��஫� ��
     * @return true � ��砥 �ᯥ�, ���� false
     */
    bool create_cfg(const Wifi_Data &data, string password);

    /**
     * @brief splitString ࠧ������� ��ப� �� ᨬ�����
     * @param divisibleStr ࠧ���塞�� ��ப�
     * @param delimiter ࠧ����⥫�
     * @return ����� ��ப, �᫨ ���������� ࠧ������ - ���⮩ �����
     */
    vector<string> splitString(string divisibleStr, char delimiter);

    /**
     * @brief is_wpa_run �஢�ઠ ࠡ��� wpa_supplicant
     * @return
     */
    int8_t is_wpa_run();

    /**
     * @brief convert_cyrillic �������� ��ਫ���� �� wpa_cli � IBM-866
     * @param ssid �������� ��
     * @return ᪮�����஢����� ��ப� � ���������
     */
    string convert_cyrillic(string ssid);

    /**
     * @brief status ����� ࠡ��� wi-fi
     * @return ���祭�� �� enum WIFI_STATUS
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
     * @brief init �����  wpa_supplicant � ��ࠬ��ࠬ� �� wpa-supplicant.conf
     * @return true � ��砥 ����饭���� wpa_supplicant, ���� false
     */
    bool init();

    /**
     * @brief deinit ��⠭�� wpa_supplicant
     */
    void deinit();

    /**
     * @brief clear ������ ��
     * @return true � ��砥 �ᯥ�, ���� false
     */
    bool clear();

    /**
     * @brief scan_net ᪠��஢��� ��
     * @return true � ��砥 �ᯥ譮�� ���᪠, ���� false
     */
    bool scan_net();

    /**
     * @brief get_status ����� ࠡ��� wi-fi
     * @return ���祭�� �� enum WIFI_STATUS
     */
    WIFI_STATUS get_status();

    /**
     * @brief request_status ����� ⥪�饣� ����� (���� wpa_cli)
     * @return ���祭�� �� enum WIFI_STATUS
     */
    WIFI_STATUS request_status();

    /**
     * @brief create_def_cfg ᮧ����� ��䮫⭮�� ���䨣� wpa_supplicant.conf
     * @return true � ��砥 �ᯥ譮�� ᮧ�����, ���� false
     */
    bool create_def_cfg();

    /**
     * @brief is_ip_available �஢�ઠ ����祭�� ip
     * @return true � ��砥 ��⠭��������� ip, ���� false
     */
    bool is_ip_available();

    /**
     * @brief connect ������祭�� � �� wi-fi
     * @param idNet ������ ��࠭��� �� �� ����� ��������� �⥩
     * @param password ��஫� ��
     * @return true � ��砥 �ᯥ譮�� ������祭�� � ��, ���� false
     */
    bool connect( uint8_t idNet, string password );

    /**
     * @brief request_ip ����⪠ ����祭�� IP
     */
    void request_ip();

    /**
     * @brief turn_on ������� ����䥩� wlan0
     */
    void turn_on();

    /**
     * @brief turn_off ������� ����䥩� wlan0
     */
    void turn_off();

    /**
     * @brief getWifi_networks ������ ����� ��������� �⥩
     * @return ����� ������� � ����묨 �� ������ ��������� ��
     */
    vector<Wifi_Data> getWifi_networks() const;

    /**
     * @brief getPassword ����祭�� ��஫� �� �� BSSID
     * @param bssid MAC-���� ����
     * @return ��஫� �� ��� ������ ��ப�
     */
    string getPassword(string bssid);
};

#endif  /* LIB_FISGO_WIFI_H_ */
