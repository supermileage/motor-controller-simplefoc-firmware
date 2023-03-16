#include "CanThrottle.h"
#include "can_common.h"

void CanThrottle::begin() {
    uint8_t error = can.begin(CAN_SPEED, CAN_CONTROLLER_SPEED);
    if (CAN_DEBUG)
        Serial.println("CAN INIT: " + getCanError(error));
}

void CanThrottle::loop()
{
    uint16_t newThrottleVal = throttleVal;

    // Listen for CAN messages
    if (can.checkReceive() == CAN_MSGAVAIL)
    {
        CanMessage message;
        message.dataLength = 0;
        can.readMsgBuf(&message.dataLength, message.data);
        message.id = can.getCanId();

        if (message.id == CAN_STEERING_THROTTLE)
        {
            newThrottleVal = message.data[0];
            timeLastCanMessage = millis();
        }

        if (CAN_DEBUG)
        {
            Serial.println("-----------------------------");
            Serial.print("CAN MESSAGE RECEIVED - ID: 0x");
            Serial.println(message.id, HEX);

            for (int i = 0; i < message.dataLength; i++)
            {
                Serial.print("0x");
                Serial.print(message.data[i], HEX);
                Serial.print("\t");
            }
            Serial.println();
        }
    }

    if (newThrottleVal != throttleVal)
    {
        // If new throttle value is higher, then increase throttle value by 1 at most once every
        // THROTTLE_RAMP_INTERVAL milliseconds.
        // This is here to prevent instant torque on the motor if mashing the throttle pedal. 
        if (newThrottleVal > throttleVal && millis() > timeLastThrottleRamp + THROTTLE_RAMP_INTERVAL)
        {
            throttleVal++;
            timeLastThrottleRamp = millis();
        }

        // If new throttle value is lower, lower it instantly.
        if (newThrottleVal < throttleVal)
        {
            throttleVal = newThrottleVal;
        }
    }

    checkStale();

    // Send a periodic heartbeat CAN message to let other devices on the CAN bus know we are connected
    if (millis() > timeLastHeartbeat + CAN_HEARTBEAT_TIME)
    {
        timeLastHeartbeat = millis();

        CanMessage msg;
        msg.id = THROTTLE_HEARTBEAT;
        msg.dataLength = 1;
        msg.data[0] = 0x1;
        uint8_t error = can.MCP_CAN::sendMsgBuf(msg.id, CAN_FRAME, msg.dataLength, msg.data);
        if (CAN_DEBUG)
            Serial.println("HEARTBEAT SEND: " + getCanError(error));
    }

    yield();
}

uint8_t CanThrottle::get()
{
    checkStale();
    return throttleVal;
}

void CanThrottle::checkStale() {
    // If the output is not 0 and it's been more than CAN_STALE_TIME milliseconds since the last
    // throttle CAN message, set the output to 0 as a safety measure
    if (throttleVal && (millis() > timeLastCanMessage + CAN_STALE_TIME))
    {
        throttleVal = 0;
        if (CAN_DEBUG)
            Serial.println("ERROR: NO DATA - Output set to 0v");
    }
}

String CanThrottle::getCanError(uint8_t errorCode)
{
    switch (errorCode)
    {
    case CAN_OK:
        return "CAN OK";
        break;
    case CAN_FAILINIT:
        return "CAN FAIL INIT";
        break;
    case CAN_FAILTX:
        return "CAN FAIL TX";
        break;
    case CAN_MSGAVAIL:
        return "CAN MSG AVAIL";
        break;
    case CAN_NOMSG:
        return "CAN NO MSG";
        break;
    case CAN_CTRLERROR:
        return "CAN CTRL ERROR";
        break;
    case CAN_GETTXBFTIMEOUT:
        return "CAN TX BF TIMEOUT";
        break;
    case CAN_SENDMSGTIMEOUT:
        return "CAN SEND MSG TIMEOUT";
        break;
    default:
        return "CAN FAIL";
        break;
    }
}
