#include "hal_task.h"
#include "hal/HardwareManager.h"
#include "utils/Queue.h"
#include "config/config_global.h"
#include "config/config_ioa.h"
#include "IEC104/core/IEC104Core.h"

extern HardwareManager hardwareManager;

void hal_task(void *pvParameters)
{
    QueueHandle_t measurementQueue = static_cast<QueueHandle_t>(pvParameters);

    while (true)
    {
        hardwareManager.readAllDigitalInputs();

        for (int i = 0; i < hardwareManager.getDigitalInputCount(); ++i) {
            if (hardwareManager.hasDigitalInputChanged(i)) {
                MeasurementData data;
                data.ioa = hardwareManager.getDigitalInputIoa(i);
                data.value = hardwareManager.getDigitalInputValue(i);
                data.type = M_SP_NA_1; // Single Point
                xQueueSend(measurementQueue, &data, pdMS_TO_TICKS(10));
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Read every 100ms
    }
}
