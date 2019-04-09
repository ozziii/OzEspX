#include <wifi_o.h>

#ifdef DEBUG

static void infoWifi()
{

    if (WiFi.isConnected())
    {

        uint8_t *bssid = WiFi.BSSID();

        DEBUG_MSG_P(PSTR("[WIFI] MODE STA -------------------------------------\n"));
        DEBUG_MSG_P(PSTR("[WIFI] SSID  %s\n"), WiFi.SSID().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] BSSID %02X:%02X:%02X:%02X:%02X:%02X\n"),
                    bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
        DEBUG_MSG_P(PSTR("[WIFI] CH    %d\n"), WiFi.channel());
        DEBUG_MSG_P(PSTR("[WIFI] RSSI  %d\n"), WiFi.RSSI());
        DEBUG_MSG_P(PSTR("[WIFI] IP    %s\n"), WiFi.localIP().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] MAC   %s\n"), WiFi.macAddress().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] GW    %s\n"), WiFi.gatewayIP().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] MASK  %s\n"), WiFi.subnetMask().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] DNS   %s\n"), WiFi.dnsIP().toString().c_str());
#ifdef ESP32
        DEBUG_MSG_P(PSTR("[WIFI] HOST  %s\n"), WiFi.getHostname());
#else
        DEBUG_MSG_P(PSTR("[WIFI] HOST  %s\n"), WiFi.hostname().c_str());
#endif
        DEBUG_MSG_P(PSTR("[WIFI] ----------------------------------------------\n"));
    }

    if (WiFi.getMode() & WIFI_AP)
    {

        DEBUG_MSG_P(PSTR("[WIFI] MODE AP --------------------------------------\n"));
        DEBUG_MSG_P(PSTR("[WIFI] SSID  %s\n"), stringjoin(WIFI_AP_SSID, SETTING_P(PSTR("espname"))));
        DEBUG_MSG_P(PSTR("[WIFI] IP    %s\n"), WiFi.softAPIP().toString().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] MAC   %s\n"), WiFi.softAPmacAddress().c_str());
        DEBUG_MSG_P(PSTR("[WIFI] ----------------------------------------------\n"));
    }
}

static void infoCallback(justwifi_messages_t code, char *parameter)
{

    // -------------------------------------------------------------------------

    if (code == MESSAGE_TURNING_OFF)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Turning OFF\n"));
    }

    if (code == MESSAGE_TURNING_ON)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Turning ON\n"));
    }

    // -------------------------------------------------------------------------

    if (code == MESSAGE_SCANNING)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Scanning\n"));
    }

    if (code == MESSAGE_SCAN_FAILED)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Scan failed\n"));
    }

    if (code == MESSAGE_NO_NETWORKS)
    {
        DEBUG_MSG_P(PSTR("[WIFI] No networks found\n"));
    }

    if (code == MESSAGE_NO_KNOWN_NETWORKS)
    {
        DEBUG_MSG_P(PSTR("[WIFI] No known networks found\n"));
    }

    if (code == MESSAGE_FOUND_NETWORK)
    {
        DEBUG_MSG_P(PSTR("[WIFI] %s\n"), parameter);
    }

    // -------------------------------------------------------------------------

    if (code == MESSAGE_CONNECTING)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Connecting to %s\n"), parameter);
    }

    if (code == MESSAGE_CONNECT_WAITING)
    {
        // too much noise
    }

    if (code == MESSAGE_CONNECT_FAILED)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Could not connect to %s\n"), parameter);
    }

    if (code == MESSAGE_CONNECTED)
    {
        infoWifi();
    }

    if (code == MESSAGE_DISCONNECTED)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Disconnected\n"));
    }

    // -------------------------------------------------------------------------

    if (code == MESSAGE_ACCESSPOINT_CREATED)
    {
        infoWifi();
    }

    if (code == MESSAGE_ACCESSPOINT_DESTROYED)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Disconnecting access point\n"));
    }

    if (code == MESSAGE_ACCESSPOINT_CREATING)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Creating access point\n"));
    }

    if (code == MESSAGE_ACCESSPOINT_FAILED)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Could not create access point\n"));
    }

    // ------------------------------------------------------------------------

    if (code == MESSAGE_WPS_START)
    {
        DEBUG_MSG_P(PSTR("[WIFI] WPS started\n"));
    }

    if (code == MESSAGE_WPS_SUCCESS)
    {
        DEBUG_MSG_P(PSTR("[WIFI] WPS succeded!\n"));
    }

    if (code == MESSAGE_WPS_ERROR)
    {
        DEBUG_MSG_P(PSTR("[WIFI] WPS failed\n"));
    }

    // ------------------------------------------------------------------------

    if (code == MESSAGE_SMARTCONFIG_START)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Smart Config started\n"));
    }

    if (code == MESSAGE_SMARTCONFIG_SUCCESS)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Smart Config succeded!\n"));
    }

    if (code == MESSAGE_SMARTCONFIG_ERROR)
    {
        DEBUG_MSG_P(PSTR("[WIFI] Smart Config failed\n"));
    }
};
#endif

void wifi_o::Begin()
{

    // SET WIFI POWER
    // WiFi.setOutputPower(30);

    String espname = SETTING_P(PSTR("espname"));

    js.setHostname(espname.c_str());
#ifdef DEBUG
    js.subscribe(infoCallback);
#endif
    js.setSoftAP(stringjoin(WIFI_AP_SSID, espname).c_str(), WIFI_AP_PASS); //TODO Add espname to SSID
    js.enableAPFallback(true);

    js.enableSTA(true);
    js.enableScan(true);
    js.cleanNetworks();

    String SSID1 = SETTING_P(PSTR("WIFI1SSID"));
    String PASS1 = SETTING_P(PSTR("WIFI1PASS"));

    if (SSID1.length() > 0 && PASS1.length() > 0)
    {
        js.addNetwork(SSID1.c_str(), PASS1.c_str());
    }

    String SSID2 = SETTING_P(PSTR("WIFI2SSID"));
    String PASS2 = SETTING_P(PSTR("WIFI2PASS"));

    if (SSID2.length() > 0 && PASS2.length() > 0)
    {
        js.addNetwork(SSID2.c_str(), PASS2.c_str());
    }

    String SSID3 = SETTING_P(PSTR("WIFI3SSID"));
    String PASS3 = SETTING_P(PSTR("WIFI3PASS"));

    if (SSID3.length() > 0 && PASS3.length() > 0)
    {
        js.addNetwork(SSID3.c_str(), PASS3.c_str());
    }
}

bool wifi_o::loop()
{
    js.loop();

    if (js.connectable())
    {
        if (lastcheck + WIFI_AP_STA_DELAY < millis())
        {
            js.enableAP(false);
            js.reset();
            lastcheck = millis();
        }
    }
    else
    {
        if (js.getStatus() == WL_CONNECTED)
        {
            return true;
        }
    }

    return false;
}

bool wifi_o::connected()
{
    if (js.getStatus() == WL_CONNECTED && !js.connectable())
    {
        return true;
    }
    return false;
}