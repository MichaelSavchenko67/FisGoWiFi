#ifndef LIB_FISGO_WIFI_DB_H_
#define LIB_FISGO_WIFI_DB_H_

#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>

#include <mutex>
#include <string>
#include <iostream>

#include <sqlite3.h>

using namespace std;

class Fisgo_Wifi_DB
{

    static Fisgo_Wifi_DB *_wifi_db;

private:
    sqlite3 *wifiDB;
    mutex mutexDB;

    bool isWifiDirExist();

    /**
     * @brief createDB ᮧ����� ⠡���� ��� �࠭���� ��஫��
     * @return true � ��砥 �ᯥ�, ���� false
     */
    bool createDB();

    Fisgo_Wifi_DB();
    ~Fisgo_Wifi_DB();

    Fisgo_Wifi_DB(Fisgo_Wifi_DB const&) = delete;
    void operator=(Fisgo_Wifi_DB const&) = delete;

public:
    /**
     * @brief instance ����祭�� ����㯪� � ��
     * @return 㪠��⥫� ��� ࠡ��� � ��
     */
    static Fisgo_Wifi_DB* instance();

    /**
     * @brief releaseInstance �᢮�������� ����ᮢ
     */
    static void releaseInstance();

    /**
     * @brief savePassword ��࠭���� ��஫� � ��
     * @param bssid MAC-���� ����
     * @param password ��஫� ��
     * @return true � ��砥 �ᯥ�, ���� false
     */
    bool savePassword(string bssid, string password);

    /**
     * @brief getPassword ����祭�� ��஫� �� BSSID ����
     * @param bssid MAC-���� ����
     * @return ��஫� �� � ��砥 ������, ���� ������ ��ப�
     */
    string getPassword(string bssid);

    /**
     * @brief clearDB ���⪠ ���� ��஫��
     * @return true � ��砥 �ᯥ�, ���� false
     */
    bool clearDB();
};

#endif /* LIB_FISGO_WIFI_DB_H_ */
