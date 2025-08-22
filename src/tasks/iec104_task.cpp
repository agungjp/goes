#include "tasks/iec104_task.h"
#include <Arduino.h>
#include "iec104/core/IEC104Core.h"
#include "utils/Queue.h"
#include "iec104/iec104_config.h"

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
            if (len > (int)sizeof(frame.buffer)) len = sizeof(frame.buffer);
            frame.length = static_cast<uint16_t>(len);
            memcpy(frame.buffer, buf, frame.length);
            if (xPortInIsrContext()) {
                BaseType_t hp = pdFALSE;
                xQueueSendFromISR(queue, &frame, &hp);
                if (hp == pdTRUE) portYIELD_FROM_ISR();
            } else {
                xQueueSend(queue, &frame, pdMS_TO_TICKS(20));
            }
        }
    }, outgoingQueue);

    core->setCommandCallback([](void* ctx, uint16_t ioa, uint8_t command) {
        if (!ctx) return;
        HardwareManager* hw = static_cast<HardwareManager*>(ctx);
        for (int i = 0; i < circuitBreakerCount; ++i) {
            if (circuitBreakers[i].command_ioa == ioa) {
                hw->operateCircuitBreaker(circuitBreakers[i].cbNumber, command);
                break;
            }
        }
    }, params->hardwareManager);

    while (true) {
        core->loop();

        FrameData receivedFrame;
    if (xQueueReceive(incomingQueue, &receivedFrame, 0) == pdPASS) {
            core->processFrame(receivedFrame.buffer, receivedFrame.length);
        }

        MeasurementData measurement;
        if (xQueueReceive(measurementQueue, &measurement, 0) == pdPASS) {
            FrameData frame;
            if (core->buildDataFrame(measurement.ioa, measurement.value, measurement.type, frame.buffer, (byte&)frame.length)) {
                // Direct queue send (no callback path) – ISR context not expected here
                xQueueSend(outgoingQueue, &frame, pdMS_TO_TICKS(10));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
