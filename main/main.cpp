

#include "G2Alarm.h"
#include "wifi-creds.h"

extern "C" void app_main()
{
    // Create the application, it will run on the main task
    // so set an appropriate stack size in the config.
    G2Alarm app;

    smooth::core::filesystem::Filesystem fs("/config", "app_storage", 3, true);
    fs.mount();

    smooth::core::network::Wifi& wifi = app.get_wifi();
    wifi.set_host_name("Alarm");
    wifi.set_ap_credentials(WIFI_SSID, WIFI_PASSWORD);
    wifi.set_auto_connect(true);
    wifi.connect_to_ap();

    // Start the application. Note that this function never returns.
    app.start();
}
