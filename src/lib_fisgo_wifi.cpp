#include "lib_fisgo_wifi.h"


Fisgo_Wifi::Fisgo_Wifi()
{
    string sbin_wpa_cli  = "/usr/sbin/wpa_cli";
    string local_wpa_cli = "/usr/local/bin/wpa_cli";

    if ( access(sbin_wpa_cli.c_str(), F_OK) != -1 )
    {
        wpa_cli = sbin_wpa_cli;
    }
    else if ( access(local_wpa_cli.c_str(), F_OK) != -1 )
    {
        wpa_cli = local_wpa_cli;
    }

#ifdef DREAMKAS_F
    exp_ld = "export LD_LIBRARY_PATH=/usr/local/lib/ && ";
#endif

    Fisgo_Wifi_DB::instance();
}

Fisgo_Wifi::~Fisgo_Wifi()
{
    Fisgo_Wifi_DB::releaseInstance();
}

string Fisgo_Wifi::convert_cyrillic(string ssid)
{
    uint32_t copy_size      = 0;                // размер байт для копирования
    uint32_t rest           = ssid.size();      // остаток байт
    const uint8_t size_code = sizeof(uint64_t); // размер русского символа

    string tmp = "";

    for ( uint32_t index = 0; index < ssid.size(); )
    {
        uint64_t code = 0;

        if ( size_code < rest )
        {
            copy_size = size_code;
        }
        else
        {
            copy_size = rest;
        }

        memcpy(&code, &ssid[index], copy_size);

        uint64_t ibm1 = code & CYR_TEST_1;
        uint64_t ibm2 = code & CYR_TEST_2;

        if ( (ibm1 == CYR_TEST_1) || (ibm2 == CYR_TEST_2) )
        {
            switch ( code )
            {
            case CYR_UP_1 : tmp.append("А"); break;
            case CYR_UP_2 : tmp.append("Б"); break;
            case CYR_UP_3 : tmp.append("В"); break;
            case CYR_UP_4 : tmp.append("Г"); break;
            case CYR_UP_5 : tmp.append("Д"); break;
            case CYR_UP_6 : tmp.append("Е"); break;
            case CYR_UP_7 : tmp.append("Ё"); break;
            case CYR_UP_8 : tmp.append("Ж"); break;
            case CYR_UP_9 : tmp.append("З"); break;
            case CYR_UP_10: tmp.append("И"); break;
            case CYR_UP_11: tmp.append("Й"); break;
            case CYR_UP_12: tmp.append("К"); break;
            case CYR_UP_13: tmp.append("Л"); break;
            case CYR_UP_14: tmp.append("М"); break;
            case CYR_UP_15: tmp.append("Н"); break;
            case CYR_UP_16: tmp.append("О"); break;
            case CYR_UP_17: tmp.append("П"); break;
            case CYR_UP_18: tmp.append("Р"); break;
            case CYR_UP_19: tmp.append("С"); break;
            case CYR_UP_20: tmp.append("Т"); break;
            case CYR_UP_21: tmp.append("У"); break;
            case CYR_UP_22: tmp.append("Ф"); break;
            case CYR_UP_23: tmp.append("Х"); break;
            case CYR_UP_24: tmp.append("Ц"); break;
            case CYR_UP_25: tmp.append("Ч"); break;
            case CYR_UP_26: tmp.append("Ш"); break;
            case CYR_UP_27: tmp.append("Щ"); break;
            case CYR_UP_28: tmp.append("Ъ"); break;
            case CYR_UP_29: tmp.append("Ы"); break;
            case CYR_UP_30: tmp.append("Ь"); break;
            case CYR_UP_31: tmp.append("Э"); break;
            case CYR_UP_32: tmp.append("Ю"); break;
            case CYR_UP_33: tmp.append("Я"); break;

            case CYR_LOW_1 : tmp.append("а"); break;
            case CYR_LOW_2 : tmp.append("б"); break;
            case CYR_LOW_3 : tmp.append("в"); break;
            case CYR_LOW_4 : tmp.append("г"); break;
            case CYR_LOW_5 : tmp.append("д"); break;
            case CYR_LOW_6 : tmp.append("е"); break;
            case CYR_LOW_7 : tmp.append("ё"); break;
            case CYR_LOW_8 : tmp.append("ж"); break;
            case CYR_LOW_9 : tmp.append("з"); break;
            case CYR_LOW_10: tmp.append("и"); break;
            case CYR_LOW_11: tmp.append("й"); break;
            case CYR_LOW_12: tmp.append("к"); break;
            case CYR_LOW_13: tmp.append("л"); break;
            case CYR_LOW_14: tmp.append("м"); break;
            case CYR_LOW_15: tmp.append("н"); break;
            case CYR_LOW_16: tmp.append("о"); break;
            case CYR_LOW_17: tmp.append("п"); break;
            case CYR_LOW_18: tmp.append("р"); break;
            case CYR_LOW_19: tmp.append("с"); break;
            case CYR_LOW_20: tmp.append("т"); break;
            case CYR_LOW_21: tmp.append("у"); break;
            case CYR_LOW_22: tmp.append("ф"); break;
            case CYR_LOW_23: tmp.append("х"); break;
            case CYR_LOW_24: tmp.append("ц"); break;
            case CYR_LOW_25: tmp.append("ч"); break;
            case CYR_LOW_26: tmp.append("ш"); break;
            case CYR_LOW_27: tmp.append("щ"); break;
            case CYR_LOW_28: tmp.append("ъ"); break;
            case CYR_LOW_29: tmp.append("ы"); break;
            case CYR_LOW_30: tmp.append("ь"); break;
            case CYR_LOW_31: tmp.append("э"); break;
            case CYR_LOW_32: tmp.append("ю"); break;
            case CYR_LOW_33: tmp.append("я"); break;
            default: break;
            }

            index += CYR_SIZE;
            rest -= CYR_SIZE;
        }
        else
        {
            tmp.push_back( ssid.at(index) );
            ++index;
            --rest;
        }
    }

    return tmp;
}

bool Fisgo_Wifi::scan_net()
{
    lock_guard<mutex> locker(wifi_mutex);

    const uint8_t TRY_COUNT_SCAN = 10;

    string result = "";
    for ( uint8_t tryCount = 0; tryCount != TRY_COUNT_SCAN; ++tryCount )
    {
        string scan = exp_ld + wpa_cli + " -i wlan0 scan > /FisGo/wifi/wifi_scan";
        system( scan.c_str() );
        usleep(100000);

        ifstream fileScan;
        fileScan.open( "/FisGo/wifi/wifi_scan" );
        if ( fileScan.is_open() )
        {
            getline( fileScan,  result);
            fileScan.close();
            remove("/FisGo/wifi/wifi_scan");
        }

        if ( result.compare("OK") == 0 )
        {
            break;
        }
        else
        {
            sleep(1);
        }
    }

    string scan_result = exp_ld + wpa_cli + " -i wlan0 scan_results > /FisGo/wifi/wifi_scan_results";
    system( scan_result.c_str() );
    usleep(100000);

    wifi_networks.clear();

    ifstream file;
    file.open( "/FisGo/wifi/wifi_scan_results" );

    if ( file.is_open() )
    {
        string str;
        while ( getline( file,  str) )
        {
            Wifi_Data data;
            vector<string> list = splitString(str, '\t');

            if ( list.size() < 5 )
            {
                continue;
            }

            data.bssid = list.at(0);
            data.freq  = stoi( list.at(1), nullptr, 10);
            data.level = stoi( list.at(2), nullptr, 10);
            data.flags = list.at(3);
            data.ssid  = list.at(4);

            #ifdef DREAMKAS_RF
            // перевод имени сети в читаемый вид при наличии кириллических символов
            data.ssid  = convert_cyrillic( data.ssid );
            #endif

            if ( data.ssid.empty() )
            {
                continue;
            }

            bool skip = false;
            for ( Wifi_Data wifiName : wifi_networks )
            {
                if ( wifiName.ssid.compare( data.ssid ) == 0 )
                {
                    skip = true;
                    break;
                }
            }

            if ( skip )
            {
                continue;
            }

            if ( !data.flags.empty() )
            {
                if ( data.flags.find("WPA-PSK") != string::npos )
                {
                    data.sec = WIFI_SEC_WPA;
                }
                else if ( data.flags.find("WPA2-PSK") != string::npos )
                {
                    data.sec = WIFI_SEC_WPA2;
                }
                else if ( data.flags.find("WEP") != string::npos )
                {
                    data.sec = WIFI_SEC_WEP;
                }
                else if ( data.flags.find("EAP") != string::npos )
                {
                    data.sec = WIFI_SEC_EAP;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            wifi_networks.push_back( data );
        }

        file.close();
        remove("/FisGo/wifi/wifi_scan_results");
    }
    else
    {
        return false;
    }

    return true;
}

Fisgo_Wifi::WIFI_STATUS Fisgo_Wifi::get_status()
{
    return state;
}

Fisgo_Wifi::WIFI_STATUS Fisgo_Wifi::request_status()
{
    return status();
}

bool Fisgo_Wifi::create_cfg(const Wifi_Data &data, string password)
{
    ofstream wpa_conf;
    wpa_conf.open("/etc/wpa_supplicant.conf", ofstream::trunc);

    if ( wpa_conf.is_open() )
    {
        wpa_conf << "ctrl_interface=/var/run/wpa_supplicant" << endl;
        wpa_conf << "network={" << endl;
        #ifdef DREAMKAS_RF
        wpa_conf << "bssid=" << data.bssid << ""<< endl;
        #else
        wpa_conf << "ssid=\"" << data.ssid << "\""<< endl;
        #endif
        wpa_conf << "psk=\"" << password << "\""<< endl;
        wpa_conf << "}" << endl;
        wpa_conf.flush();
        wpa_conf.close();

        sync();
        sync();

        return true;
    }

    return false;
}

bool Fisgo_Wifi::create_def_cfg()
{
    ofstream wpa_conf;
    wpa_conf.open("/etc/wpa_supplicant.conf", ofstream::trunc);

    if ( wpa_conf.is_open() )
    {
        wpa_conf << "ctrl_interface=/var/run/wpa_supplicant" << endl;
        wpa_conf << "network={" << endl;
        #ifdef DREAMKAS_RF
        wpa_conf << "bssid=BE:EF:13:00:00:00"<< endl;
        #else
        wpa_conf << "ssid=\"DreamkasGuest\""<< endl;
        #endif
        wpa_conf << "psk=\"12345678\""<< endl;
        wpa_conf << "}" << endl;
        wpa_conf.flush();
        wpa_conf.close();

        sync();
        sync();

        return true;
    }

    return false;
}

int8_t Fisgo_Wifi::is_wpa_run()
{
    int8_t err = -1;

    for ( int8_t ret = 5; ret > 0; --ret )
    {
        system("pidof wpa_supplicant > /FisGo/wifi/wpa_supp_pid");
        usleep(100000);

        ifstream pid("/FisGo/wifi/wpa_supp_pid");

        if ( pid.is_open() )
        {
            string pid_str;
            getline( pid, pid_str );
            pid.close();
            remove("/FisGo/wifi/wpa_supp_pid");

            if ( !pid_str.empty() )
            {
                err = 0;
                break;
            }
            else
            {
                continue;
            }
        }
        else
        {
            continue;
        }
    }

    return err;
}

bool Fisgo_Wifi::savePassword(string bssid, string password)
{
    return Fisgo_Wifi_DB::instance()->savePassword( bssid, password );
}

string Fisgo_Wifi::getPassword(string bssid)
{
    return Fisgo_Wifi_DB::instance()->getPassword( bssid );
}

bool Fisgo_Wifi::init()
{
    state = WIFI_INITIALIZATION;

    deinit();
    turn_on();

    int8_t err = -1;

    for ( uint8_t try_count = 0; try_count < 2; ++try_count )
    {

#ifdef DREAMKAS_F
        system("wpa_supplicant -Dwext -iwlan0 -c/etc/wpa_supplicant.conf &");
#else
        system("wpa_supplicant -Dnl80211 -iwlan0 -c/etc/wpa_supplicant.conf &");
#endif
        usleep(100000);

        err = is_wpa_run();

        if ( err == -1 )
        {
            create_def_cfg();
        }
        else
        {
            break;
        }
    }

    sleep(5);

    // обновление текущего статуса wi-fi
    status();

    if ( err == -1 )
    {
        return false;
    }

    return true;
}

void Fisgo_Wifi::deinit()
{
    system("pidof wpa_supplicant > /FisGo/wifi/wpa_supp_pid");
    usleep(100000);

    ifstream pid("/FisGo/wifi/wpa_supp_pid");

    if ( pid.is_open() )
    {
        string pid_str;
        getline( pid, pid_str );
        pid.close();

        if ( !pid_str.empty() )
        {
            system("killall wpa_supplicant");
        }
    }

    remove("/FisGo/wifi/wpa_supp_pid");
}

bool Fisgo_Wifi::clear()
{
    return Fisgo_Wifi_DB::instance()->clearDB();
}

Fisgo_Wifi::WIFI_STATUS Fisgo_Wifi::status()
{
    string state_wpa_cli = exp_ld +  wpa_cli + " status | grep wpa_state | awk -F \"=\" '{print $2}' > /FisGo/wifi/wpa_state";
    system( state_wpa_cli.c_str() );
    usleep(100000);

    ifstream wpa_state("/FisGo/wifi/wpa_state");

    if ( wpa_state.is_open() )
    {
        string wpa_state_str;
        getline( wpa_state, wpa_state_str );
        wpa_state.close();
        remove("/FisGo/wifi/wpa_state");

        if ( wpa_state_str.compare("INTERFACE_DISABLED") == 0 )
        {
            state = WIFI_INTERFACE_DISABLED;
        }
        else if ( wpa_state_str.compare("SCANNING") == 0 )
        {
            state = WIFI_SCANNING;
        }
        else if ( wpa_state_str.compare("ASSOCIATING") == 0 )
        {
            state = WIFI_ASSOCIATING;
        }
        else if ( wpa_state_str.compare("COMPLETED") == 0 )
        {
            state = WIFI_COMPLETED;
        }
        else if ( wpa_state_str.compare("DISCONNECTED") == 0 )
        {
            state = WIFI_DISCONNECTED;
        }
        else
        {
            state = WIFI_UNKNOWN_STATE;
        }
    }
    else
    {
        state = WIFI_UNKNOWN_STATE;
    }

    return state;
}

bool Fisgo_Wifi::is_ip_available()
{
    string state_ip = exp_ld + wpa_cli + " status | grep ip_address | awk -F \"=\" '{print $2}' > /FisGo/wifi/wpa_ip_available";
    system( state_ip.c_str() );
    usleep(100000);

    ifstream ip("/FisGo/wifi/wpa_ip_available");

    if ( ip.is_open() )
    {
        string available_ip;
        getline( ip, available_ip );
        ip.close();
        remove("/FisGo/wifi/wpa_ip_available");

        if ( !available_ip.empty() )
        {
            return true;
        }
    }

    return false;
}

bool Fisgo_Wifi::connect(uint8_t idNet, string password)
{
    lock_guard<mutex> locker(wifi_mutex);

    if ( is_wpa_run() == 0 )
    {
        deinit();
    }

    if ( wifi_networks.size() < idNet )
    {
        return false;
    }

    if ( create_cfg( wifi_networks.at(idNet), password ) == false )
    {
        return false;
    }

    if ( init() == false )
    {
        return false;
    }

    // попытка получения адреса к последней подключенной сети
    // 4 попытки получения с интервалом в 3 секунды
    system("udhcpc -n -t 6 -i wlan0 -q >> /dev/null");

    if ( is_ip_available() == false )
    {
        return false;
    }

    // Запрос нового статуса
    status();

    savePassword( wifi_networks.at(idNet).bssid, password );

    return true;
}

void Fisgo_Wifi::request_ip()
{
    if ( wifi_mutex.try_lock() == true )
    {
        if ( state != WIFI_INITIALIZATION )
        {
            // сеть подключена, нет IP
            if ( is_ip_available() == false )
            {
                // попытка получения адреса к последней подключенной сети
                // 4 попытки получения с интервалом в 3 секунды
                system("udhcpc -n -t 6 -i wlan0 -q >> /dev/null");

                // обновление текущего статуса wi-fi
                status();
            }
        }

        wifi_mutex.unlock();
    }
}

void Fisgo_Wifi::turn_on()
{
    system("ifconfig wlan0 up");
    sleep(3);
}

void Fisgo_Wifi::turn_off()
{
    system("ifconfig wlan0 0.0.0.0");
    system("ifconfig wlan0 down");
    sleep(3);
}

vector<Fisgo_Wifi::Wifi_Data> Fisgo_Wifi::getWifi_networks() const
{
    return wifi_networks;
}

vector<string> Fisgo_Wifi::splitString(string divisibleStr, char delimiter)
{
    vector<string> rv;

    if ( divisibleStr.empty() )
        return rv;

    string tmpString;
    istringstream streamStr(divisibleStr);

    while ( getline(streamStr, tmpString, delimiter) )
    {
       rv.push_back( tmpString );
    }

    return rv;
}
