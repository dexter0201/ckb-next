#include "command.h"
#include "device.h"
#include "devnode.h"
#include "input.h"
#include "usb.h"

int cmd_active_laptop(usbdevice* kb, usbmode* dummy1, int dummy2, int dummy3, const char* dummy4){
    (void)dummy1;
    (void)dummy2;
    (void)dummy3;
    (void)dummy4;

    if(kb->active)
        return 0;

    uchar pkt[192] = { 0x07, 0x01, 0x00 };
    usbsend_control(kb, pkt, 192, 0x09, 0x0307, 0);
    kb->active = 1;

    return 0;
}

int cmd_idle_laptop(usbdevice* kb, usbmode* dummy1, int dummy2, int dummy3, const char* dummy4){
    (void)dummy1;
    (void)dummy2;
    (void)dummy3;
    (void)dummy4;

    if(!kb->active)
        return 0;

    uchar pkt[192] = { 0x07, 0xB1, 0x01, 0x00 };
    usbsend_control(kb, pkt, 192, 0x09, 0x0307, 0);
    kb->active = 0;

    return 0;
}

static int rgbcmp(const lighting* lhs, const lighting* rhs){
    // Compare two light structures, up until the first three generic zones
    return memcmp(lhs->r, rhs->r, LED_GENERIC_FIRST + 3) || memcmp(lhs->g, rhs->g, LED_GENERIC_FIRST + 3) || memcmp(lhs->b, rhs->b, LED_GENERIC_FIRST + 3);
}

int updatergb_laptop(usbdevice* kb, int force){
    if(!kb->active)
        return 0;
    lighting* lastlight = &kb->profile->lastlight;
    lighting* newlight = &kb->profile->currentmode->light;
    // Don't do anything if the lighting hasn't changed
    if(!force && !lastlight->forceupdate && !newlight->forceupdate
            && !rgbcmp(lastlight, newlight))
        return 0;
    lastlight->forceupdate = newlight->forceupdate = 0;
    
    uchar pkts[3][192] =
    {
        {
            0x07, 0xa1, 0x2f, 0x00,
            0xc3, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, 0xc5, 0x00, 0x00, 0x00, 0x1d, 0x00, 0x00, 0x00,
            0x1c, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
            0x4c, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
            0x54, 0x00, 0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00,
            0x7b, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00,
            0x30, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x00,
            0x34, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x35, 0x00, 0x00, 0x00,
            0x3a, 0x00, 0x00, 0x00, 0x37, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00,
            0x18, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00,
            0x1e, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00,
            0x22, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00,
            0x26, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
        },
        {
            0x07, 0xa1, 0x2f, 0x00,
            0x27, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
            0x06, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x00, 0x00,
            0x0b, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
            0x12, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00,
            0x16, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x6e, 0x00, 0x00, 0x00, 0x71, 0x00, 0x00, 0x00,
            0x70, 0x00, 0x00, 0x00, 0x73, 0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x75, 0x00, 0x00, 0x00,
            0x74, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x79, 0x00, 0x00, 0x00,
            0x78, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x7a, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x97, 0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 0x00,
            0x99, 0x00, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, 0xc2, 0x00, 0x00, 0x00,
            0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        },
        {
            0x07, 0xa1, 0x1c, 0x00,
            0x8f, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x91, 0x00, 0x00, 0x00,
            0x8e, 0x00, 0x00, 0x00, 0x8d, 0x00, 0x00, 0x00, 0x00,
        },
    };

    for(int i = 0; i < 3; i++){
        for(int j = 4; j < 192; j += 4){
            uchar* ptr = (uchar*)pkts[i] + j;
            uchar index = *ptr;

            if(!index)
                break;
            
            // Move to the data
            ptr++;

            // Copy it to the packet
            *ptr++ = newlight->r[index];
            *ptr++ = newlight->g[index];
            *ptr = newlight->b[index];
        }
        usbsend_control(kb, pkts[i], 192, 0x09, 0x0307, 0);
    }

    return 0;
}

int start_laptop_kb(usbdevice* kb, int makeactive){
    kb->features = FEAT_RGB;
    
    // Try to get the hardware layout
    
/*    uchar layout = response[4];

    switch(layout)
    {
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 30:
        case 31:
            kb->layout = LAYOUT_ANSI;
            break;

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        default:
            kb->layout = LAYOUT_ANSI;
    }
*/
    kb->layout = LAYOUT_UNKNOWN;
    return 0;
}
