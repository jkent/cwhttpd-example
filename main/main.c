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
#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
    ehttpd_inst_t *inst = ehttpd_init(NULL, EHTTPD_FLAG_NONE);
#else
    ehttpd_inst_t *inst = ehttpd_init("0.0.0.0:8080", EHTTPD_FLAG_NONE);
#endif
    inst->espfs = espfs;
    ehttpd_route_append(inst, "/websocket/ws.cgi",
            ehttpd_route_ws, 1, ws_demo_handler);
    ehttpd_route_append(inst, "/websocket/echo.cgi",
            ehttpd_route_ws, 1, ws_echo_handler);
#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
    ehttpd_route_append(inst, "/espfs/*",
            ehttpd_route_fs_get, 1, "/espfs/");
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
    ehttpd_thread_create(ws_broadcast_task, inst, NULL);
    ehttpd_start(inst);

#if defined(CONFIG_EHTTPD_TLS_MBEDTLS) || defined(CONFIG_EHTTPD_TLS_OPENSSL)
#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
    ehttpd_inst_t *tls_inst = ehttpd_init(NULL, EHTTPD_FLAG_TLS);
#else
    ehttpd_inst_t *tls_inst = ehttpd_init("0.0.0.0:8443", EHTTPD_FLAG_TLS);
#endif
    tls_inst->espfs = espfs;
    ehttpd_set_cert_and_key(tls_inst, cacert_der, cacert_der_len, prvtkey_der,
            prvtkey_der_len);
    ehttpd_route_append(tls_inst, "/websocket/ws.cgi",
            ehttpd_route_ws, 1, ws_demo_handler);
    ehttpd_route_append(tls_inst, "/websocket/echo.cgi",
            ehttpd_route_ws, 1, ws_echo_handler);
    ehttpd_route_append(tls_inst, "/espfs/*",
            ehttpd_route_fs_get, 1, "/espfs/");
#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
    ehttpd_route_append(tls_inst, "/sdcard/*",
            ehttpd_route_fs_get, 1, "/sdcard/");
    ehttpd_route_append(tls_inst, "/root/*",
            ehttpd_route_fs_get, 1, "/");
#else
    ehttpd_route_append(tls_inst, "/html/*",
            ehttpd_route_fs_get, 1, "html/");
#endif
    ehttpd_route_append(tls_inst, "*",
            ehttpd_route_espfs_get, 0, NULL);
    ehttpd_route_append(tls_inst, "*",
            ehttpd_route_espfs_index, 0, NULL);
    ehttpd_thread_create(ws_broadcast_task, tls_inst, NULL);
    ehttpd_start(tls_inst);
#endif

    ehttpd_captdns_start("0.0.0.0:53");
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
