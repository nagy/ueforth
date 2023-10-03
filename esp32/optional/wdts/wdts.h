// Copyright 2023 Daniel Nagy
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * ESP32forth WDTS v{{VERSION}}
 * Revision: {{REVISION}}
 */

// More documentation
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/wdts.html

#include <esp_task_wdt.h>
#include <esp_wifi.h>

#include <WiFi.h>
static WiFiClient client;
static WiFiUDP clientudp;

static void WiFiEvent(WiFiEvent_t event){
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
        case ARDUINO_EVENT_WIFI_READY:
            Serial.println("WiFi interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("WiFi client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Serial.println("AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.println("STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Serial.println("Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: break;
    }
}

/* static size_t CLIENTUDPWRITE(const uint8_t *buffer, size_t size) { */
/*   /\* clientudp.beginPacket( {192, 168, 3, 1}, 8888 ); *\/ */
/*   size_t ret = clientudp.write(buffer, size); */
/*   /\* if ( !clientudp.endPacket() ) { *\/ */
/*   /\*   Serial.println("NOT SENT!"); *\/ */
/*   /\* } *\/ */
/*   return ret; */
/* } */

#define OPTIONAL_WDTS_VOCABULARY V(wdts)
#define OPTIONAL_WDTS_SUPPORT \
  XV(internals, "wdts-source", WDTS_SOURCE, \
      PUSH wdts_source; PUSH sizeof(wdts_source) - 1) \
  YV(wdts, wdts_init   , n0 = esp_task_wdt_init(n0, true);) \
  YV(wdts, wdts_reset  , PUSH esp_task_wdt_reset();) \
  YV(wdts, wdts_add    , PUSH esp_task_wdt_add(NULL)) \
  YV(wdts, wdts_delete , PUSH esp_task_wdt_delete(NULL)) \
  YV(wdts, wdts_deinit , PUSH esp_task_wdt_deinit();) \
  YV(wdts, wifiregister, WiFi.onEvent(WiFiEvent)) \
  YV(wdts, wifi6, PUSH esp_wifi_config_80211_tx_rate(WIFI_IF_STA, WIFI_PHY_RATE_6M)) \
  YV(wdts, wifi54, PUSH esp_wifi_config_80211_tx_rate(WIFI_IF_STA, WIFI_PHY_RATE_54M)) \
  YV(wdts, wifi48, PUSH esp_wifi_config_80211_tx_rate(WIFI_IF_STA, WIFI_PHY_RATE_48M)) \
  YV(wdts, wifiram, PUSH esp_wifi_set_storage(WIFI_STORAGE_RAM)) \
  YV(wdts, wifimax, PUSH esp_wifi_config_80211_tx_rate(WIFI_IF_STA, WIFI_PHY_RATE_MAX)) \
  YV(wdts, wifisetpsnone, PUSH esp_wifi_set_ps(WIFI_PS_NONE)) \
  YV(wdts, wifisetpsmin, PUSH esp_wifi_set_ps(WIFI_PS_MIN_MODEM)) \
  YV(wdts, wifisetpsmax, PUSH esp_wifi_set_ps(WIFI_PS_MAX_MODEM)) \
  YV(wdts, udp_begin, n0 = clientudp.begin(n1, n0); NIP) \
  YV(wdts, udp_beginpacket, n0 = clientudp.beginPacket(n1, n0); NIP) \
  YV(wdts, udp_send, n0 = clientudp.write(b1, n0); NIP) \
  YV(wdts, udp_endpacket, PUSH clientudp.endPacket()) \
  YV(wdts, tcp_client_connect, n0 = client.connect(n1, n0); NIP) \
  YV(wdts, tcp_client_nodelay, n0 = client.setNoDelay(n0)) \
  YV(wdts, tcp_client_stop, client.stop()) \
  YV(wdts, tcp_client_flush, client.flush()) \
  YV(wdts, tcp_client_write, n0 = client.write(b1, n0); NIP) \


{{wdts}}
