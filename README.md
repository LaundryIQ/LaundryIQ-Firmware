# LaundryIQ Firmware

ESP32-S3 firmware for the LaundryIQ smart plug device.

## Hardware

- **Dev board**: ESP32-S3-DevKitM-1
- **Prod MCU**: ESP32-S3-MINI-1-N8 (LCSC C2913206)
- **Current sensor**: MLX91221KDC-ABR-050-RE (Melexis Hall Effect, SOIC-8)
- **AC-DC**: HLK-PM01 (5V, 3W, 85-265V AC input)

## Setup

1. Install [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) or PlatformIO CLI
2. Clone repo
3. Copy `.env.example` to `.env` and fill in dev values (never commit `.env`)
4. Run `pio run -e esp32s3_dev --target upload` to flash dev board

## Project Structure

```
src/        # Main source files
include/    # Shared headers
lib/        # Local libraries
test/       # Unit tests
platformio.ini  # Board and build config
```

## API

Device communicates with `api.laundryiq.app` — see [LaundryIQ-Plan/docs/software/api-requests.http](../../Planning/LaundryIQ-Plan/docs/software/api-requests.http) for the full endpoint reference.

## OTA

Firmware updates are distributed via GitHub Releases. The device checks for updates every 6 hours automatically.
