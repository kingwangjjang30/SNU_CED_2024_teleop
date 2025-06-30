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

typedef struct sw_data {
  int32_t goal_position;
} __attribute__((packed)) sw_data_t;

sw_data_t sw_data[DXL_ID_COUNT];
DYNAMIXEL::InfoSyncWriteInst_t sw_infos;
DYNAMIXEL::XELInfoSyncWrite_t info_xels_sw[DXL_ID_COUNT];

// This namespace is required to use DYNAMIXEL Control table item name definitions
using namespace ControlTableItem;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  DEBUG_SERIAL.begin(115200); 
  COM_SERIAL.begin(57600); // HC-05 communication speed
  dxl.begin(1000000);
  dxl.setPortProtocolVersion(DYNAMIXEL_PROTOCOL_VERSION);

  // Turn on torque for all motors.
  for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
    dxl.torqueOff(DXL_IDS[i]);
    if (i == 4 || i == 9) {
      dxl.setOperatingMode(DXL_IDS[i], OP_CURRENT_BASED_POSITION);
      dxl.setGoalCurrent(DXL_IDS[i], 740);
    } else {
      dxl.setOperatingMode(DXL_IDS[i], OP_POSITION);
    }
    dxl.torqueOn(DXL_IDS[i]);
    delay(10);
  }

  // Fill the members of the structure to syncWrite using the internal packet buffer
  sw_infos.packet.p_buf = nullptr;
  sw_infos.packet.is_completed = false;
  sw_infos.addr = GOAL_POSITION_ADDR;
  sw_infos.addr_length = 4;
  sw_infos.p_xels = info_xels_sw;
  sw_infos.xel_count = 0;

  for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
    info_xels_sw[i].id = DXL_IDS[i];
    info_xels_sw[i].p_data = (uint8_t*)&sw_data[i].goal_position;
    sw_infos.xel_count++;
  }

  sw_infos.is_info_changed = true;

  // Array of motor positions
  int initialPositions[DXL_ID_COUNT] = {2095, 917, 3087, 2079, 1075, 2095, 917, 3087, 2079, 1075};

  // Set the profile velocity for each motor to move over 3 seconds
  for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
    int currentPosition = dxl.getPresentPosition(DXL_IDS[i]);
    int targetPosition = initialPositions[i];
    int distance = abs(targetPosition - currentPosition);
    // int profileVelocity = distance / 3.0; // Set speed to move over 3 seconds
    dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_IDS[i], 20);
  }

  // Set the initial position for all motors.
  for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
    dxl.writeControlTableItem(GOAL_POSITION, DXL_IDS[i], initialPositions[i]);
    DEBUG_SERIAL.print("Set motor ");
    DEBUG_SERIAL.print(DXL_IDS[i]);
    DEBUG_SERIAL.print(" initial position to: ");
    DEBUG_SERIAL.println(initialPositions[i]);
  }

  DEBUG_SERIAL.println("Setup completed.");
}

void loop() {
  if (COM_SERIAL.available()) {
    String data = COM_SERIAL.readStringUntil('\n'); // Read data from HC-05.

    // Parse the data separated by commas.
    sw_infos.is_info_changed = false;
    sw_infos.xel_count = 0;
    int startIndex = 0;

    for (uint8_t i = 0; i < DXL_ID_COUNT; i++) {
      int endIndex = data.indexOf(',', startIndex);
      if (endIndex == -1) {
        if (i == 9) {
          endIndex = data.length();
          sw_infos.is_info_changed = true;
        } else {
          break;
        }
      }

      sw_data[i].goal_position = data.substring(startIndex, endIndex).toInt();
      sw_infos.xel_count++;

      if ((sw_data[i].goal_position < 0) || (4095 < sw_data[i].goal_position)) {
        sw_infos.is_info_changed = false;
        break;
      }
      startIndex = endIndex + 1;
    }

    // Set the goal position for all motors.
    if (sw_infos.is_info_changed) {
      dxl.syncWrite(&sw_infos);
    }

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
