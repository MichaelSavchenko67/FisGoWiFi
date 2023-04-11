#include "lib_fisgo_wifi_db.h"

Fisgo_Wifi_DB* Fisgo_Wifi_DB::_wifi_db = nullptr;

/**
 * @brief sqlite_hash создание hash в sqlite3
 * @param context контекст вызова функции
 * @param argc количество аргументов при вызове в SQL
 * @param values значение переданного в функцию параметра
 */
static void sqlite_hash(sqlite3_context *context, int argc, sqlite3_value **values)
{
    if ( argc != 1 )
    {
        sqlite3_result_error(context, "SQLite invalid argument!", -1);
        return;
    }

    string strVal = string(reinterpret_cast<const char*>( sqlite3_value_text(values[0]) ));

    hash<string> strHash;
    int64_t rv = strHash(strVal);

    sqlite3_result_int64(context, rv);
}

Fisgo_Wifi_DB *Fisgo_Wifi_DB::instance()
{
    if ( _wifi_db == nullptr )
    {
        _wifi_db = new Fisgo_Wifi_DB();
    }

    return _wifi_db;
}

void Fisgo_Wifi_DB::releaseInstance()
{
    if ( _wifi_db != nullptr )
    {
        delete _wifi_db;

        _wifi_db = nullptr;
    }
}

Fisgo_Wifi_DB::Fisgo_Wifi_DB()
{
    createDB();
}

Fisgo_Wifi_DB::~Fisgo_Wifi_DB()
{

}

bool Fisgo_Wifi_DB::isWifiDirExist()
{
    string dirWifi = "/FisGo/wifi";
    struct stat statWifiDir;

    if ( stat(dirWifi.c_str(), &statWifiDir) != 0 )
    {
        if ( mkdir("wifi", ACCESSPERMS) != 0 )
        {
            return false;
        }
    }

    return true;
}

bool Fisgo_Wifi_DB::createDB()
{
    if ( isWifiDirExist() == false )
    {
        return false;
    }

    lock_guard<mutex> locker(mutexDB);

    int rc = 0;

    rc = sqlite3_open("/FisGo/wifi/wifiDB.db", &wifiDB);

    if ( rc != SQLITE_OK )
    {
        return false;
    }

    string createTable = "CREATE TABLE IF NOT EXISTS PWD_LOCKER("
                         "BSSID_HASH INT PRIMARY KEY NOT NULL,"
                         "PWD        TEXT NOT NULL);";

    rc = sqlite3_exec(wifiDB, createTable.c_str(), 0, nullptr, nullptr);

    if ( rc != SQLITE_OK )
    {
        sqlite3_close( wifiDB );
        return false;
    }

    string createIndex = "CREATE UNIQUE INDEX IF NOT EXISTS BSSID_INDEX ON PWD_LOCKER(BSSID_HASH);";

    rc = sqlite3_exec(wifiDB, createIndex.c_str(), 0, nullptr, nullptr);

    if ( rc != SQLITE_OK )
    {
        sqlite3_close( wifiDB );
        return false;
    }

    sqlite3_close( wifiDB );

    return true;
}

bool Fisgo_Wifi_DB::savePassword(string bssid, string password)
{
    lock_guard<mutex> locker(mutexDB);

    int rc = 0;

    rc = sqlite3_open("/FisGo/wifi/wifiDB.db", &wifiDB);

    if ( rc != SQLITE_OK )
    {
        return false;
    }

    rc = sqlite3_create_function(wifiDB, "hash", 1, SQLITE_ANY, 0, &sqlite_hash, 0, 0);
    if (rc != SQLITE_OK)
    {
        sqlite3_close( wifiDB );
        return false;
    }

    string insertQuery = "INSERT OR REPLACE INTO PWD_LOCKER VALUES(HASH('" + bssid + "'), \"" + password + "\");";

    rc = sqlite3_exec(wifiDB, insertQuery.c_str(), 0, nullptr, nullptr);

    if ( rc != SQLITE_OK )
    {
        sqlite3_close( wifiDB );
        return false;
    }

    sqlite3_close( wifiDB );

    return true;
}

string Fisgo_Wifi_DB::getPassword(string bssid)
{
    lock_guard<mutex> locker(mutexDB);

    string pwd = "";
    int rc = 0;

    rc = sqlite3_open("/FisGo/wifi/wifiDB.db", &wifiDB);

    if ( rc != SQLITE_OK )
    {
        return pwd;
    }

    rc = sqlite3_create_function(wifiDB, "hash", 1, SQLITE_ANY, 0, &sqlite_hash, 0, 0);
    if (rc != SQLITE_OK)
    {
        sqlite3_close( wifiDB );
        return pwd;
    }

    sqlite3_stmt *statement;
    string querySelect = "SELECT PWD FROM PWD_LOCKER WHERE BSSID_HASH=HASH('" + bssid + "');";

    rc = sqlite3_prepare(wifiDB, querySelect.c_str(), -1, &statement, NULL);

    if (rc != SQLITE_OK)
    {
        sqlite3_finalize( statement );
        sqlite3_close( wifiDB );
        return pwd;
    }
    else
    {
        if ( sqlite3_step(statement) == SQLITE_ROW )
        {
            pwd = string(reinterpret_cast<const char *>( sqlite3_column_text(statement, 0) ));
        }

        sqlite3_finalize( statement );
    }

    sqlite3_close( wifiDB );

    return pwd;
}

bool Fisgo_Wifi_DB::clearDB()
{
    lock_guard<mutex> locker(mutexDB);

    int rc = 0;

    rc = sqlite3_open("/FisGo/wifi/wifiDB.db", &wifiDB);

    if ( rc != SQLITE_OK )
    {
        return false;
    }

    string insertQuery = "DELETE * FROM PWD_LOCKER;";

    rc = sqlite3_exec(wifiDB, insertQuery.c_str(), 0, nullptr, nullptr);

    if ( rc != SQLITE_OK )
    {
        sqlite3_close( wifiDB );
        return false;
    }

    sqlite3_close( wifiDB );

    return true;
}
