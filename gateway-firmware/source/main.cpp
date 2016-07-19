/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include <sys/time.h>
#include "ble/BLE.h"
#include "ble/DiscoveredCharacteristic.h"
#include "ble/DiscoveredService.h"
#include "security.h" // get this from https://connector.mbed.com/#credentials
#include "mbed-client-ethernet-c-style/client.h"

#define SCAN_INT  0x30 // 30 ms = 48 * 0.625 ms
#define SCAN_WIND 0x30 // 30 ms = 48 * 0.625 ms

#define LIMIT_SYNC_SEC          5   // Only sync once per 5s.

static Serial &pc = get_stdio_serial();

static DigitalOut green(LED_GREEN);

// so we got this device list
std::string device_list[] = {
    "db91f0f56813",
    "e1f44f7e25f1"
};

std::map<std::string, time_t> last_synced;

std::string get_temp_string(std::string addr)
{
    std::stringstream temp;
    temp << addr;
    temp << "/temperature";
    return temp.str().c_str();
}

std::string get_rssi_string(std::string addr)
{
    std::stringstream rssi;
    rssi << addr;
    rssi << "/rssi";
    return rssi.str().c_str();
}

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    (void)params;
    pc.printf("disconnected\n\r");
}

void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params)
{
    // ex: 02 01 06 05 ff 13 37 09 2e
    if (params->advertisingDataLen < 9) return;

    if (params->advertisingData[5] != 0x13 || params->advertisingData[6] != 0x37) return;

    char mac[12];
    sprintf(mac, "%02x%02x%02x%02x%02x%02x",
        params->peerAddr[5], params->peerAddr[4], params->peerAddr[3], params->peerAddr[2], params->peerAddr[1], params->peerAddr[0]);

    string addr(mac);

    struct timeval tvp;
    gettimeofday(&tvp, NULL);

    if (last_synced.count(addr) == 0 || last_synced[addr] < tvp.tv_sec - LIMIT_SYNC_SEC) {
        pc.printf("mac = %s, rssi = %d, temp = %d\n", mac, params->rssi, (params->advertisingData[7] << 8) + params->advertisingData[8]);

        mbed_client_set(get_rssi_string(addr), params->rssi);
        mbed_client_set(get_temp_string(addr), (params->advertisingData[7] << 8) + params->advertisingData[8]);

        last_synced[addr] = tvp.tv_sec;
    }
}

/**
 * This function is called when the ble initialization process has failled
 */
void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Initialization error handling should go here */
}

/**
 * Callback triggered when the ble initialization process has finished
 */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    pc.printf("bleInitComplete\n");

    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        /* In case of error, forward the error handling to onBleInitError */
        onBleInitError(ble, error);
        return;
    }

    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    // Set BT Address
    // ble.gap().setAddress(BLEProtocol::AddressType::PUBLIC, BLE_address_BE);

    ble.gap().setScanParams(SCAN_INT, SCAN_WIND);
    ble.gap().startScan(advertisementCallback);
}

void registered()
{
    green = 0;

    printf("registered!\n");

    BLE::Instance().init(bleInitComplete);
}

void app_start(int, char**)
{
    green = 1;

    pc.printf("in app_start\n");

    for (size_t ix = 0; ix < sizeof(device_list) / sizeof(device_list[0]); ix++) {
        mbed_client_define_resource(get_rssi_string(device_list[ix]), 0, M2MBase::GET_ALLOWED, true);
        mbed_client_define_resource(get_temp_string(device_list[ix]), 0, M2MBase::GET_ALLOWED, true);
    }

    pc.printf("so now gonna setup i guess\n");

    struct MbedClientOptions opts = mbed_client_get_default_options();
    opts.DeviceType = "BLE_Watch_Gateway";

    bool setup = mbed_client_setup(opts);
    if (!setup) {
        pc.printf("Setup failed (e.g. couldn't get IP address. Check serial output.\r\n");
        return;
    }
    mbed_client_on_registered(&registered);
}
