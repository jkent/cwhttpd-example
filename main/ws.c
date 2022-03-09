/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "ws.h"

#include "cwhttpd/httpd.h"
#include "cwhttpd/port.h"
#include "cwhttpd/ws.h"

#include <stdio.h>
#include <string.h>


// Broadcast the uptime in seconds every second over connected websockets
void ws_broadcast_task(void *arg)
{
    cwhttpd_inst_t *inst = (cwhttpd_inst_t *) arg;
    char buf[128];

    while (1) {
        size_t ctr = (size_t) inst->user++;
        sprintf(buf, "Up for %d minutes %d seconds!\n", ctr / 60, ctr % 60);
        cwhttpd_ws_broadcast(inst, "/websocket/ws.cgi", (uint8_t *) buf,
                strlen(buf), CWHTTPD_WS_FLAG_NONE);
        cwhttpd_delay_ms(1000);
    }
}

// Websocket connected. Install reception handler and send welcome message.
void ws_demo_handler(cwhttpd_ws_t *ws)
{
    char buf[128];
    char *p;

    cwhttpd_ws_send(ws, "Hi, Websocket!", 14, CWHTTPD_WS_FLAG_NONE);

    strcpy(buf, "You sent: ");
    p = buf + strlen(buf);

    while (true) {
        ssize_t ret = cwhttpd_ws_recv(ws, p, sizeof(buf) - (p - buf));
        if (ret <= 0) {
            break;
        }
        cwhttpd_ws_send(ws, buf, ret + (p - buf), CWHTTPD_WS_FLAG_NONE);
    }
}

// Echo websocket
void ws_echo_handler(cwhttpd_ws_t *ws)
{
    char buf[128];

    while (true) {
        ssize_t ret = cwhttpd_ws_recv(ws, buf, sizeof(buf));
        if (ret <= 0) {
            break;
        }
        cwhttpd_ws_send(ws, buf, ret, CWHTTPD_WS_FLAG_NONE);
    }
}
