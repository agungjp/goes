#ifndef WEBSERVER_MANAGER_H
#define WEBSERVER_MANAGER_H

#include <ESPAsyncWebServer.h>
#include "../config/ConfigManager.h"

class WebServerManager {
public:
    WebServerManager(ConfigManager& configManager);
    void begin();

private:
    AsyncWebServer _server;
    ConfigManager& _configManager;

    void _setupRoutes();
};

#endif // WEBSERVER_MANAGER_H
