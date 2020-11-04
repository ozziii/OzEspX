#include <wifi_o.h>

void wifi_o::Begin()
{
    this->_state = STATE_STA_NOT_CONNECTED;
    this->espname = SETTING_P(PSTR("espname"));

    String SSID1 = SETTING_P(PSTR("WIFI1SSID"));
    if (SSID1.length() > 0)
    {
        String PASS = SETTING_P(PSTR("WIFI1PASS"));
        this->add_new_network(SSID1, PASS);
    }

    String SSID2 = SETTING_P(PSTR("WIFI2SSID"));
    if (SSID2.length() > 0)
    {
        String PASS = SETTING_P(PSTR("WIFI2PASS"));
        this->add_new_network(SSID2, PASS);
    }

    String SSID3 = SETTING_P(PSTR("WIFI3SSID"));
    if (SSID3.length() > 0)
    {
        String PASS = SETTING_P(PSTR("WIFI3PASS"));
        this->add_new_network(SSID3, PASS);
    }
}

bool wifi_o::loop()
{
    switch (_state)
    {

    case STATE_STA_CONNECTION:
    {
        this->_startSTA();
        this->_change_state(STATE_STA_CONNECTING);
        return false;
    }
    break;

    case STATE_STA_CONNECTING:
    {
        switch (WiFi.status())
        {
        case WL_CONNECT_FAILED:
            this->_change_state(STATE_STA_FAILED);
            return false;
        case WL_CONNECTED:
            WiFi.setAutoConnect(true);
            WiFi.setAutoReconnect(true);
            this->_change_state(STATE_STA_CONNECTED);
#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[WIFI] CONNECTED!! MY IP: %s \n"), WiFi.localIP().toString().c_str());
#endif
            return true;
        case WL_CONNECTION_LOST:
            break;
        }

        if (this->_timeout + WIFI_O_STA_CONNECTING_TIMEOUT < millis())
            this->_change_state(STATE_STA_FAILED);

        return false;
    }
    break;

    case STATE_STA_CONNECTED:
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        else
        {
#ifdef DEBUG_ERROR
            DEBUG_MSG_P(PSTR("[WIFI] CONNECTION LOST! \n"));
#endif
            this->_change_state(STATE_STA_NOT_CONNECTED);
        }
    }
    break;

    case STATE_STA_NOT_CONNECTED:
    {
        WiFi.disconnect();
        WiFi.enableSTA(false);
        WiFi.enableAP(false);
        this->_sta_connection_try = 0;
        this->_change_state(STATE_SCAN_START);
    }
    break;

    case STATE_STA_FAILED:
    {
        if (this->_sta_connection_try < WIFI_O_STA_CONNECTION_TRY)
        {
#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[WIFI] CONNECTION FAILED! NEW TRY %d \n"), this->_sta_connection_try + 1);
#endif
            this->_sta_connection_try++;
            this->_change_state(STATE_STA_CONNECTION);
        }
        else
        {
#ifdef DEBUG_ERROR
            DEBUG_MSG_P(PSTR("[WIFI] CONNECTION FAILED! NO MORE TRY: START ACCESS POINT\n"));
#endif
            this->_change_state(STATE_AP_CREATE);
        }
    }
    break;

    case STATE_AP_CREATE:
    {
        if (this->_startAP())
        {
            this->_change_state(STATE_AP_OK);
        }
        else
        {
            this->_change_state(STATE_AP_FAILED);
        }
    }
    break;

    case STATE_AP_OK:
    {
        if (this->_timeout + WIFI_O_AP_TIMEOUT < millis())
            this->_change_state(STATE_STA_NOT_CONNECTED);
    }
    break;

    case STATE_AP_FAILED:
    {
        this->_change_state(STATE_STA_NOT_CONNECTED);
    }
    break;

    case STATE_SCAN_START:
    {
#ifdef DEBUG_LOG
        DEBUG_MSG_P(PSTR("[WIFI] WIFI SCAN BEGIN... \n"));
#endif
        this->_startSCAN();
        this->_change_state(STATE_SCANIING);
    }
    break;

    case STATE_SCANIING:
    {
        if (this->_timeout + WIFI_O_SCANNING_TIMEOUT < millis())
            this->_change_state(STATE_SCAN_FAILED);

        int8_t scanResult = WiFi.scanComplete();

        if (scanResult == WIFI_SCAN_FAILED)
            this->_change_state(STATE_SCAN_FAILED);

        if (scanResult >= 0)
            this->_change_state(STATE_SCAN_COMPLETE);
    }
    break;

    case STATE_SCAN_COMPLETE:
    {
        int16_t scanResult = WiFi.scanComplete();

        if (scanResult > 0)
        {
#ifdef DEBUG_LOG
        DEBUG_MSG_P(PSTR("[WIFI] SCAN FINISH WHIT %d NETWORK FOUD \n"),scanResult);
#endif
            if (this->_populate_network(scanResult))
            {
                this->_change_state(STATE_STA_CONNECTION);
                return false;
            }
            else
            {
#ifdef DEBUG_ERROR
                DEBUG_MSG_P(PSTR("[WIFI] NO NETWORK KNOWN START ACCESS POINT \n"));
#endif
                this->_change_state(STATE_SCAN_NO_NETWORK);
            }
        }
        else
        {
#ifdef DEBUG_LOG
            DEBUG_MSG_P(PSTR("[WIFI] NO NETWORK FOUND START ACCESS POINT \n"));
#endif
            this->_change_state(STATE_SCAN_NO_NETWORK);
        }
    }
    break;

    case STATE_SCAN_NO_NETWORK:
    {
        this->_change_state(STATE_AP_CREATE);
    }
    break;

    case STATE_SCAN_FAILED:
    {
        this->_change_state(STATE_AP_CREATE);
    }
    break;
    }

    return false;
}

void wifi_o::_change_state(wifi_o_states_t state)
{
    this->_timeout = millis();
    this->_state = state;

#ifdef WIFI_O_DEBUG

    switch (_state)
    {

    case STATE_STA_CONNECTION:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: STA_CONNECTION \n"));
        break;

    case STATE_STA_CONNECTING:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: STA_CONNECTING \n"));
        break;

    case STATE_STA_CONNECTED:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: STA_CONNECTED\n"));
        break;

    case STATE_STA_NOT_CONNECTED:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: STA_NOT_CONNECTED\n"));
        break;

    case STATE_STA_FAILED:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: STA_FAILED\n"));
        break;

    case STATE_AP_CREATE:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: AP_CREATE \n"));
        break;

    case STATE_AP_OK:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: AP_OK\n"));
        break;

    case STATE_AP_FAILED:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: AP_FAILED \n"));
        break;

    case STATE_SCAN_START:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: SCAN_START\n"));
        break;

    case STATE_SCANIING:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: SCANIING\n"));
        break;

    case STATE_SCAN_COMPLETE:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: SCAN_COMPLETE\n"));
        break;

    case STATE_SCAN_NO_NETWORK:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: SCAN_NO_NETWORK\n"));
        break;

    case STATE_SCAN_FAILED:
        DEBUG_MSG_P(PSTR("[WIFI] STATE: SCAN_FAILED\n"));
        break;
    };

#endif
}

void wifi_o::add_new_network(String SSID, String PASSWORD)
{
    network_t new_network;
    new_network.ssid = SSID;
    new_network.pass = PASSWORD;
    new_network.scanned = false;
    //new_network.ip = nullptr;
    //new_network.gw = nullptr;
    //new_network.netmask = nullptr;
    //new_network.dns = nullptr;
    new_network.rssi = 0;
    new_network.security = WIFI_AUTH_OPEN;
    new_network.channel = 0;
    this->_network_list.push_back(new_network);
}

bool wifi_o::_populate_network(uint8_t networkCount)
{
    String ssid_scan;
    int32_t rssi_scan;
    uint8_t sec_scan;
    uint8_t *BSSID_scan;
    int32_t chan_scan;
    bool hidden_scan;

    bool found_any_know_network = false;

    for (uint8_t i = 0; i < networkCount; ++i)
    {
#ifdef ESP32
        WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan);
#else
        WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);
#endif

        bool known = false;

        for (uint8_t j = 0; j < this->_network_list.size(); j++)
        {
            network_t *entry = &_network_list[j];

            if (ssid_scan.equals(entry->ssid))
            {
                // In case of several networks with the same SSID
                // we want to get the one with the best RSS
                if (!entry->scanned || entry->rssi < rssi_scan)
                {
                    entry->rssi = rssi_scan;
                    entry->security = sec_scan;
                    entry->scanned = true;
                    entry->channel = chan_scan;
                    memcpy((void *)&entry->bssid, (void *)BSSID_scan, sizeof(entry->bssid));
                }

                known = true;
                found_any_know_network = true;
                break;
            }
        }

#ifdef DEBUG_LOG
        {
            DEBUG_MSG_P(PSTR("%s BSSID: %02X:%02X:%02X:%02X:%02X:%02X CH: %2d RSSI: %3d SSID: %s \n"),
                        (known ? "-->" : "   "),
                        BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5],
                        chan_scan,
                        rssi_scan,
                        ssid_scan.c_str());
        }
#endif
    }

    WiFi.scanDelete();

    return found_any_know_network;
}

void wifi_o::_startSTA()
{
    WiFi.enableSTA(true);
    // FOUD BEST NETWORK
    int8_t network_number = -1;
    int32_t best_network_rssi = -1000;

    for (uint8_t j = 0; j < this->_network_list.size(); j++)
    {
        network_t *entry = &(this->_network_list[j]);

        if (entry->scanned && entry->rssi > best_network_rssi)
        {
            best_network_rssi = entry->rssi;
            network_number = j;
        }
    }

    // NO NETWORK FOUND
    if (network_number < 0)
    {
#ifdef WIFI_O_DEBUG
        DEBUG_MSG_P(PSTR("[WIFI] START STA NO NETWORK FOUD \n"));
#endif
        return;
    }

    // GET BEST NETWORK
    network_t network = this->_network_list[network_number];

#ifdef ESP32
    WiFi.setHostname(this->espname.c_str());
#else
    WiFi.hostname(this->espname);
#endif

    //if (network.ip != nullptr)
    //{
    //    WiFi.config(network.ip, network.gw, network.netmask, network.dns);
    //}

#ifdef DEBUG_LOG
    DEBUG_MSG_P(PSTR("[WIFI] TRY TO CONNECT SSID: %s \n"), network.ssid.c_str());
#endif

    if (network.channel == 0)
    {
        WiFi.begin(network.ssid.c_str(), network.pass.c_str());
    }
    else
    {
        WiFi.begin(network.ssid.c_str(), network.pass.c_str(), network.channel, network.bssid);
    }

#ifdef MAX_POWER_WIFI
#ifdef ESP32
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
#else
    WiFi.setOutputPower(20.5);
#endif
#endif
}

void wifi_o::_startSCAN()
{
    for (uint8_t j = 0; j < this->_network_list.size(); j++)
    {
        this->_network_list[j].scanned = false;
    }

    WiFi.disconnect();
    WiFi.enableSTA(true);
    WiFi.scanNetworks(true, true);
}

bool wifi_o::_startAP()
{
    WiFi.disconnect();
    WiFi.enableSTA(false);
    WiFi.enableAP(true);
    return WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
}

bool wifi_o::connected()
{
    return WiFi.isConnected();
}