/*
 * @file enumarate.c USB USB ENUM Driver C File
 *
 ******************************************************************************
 * Copyright (C) 2016-2018 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

#include <enumerate.h>
#include <string.h>
#include <usb_event.h>
#include "mml_usb.h"

/*Definitions */
#define ENUM_DESC_SIZE     10   /* Size of the entire descriptor table */

/* Maximum # of descriptors supported other than (mandatory) device and configuration */
#define ENUM_NUM_STRINGS   (ENUM_DESC_SIZE - ENUM_DESC_STRING)

/* File Scope Data */

/* Structure to hold callback pointers */
typedef struct {
  int (*fnaddr)(usb_setup_pkt *, void *);
  void *cbdata;
} callback_t;

/* Hold function pointers for all callbacks */
static callback_t callback[ENUM_NUM_CALLBACKS];

static void (*callback_getdescriptor)(usb_setup_pkt *, const uint8_t **, uint16_t *) = NULL;

#ifdef __IAR_SYSTEMS_ICC__
#pragma data_alignment = 4
#elif __GNUC__
__attribute__((aligned(4)))
#endif
static uint8_t config_value = 0;
#ifdef __IAR_SYSTEMS_ICC__
#pragma data_alignment = 4
#elif __GNUC__
__attribute__((aligned(4)))
#endif
static uint8_t enum_data[2];
static usb_req_t enum_req;

/* Holds descriptor pointers */
const uint8_t *enum_desc_table[ENUM_NUM_STRINGS]; 

/* Function Prototypes */
static int setup(maxusb_event_t event, void *cbdata);
static int std(usb_setup_pkt *sud);
static int getstatus(usb_setup_pkt *sud);
static int clearfeature(usb_setup_pkt *sud);
static int setfeature(usb_setup_pkt *sud);
static int getdescriptor(usb_setup_pkt *sud);
static int getconfig(usb_setup_pkt *sud);
static int setconfig(usb_setup_pkt *sud);
static int getinterface(usb_setup_pkt *sud);
static int setinterface(usb_setup_pkt *sud);
static void acksetup_callback(void *cbdata);

/* USB request object for enumeration packets */
static const usb_req_t enum_req_init = {
  0,          /* ep */
  NULL,       /* data */
  0,          /* reqlen */
  0,          /* actlen */
  0,          /* error_code */
  acksetup_callback,
  &enum_req,   /* cbdata */
  MAXUSB_TYPE_TRANS /* type */
};

/* Holds the device status for getstatus */
static uint8_t device_status;

const char * enum_get_version(void) {
	return ENUM_VERSION_STRING;
}

int enum_init(void)
{
  memset(callback, 0, sizeof(callback));
  memset(enum_desc_table, 0, sizeof(enum_desc_table));
  device_status = 0;

  /* register setup handler */
  return usb_event_enable(MAXUSB_EVENT_SUDAV, setup, NULL);
}

int enum_register_callback(enum_callback_t type, int (*func)(usb_setup_pkt *indata, void *cbdata), void *cbdata)
{
  if (type >= ENUM_NUM_CALLBACKS) {
    return -1;
  }

  /* Register the callback and data */
  callback[type].fnaddr = func;
  callback[type].cbdata = cbdata;

  return 0;
}

int enum_register_getdescriptor(void (*func)(usb_setup_pkt *, const uint8_t **desc, uint16_t *desclen))
{
  /* Register the callback */
  callback_getdescriptor = func;
  return 0;
}

static void acksetup_callback(void *cbdata)
{
  usb_req_t *req = (usb_req_t*)cbdata;

  if (req->error_code == 0) {
    /* Send ACK to Status stage */
    mml_usb_ackstat(0);
  } else {
    mml_usb_stall(0);
  }

  /* Clear the request to indicate completion */
  memset(req, 0, sizeof(usb_req_t));
}

int enum_register_descriptor(enum_descriptor_t type, const uint8_t *desc, uint8_t index)
{
  if ((type >= ENUM_NUM_DESCRIPTORS) || (index >= ENUM_NUM_STRINGS)) { 
    return -1;
  }

  if (type < ENUM_DESC_STRING) {
    /* Ignore index. Force to zero. */
    index = 0;
  }

  if ( (type+index) >= ENUM_NUM_STRINGS) {
	 return -1;
  }
  
  enum_desc_table[type+index] = desc;

  if(type == ENUM_DESC_CONFIG) {
    if(((usb_configuration_descriptor_t*)(enum_desc_table[ENUM_DESC_CONFIG]))->bmAttributes & BMATT_SELF_POWERED) {
      device_status |= STATUS_DEV_SELF_POWERED;
    }
  }

  return 0;
}

static int setup(maxusb_event_t event, void *cbdata)
{
  usb_setup_pkt sud;
  int result;
  (void)event;
  (void)cbdata;

  if (mml_usb_get_setup(&sud) < 0) {
    return -1;
  }

  if (enum_req.data != NULL) {
    /* ERROR: overlapping SETUP packets */
    mml_usb_stall(0);
    return -1;
  }

  /* Decode device requests into standard / class / vendor */
  switch (sud.bmRequestType & RT_TYPE_MASK) {
    case RT_TYPE_STD:
      /* Standard */
      result = std(&sud);
      break;

    case RT_TYPE_CLASS:
      /* Class */
      if (callback[ENUM_CLASS_REQ].fnaddr != NULL) {
        result = callback[ENUM_CLASS_REQ].fnaddr(&sud, callback[ENUM_CLASS_REQ].cbdata);
      } else {
        /* Default class request handler (stall) */
        result = -1;
      }
      break;

    case RT_TYPE_VENDOR:
      /* Vendor */
      if (callback[ENUM_VENDOR_REQ].fnaddr != NULL) {
        result = callback[ENUM_VENDOR_REQ].fnaddr(&sud, callback[ENUM_VENDOR_REQ].cbdata);
      } else {
        /* Default vendor request handler (stall) */
        result = -1;
      }
      break;

    default:
      /* Reserved */
      result = -1;
      break;
  }

  /* Unsupported bmRequestType */
  if (result < 0) {
    mml_usb_stall(0);
  }

  return result;
}

static int std(usb_setup_pkt *sud)
{
  int result;

  switch (sud->bRequest) {
    case SDR_GET_STATUS:
      result = getstatus(sud); 
      break;
    case SDR_CLEAR_FEATURE:
      result = clearfeature(sud);
      break;
    case SDR_SET_FEATURE:
      result = setfeature(sud);
      break;
    case SDR_SET_ADDRESS:
      /* Hardware does this for us. Just ACK the setup packet */
      result = mml_usb_ackstat(0);
      break;
    case SDR_GET_DESCRIPTOR:
      result = getdescriptor(sud);
      break;
    case SDR_SET_DESCRIPTOR:
      /* Descriptor creation and modification is not implemented */
      result = -1;
      break;
    case SDR_GET_CONFIG:
      result = getconfig(sud);
      break;
    case SDR_SET_CONFIG:
      result = setconfig(sud);
      break;
    case SDR_GET_INTERFACE:
      result = getinterface(sud);
      break;
    case SDR_SET_INTERFACE:
      result = setinterface(sud);
      break;
    case SDR_SYNCH_FRAME:
      /* Isochronous support not implemented */
      result = -1;
      break;
    default:
      result = -1;
      break;
  }

  return result;
}

static int getstatus(usb_setup_pkt *sud)
{
  int result;

  /* If any values are out of range (per USB 2.0 spec), stall */
  if ((sud->wValue != 0) || (sud->wLength != 2) || 
      (((sud->bmRequestType & RT_RECIP_MASK) == RT_RECIP_DEVICE) && (sud->wIndex != 0))) {
    return mml_usb_stall(0);
  }
 
  /* Device and Interfaces always return a 16-bit zero value in the data phase */
  if ((sud->bmRequestType & RT_RECIP_MASK) == RT_RECIP_ENDP) {

    /* Interface halt status taken directly from USB SIE registers */
    result = mml_usb_is_stalled(sud->wIndex & 0x0f);
    if (result < 0) {
      mml_usb_stall(0);
      return result;
    }
    enum_data[0] = (result > 0) ? 0x01 : 0x00;
    enum_data[1] = 0;

  } else if ((sud->bmRequestType & RT_RECIP_MASK) == RT_RECIP_IFACE) {
    enum_data[0] = 0;
    enum_data[1] = 0;

  } else if((sud->bmRequestType & RT_RECIP_MASK) == RT_RECIP_DEVICE) {
    enum_data[0] = device_status;
    enum_data[1] = 0;
  }

  /* Write the resulting data into the endpoint and arm it */
  memcpy(&enum_req, &enum_req_init, sizeof(usb_req_t));
  enum_req.data = enum_data;
  enum_req.reqlen = 2;
  result = mml_usb_write_endpoint(&enum_req);
  if (result < 0) {
    mml_usb_stall(0);
    return result;
  }

  /* Immediately return, Status stage ACK will be handled by acksetup_callback callback */
  return 0;
}

static int clearfeature(usb_setup_pkt *sud)
{
  int result;

  /* If any values are out of range (per USB 2.0 spec), stall */
  if ((sud->bmRequestType & RT_DEV_TO_HOST) || 
      (sud->wLength != 0) ||
      ((sud->wValue == FEAT_ENDPOINT_HALT) && ((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_ENDP)) ||
      ((sud->wValue == FEAT_REMOTE_WAKE) && ((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_DEVICE)) ||
      ((sud->wValue == FEAT_TEST_MODE) && ((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_DEVICE))) {
    return mml_usb_stall(0);
  }

  if ((sud->wValue == FEAT_ENDPOINT_HALT) && (sud->wIndex > 0)) {
    /* Unhalt endpoint */
    result = mml_usb_unstall(sud->wIndex & 0x0f);
    if (result < 0) {
      mml_usb_stall(0);
      return result;
    }
  } else if((sud->wValue == FEAT_REMOTE_WAKE)) {
    /* Clear the remote wakeup feature */
    if(callback[ENUM_CLRFEATURE].fnaddr != NULL) {
      result = callback[ENUM_CLRFEATURE].fnaddr(sud, NULL);
      if(result < 0) {
        mml_usb_stall(0);
        return result;
      }
      device_status &= ~STATUS_DEV_REMOTE_WAKE;
    } else {
      mml_usb_stall(0);
      return -1;
    }
  } else if((sud->wValue == FEAT_TEST_MODE)) {
    /* Clear the test mode feature */
    if(callback[ENUM_CLRFEATURE].fnaddr != NULL) {
      result = callback[ENUM_CLRFEATURE].fnaddr(sud, NULL);
      if(result < 0) {
        mml_usb_stall(0);
        return result;
      }
    } else {
      mml_usb_stall(0);
      return -1;
    }
  } else {
    /* Unsupported */
    mml_usb_stall(0);
    return -1;
  }

  return mml_usb_ackstat(0);
}

static int setfeature(usb_setup_pkt *sud)
{
  int result;

  /* If any values are out of range (per USB 2.0 spec), stall */
  if ((sud->wLength !=0) ||
      ((sud->wValue == FEAT_ENDPOINT_HALT) && ((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_ENDP)) ||
      ((sud->wValue == FEAT_REMOTE_WAKE) && ((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_DEVICE)) ||
      ((sud->wValue == FEAT_TEST_MODE) && ((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_DEVICE))) {
    return mml_usb_stall(0);
  }

  if ((sud->wValue == FEAT_ENDPOINT_HALT) && (sud->wIndex > 0)){
    /* Halt endpoint */
    result = mml_usb_stall(sud->wIndex & 0x0f);
    if (result < 0) {
      mml_usb_stall(0);
      return result;
    }
  } else if((sud->wValue == FEAT_REMOTE_WAKE)) {
    /* Clear the remote wakeup feature */
    if(callback[ENUM_SETFEATURE].fnaddr != NULL) {
      result = callback[ENUM_SETFEATURE].fnaddr(sud, NULL);
      if(result < 0) {
        mml_usb_stall(0);
        return result;
      }
      device_status |= STATUS_DEV_REMOTE_WAKE;
    } else {
      mml_usb_stall(0);
      return -1;
    }
  } else if((sud->wValue == FEAT_TEST_MODE)) {
    /* Clear the test mode feature */
    if(callback[ENUM_SETFEATURE].fnaddr != NULL) {
      result = callback[ENUM_SETFEATURE].fnaddr(sud, NULL);
      if(result < 0) {
        mml_usb_stall(0);
        return result;
      }
    } else {
      mml_usb_stall(0);
      return -1;
    }
  } else {
    /* Unsupported */
    mml_usb_stall(0);
    return -1;
  }
  return mml_usb_ackstat(0);
}

static int getdescriptor(usb_setup_pkt *sud)
{
  int result;
  const uint8_t *dsc;
  uint16_t dsclen;

  /* Default to stall */
  dsc = NULL;
  dsclen = 0;

  switch (sud->wValue >> 8) {
    case DESC_DEVICE:
      if (((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_DEVICE) ||
          (sud->wIndex != 0)) {
        /* Stall */
        dsclen = 0;
      } else {
        dsc = enum_desc_table[ENUM_DESC_DEVICE];
        if (dsc == NULL) {
          dsclen = 0;
        } else {
          dsclen = dsc[0];
        }
      }
      break;

    case DESC_CONFIG:
      if (((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_DEVICE) ||
          (sud->wIndex != 0)) {
        /* Stall */
        dsclen = 0;
      } else {
        dsc = enum_desc_table[ENUM_DESC_CONFIG];
        if (dsc == NULL) {
          dsclen = 0;
        } else {
          dsclen = (dsc[3] << 8) + dsc[2];
        }
      }
      break;

    case DESC_STRING:
      if ((sud->bmRequestType & RT_RECIP_MASK) != RT_RECIP_DEVICE) {
        /* Let the class-specific callback handle it */
        goto default_handler;
      } else {
        if ((sud->wValue & 0xff) < ENUM_NUM_STRINGS) {
          dsc = enum_desc_table[ENUM_DESC_STRING + (sud->wValue & 0xff)];
          dsclen = dsc[0];
        } else {
          /* Host asked for an out-of-range string descriptor */
          dsclen = 0;
        }
      }
      break;

    default:
    default_handler:
      /* If a class-specific handler is registered, pass this request off to it */
      if (callback_getdescriptor != NULL) {
        callback_getdescriptor(sud, &dsc, &dsclen);
        if (dsc == NULL) {
          dsclen = 0;
        }
      } else {
        /* Stall */
        dsclen = 0;
      }
      break;
  }

  /* Adjust dsclen if descriptor is longer than the requested number of bytes */
  if (dsclen > sud->wLength) {
    dsclen = sud->wLength;
  }

  if (dsclen > 0) {
    /* Write data into endpoint buffer */
    memcpy(&enum_req, &enum_req_init, sizeof(usb_req_t));
    enum_req.data = (uint8_t*)dsc;
    enum_req.reqlen = dsclen;
    result = mml_usb_write_endpoint(&enum_req);
    if (result < 0) {
      mml_usb_stall(0);
      return result;
    }
  } else {
    return mml_usb_stall(0);
  }

  /* Immediately return, Status stage ACK will be handled by acksetup_callback callback */
  return 0;  
}

static int getconfig(usb_setup_pkt *sud)
{
  int result;

  /* If any values are out of range (per USB 2.0 spec), stall */
  if ((sud->wValue != 0) || (sud->wIndex != 0) || (sud->wLength != 1)) {
    return mml_usb_stall(0);
  }

  /* Write the resulting data into the endpoint and arm it */
  memcpy(&enum_req, &enum_req_init, sizeof(usb_req_t));
  enum_req.data = &config_value;
  enum_req.reqlen = 1;
  result = mml_usb_write_endpoint(&enum_req);
  if (result < 0) {
    mml_usb_stall(0);
    return result;
  }

  /* Immediately return, Status stage ACK will be handled by acksetup_callback callback */
  return 0;
}

static int setconfig(usb_setup_pkt *sud)
{
  /* If any values are out of range (per USB 2.0 spec), stall */
  if ((sud->wIndex != 0) || (sud->wLength != 0)) {
    return mml_usb_stall(0);
  }
  
  /* If the callback is set for a configuration value validator, use it */
  /* Otherwise, assume any wValue is acceptable and ACK the Status stage */
  if (callback[ENUM_SETCONFIG].fnaddr != NULL) {
    if (callback[ENUM_SETCONFIG].fnaddr(sud, callback[ENUM_SETCONFIG].cbdata) < 0) {
      /* Validation routine did not accept value, stall */
      return mml_usb_stall(0);
    }
  }

  /* Save off configuration value */
  config_value = sud->wValue;

  return mml_usb_ackstat(0);
}

static int getinterface(usb_setup_pkt *sud)
{
  int result;

  /* If any values are out of range (per USB 2.0 spec), stall */
  if ((sud->wValue != 0) || (sud->wLength != 1)) {
    return mml_usb_stall(0);
  }

  /* Check for application-specific handler */
  if (callback[ENUM_GETINTERFACE].fnaddr != NULL) {
    result = callback[ENUM_GETINTERFACE].fnaddr(sud, callback[ENUM_GETINTERFACE].cbdata);
    if (result < 0) {
      return mml_usb_stall(0);
    }
    /* Lower 8 bits from the callback are passed to host */
    enum_data[0] = (uint8_t)result;
  } else {
    enum_data[0] = 0;
  }
  
  /* If the default handler is used, wIndex is ignored, and 0x00 is returned */
  /*  during the Data phase */
  memcpy(&enum_req, &enum_req_init, sizeof(usb_req_t));
  enum_req.data = enum_data;
  enum_req.reqlen = 1;
  result = mml_usb_write_endpoint(&enum_req);
  if (result < 0) {
    mml_usb_stall(0);
    return result;
  }

  /* Immediately return, Status stage ACK will be handled by acksetup_callback callback */
  return 0;
}

static int setinterface(usb_setup_pkt *sud)
{
  /* If any values are out of range (per USB 2.0 spec), stall */
  if (sud->wLength != 0) {
    return mml_usb_stall(0);
  }
  
  /* Check for application-specific handler */
  if (callback[ENUM_GETINTERFACE].fnaddr != NULL) {
    if (callback[ENUM_SETINTERFACE].fnaddr(sud, callback[ENUM_SETINTERFACE].cbdata) < 0) {
      return mml_usb_stall(0);
    }
  } else {
    /* If the default handler is used, wIndex is ignored, and wValue must be */
    /*  zero (no alternate setting) */
    if (sud->wValue != 0) {
      return mml_usb_stall(0);
    }
  }

  return mml_usb_ackstat(0);
}

uint8_t enum_getconfig(void)
{
  return config_value;
}

void enum_clearconfig(void)
{
  config_value = 0;
}
