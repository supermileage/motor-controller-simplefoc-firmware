#pragma once

#define CAN_DEBUG               1

#define PIN_CAN_CS              5
#define CAN_SPEED               CAN_500KBPS
#define CAN_CONTROLLER_SPEED    MCP_8MHz
#define CAN_FRAME               0x0
#define CAN_STALE_TIME          1000
#define CAN_HEARTBEAT_TIME      250

#define THROTTLE_RAMP_INTERVAL  4

#include "mcp2515_can.h"

// This struct contains all the components of a CAN message. dataLength must be <= 8, 
// and the first [dataLength] positions of data[] must contain valid data
typedef uint8_t CanBuffer[8];
struct CanMessage {
        uint32_t id;
        uint8_t dataLength;
        CanBuffer data;
};

class CanThrottle {

    public:

        /**
         * CONSTRUCTOR
         * */
        CanThrottle();

        /**
         * @brief Run every loop to monitor CAN bus and update throttle value
         * */
        void loop();

        /**
         * @return Current throttle value
         * */
        uint8_t get();

    private:

        mcp2515_can can = mcp2515_can(PIN_CAN_CS);
        uint8_t throttleVal = 0;
        uint32_t timeLastCanMessage = 0;
        uint32_t timeLastThrottleRamp = 0;
        uint32_t timeLastHeartbeat = 0;

        /**
         * @brief Set throttle to 0 if last CAN message received is too old
         * */
        void checkStale();

        /**
         * @brief Converts CAN status message to readable output
         * 
         * @param errorCode CAN status message
         * @return Readable output
         * */
        String getCanError(uint8_t errorCode);

};