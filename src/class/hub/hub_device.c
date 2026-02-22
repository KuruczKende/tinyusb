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

#include "tusb_option.h"

#if (CFG_TUD_ENABLED && CFG_TUD_HUB)

//--------------------------------------------------------------------+
// INCLUDE
//--------------------------------------------------------------------+
#include "device/usbd_pvt.h"

#include "hub_device.h"


#if CFG_TUSB_DEBUG >= HUB_DEBUG
static char const* const _hub_feature_str[] = {
  [HUB_FEATURE_PORT_CONNECTION          ] = "PORT_CONNECTION",
  [HUB_FEATURE_PORT_ENABLE              ] = "PORT_ENABLE",
  [HUB_FEATURE_PORT_SUSPEND             ] = "PORT_SUSPEND",
  [HUB_FEATURE_PORT_OVER_CURRENT        ] = "PORT_OVER_CURRENT",
  [HUB_FEATURE_PORT_RESET               ] = "PORT_RESET",
  [HUB_FEATURE_PORT_POWER               ] = "PORT_POWER",
  [HUB_FEATURE_PORT_LOW_SPEED           ] = "PORT_LOW_SPEED",
  [HUB_FEATURE_PORT_CONNECTION_CHANGE   ] = "PORT_CONNECTION_CHANGE",
  [HUB_FEATURE_PORT_ENABLE_CHANGE       ] = "PORT_ENABLE_CHANGE",
  [HUB_FEATURE_PORT_SUSPEND_CHANGE      ] = "PORT_SUSPEND_CHANGE",
  [HUB_FEATURE_PORT_OVER_CURRENT_CHANGE ] = "PORT_OVER_CURRENT_CHANGE",
  [HUB_FEATURE_PORT_RESET_CHANGE        ] = "PORT_RESET_CHANGE",
  [HUB_FEATURE_PORT_TEST                ] = "PORT_TEST",
  [HUB_FEATURE_PORT_INDICATOR           ] = "PORT_INDICATOR",
};
static char const* const _hub_request_str[] = {
  [HUB_REQUEST_GET_STATUS       ] = "GET_STATUS",
  [HUB_REQUEST_CLEAR_FEATURE    ] = "CLEAR_FEATURE",
  [HUB_REQUEST_SET_FEATURE      ] = "SET_FEATURE",
  [HUB_REQUEST_GET_DESCRIPTOR   ] = "GET_DESCRIPTOR",
  [HUB_REQUEST_SET_DESCRIPTOR	] = "SET_DESCRIPTOR",
  [HUB_REQUEST_CLEAR_TT_BUFFER	] = "CLEAR_TT_BUFFER",
  [HUB_REQUEST_RESET_TT         ] = "RESET_TT",
  [HUB_REQUEST_GET_TT_STATE		] = "GET_TT_STATE",
  [HUB_REQUEST_STOP_TT			] = "STOP_TT",
};
#endif


//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF
//--------------------------------------------------------------------+
typedef struct {
	hub_status_response_t sHubState;
	hub_port_status_response_t sHubPortStates[HUB_MAX_PORT_NUM];
} hubd_states;

typedef struct {
	usbd_device_t deviceT;
	uint8_t* deviceDescriptor; // How the config, interface, enpoint is stored?
} whole_device_data;

typedef struct {
  uint8_t ep_in;
  uint8_t ep_out;       // optional Out endpoint

  hubd_states states;

  usbd_device_t devices[HUB_MAX_PORT_NUM];
  uint8_t last_added_port;

  const hub_desc_cs_t *hub_descriptor;
} hubd_interface_t;

typedef struct {
  TUD_EPBUF_DEF(ctrl , CFG_TUD_HUB_EP_BUFSIZE);
  TUD_EPBUF_DEF(epin , CFG_TUD_HUB_EP_BUFSIZE);
  TUD_EPBUF_DEF(epout, CFG_TUD_HUB_EP_BUFSIZE);
} hubd_epbuf_t;

CFG_TUD_MEM_SECTION static hubd_epbuf_t _hubd_epbuf;
static hubd_interface_t hub_itf;

/*------------- Helpers -------------*/
TU_ATTR_ALWAYS_INLINE static inline uint8_t get_index_by_itfnum(uint8_t itf_num) {
  return 0xFF;
}
static void hubd_resetFeatures();
static void hubd_setPortFeature(uint8_t u8PortNum, uint8_t u8PortFeature);

//--------------------------------------------------------------------+
// Weak stubs: invoked if no strong implementation is available
//--------------------------------------------------------------------+
TU_ATTR_WEAK void tud_hub_set_protocol_cb(uint8_t instance, uint8_t protocol) {
  (void) instance;
  (void) protocol;
}

TU_ATTR_WEAK bool tud_hub_set_idle_cb(uint8_t instance, uint8_t idle_rate) {
  (void) instance;
  (void) idle_rate;
  return true;
}

TU_ATTR_WEAK void tud_hub_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) {
  (void) instance;
  (void) report;
  (void) len;
}

// Invoked when a transfer wasn't successful
/*TU_ATTR_WEAK void tud_hub_report_failed_cb(uint8_t instance, hub_report_type_t report_type, uint8_t const* report, uint16_t xferred_bytes) {
  (void) instance;
  (void) report_type;
  (void) report;
  (void) xferred_bytes;
}*/

//--------------------------------------------------------------------+
// APPLICATION API
//--------------------------------------------------------------------+
bool boIsHubReq(uint8_t bRequest){
	return ((bRequest == HUB_REQUEST_GET_STATUS)||
			(bRequest == HUB_REQUEST_CLEAR_FEATURE)||
			(bRequest == HUB_REQUEST_SET_FEATURE)||
			(bRequest == HUB_REQUEST_GET_DESCRIPTOR)||
			(bRequest == HUB_REQUEST_SET_DESCRIPTOR)||
			(bRequest == HUB_REQUEST_CLEAR_TT_BUFFER)||
			(bRequest == HUB_REQUEST_RESET_TT)||
			(bRequest == HUB_REQUEST_GET_TT_STATE)||
			(bRequest == HUB_REQUEST_STOP_TT));
}
//--------------------------------------------------------------------+
// USBD-CLASS API
//--------------------------------------------------------------------+
void hubd_init(void) {
  hubd_reset(0);
}

bool hubd_deinit(void) {
  return true;
}

void hubd_reset(uint8_t rhport) {
  (void)rhport;
  //tu_memclr(hub_itf, sizeof(hub_itf));
}
void hubd_resetStates(){
	memset(&hub_itf.states,0,sizeof(hubd_states));
}

uint16_t hubd_open(uint8_t rhport, tusb_desc_interface_t const *desc_itf, uint16_t max_len) {
	  TU_VERIFY(TUSB_CLASS_HUB == desc_itf->bInterfaceClass, 0);

	  // len = interface + hub + n*endpoints
	  uint16_t const drv_len = (uint16_t) (sizeof(tusb_desc_interface_t) + sizeof(hub_desc_cs_t) +
	                                       desc_itf->bNumEndpoints * sizeof(tusb_desc_endpoint_t));
	  TU_LOG_USBD("Size: %d, Ep: %d\r\n",drv_len,desc_itf->bNumEndpoints);

	  TU_ASSERT(max_len >= drv_len, 0);

	  hubd_epbuf_t *p_epbuf = &_hubd_epbuf;

	  uint8_t const *p_desc = (uint8_t const *)desc_itf;

	  //------------- HUB descriptor -------------//
      TU_LOG_BUF(p_desc, max_len);
	  p_desc = tu_desc_next(p_desc);
	  TU_ASSERT(0x19 == tu_desc_type(p_desc), 0);
	  hub_itf.hub_descriptor = (hub_desc_cs_t const *)p_desc;
	  hubd_resetStates();

	  //------------- Endpoint Descriptor -------------//
	  p_desc = tu_desc_next(p_desc);
	  TU_ASSERT(usbd_open_edpt_pair(rhport, p_desc, desc_itf->bNumEndpoints, TUSB_XFER_INTERRUPT, &hub_itf.ep_out, &hub_itf.ep_in), 0);

	  // Prepare for output endpoint
	  if (hub_itf.ep_out) {
	    TU_ASSERT(usbd_edpt_xfer(rhport, 0, hub_itf.ep_out, p_epbuf->epout, CFG_TUD_HUB_EP_BUFSIZE), drv_len);
	  }

	  return drv_len;
}
// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
tusb_control_request_t lastReq = {0};
bool hubd_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
  // Use this for control messages
		int8_t ret = memcmp(&lastReq,request,sizeof(tusb_control_request_t));
		TU_LOG_USBD("HubXferCb %d", ret);
		if(ret == 0){
			memcpy(&lastReq,request,sizeof(tusb_control_request_t));
			return true;
		}
		memcpy(&lastReq,request,sizeof(tusb_control_request_t));
	  //uint8_t const hid_itf = get_index_by_itfnum((uint8_t)request->wIndex);
	  //TU_VERIFY(hid_itf < CFG_TUD_HUB);
	  //hidd_interface_t *p_hid = &_hidd_itf[hid_itf];
	  //hidd_epbuf_t *p_epbuf = &_hidd_epbuf[hid_itf];

	  if (request->bmRequestType_bit.type == TUSB_REQ_TYPE_STANDARD) {
	    //------------- STD Request -------------//
	    /*if (stage == CONTROL_STAGE_SETUP) {
	      uint8_t const desc_type = tu_u16_high(request->wValue);
	      // uint8_t const desc_index = tu_u16_low (request->wValue);

	      if (request->bRequest == TUSB_REQ_GET_DESCRIPTOR && desc_type == HUB_DESC_TYPE_HUB) {
	        TU_VERIFY(p_hid->hid_descriptor);
	        TU_VERIFY(tud_control_xfer(rhport, request, (void *)(uintptr_t)p_hid->hid_descriptor, p_hid->hid_descriptor->bLength));
	      } else if (request->bRequest == TUSB_REQ_GET_DESCRIPTOR && desc_type == HUB_DESC_TYPE_REPORT) {
	        uint8_t const *desc_report = tud_hid_descriptor_report_cb(hid_itf);
	        tud_control_xfer(rhport, request, (void *)(uintptr_t)desc_report, p_hid->report_desc_len);
	      } else {
	        return false; // stall unsupported request
	      }
	    }*/
	  } else if (request->bmRequestType_bit.type == TUSB_REQ_TYPE_CLASS) {
	    //------------- Class Specific Request -------------//
	    switch (request->bRequest) {
	    case HUB_REQUEST_GET_STATUS:
			tud_control_xfer(rhport,request,&hub_itf.states.sHubState,sizeof(hub_status_response_t));
	    	break;
	    case HUB_REQUEST_CLEAR_FEATURE:
	    	break;
	    case HUB_REQUEST_SET_FEATURE:
	    	break;
	    case HUB_REQUEST_GET_DESCRIPTOR:
	    	if (request->wValue==0x2900&&
	    		request->wIndex==0x0000){
	    		tud_control_xfer(rhport, request, tud_hub_descriptor_report_cb(), 9);
	    	}
	    	else{

	    	    TU_LOG_USBD("ezt is kezeld le\r\n");
	    	}
	    	break;
	    case HUB_REQUEST_SET_DESCRIPTOR:
	    	break;
	    case HUB_REQUEST_CLEAR_TT_BUFFER:
	    	break;
	    case   HUB_REQUEST_RESET_TT:
	    	break;
	    case HUB_REQUEST_GET_TT_STATE:
	    	break;
	    case HUB_REQUEST_STOP_TT:
	    	break;
	    default:
	    	break;
	    }
	  } else {
	    return false; // stall unsupported request
	  }
	  return true;
}

bool hubd_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes) {
    TU_LOG_USBD("hubd_xfer_cb\r\n");

	// Use this for messages
  return true;
}

static void hubd_resetFeatures(){
	memset(&hub_itf.states, 0, sizeof(hubd_states));
}
//u8PortNum from 1 to bNbrPorts
//u8PortFeature: the bit idx includeing state and change (hub.h 69.line)
static void hubd_setPortFeature(uint8_t u8PortNum, uint8_t u8PortFeature){
	hub_itf.states.sHubPortStates[u8PortNum-1].status.value |= (0xffff&(0x01<<(u8PortFeature)));
}
static void hubd_clearPortFeature(uint8_t u8PortNum, uint8_t u8PortFeature){
	hub_itf.states.sHubPortStates[u8PortNum-1].change.value &= (0xffff&~(0x01<<(u8PortFeature-HUB_FEATURE_PORT_CONNECTION_CHANGE)));
}
static hub_port_status_response_t* hubd_PortFeature(uint8_t u8PortNum){
	return &(hub_itf.states.sHubPortStates[u8PortNum-1]);
}

void hubd_connectDevice(uint8_t u8PortNum, uint8_t* usb_desc){
	hub_itf.states.sHubPortStates[u8PortNum].status.connection = 1;
	hub_itf.states.sHubPortStates[u8PortNum].change.connection = 1;
	last_added_port = u8PortNum;
}
/*usbd_device_t*/uint8_t* hubd_getUsbdDev(uint8_t dev_num){
	return (uint8_t*)&hub_itf.devices[dev_num];
}
uint8_t resetResponse = 0x02;
bool hubd_handle_controll_port_request(uint8_t rhport, const tusb_control_request_t* p_request) {
    TU_LOG_USBD("hub %s request to port %d\r\n",_hub_request_str[p_request->bRequest],p_request->wIndex);

	switch(p_request->bRequest) {
	case HUB_REQUEST_GET_STATUS: {
		if (p_request->wIndex == 0) {
			tud_control_xfer(rhport,p_request,&hub_itf.states.sHubState,sizeof(hub_status_response_t));
		}
		else if (p_request->wIndex <= hub_itf.hub_descriptor->bNbrPorts) {
			tud_control_xfer(rhport,p_request,&hub_itf.states.sHubPortStates[p_request->wIndex-1],sizeof(hub_port_status_response_t));
		}
		break;
	}
	case HUB_REQUEST_CLEAR_FEATURE: {
		TU_LOG_USBD("Clear Feature: %s\r\n",_hub_feature_str[p_request->wValue]);
		hubd_clearPortFeature(p_request->wIndex,p_request->wValue);
		tud_control_status(rhport, p_request);
		break;
	}
	case HUB_REQUEST_SET_FEATURE: {
		TU_LOG_USBD("Set Feature: %s\r\n",_hub_feature_str[p_request->wValue]);
		tud_control_status(rhport, p_request);
		hub_port_status_response_t* pportStatus = hubd_PortFeature(p_request->wIndex);
		switch(p_request->wValue){
		case HUB_FEATURE_PORT_RESET:
			usbd_edpt_xfer(rhport, 0, 0x81, &resetResponse, 1);
			pportStatus->change.reset = 1;
			pportStatus->status.reset = 0;
			pportStatus->status.port_enable = 1;
			break;
		default:
			hubd_setPortFeature(p_request->wIndex,p_request->wValue);
			break;
		}
		if(p_request->wValue == HUB_FEATURE_PORT_RESET){
		}
		break;
	}
	case HUB_REQUEST_GET_DESCRIPTOR: {
		TU_LOG_USBD("Get Descriptor for %u",p_request->wIndex);
		break;
	}
	case HUB_REQUEST_SET_DESCRIPTOR: {
		break;
	}
	case HUB_REQUEST_CLEAR_TT_BUFFER: {
		break;
	}
	case HUB_REQUEST_RESET_TT: {
		break;
	}
	case HUB_REQUEST_GET_TT_STATE: {
		break;
	}
	case HUB_REQUEST_STOP_TT: {
		break;
	}
	default:
		// Unexpected requeset
		return false;
	}
	return true;
}

#endif
