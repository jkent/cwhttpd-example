/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "libesphttpd/ws.h"


void ws_broadcast_task(void *arg);
void ws_demo_handler(ehttpd_ws_t *ws);
void ws_echo_handler(ehttpd_ws_t *ws);
