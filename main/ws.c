/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ws.h"

#include "libesphttpd/httpd.h"
#include "libesphttpd/port.h"
#include "libesphttpd/ws.h"

#include <stdio.h>
#include <string.h>


// Broadcast the uptime in seconds every second over connected websockets
void ws_broadcast_task(void *arg)
{
    ehttpd_inst_t *inst = (ehttpd_inst_t *) arg;
    char buf[128];

    while (1) {
        size_t ctr = (size_t) inst->user++;
        sprintf(buf, "Up for %d minutes %d seconds!\n", ctr / 60, ctr % 60);
        ehttpd_ws_broadcast(inst, "/websocket/ws.cgi", (uint8_t *) buf,
                strlen(buf), EHTTPD_WS_FLAG_NONE);
        ehttpd_delay_ms(1000);
    }
}

// Websocket connected. Install reception handler and send welcome message.
void ws_demo_handler(ehttpd_ws_t *ws)
{
    char buf[128];
    char *p;

    ehttpd_ws_send(ws, "Hi, Websocket!", 14, EHTTPD_WS_FLAG_NONE);

    strcpy(buf, "You sent: ");
    p = buf + strlen(buf);

    while (true) {
        ssize_t ret = ehttpd_ws_recv(ws, p, sizeof(buf) - (p - buf));
        if (ret <= 0) {
            break;
        }
        ehttpd_ws_send(ws, buf, ret + (p - buf), EHTTPD_WS_FLAG_NONE);
    }
}

// Echo websocket
void ws_echo_handler(ehttpd_ws_t *ws)
{
    char buf[128];

    while (true) {
        ssize_t ret = ehttpd_ws_recv(ws, buf, sizeof(buf));
        if (ret <= 0) {
            break;
        }
        ehttpd_ws_send(ws, buf, ret, EHTTPD_WS_FLAG_NONE);
    }
}
