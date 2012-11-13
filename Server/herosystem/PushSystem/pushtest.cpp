/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose. You are free to modify it and use it in any way you want,
** but you have to leave this header intact. Also if you find it useful,
** a comment about it on blog.toshsoft.de would be nice. 
**
**
** pushtest.c
** Command Line testing Utility for the push service
**
** Author: Oliver Pahl
** -------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "Helper/RemoteNotification.h"
#include "log.h"

/* test Function */
int send_Push_Message(int argc, char *argv)
{
    //int     err;

    /* Phone specific Payload message as well as hex formated device token */
    const char     *deviceTokenHex = NULL;
    if(argc == 1)
    {
        deviceTokenHex = "ba458798b1996bef05e7b0cff7b554b82aaf7f61d5e062cab3d4ee5fed417bb2";
    }
    else
    {
        deviceTokenHex = argv;
    }

    if(strlen(deviceTokenHex) < 64 || strlen(deviceTokenHex) > 70)
    {
        log_error("Device Token is to short or to long. Length without spaces should be 64 chars...,the devicetoken is %s\n", deviceTokenHex);
        return -1;
    }
    
    Payload *payload = (Payload*)malloc(sizeof(Payload));
    init_payload(payload);

    // This is the message the user gets once the Notification arrives
    payload->message = "Message to print out";

    // This is the red numbered badge that appears over the Icon
    payload->badgeNumber = 1;

    // This is the Caption of the Action key on the Dialog that appears
    payload->actionKeyCaption = "Caption of the second Button";

    // These are two dictionary key-value pairs with user-content
    payload->dictKey[0] = "Key1";
    payload->dictValue[0] = "Value1";

    payload->dictKey[1] = "Key2";
    payload->dictValue[1] = "Value2";

    /* Send the payload to the phone */
    log_error("Sending APN to Device with UDID: %s\n", deviceTokenHex);
    send_remote_notification(deviceTokenHex, payload);

    return 0;
}


