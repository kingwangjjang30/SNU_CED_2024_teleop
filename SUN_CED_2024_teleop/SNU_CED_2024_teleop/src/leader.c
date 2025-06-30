#include <Dynamixel2Arduino.h>

// Apply settings based on the board and hardware being used.
#if defined(ARDUINO_OpenRB)  // When using OpenRB-150
    // OpenRB does not require a DIR control pin.
    #define DXL_SERIAL Serial1
    #define DEBUG_SERIAL Serial
    #define COM_SERIAL Serial2
    const int DXL_DIR_PIN = -1;
#endif

const float DYNAMIXEL_PROTOCOL_VERSION = 2.0;
const uint8_t DXL_ID_COUNT = 10;  // Number of motors to control
const uint8_t DXL_IDS[DXL_ID_COUNT] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // Array of motor IDs to control

const uint16_t GOAL_POSITION_ADDR = 116;       // Goal Position address
const uint16_t PROFILE_VELOCITY_ADDR = 112;    // Profile Velocity address

Dynamixel2Arduino dxl(DXL_SERIAL, DXL_DIR_PIN);

const uint16_t user_pkt_buf_cap = 1024;
uint8_t user_pkt_buf[user_pkt_buf_cap];

typedef struct sr_data {
    int32_t present_position;
} __attribute__((packed)) sr_data_t;

sr_data_t sr_data[DXL_ID_COUNT];
DYNAMIXEL::InfoSyncReadInst_t sr_infos;
DYNAMIXEL::XELInfoSyncRead_t info_xels_sr[DXL_ID_COUNT];

// This namespace is required to use DYNAMIXEL Control table item name definitions
using namespace ControlTableItem;

uint32_t tick;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    DEBUG_SERIAL.begin(115200);
    COM_SERIAL.begin(57600); // HC-05 communication speed
    dxl.begin(1000000);
    dxl.setPortProtocolVersion(DYNAMIXEL_PROTOCOL_VERSION);

    // Turn on torque for all motors.
    for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
        dxl.torqueOff(DXL_IDS[i]);
        dxl.setOperatingMode(DXL_IDS[i], OP_POSITION);
        dxl.torqueOn(DXL_IDS[i]);
    }

    sr_infos.packet.p_buf = user_pkt_buf;
    sr_infos.packet.buf_capacity = user_pkt_buf_cap;
    sr_infos.packet.is_completed = false;
    sr_infos.addr = 132;
    sr_infos.addr_length = 4;
    sr_infos.p_xels = info_xels_sr;
    sr_infos.xel_count = 0;

    for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
        info_xels_sr[i].id = DXL_IDS[i];
        info_xels_sr[i].p_recv_buf = (uint8_t*)&sr_data[i];
        sr_infos.xel_count++;
    }

    sr_infos.is_info_changed = true;

    // Array of initial positions for the motors
    int initialPositions[DXL_ID_COUNT] = {2095, 917, 3087, 2079, 1075, 2095, 917, 3087, 2079, 1075};

    // Set the profile velocity for each motor to move over 3 seconds
    for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
        int currentPosition = dxl.getPresentPosition(DXL_IDS[i]);
        int targetPosition = initialPositions[i];
        int distance = abs(targetPosition - currentPosition);
        int profileVelocity = distance / 3.0; // Set speed to move over 3 seconds
        dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_IDS[i], profileVelocity);
    }

    // Set the initial position for all motors.
    for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
        dxl.writeControlTableItem(GOAL_POSITION, DXL_IDS[i], initialPositions[i]);
        DEBUG_SERIAL.print("Set motor ");
        DEBUG_SERIAL.print(DXL_IDS[i]);
        DEBUG_SERIAL.print(" initial position to: ");
        DEBUG_SERIAL.println(initialPositions[i]);
    }

    // Wait 3 seconds for all motors to reach the target position
    delay(3000);

    // Turn off torque for all motors.
    for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
        dxl.torqueOff(DXL_IDS[i]);
    }

    DEBUG_SERIAL.println("Setup completed.");
    tick = micros();
}

void loop() {
    uint8_t recv_cnt = dxl.syncRead(&sr_infos);

    if (recv_cnt > 0) {
    // Concatenate the current position of all motors into one string
    String a = "";
    for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
        a += String(sr_data[i].present_position, DEC);
        if (i < DXL_ID_COUNT - 1) {
            a += ","; // Separate each position value with a comma
        }
    }

    if (10000 <= (micros() - tick)) {
        tick = micros();
        // Send data to the serial monitor and HC-05
        DEBUG_SERIAL.println(a);
        COM_SERIAL.println(a);
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
    }
}
