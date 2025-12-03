# GDevelop: Parse AprilTag Flat JSON (Simplified!)

## 📦 Complete Guide for FLAT JSON Structure

**✨ NEW SIMPLIFIED FORMAT** - No nested objects, all fields at top level!

---

## 🎯 What We're Receiving

The server now sends this **FLAT JSON** format (much simpler!):

```json
{
  "event": "apriltag_data",
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
  "timestamp": 12345,
  "server_timestamp": 1734960000.123
}
```

**No more nested `data` object!** All fields are directly accessible.

---

## 📝 Step 1: Create Scene Structure Variable

### In GDevelop Editor:

1. **Open Scene Variables**
   - Click on the scene
   - Go to **Scene Properties** → **Scene Variables**

2. **Create Structure Variable** named `AprilTag`
   - Click **Add a variable**
   - Name it: `AprilTag`
   - Type: **Structure**

3. **Add Child Variables** to `AprilTag`:
   ```
   AprilTag (Structure)
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
   └─ timestamp (Number) = 0
   ```

---

## ⚡ Step 2: Set Up WebSocket Events

### Event 1: Connect to Server (Scene Start)

```
Condition: At the beginning of the scene

Actions:
  → WebSocket: Connect to "ws://10.0.0.149:5000/websocket"
```

### Event 2: Send Identify Message

```
Condition: WebSocket: Is connected

Actions:
  → WebSocket: Send message {"event":"identify","data":{"type":"gdevelop"}}
```

---

## 🔧 Step 3: Parse Flat JSON with JavaScript

### Event 3: Receive and Parse AprilTag Data

**Conditions:**
```
→ WebSocket: Has received a message
```

**Actions:**
```
→ JavaScript Code: Execute this code ↓
```

**JavaScript Code (SIMPLIFIED for Flat JSON):**

```javascript
// Get the received WebSocket message
const message = runtimeScene.getVariables().get("WebSocket_LastMessage").getAsString();

try {
    // Parse the JSON message
    const jsonData = JSON.parse(message);
    
    // Check if this is an apriltag_data event
    if (jsonData.event === "apriltag_data") {
        // Get the AprilTag structure variable
        const tag = runtimeScene.getVariables().get("AprilTag");
        
        // Store all data directly (NO jsonData.data - it's flat!)
        tag.getChild("tag_id").setNumber(jsonData.tag_id || 0);
        tag.getChild("smartcam_ip").setString(jsonData.smartcam_ip || "");
        tag.getChild("camera_name").setString(jsonData.camera_name || "");
        tag.getChild("x_cm").setNumber(jsonData.x_cm || 0);
        tag.getChild("y_cm").setNumber(jsonData.y_cm || 0);
        tag.getChild("z_cm").setNumber(jsonData.z_cm || 0);
        tag.getChild("yaw").setNumber(jsonData.yaw || 0);
        tag.getChild("pitch").setNumber(jsonData.pitch || 0);
        tag.getChild("roll").setNumber(jsonData.roll || 0);
        tag.getChild("distance_cm").setNumber(jsonData.distance_cm || 0);
        tag.getChild("tag_size_percent").setNumber(jsonData.tag_size_percent || 0);
        tag.getChild("timestamp").setNumber(jsonData.timestamp || 0);
        
        console.log("✅ AprilTag data stored:", {
            tag_id: jsonData.tag_id,
            position: [jsonData.x_cm, jsonData.y_cm, jsonData.z_cm],
            yaw: jsonData.yaw
        });
    } else {
        console.log("📨 Other message:", jsonData.event);
    }
} catch (error) {
    console.error("❌ Parse error:", error);
    console.error("   Message:", message);
}
```

**Key Difference from Old Code:**
- OLD (nested): `jsonData.data.tag_id`
- NEW (flat): `jsonData.tag_id` ✨ Much simpler!

---

## 🎮 Step 4: Use the Data in Events

### Example: Move Sprite Based on AprilTag Position

```
Conditions:
  → Variable AprilTag.tag_id > 0

Actions:
  → Sprite: Set X to 400 + Variable(AprilTag.x_cm) * 5
  → Sprite: Set Y to 300 - Variable(AprilTag.y_cm) * 5
  → Sprite: Rotate to Variable(AprilTag.yaw) degrees
```

### Example: Display Tag Info as Text

```
Conditions:
  → Always

Actions:
  → Text: Set text to "Tag: " + ToString(Variable(AprilTag.tag_id)) + 
                       "\nPos: (" + ToString(Variable(AprilTag.x_cm)) + 
                       ", " + ToString(Variable(AprilTag.y_cm)) + 
                       ", " + ToString(Variable(AprilTag.z_cm)) + ") cm" +
                       "\nDist: " + ToString(Variable(AprilTag.distance_cm)) + " cm"
```

### Example: Change Color Based on Distance

```
Conditions:
  → Variable AprilTag.distance_cm < 20

Actions:
  → Sprite: Change color to Red (255, 0, 0)
```

---

## 🔍 Debugging Tips

### 1. Print All Incoming Messages

```javascript
const message = runtimeScene.getVariables().get("WebSocket_LastMessage").getAsString();
console.log("📨 WebSocket received:", message);
```

### 2. Verify Data is Stored

```javascript
const tag = runtimeScene.getVariables().get("AprilTag");
console.log("Current data:", {
    tag_id: tag.getChild("tag_id").getAsNumber(),
    x: tag.getChild("x_cm").getAsNumber(),
    y: tag.getChild("y_cm").getAsNumber()
});
```

### 3. Test with Manual Data

```javascript
const tag = runtimeScene.getVariables().get("AprilTag");
tag.getChild("tag_id").setNumber(99);
tag.getChild("x_cm").setNumber(10.5);
tag.getChild("y_cm").setNumber(-5.2);
tag.getChild("z_cm").setNumber(30.0);
console.log("✅ Test data loaded!");
```

---

## ✅ Summary

**What Changed:**

1. ✅ **Simpler JSON** - No nested `data` object
2. ✅ **Easier Parsing** - Direct field access: `jsonData.tag_id`
3. ✅ **Less Code** - Fewer lines to parse
4. ✅ **Less Confusion** - No more "where is my data?" questions

**Key Points:**

- Use **double quotes** in JSON: `{"event":"identify"}`
- Access fields directly: `jsonData.x_cm` (not `jsonData.data.x_cm`)
- Store with `.getChild("field").setNumber()` or `.setString()`
- Access in events with `Variable(AprilTag.x_cm)`

**Migration from Old Format:**

If you have old code using `jsonData.data.tag_id`, simply change to `jsonData.tag_id`.

Now you have real-time AprilTag data with simpler code! 🎉
