#include "tasks/iec104_task.h"
#include <Arduino.h>
#include "iec104/core/IEC104Core.h"
#include "utils/Queue.h"
#include "config/config_global.h"

void iec104_task(void *pvParameters) {
    Iec104TaskParams* params = static_cast<Iec104TaskParams*>(pvParameters);
    IEC104Core* core = params->core;
    QueueHandle_t incomingQueue = params->incomingQueue;
    QueueHandle_t outgoingQueue = params->outgoingQueue;
    QueueHandle_t measurementQueue = params->measurementQueue;

    core->setFrameReadyCallback([](void* ctx, const byte* buf, byte len) {
        QueueHandle_t queue = static_cast<QueueHandle_t>(ctx);
        if (queue != NULL) {
            FrameData frame;
            frame.length = len;
            memcpy(frame.buffer, buf, len);
            xQueueSend(queue, &frame, portMAX_DELAY);
        }
    }, outgoingQueue);

    while (true) {
        core->loop();

        FrameData receivedFrame;
        if (xQueueReceive(incomingQueue, &receivedFrame, 0) == pdPASS) {
            core->processFrame(receivedFrame.buffer, receivedFrame.length);
        }

        MeasurementData measurement;
        if (xQueueReceive(measurementQueue, &measurement, 0) == pdPASS) {
            core->sendData(measurement.ioa, measurement.value, measurement.type);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
