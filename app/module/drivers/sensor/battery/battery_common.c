/*
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <math.h>
#include <zephyr/drivers/sensor.h>

#include "battery_common.h"

int battery_channel_get(const struct battery_value *value, enum sensor_channel chan,
                        struct sensor_value *val_out) {
    switch (chan) {
    case SENSOR_CHAN_GAUGE_VOLTAGE:
        val_out->val1 = value->millivolts / 1000;
        val_out->val2 = (value->millivolts % 1000) * 1000U;
        break;

    case SENSOR_CHAN_GAUGE_STATE_OF_CHARGE:
        val_out->val1 = value->state_of_charge;
        val_out->val2 = 0;
        break;

    default:
        return -ENOTSUP;
    }

    return 0;
}

uint8_t lithium_ion_mv_to_pct(int16_t bat_mv) {
    // Map the measured cell voltage onto Adafruit's LiPo discharge curve.
    double pct = 106.818 + (-0.032685 - 106.818) /
                               pow(1 + pow((double)bat_mv / 3679.35, 58.979), 0.347386);

    if (pct <= 0) {
        return 0;
    } else if (pct >= 100) {
        return 100;
    }

    return (uint8_t)round(pct);
}
