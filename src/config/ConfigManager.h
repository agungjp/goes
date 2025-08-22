#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

class ConfigManager {
public:
    ConfigManager();
    void begin();
    bool saveDeviceConfig(String json);
    String getDeviceConfig();
    bool saveIoaConfig(String json);
    String getIoaConfig();
    // Returns a deterministic revision hash (hex) derived from persisted configs
    String getRevision();
    void loadFeatureFlags(bool &featDI, bool &featCB, bool &featDO);
    // Extended granular feature flags (optional use). Any flag not present defaults to true to avoid disabling functionality implicitly.
    void loadExtendedFeatureFlags(
        bool &featGFD,
        bool &featSupply,
        bool &featRemote,
        bool &featCB1,
        bool &featCB2,
        bool &featCB3,
        bool &featTemp,
        bool &featBMS,
        bool &featDICadangan,
        bool &featDOCadangan,
        bool &featEthernet,
        bool &featRTC
    );

    // Debug helpers (raw file contents without defaults merge)
    String getRawDeviceFile();
    String getRawIoaFile();

private:
    bool _initFS;
    String _readFile(const char* path);
    bool _writeFile(const char* path, String json);
};

#endif // CONFIG_MANAGER_H
