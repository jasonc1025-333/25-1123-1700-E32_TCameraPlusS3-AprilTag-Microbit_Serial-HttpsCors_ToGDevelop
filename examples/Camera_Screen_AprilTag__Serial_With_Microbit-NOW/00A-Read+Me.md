# 🚀 Quick Start Guide - ESP32 Smart Camera System

## One-Command Startup

This system includes an automated startup script that launches everything you need with debug terminals.

### Usage

```bash
cd examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW/
chmod +x 00B-Run_Me.sh
./00B-Run_Me.sh
```

### What It Does

The script automatically:

1. ✅ **Checks prerequisites** (Python3, PlatformIO, gnome-terminal)
2. ✅ **Sets up Python virtual environment** (if needed)
3. ✅ **Installs dependencies** (Flask, Flask-CORS, simple-websocket)
4. 🖥️ **Launches Ubuntu Server** in separate terminal with debug output
5. 📱 **Uploads ESP32 code** and starts serial monitor in separate terminal

### What You'll See

After running the script, you'll have **3 windows**:

1. **Main terminal** - Script status and summary
2. **Ubuntu WebSocket Server** terminal - Server debug prints
3. **ESP32 Serial Monitor** terminal - ESP32 debug prints

### System Architecture

```
┌─────────────────┐      WebSocket       ┌─────────────────┐
│   ESP32 Client  │ ◄──────────────────► │  Ubuntu Server  │
│  (T-Camera S3)  │  ws://10.0.0.149:5000│   (Python Hub)  │
└─────────────────┘                      └─────────────────┘
        │                                         │
        │ HTTP (Video)                            │ WebSocket
        │ http://10.0.0.149:5000/video            │
        └─────────────────────────────────────────┤
                                                  │
                                          ┌───────▼────────┐
                                          │ GDevelop Client│
                                          │  (Web Browser) │
                                          └────────────────┘
```

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

### 2. Upload & Monitor ESP32

```bash
# From project root
pio run -e lilygo-t-camera-plus-s3 --target upload
pio device monitor -e lilygo-t-camera-plus-s3
```

### 3. Open GDevelop Client

```bash
cd 03-GDevelop-Client/
# Open 03A-Gdevelop-Client-Test.html in browser
```

## Configuration

### WiFi Settings (ESP32)

Edit `01-Esp32-Client/01B-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp`:

```cpp
const char* WIFI_SSID = "Chan-Comcast";  // Your WiFi name
const char* WIFI_PASSWORD = "Jesus333!"; // Your WiFi password
```

### Server IP Address

Update both ESP32 and GDevelop client:

**ESP32** (`01B-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp`):
```cpp
const char* WS_HOST = "10.0.0.149";  // Your Ubuntu server IP
```

**GDevelop** (`03A-Gdevelop-Client-Test.html`):
```javascript
const WS_URL = 'ws://10.0.0.149:5000/websocket';
const VIDEO_URL = 'http://10.0.0.149:5000/video_stream';
```

### Timing Configuration (ESP32)

Adjust FPS/performance in `01B-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp`:

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

| Component | Port | Protocol |
|-----------|------|----------|
| WebSocket Server | 5000 | ws:// |
| Video Stream | 5000 | http:// |
| Serial Monitor | varies | USB |

## Getting Help

1. Check terminal outputs for error messages
2. Verify network connectivity (`ping 10.0.0.149`)
3. Review debug prints in both terminals
4. Check WiFi SSID/password configuration
5. Ensure all dependencies are installed

---

**Happy coding! 🎉**
