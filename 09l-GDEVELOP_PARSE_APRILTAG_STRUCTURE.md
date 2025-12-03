# GDevelop: Parse AprilTag Data into Structure Variable

## 📦 Complete Guide to Parsing WebSocket JSON into GDevelop Structures

This guide shows you how to receive AprilTag data from the WebSocket server and store it in a GDevelop structure variable for easy access.

---

## 🎯 What We're Receiving

The server sends this JSON format:

```json
{
  "event": "apriltag_data",
  "data": {
    "tag_id": 5,
    "smartcam_ip": "10.0.0.26",
    "camera_name": "OV2640",
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

---

## 📝 Step 1: Create Scene Structure Variable

### In GDevelop Editor:

1. **Open Scene Variables**
   - Click on the scene
   - Go to **Scene Properties** → **Scene Variables**

2. **Create Structure Variable** named `AprilTagData`
   - Click **Add a variable**
   - Name it: `AprilTagData`
   - Type: **Structure**

3. **Add Child Variables** to `AprilTagData`:
   ```
   AprilTagData (Structure)
   ├─ tag_id (Number) = 0
   ├─ smartcam_ip (String) = ""
   ├─ camera_name (String) = ""
   ├─ x_cm (Number) = 0
   ├─ y_cm (Number) = 0
   ├─ z_cm (Number) = 0
   ├─ yaw (Number) = 0
   ├─ pitch (Number) = 0
   ├─ roll (Number) = 0
   ├─ distance_cm (Number) = 0
   ├─ tag_size_percent (Number) = 0
   └─ server_timestamp (Number) = 0
   ```

---

## ⚡ Step 2: Set Up WebSocket Events

### Event 1: Connect to Server (Scene Start)

```
Condition: At the beginning of the scene

Actions:
  → WebSocket: Connect to "ws://192.168.1.100:5000/websocket"
```

### Event 2: Send Identify Message

```
Condition: WebSocket: Is connected

Actions:
  → WebSocket: Send message {"event":"identify","data":{"type":"gdevelop"}}
```

Note: Use **double quotes** not single quotes!

---

## 🔧 Step 3: Parse Incoming Messages with JavaScript

### Event 3: Receive and Parse AprilTag Data

**Conditions:**
```
→ WebSocket: Has received a message
```

**Actions:**
```
→ JavaScript Code: Execute this code ↓
```

**JavaScript Code:**

```javascript
// Get the received WebSocket message
const message = runtimeScene.getVariables().get("WebSocket_LastMessage").getAsString();

// OR if using the extension directly:
const message = runtimeScene.getGame().getVariables().get("_WebSocketMessage").getAsString();

try {
    // Parse the JSON message
    const jsonData = JSON.parse(message);
    
    // Check if this is an apriltag_data event
    if (jsonData.event === "apriltag_data" && jsonData.data) {
        const tagData = jsonData.data;
        
        // Get the AprilTagData structure variable
        const aprilTagVar = runtimeScene.getVariables().get("AprilTagData");
        
        // Store all data in the structure
        aprilTagVar.getChild("tag_id").setNumber(tagData.tag_id || 0);
        aprilTagVar.getChild("smartcam_ip").setString(tagData.smartcam_ip || "");
        aprilTagVar.getChild("camera_name").setString(tagData.camera_name || "");
        aprilTagVar.getChild("x_cm").setNumber(tagData.x_cm || 0);
        aprilTagVar.getChild("y_cm").setNumber(tagData.y_cm || 0);
        aprilTagVar.getChild("z_cm").setNumber(tagData.z_cm || 0);
        aprilTagVar.getChild("yaw").setNumber(tagData.yaw || 0);
        aprilTagVar.getChild("pitch").setNumber(tagData.pitch || 0);
        aprilTagVar.getChild("roll").setNumber(tagData.roll || 0);
        aprilTagVar.getChild("distance_cm").setNumber(tagData.distance_cm || 0);
        aprilTagVar.getChild("tag_size_percent").setNumber(tagData.tag_size_percent || 0);
        aprilTagVar.getChild("server_timestamp").setNumber(tagData.server_timestamp || 0);
        
        console.log("✅ AprilTag data stored in structure:", {
            tag_id: tagData.tag_id,
            position: [tagData.x_cm, tagData.y_cm, tagData.z_cm],
            rotation: [tagData.yaw, tagData.pitch, tagData.roll]
        });
    }
} catch (error) {
    console.error("❌ Error parsing WebSocket message:", error);
}
```

---

## 🎮 Step 4: Use the Structure Data in Events

Now you can access the parsed data in regular GDevelop events!

### Example Event 4: Move Sprite Based on AprilTag Position

```
Conditions:
  → Variable AprilTagData.tag_id > 0

Actions:
  → Sprite: Set X position to 400 + Variable(AprilTagData.x_cm) * 5
  → Sprite: Set Y position to 300 - Variable(AprilTagData.y_cm) * 5
  → Sprite: Rotate to Variable(AprilTagData.yaw) degrees
```

### Example Event 5: Display Tag Info as Text

```
Conditions:
  → Always

Actions:
  → Text: Change text to "Tag ID: " + ToString(Variable(AprilTagData.tag_id)) + 
                        "\nPosition: (" + ToString(Variable(AprilTagData.x_cm)) + 
                        ", " + ToString(Variable(AprilTagData.y_cm)) + 
                        ", " + ToString(Variable(AprilTagData.z_cm)) + ") cm" +
                        "\nDistance: " + ToString(Variable(AprilTagData.distance_cm)) + " cm"
```

### Example Event 6: Change Sprite Color Based on Distance

```
Conditions:
  → Variable AprilTagData.distance_cm < 20

Actions:
  → Sprite: Change color to Red (255, 0, 0)

---

Conditions:
  → Variable AprilTagData.distance_cm >= 20
  → Variable AprilTagData.distance_cm < 50

Actions:
  → Sprite: Change color to Yellow (255, 255, 0)

---

Conditions:
  → Variable AprilTagData.distance_cm >= 50

Actions:
  → Sprite: Change color to Green (0, 255, 0)
```

---

## 🎯 Alternative: Simpler Approach Without Full Structure

If you don't need all fields, you can extract just what you need:

```javascript
const message = runtimeScene.getVariables().get("WebSocket_LastMessage").getAsString();

try {
    const jsonData = JSON.parse(message);
    
    if (jsonData.event === "apriltag_data") {
        const data = jsonData.data;
        
        // Store only the fields you need
        runtimeScene.getVariables().get("TagID").setNumber(data.tag_id || 0);
        runtimeScene.getVariables().get("TagX").setNumber(data.x_cm || 0);
        runtimeScene.getVariables().get("TagY").setNumber(data.y_cm || 0);
        runtimeScene.getVariables().get("TagZ").setNumber(data.z_cm || 0);
        runtimeScene.getVariables().get("TagYaw").setNumber(data.yaw || 0);
        
        console.log("Tag detected:", data.tag_id, "at", data.x_cm, data.y_cm, data.z_cm);
    }
} catch (error) {
    console.error("Parse error:", error);
}
```

---

## 📊 Complete Working Example Scene

### Scene Variables Setup:
```
AprilTagData (Structure)
├─ tag_id (Number) = 0
├─ x_cm (Number) = 0
├─ y_cm (Number) = 0
├─ z_cm (Number) = 0
├─ yaw (Number) = 0
├─ pitch (Number) = 0
├─ roll (Number) = 0
└─ distance_cm (Number) = 0

WebSocket_ServerURL (String) = "ws://192.168.1.100:5000/websocket"
WebSocket_Connected (Boolean) = False
WebSocket_LastMessage (String) = ""
```

### Scene Objects:
- **Sprite**: Named `Robot` (to be controlled by AprilTag)
- **Text**: Named `DebugText` (to display tag info)

### Events:

**Event 1: Connect on Start**
```
Condition: At the beginning of the scene

Actions:
  → WebSocket: Connect to Variable(WebSocket_ServerURL)
```

**Event 2: Identify Once Connected**
```
Condition: WebSocket: Is connected
Sub-Condition: WebSocket_Connected = False

Actions:
  → Variable: Set WebSocket_Connected to True
  → WebSocket: Send message {"event":"identify","data":{"type":"gdevelop"}}
```

**Event 3: Parse Incoming Data**
```
Condition: WebSocket: Has received a message

Actions:
  → Variable: Set WebSocket_LastMessage to WebSocket.ReceivedMessage()
  → JavaScript: [Paste the parsing code from Step 3]
```

**Event 4: Update Robot Position**
```
Condition: AprilTagData.tag_id > 0

Actions:
  → Robot: Set X to 400 + AprilTagData.x_cm * 3
  → Robot: Set Y to 300 - AprilTagData.y_cm * 3
  → Robot: Rotate to AprilTagData.yaw degrees
```

**Event 5: Update Debug Text**
```
Condition: Always

Actions:
  → DebugText: Set text to "Tag: " + ToString(AprilTagData.tag_id) +
                            "\nPos: " + ToString(AprilTagData.x_cm) + ", " + ToString(AprilTagData.y_cm) +
                            "\nDist: " + ToString(AprilTagData.distance_cm) + " cm"
```

---

## 🔍 Debugging Tips

### 1. Check WebSocket Messages

Add this JavaScript code to see ALL messages:

```javascript
const message = runtimeScene.getVariables().get("WebSocket_LastMessage").getAsString();
console.log("WebSocket received:", message);
```

### 2. Verify Structure Updates

```javascript
const aprilTagVar = runtimeScene.getVariables().get("AprilTagData");
console.log("Current structure:", {
    tag_id: aprilTagVar.getChild("tag_id").getAsNumber(),
    x: aprilTagVar.getChild("x_cm").getAsNumber(),
    y: aprilTagVar.getChild("y_cm").getAsNumber()
});
```

### 3. Test with Manual Data

Add a test button:

```
Condition: Key Space pressed

Actions:
  → JavaScript: 
  
const testData = {
    event: "apriltag_data",
    data: {
        tag_id: 99,
        x_cm: 10,
        y_cm: 20,
        z_cm: 30,
        yaw: 45
    }
};

const aprilTagVar = runtimeScene.getVariables().get("AprilTagData");
aprilTagVar.getChild("tag_id").setNumber(testData.data.tag_id);
aprilTagVar.getChild("x_cm").setNumber(testData.data.x_cm);
aprilTagVar.getChild("y_cm").setNumber(testData.data.y_cm);

console.log("Test data loaded!");
```

---

## ✅ Summary

**What You've Learned:**

1. ✅ Create structure variables in GDevelop
2. ✅ Parse JSON messages from WebSocket
3. ✅ Store parsed data in structure variables
4. ✅ Access structure data in events
5. ✅ Use AprilTag data to control game objects

**Key Points:**

- Use **double quotes** in JSON messages: `{"event":"identify"}`
- Parse with `JSON.parse()` in JavaScript code
- Store in structure with `.getChild("fieldname").setNumber()` or `.setString()`
- Access in events with `Variable(AprilTagData.x_cm)`

Now you have real-time AprilTag position data controlling your GDevelop game! 🎉
