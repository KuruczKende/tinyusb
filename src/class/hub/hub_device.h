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

#ifndef TUSB_HUB_DEVICE_H_
#define TUSB_HUB_DEVICE_H_

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
 bool boIsHubReq(uint8_t bRequest);
//--------------------------------------------------------------------+
// Application Callbacks
//--------------------------------------------------------------------+
 uint8_t const * tud_hub_descriptor_report_cb();

//--------------------------------------------------------------------+
// Internal Class Driver API
//--------------------------------------------------------------------+
void     hubd_init            (void);
bool     hubd_deinit          (void);
void     hubd_reset           (uint8_t rhport);
uint16_t hubd_open            (uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len);
bool     hubd_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
bool     hubd_xfer_cb         (uint8_t rhport, uint8_t ep_addr, xfer_result_t event, uint32_t xferred_bytes);

#ifdef __cplusplus
 }
#endif

#endif
