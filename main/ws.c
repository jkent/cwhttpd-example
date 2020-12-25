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
        ehttpd_ws_broadcast(inst, "/websocket/ws.cgi", (uint8_t *) buf, strlen(buf), EHTTPD_WS_FLAG_NONE);
        ehttpd_delay_ms(1000);
    }
}

// On reception of a message, send "You sent: " plus whatever the other side sent
static void ws_demo_recv(ehttpd_ws_t *ws, const uint8_t *buf, int len, ehttpd_ws_flags_t flags)
{
    char reply[128];

    strcpy(reply, "You sent: ");
    int i = strlen(reply);
    len = (len > sizeof(reply) + i + 1) ? (sizeof(reply) - i - 1) : len;
    memcpy(reply + i, buf, len);
    reply[len] = 0;
    ehttpd_ws_send(ws, buf, len, EHTTPD_WS_FLAG_NONE);
}

// Websocket connected. Install reception handler and send welcome message.
void ws_demo_connect(ehttpd_ws_t *ws)
{
    ws->recv_cb = ws_demo_recv;
    ehttpd_ws_send(ws, (uint8_t *) "Hi, Websocket!", 14, EHTTPD_WS_FLAG_NONE);
}

// On reception of a message, echo it back verbatim
static void ws_echo_recv(ehttpd_ws_t *ws, const uint8_t *buf, int len, ehttpd_ws_flags_t flags)
{
    ehttpd_ws_send(ws, buf, len, flags);
}

// Echo websocket. Install reception handler.
void ws_echo_connect(ehttpd_ws_t *ws)
{
    ws->recv_cb = ws_echo_recv;
}
