## About MeshCore

MeshCore is a lightweight, portable C++ library that enables multi-hop packet routing for embedded projects using LoRa and other packet radios. It is designed for developers who want to create resilient, decentralized communication networks that work without the internet.

## 🔍 What is MeshCore?

MeshCore now supports a range of LoRa devices, allowing for easy flashing without the need to compile firmware manually. Users can flash a pre-built binary using tools like Adafruit ESPTool and interact with the network through a serial console.
MeshCore provides the ability to create wireless mesh networks, similar to Meshtastic and Reticulum but with a focus on lightweight multi-hop packet routing for embedded projects. Unlike Meshtastic, which is tailored for casual LoRa communication, or Reticulum, which offers advanced networking, MeshCore balances simplicity with scalability, making it ideal for custom embedded solutions, where devices (nodes) can communicate over long distances by relaying messages through intermediate nodes. This is especially useful in off-grid, emergency, or tactical situations where traditional communication infrastructure is unavailable.

## ⚡ Key Features

* Multi-Hop Packet Routing
  * Devices can forward messages across multiple nodes, extending range beyond a single radio's reach.
  * Supports up to a configurable number of hops to balance network efficiency and prevent excessive traffic.
  * Nodes use fixed roles where "Companion" nodes are not repeating messages at all to prevent adverse routing paths from being used.
* Supports LoRa Radios – Works with Heltec, RAK Wireless, and other LoRa-based hardware.
* Decentralized & Resilient – No central server or internet required; the network is self-healing.
* Low Power Consumption – Ideal for battery-powered or solar-powered devices.
* Simple to Deploy – Pre-built example applications make it easy to get started.

---

## 🏷️ Team Edition

This branch (`main`) is the **MeshCore Team Edition** — a custom firmware build layered on top of stock MeshCore. It adds team-oriented features for GPS tracking, smart forwarding control, and unattended autonomous operation. The firmware version is suffixed with a Team build number (e.g. `v1.15.0.5`).

### ✨ Team Edition Features

#### 📡 Adaptive Forwarding Control

The companion radio firmware can now have its forwarding behaviour controlled dynamically by the connected app via new serial commands.

| Command | Code | Description |
|---|---|---|
| `CMD_SET_MAX_HOPS` | `73` | Set the maximum flood hop count (`flood_max`). `0` disables forwarding. |
| `CMD_SET_FORWARD_LIST` | `74` | Push a whitelist of up to 20 contact public-key prefixes (6 bytes each). Only messages from these contacts are forwarded. |

**Forwarding policy rules:**
- **Public channel messages are always blocked** from being forwarded, preventing channel spam.
- When a **whitelist** is active, only group messages from contacts in the list are forwarded.
- The whitelist **expires after 10 minutes** of no policy update, reverting to `flood_max`-only behaviour.
- If no policy update is received for **60 minutes**, forwarding is **hard-disabled** until the app reconnects and refreshes the policy.
- The `flood_max` value is persisted to flash, so forwarding behaviour is restored after reboots.

**Firmware capability flags** are reported in `RESP_CODE_SELF_INFO` so the app can detect support:
- `CAPABILITY_FORWARDING` (`0x01`) — Adaptive forwarding control supported.
- `CAPABILITY_AUTONOMOUS` (`0x02`) — Autonomous tracker mode supported.

---

#### 🛰️ Autonomous Tracker Mode

Autonomous mode allows a GPS-equipped companion radio to operate as a **standalone GPS beacon** without a phone or app connected. When enabled, the device periodically broadcasts `#TEL:` telemetry messages on a configured channel so other nodes can track it on the map.

| Command | Code | Description |
|---|---|---|
| `CMD_GET_AUTONOMOUS_SETTINGS` | `75` | Read the currently persisted autonomous tracker settings. |
| `CMD_SET_AUTONOMOUS_SETTINGS` | `76` | Write autonomous tracker settings. Requires a GPS unit to be present. |

**Settings:**

| Field | Type | Description |
|---|---|---|
| `autonomous_enabled` | `uint8_t` | `0` = disabled, `1` = enabled. |
| `autonomous_channel_hash` | `uint8_t` | First byte of the target channel's hash. Telemetry is sent on this channel. |
| `autonomous_interval_sec` | `uint16_t` | Minimum send interval in seconds (range: 10–3600, default: 30). |
| `autonomous_min_distance_m` | `uint16_t` | Minimum movement in metres before triggering an early send (`0` = interval-only, max: 5000). |

**Behaviour:**
- The device only sends a telemetry update when a **valid GPS fix** is available.
- Updates are triggered by **time interval** and/or **minimum distance moved** since the last send (Haversine formula).
- When autonomous mode is active, **incoming chat and channel messages are silently dropped** — the node acts as a tracker only, not a chat device.
- The splash screen and home screen both display **"Team Edition"** and an **"AUTONOMOUS"** status indicator when the mode is active.
- Message preview screens are suppressed in autonomous mode.

**Telemetry payload (`#TEL:` format):**  
An 11-byte binary struct (latitude, longitude, battery voltage, phone battery, forward status) is Base64-encoded and sent as the text of a group message: `#TEL:<base64>`.

---

#### 🔒 GPS Fix Validation

A `hasValidGpsFix()` helper ensures that GPS data is only acted on when a live, valid fix is available. This prevents stale or invalid coordinates from being transmitted.

---

#### 🔄 Device Name Change Triggers Hard Reset

When the device's advertised name is changed via `CMD_SET_ADVERT_NAME`, a hardware reset (`board.hardReset()`) is performed automatically so the new BLE advertising name takes effect immediately.

---

#### 💾 Backward-Compatible Preferences Storage

New Team Edition preferences (`flood_max`, `autonomous_enabled`, `autonomous_channel_hash`, `autonomous_interval_sec`, `autonomous_min_distance_m`) are appended to the existing preferences file using optional reads, maintaining full backward compatibility with devices running older firmware.

### NOTE: Version 1.15.0.4 Requires MeshCore-TEAM  1.1.0 or higher. No issues should arrise using older version of the app, or older versions but the special firmware functionality will not work.

## 🎯 What Can You Use MeshCore For?

* Off-Grid Communication: Stay connected even in remote areas.
* Emergency Response & Disaster Recovery: Set up instant networks where infrastructure is down.
* Outdoor Activities: Hiking, camping, and adventure racing communication.
* Tactical & Security Applications: Military, law enforcement, and private security use cases.
* IoT & Sensor Networks: Collect data from remote sensors and relay it back to a central location.

## 🚀 How to Get Started

- Watch the [MeshCore QuickStart Playlist](https://www.youtube.com/watch?v=iaFltojJrAc&list=PLshzThxhw4O4WU_iZo3NmNZOv6KMrUuF9) by The Comms Channel
- Watch the [MeshCore Technical Presentation](https://www.youtube.com/watch?v=OwmkVkZQTf4) by Liam Cottle.
- Read through our [Frequently Asked Questions](./docs/faq.md) and [Documentation](https://docs.meshcore.io).
- Flash the MeshCore firmware on a supported device.
- Connect with a supported client.

For developers:

- Install [PlatformIO](https://docs.platformio.org) in [Visual Studio Code](https://code.visualstudio.com).
- Clone and open the MeshCore repository in Visual Studio Code.
- See the example applications you can modify and run:
  - [Companion Radio](./examples/companion_radio) - For use with an external chat app, over BLE, USB or Wi-Fi.
  - [KISS Modem](./examples/kiss_modem) - Serial KISS protocol bridge for host applications. ([protocol docs](./docs/kiss_modem_protocol.md))
  - [Simple Repeater](./examples/simple_repeater) - Extends network coverage by relaying messages.
  - [Simple Room Server](./examples/simple_room_server) - A simple BBS server for shared Posts.
  - [Simple Secure Chat](./examples/simple_secure_chat) - Secure terminal based text communication between devices.
  - [Simple Sensor](./examples/simple_sensor) - Remote sensor node with telemetry and alerting.

The Simple Secure Chat example can be interacted with through the Serial Monitor in Visual Studio Code, or with a Serial USB Terminal on Android.

## ⚡️ MeshCore Flasher

We have prebuilt firmware ready to flash on supported devices.

- Launch https://meshcore.io/flasher
- Select a supported device
- Flash one of the firmware types:
  - Companion, Repeater or Room Server
- Once flashing is complete, you can connect with one of the MeshCore clients below.

## 📱 MeshCore Clients

**Companion Firmware**

The companion firmware can be connected to via BLE, USB or Wi-Fi depending on the firmware type you flashed.

- Web: https://app.meshcore.nz
- Android: https://play.google.com/store/apps/details?id=com.liamcottle.meshcore.android
- iOS: https://apps.apple.com/us/app/meshcore/id6742354151?platform=iphone
- NodeJS: https://github.com/liamcottle/meshcore.js
- Python: https://github.com/fdlamotte/meshcore-cli

**Repeater and Room Server Firmware**

The repeater and room server firmware can be set up via USB in the web config tool.

- https://config.meshcore.io

They can also be managed via LoRa in the mobile app by using the Remote Management feature.

## 🛠 Hardware Compatibility

MeshCore is designed for devices listed in the [MeshCore Flasher](https://meshcore.io/flasher)

## 📜 License

MeshCore is open-source software released under the MIT License. You are free to use, modify, and distribute it for personal and commercial projects.

## Contributing

Please submit PR's using 'dev' as the base branch!
For minor changes just submit your PR and we'll try to review it, but for anything more 'impactful' please open an Issue first and start a discussion. It is better to sound out what it is you want to achieve first, and try to come to a consensus on what the best approach is, especially when it impacts the structure or architecture of this codebase.

Here are some general principles you should try to adhere to:
* Keep it simple. Please, don't think like a high-level lang programmer. Think embedded, and keep code concise, without any unnecessary layers.
* No dynamic memory allocation, except during setup/begin functions.
* Use the same brace and indenting style that's in the core source modules. (A .clang-format is probably going to be added soon, but please do NOT retroactively re-format existing code. This just creates unnecessary diffs that make finding problems harder)

Help us prioritize! Please react with thumbs-up to issues/PRs you care about most. We look at reaction counts when planning work.

### Running unit tests

To run unit tests, run the following command:

```bash
pio test --environment native --verbose
```

## Road-Map / To-Do

There are a number of fairly major features in the pipeline, with no particular time-frames attached yet. In very rough chronological order:
- [X] Companion radio: UI redesign
- [X] Repeater + Room Server: add ACL's (like Sensor Node has)
- [X] Standardise Bridge mode for repeaters
- [ ] Repeater/Bridge: Standardise the Transport Codes for zoning/filtering
- [X] Core + Repeater: enhanced zero-hop neighbour discovery
- [ ] Core: round-trip manual path support
- [ ] Companion + Apps: support for multiple sub-meshes (and 'off-grid' client repeat mode)
- [ ] Core + Apps: support for LZW message compression
- [ ] Core: dynamic CR (Coding Rate) for weak vs strong hops
- [ ] Core: new framework for hosting multiple virtual nodes on one physical device
- [ ] V2 protocol spec: discussion and consensus around V2 packet protocol, including path hashes, new encryption specs, etc

## 📞 Get Support

- Report bugs and request features on the [GitHub Issues](https://github.com/ripplebiz/MeshCore/issues) page.
- Find additional guides and components on [my site](https://buymeacoffee.com/ripplebiz).
- Join [MeshCore Discord](https://meshcore.gg) to chat with the developers and get help from the community.
