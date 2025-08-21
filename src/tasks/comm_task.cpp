#include "comm_task.h"
#include "iec104/transport/IEC104Communicator.h"
#include "utils/Queue.h"
#include "IEC104/core/IEC104Core.h"

void comm_task(void *pvParameters)
{
    CommTaskParams* params = static_cast<CommTaskParams*>(pvParameters);
    IEC104Communicator* communicator = params->communicator;
    QueueHandle_t incomingQueue = params->incomingQueue;
    QueueHandle_t outgoingQueue = params->outgoingQueue;

    communicator->setFrameReceivedCallback([](void* ctx, const byte* buf, byte len) {
        QueueHandle_t queue = static_cast<QueueHandle_t>(ctx);
        if (queue != NULL) {
            FrameData frame;
            frame.length = len;
            memcpy(frame.buffer, buf, len);
            xQueueSend(queue, &frame, portMAX_DELAY);
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
