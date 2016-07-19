#include "mbed.h"
#include "ble.h"
#include "nrf_soc.h" // for internal Thermo sensor

static BLE* ble;

/**
 * @brief A very quick conversion between a float temperature and 11073-20601 FLOAT-Type.
 * @param temperature The temperature as a float.
 * @return The temperature in 11073-20601 FLOAT-Type format.
 */
uint32_t quick_ieee11073_from_float(float temperature)
{
    uint8_t  exponent = 0xFE; //exponent is -2
    uint32_t mantissa = (uint32_t)(temperature*100);

    return ( ((uint32_t)exponent) << 24) | mantissa;
}

uint32_t get_temperature(void) {
    int32_t p_temp;
    sd_temp_get(&p_temp);
    float temperature = float(p_temp)/4.;
    temperature -= 5.75;

    return int(temperature * 100);
}

void update_adv_packet(void) {
    if (!ble) return;

    uint32_t temp = get_temperature();
    printf("update_adv_packet %d\n", temp);
    uint8_t raw[] = { 0x13, 0x37, (temp >> 8) & 0xff, temp & 0xff };
    ble->gap().accumulateAdvertisingPayload(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, raw, 4);
}

void onBleInitError(BLE &ble, ble_error_t error)
{
    /* Initialization error handling should go here */
    printf("Error %d\n", error);
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error != BLE_ERROR_NONE) {
        onBleInitError(ble, error);
        return;
    }

    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }

    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);

    update_adv_packet();

    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);

    ble.setAdvertisingInterval(160); /* 100ms; in multiples of 0.625ms. */
    ble.startAdvertising();
}

int main(int, char**) {
    ble = &BLE::Instance();
    ble->init(bleInitComplete);

    Ticker t;
    t.attach(&update_adv_packet, 1.0f); // 1s

    while(true) {
        ble->waitForEvent();
    }
}
