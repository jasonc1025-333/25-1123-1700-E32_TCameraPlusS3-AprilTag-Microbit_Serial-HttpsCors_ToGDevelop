# ⚡ WebSocket Quick Start

## 🎯 What You're Getting

**Real-time ESP32 → GDevelop communication via Pure WebSocket**

- ✅ 10-50ms latency (vs 100-300ms with HTTP)
- ✅ Lower ESP32 memory usage (20KB vs 30KB)
- ✅ Event-driven (no more polling timers!)
- ✅ Simple JSON messages
- ✅ GDevelop native WebSocket support

---

## 🚀 5-Minute Setup

### **1. Install Python Server**

 Create virtual environment
       cd 11i-GDevelop_Sim/
       python3 -m venv venv

       # Activate it (Linux/Mac)
       source venv/bin/activate

       # Install packages (isolated)
       pip install -r requirements.txt

       # Run server
       python3 server_pure_websocket.py

       # When done, deactivate
       deactivate

              * examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW

              25-1202-0600
              ## How Others Use Your Code:

              ```bash
              # They clone your repo
              git clone your-repo

              # They create THEIR OWN venv
              python3 -m venv venv
              source venv/bin/activate

              # They install from YOUR requirements.txt
              pip install -r requirements.txt

```bash
cd 11i-GDevelop_Sim/
pip install -r requirements.txt
python3 server_pure_websocket.py
```

### **2. Start ngrok**

```bash
ngrok http 5000
# Copy the URL: https://abc123.ngrok-free.app >> https://mallard-happy-singularly.ngrok-free.app
```

### **3. Update ESP32**

Add to `platformio.ini`:
```ini
lib_deps = 
    ArduinoWebsockets @ ^0.5.3
    ArduinoJson @ ^6.21.0
```

Update in your `.ino` file:
```cpp
const char* WS_SERVER_URL = "wss://abc123.ngrok-free.app/websocket"; >> https://mallard-happy-singularly.ngrok-free.app
```

### **4. Update GDevelop**

**Beginning of Scene:**
```
• WebSocketClient: Connect to wss://abc123.ngrok-free.app/websocket
• Wait 0.5 seconds
• WebSocketClient: Send message {"event":"identify","data":{"type":"gdevelop"}}
```

**Every Frame (remove old timer!):**
```
Condition: WebSocketClient: Has received a message
Action: Parse JSON and apply Physics2 forces (same as before)
```

---

## 📁 Files Created

1. ✅ **`server_pure_websocket.py`** - Pure WebSocket server (simpler than Socket.IO)
2. ✅ **`requirements.txt`** - Python dependencies
3. ✅ **`WEBSOCKET_MIGRATION_GUIDE.md`** - Complete step-by-step guide
4. ✅ **`README_WEBSOCKET.md`** - This file

---

## ✅ Testing

**Check ESP32 Serial Monitor:**
```
🔌 WebSocket CONNECTED!
✅ Identified as ESP32
📤 Sent: ID=5 Pos=(10.5,20.3,50.0)
```

**Check Python Console:**
```
📹 ESP32 IDENTIFIED
📡 AprilTag → GDevelop (1 clients): ID=5
🎮 GDevelop IDENTIFIED
```

**Check Browser:** http://localhost:5000/
```
ESP32: ✅
GDevelop Clients: 1
AprilTag Events: 123
```

---

## 🔍 Why Pure WebSocket (Not Socket.IO)?

| Feature | Socket.IO | Pure WebSocket |
|---------|-----------|----------------|
| Protocol | `42["event",data]` | `{"event":"data"}` |
| ESP32 Memory | 30-40KB | 15-20KB ✅ |
| GDevelop | Not compatible ❌ | Native support ✅ |
| Complexity | Higher | Lower ✅ |
| Debugging | Harder | Easier ✅ |

**Decision:** Pure WebSocket is simpler, uses less memory, and works natively with GDevelop's WebSocketClient extension.

---

## 📚 Full Documentation

See **`WEBSOCKET_MIGRATION_GUIDE.md`** for:
- Complete ESP32 code snippets
- GDevelop event sheet changes
- Troubleshooting guide
- Performance expectations
- Rollback plan

---

## 🆘 Quick Troubleshooting

**ESP32 won't connect?**
- Check ngrok is running: `ngrok http 5000`
- URL must start with `wss://` (not `ws://`)
- Try local first: `ws://10.42.0.1:5000/websocket`

**GDevelop not receiving data?**
- Check browser console (F12) for errors
- Verify URL matches ngrok domain
- JSON paths must be `$.data.tag_id` (not `$.tag_id`)

**Old HTTP still there?**
- That's OK! Keep it commented out as backup
- You can rollback if needed

---

## 🎯 Success = Real-Time Control

You've succeeded when:
1. ESP32 connects without errors
2. AprilTag data flows in real-time (<50ms)
3. No more polling timer in GDevelop
4. Smooth, instant robot movement

**Remember:** WebSocket pushes data instantly. No more waiting for timer ticks!

---

## 📊 Architecture

```
┌─────────────┐
│   ESP32     │  ArduinoWebsockets library
│ T-CameraS3  │  Message: {"event":"apriltag_data", "data":{...}}
└──────┬──────┘
       │ WebSocket (wss://ngrok/websocket)
       │ Latency: 5-15ms
       ▼
┌──────────────┐
│ Python Server│  Flask + simple-websocket
│   (Ubuntu)   │  Broadcasts to all GDevelop clients
└──────┬───────┘
       │ WebSocket (wss://ngrok/websocket)
       │ Latency: 5-15ms
       ▼
┌─────────────┐
│  GDevelop   │  Native WebSocketClient extension
│   (Game)    │  Event-driven updates (no polling!)
└─────────────┘
```

**Total Latency:** 10-30ms ✅ (vs 100-300ms with HTTP polling)

---

**Questions?** Check the full migration guide: `WEBSOCKET_MIGRATION_GUIDE.md`
