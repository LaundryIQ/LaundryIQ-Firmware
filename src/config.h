#pragma once

// ─── API ──────────────────────────────────────────────────────────────────────
#define API_BASE "https://api.laundryiq.app"
#define API_VERSION "/api/v1"
#define API_CLAIM_PATH API_VERSION "/device/claim"
#define API_HEARTBEAT_PATH API_VERSION "/device/heartbeat"
#define API_STATE_PATH API_VERSION "/device/state"
#define API_OTA_PATH API_VERSION "/device/ota/check"

// ─── Firmware Version ─────────────────────────────────────────────────────────
#define FIRMWARE_VERSION "0.1.0"
#define HARDWARE_VERSION "hall_v1"

// ─── Timing ───────────────────────────────────────────────────────────────────
#define HEARTBEAT_INTERVAL_MS  (300 * 1000UL)  // 5 minutes
#define OTA_CHECK_INTERVAL_MS  (6 * 60 * 60 * 1000UL)  // 6 hours
#define CLAIM_POLL_INTERVAL_MS (60 * 1000UL)   // 60 seconds when unclaimed
#define RESET_DEBOUNCE_MS       200             // Pinhole button debounce

// ─── Pins ─────────────────────────────────────────────────────────────────────
#define RESET_BTN_PIN   0   // GPIO0, pinhole reset button
#define LED_PIN         2   // LED indicator (adjust per PCB rev)
#define SENSOR_ADC_PIN  4   // Current sensor analog output

// ─── SoftAP / Captive Portal ──────────────────────────────────────────────────
#define SOFTAP_SSID_PREFIX "LaundryIQ-"  // + device ID suffix
#define CAPTIVE_PORTAL_IP  "192.168.4.1"

// ─── Current Sensing ──────────────────────────────────────────────────────────
// MLX91221, 50A range, 3.3V supply, ratiometric output
// Calibrate these thresholds based on actual machine measurements
#define CURRENT_IDLE_THRESHOLD_A   0.5f   // Below = off
#define CURRENT_RUNNING_THRESHOLD_A 1.5f  // Above = running

// ─── NVS Keys ─────────────────────────────────────────────────────────────────
#define NVS_API_KEY  "api_key"
#define NVS_CLAIMED  "claimed"
