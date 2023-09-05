#include "storage.h"

#include "frogfs/frogfs.h"
#include "frogfs/vfs.h"

#if defined(ESP_PLATFORM)
# include "frogfs/vfs.h"
# include <driver/gpio.h>
# include <esp_err.h>
# include <esp_vfs_fat.h>
# include <nvs.h>
# include <nvs_flash.h>
# if defined(ESP_PLATFORM)
#  include <driver/sdmmc_host.h>
#  include <sdmmc_cmd.h>
# endif
#endif

#include <assert.h>
#include <stdint.h>


extern const uint8_t frogfs_bin[];

frogfs_fs_t* frogfs;

#if defined(ESP_PLATFORM)
sdmmc_card_t* sdcard;
#endif

bool storage_init(void)
{
#if defined(ESP_PLATFORM)
    ESP_ERROR_CHECK(nvs_flash_init());
#endif

    frogfs_config_t frogfs_config = {
#if defined(ESP_PLATFORM)
        .part_label = "frogfs",
#else
        .addr = frogfs_bin,
#endif
    };
    frogfs = frogfs_init(&frogfs_config);
    assert(frogfs != NULL);

#if defined(ESP_PLATFORM)
    frogfs_vfs_conf_t vfs_conf = {
        .base_path = "/frogfs",
        .fs = frogfs,
        .max_files = 2,
    };
    ESP_ERROR_CHECK(frogfs_vfs_register(&vfs_conf));
#endif

#if defined(ESP_PLATFORM)
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY); // CMD
    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);  // D0
    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);  // D1
    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY); // D2
    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY); // D3

    esp_err_t err = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config,
            &mount_config, &sdcard);
    if (err == ESP_OK) {
        sdmmc_card_print_info(stdout, sdcard);
    }
#endif

    return true;
}
