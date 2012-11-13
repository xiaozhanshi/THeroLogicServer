/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose. You are free to modify it and use it in any way you want,
** but you have to leave this header intact. Also if you find it useful,
** a comment about it on blog.toshsoft.de would be nice.
**
**
** RemoteNotification.c
** The main class which sends the push notification and
** prepares the payload.
**
** Author: Oliver Pahl
** -------------------------------------------------------------------------*/

#include "RemoteNotification.h"
#include "log.h"

SSL_Connection *sslcon;

/* Used internally to send the payload */
int send_payload(const char *deviceTokenHex, const char *payloadBuff, size_t payloadLength);

/* Initialize the Payload with zero values */
void init_payload(Payload *payload)
{
	bzero(payload, sizeof(Payload));
}

/* Function for sending the Payload */
int send_remote_notification(const char *deviceTokenHex, Payload *payload)
{
        char messageBuff[MAXPAYLOAD_SIZE];
        char tmpBuff[MAXPAYLOAD_SIZE];
        char badgenumBuff[3];

        strcpy(messageBuff, "{\"aps\":{");

        if(payload->message != NULL)
        {
            strcat(messageBuff, "\"alert\":");
            if(payload->actionKeyCaption != NULL)
            {
                sprintf(tmpBuff, "{\"body\":\"%s\",\"action-loc-key\":\"%s\"},", payload->message, payload->actionKeyCaption);
                strcat(messageBuff, tmpBuff);
            }
            else
            {
                sprintf(tmpBuff, "{\"%s\"},", payload->message);
                strcat(messageBuff, tmpBuff);
            }
        }

        if(payload->badgeNumber > 99 || payload->badgeNumber < 0)
            payload->badgeNumber = 1;

        sprintf(badgenumBuff, "%d", payload->badgeNumber);
        strcat(messageBuff, "\"badge\":");
        strcat(messageBuff, badgenumBuff);

        strcat(messageBuff, ",\"sound\":\"");
        strcat(messageBuff, payload->soundName == NULL ? "default" : payload->soundName);
        strcat(messageBuff, "\"},");

        int i = 0;
        while(payload->dictKey[i] != NULL && i < 5)
        {
            sprintf(tmpBuff, "\"%s\":\"%s\"", payload->dictKey[i], payload->dictValue[i]);
            strcat(messageBuff, tmpBuff);
            if(i < 4 && payload->dictKey[i + 1] != NULL)
            {
                strcat(messageBuff, ",");
            }
            i++;
        }

        strcat(messageBuff, "}");
        log_debug("Sending %s\n", messageBuff);

        send_payload(deviceTokenHex, messageBuff, strlen(messageBuff));
}

int send_payload(const char *deviceTokenHex, const char *payloadBuff, size_t payloadLength)
{
    int rtn = 0;

    //SSL_Connection *sslcon = ssl_connect(APPLE_HOST, APPLE_PORT, RSA_CLIENT_CERT, RSA_CLIENT_KEY, CA_CERT_PATH);
    if(sslcon == NULL)
    {
        log_error("Could not allocate memory for SSL Connection");
        return -1;
    }

    if (sslcon && deviceTokenHex && payloadBuff && payloadLength)
    {
        uint8_t command = 0; /* command number */
        char binaryMessageBuff[sizeof(uint8_t) + sizeof(uint16_t) + DEVICE_BINARY_SIZE + sizeof(uint16_t) + MAXPAYLOAD_SIZE];

        /* message format is, |COMMAND|TOKENLEN|TOKEN|PAYLOADLEN|PAYLOAD| */
        char *binaryMessagePt = binaryMessageBuff;
        uint16_t networkOrderTokenLength = htons(DEVICE_BINARY_SIZE);
        uint16_t networkOrderPayloadLength = htons(payloadLength);
 
        /* command */
        *binaryMessagePt++ = command;
 
        /* token length network order */
        memcpy(binaryMessagePt, &networkOrderTokenLength, sizeof(uint16_t));
        binaryMessagePt += sizeof(uint16_t);

        /* Convert the Device Token */
        int i = 0;
        int j = 0;
        int tmpi;
        char tmp[3];
        char deviceTokenBinary[DEVICE_BINARY_SIZE];
        while(i < strlen(deviceTokenHex))
        {
            if(deviceTokenHex[i] == ' ')
            {
                i++;
            }
            else
            {
                tmp[0] = deviceTokenHex[i];
                tmp[1] = deviceTokenHex[i + 1];
                tmp[2] = '\0';

                sscanf(tmp, "%x", &tmpi);
                deviceTokenBinary[j] = tmpi;

                i += 2;
                j++;
            }
        }

        /* device token */
        memcpy(binaryMessagePt, deviceTokenBinary, DEVICE_BINARY_SIZE);
        binaryMessagePt += DEVICE_BINARY_SIZE;

        /* payload length network order */
        memcpy(binaryMessagePt, &networkOrderPayloadLength, sizeof(uint16_t));
        binaryMessagePt += sizeof(uint16_t);
 
        /* payload */
        memcpy(binaryMessagePt, payloadBuff, payloadLength);
        binaryMessagePt += payloadLength;
        if (SSL_write(sslcon->ssl, binaryMessageBuff, (binaryMessagePt - binaryMessageBuff)) > 0)
            rtn = 1;
    }

    //ssl_disconnect(sslcon);

    return rtn;
}

void APN_connect()
{
	sslcon = ssl_connect(APPLE_HOST, APPLE_PORT, RSA_CLIENT_CERT, RSA_CLIENT_KEY, CA_CERT_PATH);
}

void APN_closeConnect()
{
	ssl_disconnect(sslcon);
}