#include "storage.h"
#include "ws.h"
#if defined(ESP_PLATFORM)
# include "wifi.h"
#endif

#include "frogfs/frogfs.h"
#include "frogfs/route.h"
#include "cwhttpd/captdns.h"
#include "cwhttpd/httpd.h"
#include "cwhttpd/port.h"
#include "cwhttpd/route.h"

#include <stdint.h>


extern frogfs_fs_t *frogfs;

#if defined(CONFIG_CWHTTPD_MBEDTLS)
extern const uint8_t cacert_der[];
extern const size_t cacert_der_len;
extern const uint8_t prvtkey_der[];
extern const size_t prvtkey_der_len;
#endif

void httpd_init(void)
{
#if defined(ESP_PLATFORM)
    cwhttpd_inst_t *inst = cwhttpd_init(NULL, CWHTTPD_FLAG_NONE);
#else
    cwhttpd_inst_t *inst = cwhttpd_init("0.0.0.0:8080", CWHTTPD_FLAG_NONE);
#endif
    inst->frogfs = frogfs;
    cwhttpd_route_append(inst, "/websocket/ws.cgi",
            cwhttpd_route_ws, 1, ws_demo_handler);
    cwhttpd_route_append(inst, "/websocket/echo.cgi",
            cwhttpd_route_ws, 1, ws_echo_handler);
#if defined(ESP_PLATFORM)
    cwhttpd_route_append(inst, "/frogfs/*",
            cwhttpd_route_fs_get, 1, "/frogfs/");
    cwhttpd_route_append(inst, "/sdcard/*",
            cwhttpd_route_fs_get, 1, "/sdcard/");
    cwhttpd_route_append(inst, "/root/*",
            cwhttpd_route_fs_get, 1, "/");
#else
    cwhttpd_route_append(inst, "/html/*",
            cwhttpd_route_fs_get, 1, "html/");
#endif
    cwhttpd_route_append(inst, "*",
            frogfs_route_get, 0, NULL);
    cwhttpd_route_append(inst, "*",
            frogfs_route_index, 0, NULL);
    cwhttpd_thread_create(ws_broadcast_task, inst, NULL);
    cwhttpd_start(inst);

#if defined(CONFIG_CWHTTPD_MBEDTLS)
#if defined(ESP_PLATFORM)
    cwhttpd_inst_t *tls_inst = cwhttpd_init(NULL, CWHTTPD_FLAG_TLS);
#else
    cwhttpd_inst_t *tls_inst = cwhttpd_init("0.0.0.0:8443", CWHTTPD_FLAG_TLS);
#endif
    tls_inst->frogfs = frogfs;
    cwhttpd_set_cert_and_key(tls_inst, cacert_der, cacert_der_len, prvtkey_der,
            prvtkey_der_len);
    cwhttpd_route_append(tls_inst, "/websocket/ws.cgi",
            cwhttpd_route_ws, 1, ws_demo_handler);
    cwhttpd_route_append(tls_inst, "/websocket/echo.cgi",
            cwhttpd_route_ws, 1, ws_echo_handler);
    cwhttpd_route_append(tls_inst, "/frogfs/*",
            cwhttpd_route_fs_get, 1, "/frogfs/");
#if defined(ESP_PLATFORM)
    cwhttpd_route_append(tls_inst, "/sdcard/*",
            cwhttpd_route_fs_get, 1, "/sdcard/");
    cwhttpd_route_append(tls_inst, "/root/*",
            cwhttpd_route_fs_get, 1, "/");
#else
    cwhttpd_route_append(tls_inst, "/html/*",
            cwhttpd_route_fs_get, 1, "html/");
#endif
    cwhttpd_route_append(tls_inst, "*",
            frogfs_route_get, 0, NULL);
    cwhttpd_route_append(tls_inst, "*",
            frogfs_route_index, 0, NULL);
    cwhttpd_thread_create(ws_broadcast_task, tls_inst, NULL);
    cwhttpd_start(tls_inst);
#endif

    cwhttpd_captdns_start("0.0.0.0:53");
}

#if defined(ESP_PLATFORM)
void app_main(void)
{
    storage_init();
    wifi_init();
    httpd_init();
}
#else
int main(int argc, char *argv[])
{
    storage_init();
    httpd_init();

    while(true) {
        cwhttpd_delay_ms(1000);
    }

    return 0;
}
#endif
