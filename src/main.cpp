/**
 * LaundryIQ Firmware — main.cpp
 *
 * Entry point. Initialises all modules and runs the main state loop.
 *
 * State machine:
 *   SETUP → WiFi provisioned → CLAIM → OPERATIONAL
 *
 * See AGENTS.md for module layout and API reference.
 */

#include <Arduino.h>
#include "config.h"
#include "storage.h"
#include "led.h"
#include "captive.h"
#include "api.h"
#include "sensor.h"
#include "ota.h"

enum FirmwareState {
  STATE_SETUP,     // SoftAP active, awaiting WiFi provisioning
  STATE_CLAIM,     // WiFi connected, no API key yet
  STATE_RUNNING    // Fully operational
};

FirmwareState fwState = STATE_SETUP;

unsigned long lastHeartbeat = 0;
unsigned long lastOtaCheck  = 0;

void setup() {
  Serial.begin(115200);
  storageInit();
  ledInit();
  sensorInit();

  if (storageHasApiKey()) {
    // Device already provisioned — skip setup
    ledSet(LED_GREEN);
    fwState = STATE_RUNNING;
    // TODO: reconnect to saved WiFi credentials
  } else {
    ledSet(LED_BLUE);
    captivePortalStart();
    fwState = STATE_SETUP;
  }
}

void loop() {
  unsigned long now = millis();

  // Reset button: hold RESET_DEBOUNCE_MS to wipe NVS
  if (digitalRead(RESET_BTN_PIN) == LOW) {
    delay(RESET_DEBOUNCE_MS);
    if (digitalRead(RESET_BTN_PIN) == LOW) {
      storageClear();
      ESP.restart();
    }
  }

  switch (fwState) {
    case STATE_SETUP:
      captivePortalHandle();
      if (captivePortalWifiConnected()) {
        ledSet(LED_AMBER_PULSE);
        fwState = STATE_CLAIM;
      }
      break;

    case STATE_CLAIM: {
      String apiKey = apiClaim("washer"); // TODO: allow dryer type from portal
      if (apiKey.length() > 0) {
        storageSetApiKey(apiKey);
        ledSet(LED_GREEN);
        fwState = STATE_RUNNING;
        lastHeartbeat = now;
        lastOtaCheck  = now;
      } else {
        delay(CLAIM_POLL_INTERVAL_MS);
      }
      break;
    }

    case STATE_RUNNING:
      // Heartbeat
      if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
        bool ok = apiSendHeartbeat();
        if (!ok) {
          // 401 handled inside apiSendHeartbeat — device re-enters claim mode
        }
        lastHeartbeat = now;
      }

      // Sensor + state reporting
      sensorUpdate();

      // OTA check
      if (now - lastOtaCheck >= OTA_CHECK_INTERVAL_MS) {
        otaCheck();
        lastOtaCheck = now;
      }
      break;
  }

  delay(10);
}
