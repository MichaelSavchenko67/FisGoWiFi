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
     * @brief createDB создание таблицы для хранения паролей
     * @return true в случае успеха, иначе false
     */
    bool createDB();

    Fisgo_Wifi_DB();
    ~Fisgo_Wifi_DB();

    Fisgo_Wifi_DB(Fisgo_Wifi_DB const&) = delete;
    void operator=(Fisgo_Wifi_DB const&) = delete;

public:
    /**
     * @brief instance получение доступка к БД
     * @return указатель для работы с БД
     */
    static Fisgo_Wifi_DB* instance();

    /**
     * @brief releaseInstance освобождение ресурсов
     */
    static void releaseInstance();

    /**
     * @brief savePassword сохранение пароля в БД
     * @param bssid MAC-адрес роутера
     * @param password пароль сети
     * @return true в случае успеха, иначе false
     */
    bool savePassword(string bssid, string password);

    /**
     * @brief getPassword получение пароля по BSSID роутера
     * @param bssid MAC-адрес роутера
     * @return пароль сети в случае наличия, иначе пустую строку
     */
    string getPassword(string bssid);

    /**
     * @brief clearDB очистка базы паролей
     * @return true в случае успеха, иначе false
     */
    bool clearDB();
};

#endif /* LIB_FISGO_WIFI_DB_H_ */
