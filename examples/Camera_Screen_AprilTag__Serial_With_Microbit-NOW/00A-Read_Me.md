# 🚀 Quick Start Guide - ESP32 Smart Camera System

## ⚠️ CRITICAL: Correct Startup Sequence

**To avoid old ESP32 code interfering with the server, follow this exact order:**

### Step 1: Flash ESP32 First (While Server is OFF)
```bash
# 1. Ensure Ubuntu server is NOT running
#    Check: ps aux | grep python | grep websocket
#    Stop if running: Ctrl+C or kill <pid>

# 2. Flash new code to ESP32
#    - Connect via USB
#    - Upload via PlatformIO/Arduino IDE
#    - Wait for "Hard resetting via RTS pin..."

# 3. Verify new code running:
#    - Check Serial Monitor
#    - Should see "WebSocket Init" message
#    - ESP32 will wait for server
```

### Step 2: Start Ubuntu Server (After ESP32 is Ready)
```bash
cd examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW/02-Ubuntu-Server_Hub
python3 02-ubuntu_server_websocket-NOW.py

# Watch for: "🔌 NEW WebSocket CONNECTION"
```

### Step 3: Verify Connection
- ESP32 Monitor: `✅ WebSocket CONNECTED`
- Server Terminal: `📊 Active Connections: 1`

### Why This Order Matters
1. Old ESP32 code may have different settings
2. Starting server first = old code sends invalid data
3. Flashing first = clean state before server starts

---

## One-Command Startup (Automated)

This system includes an automated startup script that launches everything you need with debug terminals.

### Usage

```bash
cd examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW/
chmod +x 00-Run_Me.sh
./00-Run_Me.sh
```

### What It Does

The script automatically:

1. ✅ **Checks prerequisites** (Python3, gnome-terminal)
2. ✅ **Sets up virtual environments** (Server + ESP32/PlatformIO)
3. ️ **Launches Ubuntu WebSocket Server** (port 5000) in separate terminal
4. 🎮 **Launches GDevelop Game Server** (port 5100) in separate terminal
5. 📱 **Uploads ESP32 code** and starts serial monitor in separate terminal

### What You'll See

After running the script, you'll have **4 windows**:

1. **Main terminal** - Script status and summary
2. **Ubuntu WebSocket Server** terminal - WebSocket server debug prints (port 5000)
3. **GDevelop Game Server** terminal - HTTP server for game (port 5100)
4. **ESP32 Serial Monitor** terminal - ESP32 upload and debug prints

### System Architecture

```
┌─────────────────┐      WebSocket       ┌─────────────────┐
│   ESP32 Client  │ ◄──────────────────► │  Ubuntu Server  │
│  (T-Camera S3)  │  ws://10.0.0.149:5000│   (WebSocket)   │
└─────────────────┘                      └─────────────────┘
        │                                         │
        │ HTTP (Video)                            │ WebSocket
        │ http://10.0.0.149:5000/video            │
        └─────────────────────────────────────────┤
                                                  │
                                          ┌───────▼────────┐
                                          │ GDevelop Game  │
                                          │ localhost:5100 │
                                          └────────────────┘
```

### Virtual Environments (Isolated Setup)

The script creates isolated virtual environments:
- **Python Server venv**: Flask, Flask-CORS, simple-websocket
- **ESP32/PlatformIO venv**: PlatformIO with compatible click==7.1.2

**Benefits:**
- ✅ No system-wide package modifications
- ✅ Fully portable (works on any Ubuntu 22 machine)
- ✅ Auto-setup on first run (~2-3 minutes, instant after)
- ✅ Fixes PlatformIO compatibility issues

**First Run:** Takes 2-3 minutes to create venvs and install packages (~550MB)  
**Subsequent Runs:** Instant startup

## Manual Startup (Alternative)

If you prefer to start components manually:

### 1. Start Ubuntu Server

```bash
cd 02-Ubuntu-Server_Hub/
python3 -m venv venv
source venv/bin/activate
pip install flask flask-cors simple-websocket
python3 02-ubuntu_server_websocket-NOW.py
```

### 2. Start GDevelop Game Server

```bash
cd ../../11k-25-1202-1330--25-1127-0950-E32_SmartCam-ToUbuntuServerHub-ToGdevelop-WebSocket-NOW/export-Jwc--Gdevelop_Html_Server-NOW/
python3 -m http.server 5100
# Open browser: http://localhost:5100
```

### 3. Upload & Monitor ESP32

```bash
# From project root (activate venv first)
source examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW/01-Esp32-Client/venv/bin/activate
pio run -e Camera_Screen_AprilTag__Serial_With_Microbit-NOW__Esp32_Client_Websocket --target upload
pio device monitor -e Camera_Screen_AprilTag__Serial_With_Microbit-NOW__Esp32_Client_Websocket
```

## Configuration

### WiFi Settings (ESP32)

Edit `01-Esp32-Client/01-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp`:

```cpp
const char* WIFI_SSID = "Chan-Comcast";  // Your WiFi name
const char* WIFI_PASSWORD = "Jesus333!"; // Your WiFi password
```

### Server IP Address

Update both ESP32 and GDevelop client:

**ESP32** (`01-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp`):
```cpp
const char* WS_HOST = "10.0.0.149";  // Your Ubuntu server IP
```

**GDevelop** (`03A-Gdevelop-Client-Test.html`):
```javascript
const WS_URL = 'ws://10.0.0.149:5000/websocket';
const VIDEO_URL = 'http://10.0.0.149:5000/video_stream';
```

### Timing Configuration (ESP32)

Adjust FPS/performance in `01-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp`:

```cpp
// AprilTag data send rate (WebSocket)
const unsigned long AprilTag_Send_INTERVAL_MS = 1500;  // 1.5s (0.67 msg/sec)

// Video frame send rate (HTTP)
const unsigned long VideoFrame_Send_INTERVAL_MS = 3000;  // 3s (0.33 FPS)
```

## Troubleshooting

### ESP32 Upload Fails
- Check USB connection
- Verify correct COM port in PlatformIO
- Try pressing BOOT button during upload

### Server Port 5000 Already in Use
```bash
# Find process using port 5000
sudo lsof -i :5000
# Kill the process
sudo kill -9 <PID>
```

### WiFi Connection Failed
- Verify SSID and password
- Ensure 2.4GHz WiFi band (ESP32-S3 limitation)
- Check router is not blocking device

### WebSocket Connection Refused
- Verify Ubuntu server is running
- Check firewall settings
- Confirm IP address is correct

### No Video Stream
- Video uploads require successful WiFi connection
- Check HTTP URL matches server IP
- Monitor ESP32 serial output for errors

## Performance Tips

- **Lower latency**: Reduce `AprilTag_Send_INTERVAL_MS` to 500ms
- **More stability**: Increase `AprilTag_Send_INTERVAL_MS` to 2000ms
- **Faster video**: Reduce `VideoFrame_Send_INTERVAL_MS` to 1000ms (may cause lag)
- **Better quality**: Increase `VIDEO_JPEG_QUALITY` (1-100, higher = better quality, slower)

## System Requirements

- **Ubuntu Server**: Python 3.7+, Flask, simple-websocket
- **ESP32**: T-CameraPlus-S3, PlatformIO, 2.4GHz WiFi
- **GDevelop**: Modern web browser with WebSocket support

## Quick Reference

| Component | Port | Protocol | Access |
|-----------|------|----------|--------|
| WebSocket Server | 5000 | ws:// | ws://localhost:5000/websocket |
| Video Stream | 5000 | http:// | http://localhost:5000/video_stream |
| GDevelop Game | 6000 | http:// | http://localhost:5100 |
| Serial Monitor | varies | USB | (via PlatformIO) |

## Port Configuration

- **Port 5000**: Ubuntu WebSocket Server (AprilTag data + video stream)
- **Port 6000**: GDevelop Game Server (HTTP server for game files)
- **ESP32**: USB serial connection (variable COM port)

## Getting Help

1. Check terminal outputs for error messages
2. Verify network connectivity (`ping 10.0.0.149`)
3. Review debug prints in both terminals
4. Check WiFi SSID/password configuration
5. Ensure all dependencies are installed

---

**Happy coding! 🎉**
