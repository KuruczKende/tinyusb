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

<<<<<<< Updated upstream
<<<<<<< Updated upstream:src/typec/usbc.h
#ifndef TUSB_UTCD_H_
#define TUSB_UTCD_H_
=======
#ifndef TUSB_HUB_DEVICE_H_
#define TUSB_HUB_DEVICE_H_
>>>>>>> Stashed changes:src/class/hub/hub_device.h
=======
#ifndef TUSB_HUB_DEVICE_H_
#define TUSB_HUB_DEVICE_H_
>>>>>>> Stashed changes

#include "hub.h"

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------+
// Class Driver Default Configure & Validation
//--------------------------------------------------------------------+

#if !defined(CFG_TUD_HUB_EP_BUFSIZE) & defined(CFG_TUD_HUB_BUFSIZE)
  // TODO warn user to use new name later on
  // #warning CFG_TUD_HUB_BUFSIZE is renamed to CFG_TUD_HUB_EP_BUFSIZE, please update to use the new name
  #define CFG_TUD_HUB_EP_BUFSIZE  CFG_TUD_HUB_BUFSIZE
#endif

#ifndef CFG_TUD_HUB_EP_BUFSIZE
  #define CFG_TUD_HUB_EP_BUFSIZE     64
#endif

//--------------------------------------------------------------------+
// Application API
//--------------------------------------------------------------------+

<<<<<<< Updated upstream
<<<<<<< Updated upstream:src/typec/usbc.h
// Init typec stack on a port
bool tuc_init(uint8_t rhport, uint32_t port_type);

// Check if typec port is initialized
bool tuc_inited(uint8_t rhport);

// Task function should be called in main/rtos loop, extended version of tud_task()
// - timeout_ms: millisecond to wait, zero = no wait, 0xFFFFFFFF = wait forever
// - in_isr: if function is called in ISR
void tuc_task_ext(uint32_t timeout_ms, bool in_isr);

// Task function should be called in main/rtos loop
TU_ATTR_ALWAYS_INLINE static inline
void tuc_task (void) {
  tuc_task_ext(UINT32_MAX, false);
}

#ifndef TUSB_TCD_H_
extern void tcd_int_handler(uint8_t rhport);
#endif

// Interrupt handler, name alias to TCD
#define tuc_int_handler tcd_int_handler
=======
=======
>>>>>>> Stashed changes
//--------------------------------------------------------------------+
// Application Callbacks
//--------------------------------------------------------------------+
 uint8_t const * tud_hub_descriptor_report_cb();
<<<<<<< Updated upstream
>>>>>>> Stashed changes:src/class/hub/hub_device.h
=======
>>>>>>> Stashed changes

//--------------------------------------------------------------------+
// Internal Class Driver API
//--------------------------------------------------------------------+
<<<<<<< Updated upstream
<<<<<<< Updated upstream:src/typec/usbc.h

bool tuc_pd_data_received_cb(uint8_t rhport, pd_header_t const* header, uint8_t const* dobj, uint8_t const* p_end);
bool tuc_pd_control_received_cb(uint8_t rhport, pd_header_t const* header);

//--------------------------------------------------------------------+
//
//--------------------------------------------------------------------+

bool tuc_msg_request(uint8_t rhport, void const* rdo);

=======
=======
>>>>>>> Stashed changes
void     hubd_init            (void);
bool     hubd_deinit          (void);
void     hubd_reset           (uint8_t rhport);
uint16_t hubd_open            (uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len);
bool     hubd_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
bool     hubd_xfer_cb         (uint8_t rhport, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes);
<<<<<<< Updated upstream
>>>>>>> Stashed changes:src/class/hub/hub_device.h
=======
>>>>>>> Stashed changes

#ifdef __cplusplus
 }
#endif

#endif
