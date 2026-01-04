/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022, Ha Thach (tinyusb.org)
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

#ifndef _TUSB_DEBUG_H_
#define _TUSB_DEBUG_H_

#include "CTRACE.h"

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------+
// Debug
//--------------------------------------------------------------------+

// CFG_TUSB_DEBUG for debugging
// 0 : no debug
// 1 : print error
// 2 : print warning
// 3 : print info
#if CFG_TUSB_DEBUG

// Enum to String for debugging purposes
#if CFG_TUSB_DEBUG >= CFG_TUH_LOG_LEVEL || CFG_TUSB_DEBUG >= CFG_TUD_LOG_LEVEL
extern char const* const tu_str_speed[];
extern char const* const tu_str_std_request[];
extern char const* const tu_str_xfer_result[];
#endif

void tu_print_mem(void const *buf, uint32_t count, uint8_t indent);

#ifdef CFG_TUSB_DEBUG_PRINTF
  extern int CFG_TUSB_DEBUG_PRINTF(const char *format, ...);
  #define tu_printf    CFG_TUSB_DEBUG_PRINTF
#else
	#define tu_printf			vUSB_TRACE_PutStringInfo
	#define tu_printfc			vUSB_TRACE_PutString
#endif

static inline void tu_print_buf(uint8_t const* buf, uint32_t bufsize) {
  for(uint32_t i=0; i<bufsize; i++) tu_printf("%02X ", buf[i]);
  tu_printf("\r\n");
}


#define TU_LOG0              vUSB_TRACE_PutStringErr
#define TU_LOG1              vUSB_TRACE_PutStringErr
#define TU_LOG2              vUSB_TRACE_PutStringWar
#define TU_LOG3              vUSB_TRACE_PutStringInfo
#define TU_LOG1D             vUSBD_TRACE_PutStringErr
#define TU_LOG2D             vUSBD_TRACE_PutStringWar
#define TU_LOG3D             vUSBD_TRACE_PutStringInfo
#define TU_LOG1H             vUSBH_TRACE_PutStringErr
#define TU_LOG2H             vUSBH_TRACE_PutStringWar
#define TU_LOG3H             vUSBH_TRACE_PutStringInfo

#define TU_LOG(n, ...)        TU_XSTRCAT(TU_LOG, n)(__VA_ARGS__)
#define TU_LOG_S(n,s, ...)    TU_XSTRCAT3(TU_LOG, n, s)(__VA_ARGS__)
#define TU_LOG_LOCATION()     tu_printf("%s: %d:\r\n", __PRETTY_FUNCTION__, __LINE__)
#define TU_LOG_FAILED()       tu_printf("%s: %d: Failed\r\n", __PRETTY_FUNCTION__, __LINE__)

#define TU_LOG_MEM(_x, _n, _k)	vUSB_TRACE_PutBuffer((uint8_t const*)(_x), _n)
#define TU_LOG_BUF(_x, _n)   	vUSB_TRACE_PutBuffer((uint8_t const*)(_x), _n)
#define TU_LOG_INT(_x)       	vUSB_TRACE_PutString(#_x " = %ld\r\n", (unsigned long) (_x) )
#define TU_LOG_HEX(_x)       	vUSB_TRACE_PutString(#_x " = 0x%lX\r\n", (unsigned long) (_x) )


typedef struct {
  uint32_t key;
  const char* data;
} tu_lookup_entry_t;

typedef struct {
  uint16_t count;
  tu_lookup_entry_t const* items;
} tu_lookup_table_t;

static inline const char* tu_lookup_find(tu_lookup_table_t const* p_table, uint32_t key) {
  tu_static char not_found[11];

  for(uint16_t i=0; i<p_table->count; i++) {
    if (p_table->items[i].key == key) return p_table->items[i].data;
  }

  // not found return the key value in hex
  snprintf(not_found, sizeof(not_found), "0x%08lX", (unsigned long) key);

  return not_found;
}

#endif // CFG_TUSB_DEBUG

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_DEBUG_H_ */
