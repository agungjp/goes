#include "comm_task.h"
#include "iec104/transport/IEC104Communicator.h"
#include "utils/Queue.h"
#include "iec104/core/IEC104Core.h"

void comm_task(void *pvParameters)
{
    CommTaskParams* params = static_cast<CommTaskParams*>(pvParameters);
    IEC104Communicator* communicator = params->communicator; // now runtime-selected
    QueueHandle_t incomingQueue = params->incomingQueue;
    QueueHandle_t outgoingQueue = params->outgoingQueue;

    communicator->setFrameReceivedCallback([](void* ctx, const byte* buf, byte len) {
        QueueHandle_t queue = static_cast<QueueHandle_t>(ctx);
        if (queue != NULL) {
            FrameData frame;
            if (len > (int)sizeof(frame.buffer)) len = sizeof(frame.buffer);
            frame.length = static_cast<uint16_t>(len);
            memcpy(frame.buffer, buf, frame.length);
            // Use ISR-aware send
            if (xPortInIsrContext()) {
                BaseType_t hp = pdFALSE;
                xQueueSendFromISR(queue, &frame, &hp);
                if (hp == pdTRUE) portYIELD_FROM_ISR();
            } else {
                xQueueSend(queue, &frame, pdMS_TO_TICKS(20));
            }
        }
    }, incomingQueue);

    while (true)
    {
        communicator->loop();

        FrameData frameToSend;
        if (xQueueReceive(outgoingQueue, &frameToSend, 0) == pdPASS) {
            communicator->send(frameToSend.buffer, frameToSend.length);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
