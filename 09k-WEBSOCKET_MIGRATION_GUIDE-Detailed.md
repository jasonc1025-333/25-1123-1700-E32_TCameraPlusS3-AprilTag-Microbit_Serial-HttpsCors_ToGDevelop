# 🚀 WebSocket Migration Guide
## From HTTP Polling to Real-Time WebSocket Communication

---

## 📊 **Overview**

### **Current System (HTTP Polling)**
```
ESP32 → HTTP POST (100ms) → Python Server ← HTTP GET (100ms poll) ← GDevelop
        (High latency)                        (High server load)
```

### **New System (Pure WebSocket)**
```
ESP32 ⟷ WebSocket ⟷ Python Server ⟷ WebSocket ⟷ GDevelop
      (Real-time push, 10-50ms latency)
```

---

## 🎯 **Benefits**

| Feature | HTTP Polling | Pure WebSocket |
|---------|-------------|----------------|
| **Latency** | 100-300ms | 10-50ms |
| **Server Load** | High (10 req/s × N clients) | Low (persistent connection) |
| **ESP32 Memory** | 30KB (HTTP client) | 20KB (WebSocket) |
| **Battery Usage** | Higher | Lower |
| **Data Loss** | Possible (queue overflow) | Minimal |
| **Protocol** | HTTP REST | Standard WebSocket + JSON |

---

## 📦 **Installation**

### **Step 1: Python Server Setup**

```bash
# Navigate to server directory
cd examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW/11i-25-1122-2000-Important-SendDataToGdevelop/11i-GDevelop_Sim/

# Install dependencies
pip install -r requirements.txt

# OR install manually:
pip install Flask==3.0.0 Flask-CORS==4.0.0 simple-websocket==1.0.0
```

### **Step 2: Start Python Server**

```bash
# Start the server
python3 server_pure_websocket.py

# You should see:
# 🚀 Pure WebSocket Server Starting...
# 📡 WebSocket Endpoint: ws://localhost:5000/websocket
# ✅ Server ready!
```

### **Step 3: Setup ngrok (for public access)**

```bash
# In a new terminal
ngrok http 5000

# Copy the HTTPS URL (e.g., https://abc123.ngrok-free.app)
# WebSocket will be: wss://abc123.ngrok-free.app/websocket
```

---

## 🔧 **ESP32 WebSocket Client**

### **A. Update platformio.ini**

```ini
lib_deps = 
    # Existing libraries...
    ArduinoWebsockets @ ^0.5.3
    ArduinoJson @ ^6.21.0
```

### **B. Add to your .ino file**

#### **1. Include Headers** (top of file)

```cpp
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

using namespace websockets;
```

#### **2. Global Variables** (after other globals)

```cpp
// WebSocket connection
WebsocketsClient webSocket;
bool webSocketConnected = false;
unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 5000;  // 5 seconds

// WebSocket server (update with your ngrok URL)
const char* WS_SERVER_URL = "wss://your-ngrok.ngrok-free.app/websocket";
```

#### **3. Event Handlers** (before setup())

```cpp
void onWebSocketEvent(WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("🔌 WebSocket CONNECTED!");
        webSocketConnected = true;
        
        // Identify as ESP32
        StaticJsonDocument<128> doc;
        doc["event"] = "identify";
        doc["data"]["type"] = "esp32";
        
        String json;
        serializeJson(doc, json);
        webSocket.send(json);
    }
    else if (event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("🔌 WebSocket DISCONNECTED!");
        webSocketConnected = false;
    }
}

void onWebSocketMessage(WebsocketsMessage message) {
    Serial.print("📨 Message: ");
    Serial.println(message.data());
    
    // Parse responses
    StaticJsonDocument<256> doc;
    deserializeJson(doc, message.data());
    
    const char* event = doc["event"];
    
    if (strcmp(event, "identify_success") == 0) {
        Serial.println("✅ Identified as ESP32");
    }
    else if (strcmp(event, "apriltag_ack") == 0) {
        Serial.println("✅ AprilTag data acknowledged");
    }
}
```

#### **4. Connection Functions** (before setup())

```cpp
void connectWebSocket() {
    Serial.println("🔌 Connecting to WebSocket...");
    Serial.println(WS_SERVER_URL);
    
    webSocket.onMessage(onWebSocketMessage);
    webSocket.onEvent(onWebSocketEvent);
    
    webSocketConnected = webSocket.connect(WS_SERVER_URL);
    
    if (webSocketConnected) {
        Serial.println("✅ Connected!");
    } else {
        Serial.println("❌ Connection failed!");
    }
}

void checkWebSocketConnection() {
    if (!webSocketConnected) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt >= RECONNECT_INTERVAL) {
            lastReconnectAttempt = now;
            Serial.println("🔄 Reconnecting...");
            connectWebSocket();
        }
    }
}
```

#### **5. Send AprilTag Data** (replaces HTTP POST)

```cpp
void sendAprilTagDataWebSocket(int tag_id, float x_cm, float y_cm, float z_cm,
                                float yaw, float pitch, float roll,
                                float tag_size_percent, float distance_cm) {
    
    if (!webSocketConnected) {
        Serial.println("⚠️  Not connected");
        return;
    }
    
    // Create JSON message
    StaticJsonDocument<512> doc;
    doc["event"] = "apriltag_data";
    
    JsonObject data = doc.createNestedObject("data");
    data["smartcam_ip"] = WiFi.localIP().toString();
    data["tag_id"] = tag_id;
    data["camera_name"] = "ESP32-CAM";
    data["x_cm"] = x_cm;
    data["y_cm"] = y_cm;
    data["z_cm"] = z_cm;
    data["yaw"] = yaw;
    data["pitch"] = pitch;
    data["roll"] = roll;
    data["tag_size_percent"] = tag_size_percent;
    data["distance_cm"] = distance_cm;
    data["timestamp"] = millis();
    
    String json;
    serializeJson(doc, json);
    
    bool sent = webSocket.send(json);
    
    if (sent) {
        Serial.print("📤 Sent: ID=");
        Serial.print(tag_id);
        Serial.print(" Pos=(");
        Serial.print(x_cm, 1);
        Serial.print(",");
        Serial.print(y_cm, 1);
        Serial.print(",");
        Serial.print(z_cm, 1);
        Serial.println(")");
    }
}
```

#### **6. Modify setup()** (add after WiFi connect)

```cpp
void setup() {
    // ... existing setup code ...
    
    // After WiFi.begin() and while() loop:
    Serial.println("✅ WiFi connected!");
    
    // Connect to WebSocket server
    connectWebSocket();
    
    // ... rest of setup ...
}
```

#### **7. Modify loop()** (add WebSocket polling)

```cpp
void loop() {
    // Poll WebSocket for messages
    if (webSocketConnected) {
        webSocket.poll();
    }
    
    // Check connection status
    checkWebSocketConnection();
    
    // ... existing AprilTag detection code ...
    
    // When AprilTag detected, REPLACE HTTP POST with:
    if (det->id >= 0) {
        sendAprilTagDataWebSocket(
            det->id,
            apriltag_x_cm,
            apriltag_y_cm,
            apriltag_z_cm,
            yaw_deg,
            pitch_deg,
            roll_deg,
            tag_size_percent,
            distance_cm
        );
    }
    
    // ... rest of loop ...
}
```

---

## 🎮 **GDevelop WebSocket Integration**

### **Message Format**

GDevelop will receive JSON messages in this format:

```json
{
  "event": "apriltag_data",
  "data": {
    "tag_id": 5,
    "x_cm": 10.5,
    "y_cm": 20.3,
    "z_cm": 50.0,
    "yaw": 45.0,
    "pitch": 10.0,
    "roll": 5.0,
    "distance_cm": 55.2,
    "tag_size_percent": 15.5,
    "smartcam_ip": "10.42.0.100",
    "camera_name": "ESP32-CAM",
    "timestamp": 123456789,
    "server_timestamp": 1701234567.89
  }
}
```

### **GDevelop Event Sheet Changes**

#### **Scene: "Game" - Beginning of Scene**

```
Conditions: (none)

Actions:
  • WebSocketClient: Connect to server
      URL: wss://your-ngrok.ngrok-free.app/websocket
  
  • Wait: 0.5 seconds (allow connection to establish)
  
  • WebSocketClient: Send message
      Message: {"event":"identify","data":{"type":"gdevelop"}}
```

#### **Scene: "Game" - Every Frame (REMOVE OLD TIMER!)**

**OLD CODE (DELETE THIS):**
```
❌ Conditions: Timer "ServerPoll_100ms_Timer" > 0.1 seconds
❌ Actions: 
     • HTTPRequest: Send GET to server
     • Reset timer "ServerPoll_100ms_Timer"
```

**NEW CODE (ADD THIS):**
```
✅ Conditions: 
     • WebSocketClient: Has received a message

✅ Actions:
     • Variable: Set ServerRelayHub_BotData_Rx_RawJSON = WebSocketClient.MessageData()
     
     • Parse JSON into structure:
         - Variable: ServerRelayHub_BotData_Rx_Structure.tag_id = 
             JSONPath(ServerRelayHub_BotData_Rx_RawJSON, "$.data.tag_id")
         
         - Variable: ServerRelayHub_BotData_Rx_Structure.x_cm = 
             JSONPath(ServerRelayHub_BotData_Rx_RawJSON, "$.data.x_cm")
         
         - Variable: ServerRelayHub_BotData_Rx_Structure.y_cm = 
             JSONPath(ServerRelayHub_BotData_Rx_RawJSON, "$.data.y_cm")
         
         - Variable: ServerRelayHub_BotData_Rx_Structure.z_cm = 
             JSONPath(ServerRelayHub_BotData_Rx_RawJSON, "$.data.z_cm")
         
         - Variable: ServerRelayHub_BotData_Rx_Structure.yaw = 
             JSONPath(ServerRelayHub_BotData_Rx_RawJSON, "$.data.yaw")
         
         (... repeat for other fields ...)
     
     • WebSocketClient: Mark message as processed
     
     • (Keep your existing Physics2 movement code - NO CHANGES NEEDED)
```

### **Keep Your Existing Movement Code!**

The Physics2 movement logic stays exactly the same. Only the data reception method changes from HTTP polling to WebSocket events.

---

## ✅ **Testing Checklist**

### **Phase 1: Python Server**
- [ ] Install dependencies: `pip install -r requirements.txt`
- [ ] Start server: `python3 server_pure_websocket.py`
- [ ] Check console shows "✅ Server ready!"
- [ ] Open browser: http://localhost:5000/
- [ ] Status page shows "Pure WebSocket Server"

### **Phase 2: ngrok Setup**
- [ ] Start ngrok: `ngrok http 5000`
- [ ] Copy ngrok URL (e.g., https://abc123.ngrok-free.app)
- [ ] Test in browser: https://abc123.ngrok-free.app/
- [ ] Should show same status page

### **Phase 3: ESP32 Connection**
- [ ] Update `WS_SERVER_URL` with ngrok URL + `/websocket`
- [ ] Example: `wss://abc123.ngrok-free.app/websocket`
- [ ] Compile and upload to ESP32
- [ ] Open Serial Monitor (115200 baud)
- [ ] Look for: "🔌 WebSocket CONNECTED!"
- [ ] Look for: "✅ Identified as ESP32"
- [ ] Python console should show: "📹 ESP32 IDENTIFIED"
- [ ] Web status page should show: ESP32 ✅

### **Phase 4: AprilTag Data Flow**
- [ ] Point ESP32 camera at AprilTag
- [ ] Serial Monitor should show: "📤 Sent: ID=..."
- [ ] Python console should show: "📡 AprilTag → GDevelop"
- [ ] Web status page "AprilTag Events" counter increments

### **Phase 5: GDevelop Connection**
- [ ] Update GDevelop WebSocket URL with ngrok URL
- [ ] Run GDevelop game (preview or export)
- [ ] Python console should show: "🎮 GDevelop IDENTIFIED"
- [ ] Web status page should show: GDevelop Clients = 1
- [ ] Robot in game should move when AprilTag detected

### **Phase 6: End-to-End Test**
- [ ] Move AprilTag left/right
- [ ] GDevelop robot moves left/right (real-time!)
- [ ] Latency should feel instant (<50ms)
- [ ] No lag or delays
- [ ] Smooth robot movement

---

## 🐛 **Troubleshooting**

### **ESP32: "WebSocket connection failed!"**

**Problem:** Can't connect to server

**Solutions:**
1. Check ngrok URL is correct (must start with `wss://`)
2. Verify ngrok is running: `ngrok http 5000`
3. Check Python server is running
4. Test URL in browser first
5. Check WiFi connection: `WiFi.status() == WL_CONNECTED`

### **ESP32: "WebSocket DISCONNECTED!" (keeps reconnecting)**

**Problem:** Connection drops repeatedly

**Solutions:**
1. Check ngrok free tier limits
2. Verify network stability
3. Check server logs for errors
4. Try local network first (ws://10.42.0.1:5000/websocket)

### **GDevelop: Not receiving data**

**Problem:** Robot doesn't move, no data in variables

**Solutions:**
1. Check GDevelop console for WebSocket errors
2. Verify WebSocket URL in GDevelop matches ngrok URL
3. Check "identify" message was sent on scene start
4. Verify JSON parsing paths are correct ($.data.tag_id not $.tag_id)
5. Check Python console shows "🎮 GDevelop IDENTIFIED"

### **Data arrives but robot doesn't move**

**Problem:** Data is received but Physics2 doesn't apply forces

**Check:**
1. Variable names match exactly (case-sensitive)
2. Physics2 forces are still in event sheet
3. Robot object has Physics2 behavior
4. Collision masks are correct

---

## 📊 **Performance Expectations**

### **Latency Breakdown**

```
ESP32 → Server: 5-15ms (WiFi + ngrok)
Server → GDevelop: 5-15ms (WebSocket)
Total: 10-30ms ✅ (vs 100-300ms with HTTP)
```

### **Memory Usage (ESP32)**

```
HTTP Client: ~30KB RAM
WebSocket: ~20KB RAM
Saved: ~10KB (33% reduction) ✅
```

### **Server Load**

```
HTTP Polling: 10 req/s × N clients = High CPU
WebSocket: Persistent connection = Low CPU ✅
```

---

## 🔄 **Rollback Plan** (if needed)

If WebSocket doesn't work, you can rollback:

1. **ESP32:** Comment out WebSocket code, uncomment HTTP POST
2. **GDevelop:** Add back HTTP GET timer, remove WebSocket events
3. **Server:** Old HTTP server still works (`E32-Camera_Screen_AprilTag-SerialToMicrobit-Server_HttpsCors_to_MiniPcIntelUbuntu_Client-GDevelop_Sim-NOW.py`)

**Pro tip:** Keep old HTTP code commented out (don't delete) until WebSocket is fully tested.

---

## 🎯 **Success Criteria**

You've successfully migrated when:

1. ✅ ESP32 Serial Monitor shows "WebSocket CONNECTED"
2. ✅ Python console shows "ESP32 IDENTIFIED" and "AprilTag → GDevelop"
3. ✅ GDevelop robot moves instantly (<50ms) when AprilTag moves
4. ✅ No polling timer in GDevelop (removed)
5. ✅ Web status page shows all clients connected
6. ✅ No lag, smooth real-time control

---

## 📚 **Additional Resources**

- **Python Server:** `server_pure_websocket.py`
- **Requirements:** `requirements.txt`
- **Status Page:** http://localhost:5000/ (when server running)
- **ngrok docs:** https://ngrok.com/docs
- **WebSocket Protocol:** https://developer.mozilla.org/en-US/docs/Web/API/WebSocket

---

## 🆘 **Need Help?**

1. Check server logs (Python console output)
2. Check ESP32 Serial Monitor output
3. Check GDevelop browser console (F12)
4. Verify all URLs match (ngrok domain)
5. Test with local network first before ngrok

**Remember:** WebSocket is event-driven, not polling. The robot reacts to data pushes, not timer-based requests!
