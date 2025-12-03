# GDevelop WebSocket Guide - Receiving AprilTag Data

## 🎮 Complete Guide for GDevelop WebSocket Integration

This guide shows you how to receive real-time AprilTag data from the Ubuntu Server in your GDevelop game.

---

## 📋 Table of Contents

1. [Quick Start](#quick-start)
2. [Message Format](#message-format)
3. [GDevelop Setup](#gdevelop-setup)
4. [Events Configuration](#events-configuration)
5. [Code Examples](#code-examples)
6. [Troubleshooting](#troubleshooting)

---

## 🚀 Quick Start

### Prerequisites

1. **Ubuntu Server Running**: 
   ```bash
   cd examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW/02-Ubuntu-Server_Hub
   python3 02-ubuntu_server_websocket-NOW.py
   ```

2. **Server Address**: Note your server IP (e.g., `192.168.1.100:5000`)

3. **GDevelop Extensions**: Install the **WebSocket Client** extension from the Extensions menu

---

## 📨 Message Format

### Server → GDevelop Messages

The server sends JSON messages with this structure:

```json
{
  "event": "apriltag_data",
  "data": {
    "tag_id": 5,
    "smartcam_ip": "192.168.1.50",
    "camera_name": "ESP32-Cam-01",
    "x_cm": 15.5,
    "y_cm": -8.2,
    "z_cm": 45.3,
    "yaw": 12.5,
    "pitch": -5.3,
    "roll": 0.8,
    "distance_cm": 48.6,
    "tag_size_percent": 15.2,
    "server_timestamp": 1234567890.123
  }
}
```

### Event Types

| Event | Description |
|-------|-------------|
| `connection_success` | Welcome message on connection |
| `identify_success` | Confirmation after identifying as GDevelop |
| `apriltag_data` | AprilTag position data (main message) |
| `no_data_available` | No data when requesting latest |
| `pong` | Response to ping |

---

## 🎮 GDevelop Setup

### Step 1: Add WebSocket Extension

1. Open your GDevelop project
2. Go to **Project Manager** → **Extensions**
3. Search for **"WebSocket Client"**
4. Click **Install** or **Add to project**

### Step 2: Create Scene Variables

Create these scene variables to store WebSocket data:

```
WebSocket_ServerURL = "ws://192.168.1.100:5000/websocket"
WebSocket_IsConnected = False (boolean)
WebSocket_LastEvent = ""
WebSocket_MessageReceived = False (boolean)

// AprilTag data variables
AprilTag_ID = 0
AprilTag_X_cm = 0
AprilTag_Y_cm = 0
AprilTag_Z_cm = 0
AprilTag_Yaw = 0
AprilTag_Pitch = 0
AprilTag_Roll = 0
AprilTag_Distance_cm = 0
```

### Step 3: Create Text Objects (for debugging)

1. Add a **Text** object named `DebugText_Connection`
2. Add a **Text** object named `DebugText_AprilTag`
3. Position them on screen for visibility

---

## ⚡ Events Configuration

### Event 1: Connect to Server (At Scene Start)

```
Condition: At the beginning of the scene

Actions:
  → WebSocket: Connect to URL Variable(WebSocket_ServerURL)
  → Text: Change the text of DebugText_Connection: "Connecting..."
```

### Event 2: Send Identify Message (When Connected)

```
Condition: WebSocket: Is connected

Sub-Condition: WebSocket_IsConnected = False (trigger once)

Actions:
  → Variable: Set WebSocket_IsConnected to True
  → WebSocket: Send message {"event": "identify", "data": {"type": "gdevelop"}}
  → Text: Change the text of DebugText_Connection: "Connected! Identified as GDevelop"
```

### Event 3: Receive Messages

```
Condition: WebSocket: Has received a message

Actions:
  → Variable: Set WebSocket_LastMessage to WebSocket.ReceivedMessage()
  → Variable: Set WebSocket_MessageReceived to True
  → Text: Change the text of DebugText_Connection: "Message: " + Variable(WebSocket_LastMessage)
```

### Event 4: Parse AprilTag Data (Advanced)

```
Condition: WebSocket_MessageReceived = True

Actions:
  → JavaScript: Execute this code (see below)
  → Variable: Set WebSocket_MessageReceived to False
```

**JavaScript Code for Parsing:**

```javascript
// Get the received message
const message = runtimeScene.getVariables().get("WebSocket_LastMessage").getAsString();

try {
    // Parse JSON
    const data = JSON.parse(message);
    
    // Check if this is AprilTag data
    if (data.event === "apriltag_data" && data.data) {
        // Store in scene variables
        runtimeScene.getVariables().get("AprilTag_ID").setNumber(data.data.tag_id || 0);
        runtimeScene.getVariables().get("AprilTag_X_cm").setNumber(data.data.x_cm || 0);
        runtimeScene.getVariables().get("AprilTag_Y_cm").setNumber(data.data.y_cm || 0);
        runtimeScene.getVariables().get("AprilTag_Z_cm").setNumber(data.data.z_cm || 0);
        runtimeScene.getVariables().get("AprilTag_Yaw").setNumber(data.data.yaw || 0);
        runtimeScene.getVariables().get("AprilTag_Pitch").setNumber(data.data.pitch || 0);
        runtimeScene.getVariables().get("AprilTag_Roll").setNumber(data.data.roll || 0);
        runtimeScene.getVariables().get("AprilTag_Distance_cm").setNumber(data.data.distance_cm || 0);
        
        // Store event type
        runtimeScene.getVariables().get("WebSocket_LastEvent").setString(data.event);
        
        console.log("AprilTag Data Received:", data.data);
    }
} catch (error) {
    console.error("Error parsing WebSocket message:", error);
}
```

### Event 5: Display AprilTag Data

```
Condition: AprilTag_ID > 0

Actions:
  → Text: Change the text of DebugText_AprilTag:
    "Tag ID: " + Variable(AprilTag_ID) + 
    "\nPosition: (" + ToString(Variable(AprilTag_X_cm)) + ", " + 
                      ToString(Variable(AprilTag_Y_cm)) + ", " + 
                      ToString(Variable(AprilTag_Z_cm)) + ") cm" +
    "\nDistance: " + ToString(Variable(AprilTag_Distance_cm)) + " cm"
```

### Event 6: Use AprilTag to Control Sprite

```
Condition: AprilTag_ID > 0

Actions:
  → Sprite: Change the position of Player:
    X: CameraCenter().X() + Variable(AprilTag_X_cm) * 2
    Y: CameraCenter().Y() - Variable(AprilTag_Y_cm) * 2
  
  → Sprite: Rotate Player to Variable(AprilTag_Yaw) degrees
```

---

## 💡 Code Examples

### Example 1: Simple Connection Test

**Purpose**: Just connect and see if messages arrive

**Events:**
```
1. At beginning of scene:
   → Connect to "ws://192.168.1.100:5000/websocket"

2. WebSocket: Is connected
   → Send message {"event": "identify", "data": {"type": "gdevelop"}}

3. WebSocket: Has received a message
   → Text: Set to WebSocket.ReceivedMessage()
```

### Example 2: Moving a Sprite with AprilTag

**Purpose**: Move a game object based on real-world AprilTag position

**Setup:**
- Create a Sprite object named `Robot`
- Create scene variables as listed above

**Events:**
```
1. At beginning of scene:
   → Connect to Variable(WebSocket_ServerURL)

2. WebSocket: Is connected AND Variable(WebSocket_IsConnected) = False
   → Send message {"event": "identify", "data": {"type": "gdevelop"}}
   → Set WebSocket_IsConnected to True

3. WebSocket: Has received a message
   → Run JavaScript code (parse JSON, see above)

4. AprilTag_ID > 0
   → Set Robot position X to AprilTag_X_cm * 5 + 400
   → Set Robot position Y to AprilTag_Y_cm * 5 + 300
   → Rotate Robot to AprilTag_Yaw degrees
```

### Example 3: Multiple AprilTags (Advanced)

**Purpose**: Track multiple AprilTags and create sprites for each

**Setup:**
- Create a Sprite object named `TagMarker`
- Create a scene structure variable `AprilTags` (array of structures)

**JavaScript Code:**
```javascript
const message = runtimeScene.getVariables().get("WebSocket_LastMessage").getAsString();

try {
    const data = JSON.parse(message);
    
    if (data.event === "apriltag_data" && data.data) {
        const tagId = data.data.tag_id;
        const tagsArray = runtimeScene.getVariables().get("AprilTags");
        
        // Find or create tag entry
        let tagIndex = -1;
        for (let i = 0; i < tagsArray.getChildrenCount(); i++) {
            if (tagsArray.getChild(i).getChild("id").getAsNumber() === tagId) {
                tagIndex = i;
                break;
            }
        }
        
        if (tagIndex === -1) {
            tagIndex = tagsArray.getChildrenCount();
            tagsArray.pushNew(tagId.toString());
        }
        
        // Store tag data
        const tag = tagsArray.getChild(tagIndex);
        tag.getChild("id").setNumber(tagId);
        tag.getChild("x").setNumber(data.data.x_cm);
        tag.getChild("y").setNumber(data.data.y_cm);
        tag.getChild("z").setNumber(data.data.z_cm);
        tag.getChild("yaw").setNumber(data.data.yaw);
        tag.getChild("lastUpdate").setNumber(Date.now());
        
        console.log("Updated tag", tagId);
    }
} catch (error) {
    console.error("Error:", error);
}
```

---

## 🐛 Troubleshooting

### Problem: Not Connecting

**Check:**
1. Server is running: `python3 02-ubuntu_server_websocket-NOW.py`
2. Server IP is correct in `WebSocket_ServerURL`
3. Use `ws://` not `wss://` for local connections
4. Port 5000 is not blocked by firewall

**Debug:**
```
Add text object showing:
"URL: " + Variable(WebSocket_ServerURL) + 
"\nConnected: " + Variable(WebSocket_IsConnected)
```

### Problem: Not Receiving Messages

**Check:**
1. Did you send identify message?
2. Is ESP32 connected to server?
3. Check server terminal for debug output

**Debug JavaScript:**
```javascript
console.log("Message received:", message);
console.log("Type:", typeof message);
```

### Problem: JSON Parse Error

**Check:**
1. Message format is correct JSON
2. No extra characters in message

**Fix:**
```javascript
try {
    const trimmed = message.trim();
    const data = JSON.parse(trimmed);
    // ... rest of code
} catch (error) {
    console.error("Parse error:", error.message);
    console.error("Raw message:", message);
}
```

### Problem: Data Not Updating

**Check:**
1. AprilTag is visible to ESP32 camera
2. ESP32 is sending data (check server terminal)
3. Variables are being updated (add debug text)

**Debug:**
```
Add event:
Condition: Always
Action: Text: Set to "Last Update: " + ToString(TimeSinceSceneStart())
```

---

## 📊 Server Debug Output

When the server is running with debug enabled, you'll see:

```
🔊 BROADCAST to GDevelop (1 clients):
   Event: apriltag_data
   Data: tag_id=5, x=15.5, y=-8.2, z=45.3
   Full JSON: {"event":"apriltag_data","data":{"tag_id":5,"x_cm":15.5...
   ✅ Sent to GDevelop client

📤 SEND to ESP32 (ACK): {"event":"apriltag_ack","status":"received"}
```

This confirms:
- ✅ Message was broadcast to GDevelop
- ✅ Data was sent successfully
- ✅ ESP32 received acknowledgment

---

## 🎯 Best Practices

1. **Connection Management**
   - Connect once at scene start
   - Store connection state in variable
   - Don't reconnect every frame

2. **Message Parsing**
   - Always use try-catch for JSON parsing
   - Validate data exists before using
   - Log errors to console

3. **Performance**
   - Don't process every message if receiving rapidly
   - Use frame rate limiting (process every Nth frame)
   - Store only needed data in variables

4. **Debugging**
   - Keep debug text visible during development
   - Use console.log() in JavaScript code
   - Check server terminal for connection status

---

## 📚 Additional Resources

- **WebSocket Extension Docs**: [GDevelop Wiki](https://wiki.gdevelop.io/)
- **Server Code**: `02-ubuntu_server_websocket-NOW.py`
- **Test HTML Client**: `03A-Gdevelop-Client-Test.html`

---

## 🎮 Example: Full Working Scene

This is a complete, working example you can copy into GDevelop:

### Scene Variables
```
WebSocket_ServerURL = "ws://192.168.1.100:5000/websocket"
WebSocket_IsConnected = False
WebSocket_LastMessage = ""
WebSocket_MessageReceived = False
AprilTag_ID = 0
AprilTag_X_cm = 0
AprilTag_Y_cm = 0
AprilTag_Z_cm = 0
```

### Objects
- Text: `DebugText` (position: 10, 10)
- Sprite: `Player` (position: 400, 300)

### Events
```
Event 1:
  Condition: At the beginning of the scene
  Actions:
    - WebSocket: Connect to Variable(WebSocket_ServerURL)
    - DebugText: Set text "Connecting..."

Event 2:
  Condition: WebSocket is connected
  Sub-Condition: WebSocket_IsConnected = False
  Actions:
    - Set WebSocket_IsConnected to True
    - WebSocket: Send {"event": "identify", "data": {"type": "gdevelop"}}
    - DebugText: Set text "Connected!"

Event 3:
  Condition: WebSocket has received message
  Actions:
    - Set WebSocket_LastMessage to WebSocket.ReceivedMessage()
    - Set WebSocket_MessageReceived to True

Event 4:
  Condition: WebSocket_MessageReceived = True
  Actions:
    - JavaScript: [Parse code from above]
    - Set WebSocket_MessageReceived to False

Event 5:
  Condition: AprilTag_ID > 0
  Actions:
    - Player: Set X position to AprilTag_X_cm * 3 + 400
    - Player: Set Y position to AprilTag_Y_cm * 3 + 300
    - DebugText: Set text "Tag " + Variable(AprilTag_ID) + 
                  " @ (" + Variable(AprilTag_X_cm) + ", " + Variable(AprilTag_Y_cm) + ")"
```

That's it! Your GDevelop game will now receive real-time AprilTag data! 🎉
