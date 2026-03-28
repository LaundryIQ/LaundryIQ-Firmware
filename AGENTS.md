# LaundryIQ Firmware — Agent Context

> Read `../LaundryIQ-Plan/PROJECT.md` for full project context before working here.

---

## What This Is

PlatformIO firmware for the ESP32-S3 LaundryIQ smart plug device.

The device:
- Measures AC current draw to determine machine state (off / idle / running)
- Reports state changes and heartbeat to the backend via HTTPS REST API
- Supports OTA firmware updates from GitHub Releases
- Provisions WiFi via SoftAP + Captive Portal
- Uses a pinhole reset button (200ms debounce, wipes NVS)

## Boards

| Environment | Board | Use |
|---|---|---|
| `esp32s3_dev` | ESP32-S3-DevKitM-1 | Development, flashing, debug |
| `esp32s3_prod` | ESP32-S3-MINI-1-N8 | Production PCB |

## Source Layout

```
src/
├── main.cpp        ← Entry point, setup(), loop()
├── config.h        ← Constants, pin defs, API endpoints
├── api.h / api.cpp ← HTTP client: heartbeat, state, claim, OTA check
├── sensor.h / sensor.cpp ← ADC + current sensing + state machine
├── storage.h / storage.cpp ← NVS: API key, WiFi creds, device config
├── captive.h / captive.cpp ← SoftAP + DNS + captive portal web server
├── led.h / led.cpp ← LED state machine
└── ota.h / ota.cpp ← OTA update check and apply
```

## API Reference

All device REST endpoints are documented in:
`../LaundryIQ-Plan/docs/software/api-requests.http`

Key endpoints:
- `POST /api/v1/device/claim` — First boot, after WiFi provisioned
- `POST /api/v1/device/heartbeat` — Every 300 seconds
- `POST /api/v1/device/state` — On state change only
- `GET /api/v1/device/ota/check` — Every 6 hours

## Auth

All authenticated requests use:
```
Authorization: Bearer liq_XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
X-Device-ID: A1B2C3D4E5F6  (12 uppercase hex from eFuse MAC)
Content-Type: application/json
```

API key stored in NVS (`storage.cpp`). Never shown to users.

## LED States

| State | Pattern | Meaning |
|---|---|---|
| Blue solid | — | Setup mode / SoftAP active |
| Amber slow pulse | — | Awaiting claim (no API key) |
| Green solid | — | Normal operation |
| Yellow flash | brief | Sending API request |
| Red solid | — | Error / cannot connect |

## State Machine

Machine states reported to API:
- `off` — No current / machine unplugged
- `idle` — Powered but not running (low current)
- `running` — Cycle active (high current)

UI-only states (derived, not from firmware):
- `complete` — Was running, now idle/off, within 5 min
- `offline` — No heartbeat for 10+ minutes

## OTA

- Pull-based over WiFi (not push)
- Device checks every 6 hours via `GET /device/ota/check`
- GitHub Releases host the compiled `.bin`
- HTTP redirect to download URL, device applies update in-place

## Build Commands

```bash
# Flash to dev board
pio run -e esp32s3_dev --target upload

# Serial monitor
pio device monitor -e esp32s3_dev

# Production build (no debug output)
pio run -e esp32s3_prod

# Run tests
pio test
```

## Coding Conventions

- No `Serial.println()` in production build (`CORE_DEBUG_LEVEL=0`)
- Use `#ifdef PROD_BUILD` to guard debug code
- JSON with `ArduinoJson` v7 — always use `JsonDocument` not `DynamicJsonDocument`
- HTTPS only — never downgrade to HTTP
- Handle 401 responses: clear API key from NVS, enter claim mode

## Compatibility

**No backward compatibility.** When changing NVS key names, API payload shapes, or LED state machine, convert fully. Never add compatibility shims for data formats the device used to use. If a field changes, flash all dev boards with the new firmware.

## Verification

For any change to the state machine, sensor logic, or API calls, document the expected behavior in a comment block at the top of the relevant function. Flash and test on dev board before considering it done.

---

*See `../LaundryIQ-Plan/` for architecture decisions and API documentation.*
