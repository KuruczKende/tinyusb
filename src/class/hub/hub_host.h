/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

#ifndef TUSB_HUB_HOST_H_
#define TUSB_HUB_HOST_H_

#include "hub.h"

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------+
// HUB API
//--------------------------------------------------------------------+

// Clear port feature
bool hubh_port_clear_feature(uint8_t hub_addr, uint8_t hub_port, uint8_t feature,
                            tuh_xfer_cb_t complete_cb, uintptr_t user_data);

// Set port feature
bool hubh_port_set_feature(uint8_t hub_addr, uint8_t hub_port, uint8_t feature,
                          tuh_xfer_cb_t complete_cb, uintptr_t user_data);

// Get port status
bool hubh_port_get_status(uint8_t hub_addr, uint8_t hub_port, void *resp,
                         tuh_xfer_cb_t complete_cb, uintptr_t user_data);

// Get status from Interrupt endpoint
bool hubh_edpt_status_xfer(uint8_t daddr);

// Reset a port
TU_ATTR_ALWAYS_INLINE static inline
bool hubh_port_reset(uint8_t hub_addr, uint8_t hub_port, tuh_xfer_cb_t complete_cb, uintptr_t user_data) {
  return hubh_port_set_feature(hub_addr, hub_port, HUB_FEATURE_PORT_RESET, complete_cb, user_data);
}

// Clear Port Reset Change
TU_ATTR_ALWAYS_INLINE static inline
bool hubh_port_clear_reset_change(uint8_t hub_addr, uint8_t hub_port, tuh_xfer_cb_t complete_cb, uintptr_t user_data) {
  return false; // hub_port_clear_feature(hub_addr, hub_port, HUB_FEATURE_PORT_RESET_CHANGE, complete_cb, user_data);
}

// Get Hub status
TU_ATTR_ALWAYS_INLINE static inline
bool hubh_get_status(uint8_t hub_addr, void* resp, tuh_xfer_cb_t complete_cb, uintptr_t user_data) {
  return hubh_port_get_status(hub_addr, 0, resp, complete_cb, user_data);
}

// Clear Hub feature
TU_ATTR_ALWAYS_INLINE static inline
bool hubh_clear_feature(uint8_t hub_addr, uint8_t feature, tuh_xfer_cb_t complete_cb, uintptr_t user_data) {
  return hubh_port_clear_feature(hub_addr, 0, feature, complete_cb, user_data);
}
//--------------------------------------------------------------------+
// Internal Class Driver API
//--------------------------------------------------------------------+
bool hubh_init       (void);
bool hubh_deinit     (void);
bool hubh_open       (uint8_t rhport, uint8_t dev_addr, tusb_desc_interface_t const *itf_desc, uint16_t max_len);
bool hubh_set_config (uint8_t dev_addr, uint8_t itf_num);
bool hubh_xfer_cb    (uint8_t daddr, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes);
void hubh_close      (uint8_t dev_addr);

#ifdef __cplusplus
 }
#endif

#endif
