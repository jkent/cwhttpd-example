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

static const ehttpd_route_t routes[] = {
    EHTTPD_ROUTE_WS("/websocket/ws.cgi", ws_demo_connect),
    EHTTPD_ROUTE_WS("/websocket/echo.cgi", ws_echo_connect),
    EHTTPD_ROUTE_FS_PATH("/espfs/*", "/espfs/"),
#if defined(CONFIG_IDF_TARGET_ESP8266) || defined(ESP_PLATFORM)
    EHTTPD_ROUTE_FS_PATH("/sdcard/*", "/sdcard/"),
    EHTTPD_ROUTE_FS_PATH("/root/*", "/"),
#else
    EHTTPD_ROUTE_FS_PATH("/html/*", "html/"),
#endif
    EHTTPD_ROUTE_ESPFS("*"),
    EHTTPD_ROUTE("*", ehttpd_route_espfs_index),
    EHTTPD_ROUTE_END()
};

void httpd_init(void)
{
    ehttpd_inst_t *inst = ehttpd_init(routes, 2, EHTTPD_FLAG_NONE);
    inst->espfs = espfs;
    ehttpd_start(inst);
    ehttpd_thread_create(ws_broadcast_task, inst, NULL);

#if defined(CONFIG_EHTTPD_TLS_MBEDTLS) || defined(CONFIG_EHTTPD_TLS_OPENSSL)
    ehttpd_inst_t *ssl_inst = ehttpd_init(routes, 2, EHTTPD_FLAG_TLS);
    ssl_inst->espfs = espfs;
    ehttpd_set_cert_and_key(ssl_inst, cacert_der, cacert_der_len, prvtkey_der,
            prvtkey_der_len);
    ehttpd_start(ssl_inst);
    ehttpd_thread_create(ws_broadcast_task, ssl_inst, NULL);
#endif

    ehttpd_captdns_start(inst);
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
