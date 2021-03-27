#include "storage.h"
#include "ws.h"
#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
# include "wifi.h"
#endif

#include "libespfs/espfs.h"
#include "libesphttpd/captdns.h"
#include "libesphttpd/httpd.h"
#include "libesphttpd/port.h"
#include "libesphttpd/route.h"

#include <stdint.h>


extern espfs_fs_t *espfs;

#if defined(CONFIG_EHTTPD_TLS_MBEDTLS) || defined(CONFIG_EHTTPD_TLS_OPENSSL)
extern const uint8_t cacert_der[];
extern const size_t cacert_der_len;
extern const uint8_t prvtkey_der[];
extern const size_t prvtkey_der_len;
#endif

void httpd_init(void)
{
    ehttpd_inst_t *inst = ehttpd_init("0.0.0.0:8080", NULL, 2,
            EHTTPD_FLAG_NONE);
    inst->espfs = espfs;

    ehttpd_route_append(inst, "/websocket/ws.cgi",
            ehttpd_route_websocket, 1, ws_demo_connect);
    ehttpd_route_append(inst, "/websocket/echo.cgi",
            ehttpd_route_websocket, 1, ws_echo_connect);
    ehttpd_route_append(inst, "/espfs/*",
            ehttpd_route_fs_get, 1, "/espfs/");
#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
    ehttpd_route_append(inst, "/sdcard/*",
            ehttpd_route_fs_get, 1, "/sdcard/");
    ehttpd_route_append(inst, "/root/*",
            ehttpd_route_fs_get, 1, "/");
#else
    ehttpd_route_append(inst, "/html/*",
            ehttpd_route_fs_get, 1, "html/");
#endif
    ehttpd_route_append(inst, "*",
            ehttpd_route_espfs_get, 0, NULL);
    ehttpd_route_append(inst, "*",
            ehttpd_route_espfs_index, 0, NULL);
    ehttpd_start(inst);
    ehttpd_thread_create(ws_broadcast_task, inst, NULL);

#if defined(CONFIG_EHTTPD_TLS_MBEDTLS) || defined(CONFIG_EHTTPD_TLS_OPENSSL)
    ehttpd_inst_t *ssl_inst = ehttpd_init("0.0.0.0:8443", NULL, 2,
            EHTTPD_FLAG_TLS);
    ssl_inst->espfs = espfs;
    ehttpd_route_append(ssl_inst, "/websocket/ws.cgi",
            ehttpd_route_websocket, 1, ws_demo_connect);
    ehttpd_route_append(ssl_inst, "/websocket/echo.cgi",
            ehttpd_route_websocket, 1, ws_echo_connect);
    ehttpd_route_append(ssl_inst, "/espfs/*",
            ehttpd_route_fs_get, 1, "/espfs/");
#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
    ehttpd_route_append(ssl_inst, "/sdcard/*",
            ehttpd_route_fs_get, 1, "/sdcard/");
    ehttpd_route_append(ssl_inst, "/root/*",
            ehttpd_route_fs_get, 1, "/");
#else
    ehttpd_route_append(ssl_inst, "/html/*",
            ehttpd_route_fs_get, 1, "html/");
#endif
    ehttpd_route_append(ssl_inst, "*",
            ehttpd_route_espfs_get, 0, NULL);
    ehttpd_route_append(ssl_inst, "*",
            ehttpd_route_espfs_index, 0, NULL);
    ehttpd_set_cert_and_key(ssl_inst, cacert_der, cacert_der_len, prvtkey_der,
            prvtkey_der_len);
    ehttpd_start(ssl_inst);
    ehttpd_thread_create(ws_broadcast_task, ssl_inst, NULL);
#endif

    ehttpd_captdns_start(inst, "0.0.0.0:53");
}

#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
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
        ehttpd_delay_ms(1000);
    }

    return 0;
}
#endif
