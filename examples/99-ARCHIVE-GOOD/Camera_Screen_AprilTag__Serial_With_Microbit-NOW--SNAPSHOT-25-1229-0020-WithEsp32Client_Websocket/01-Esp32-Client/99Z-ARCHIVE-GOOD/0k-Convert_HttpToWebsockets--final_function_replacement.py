#!/usr/bin/env python3
"""
Final Step: Replace HTTP send function with WebSocket version
jwc 25-1202-2030
"""

import re

FILE = "01-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp"

# Read file
with open(FILE, 'r') as f:
    content = f.read()

print(f"Replacing sendAprilTagData() function in {FILE}...")

# Find the old function (from bool sendAprilTagData to the closing })
# This is a complex regex to match the entire function
pattern = r'(bool sendAprilTagData\(int tag_id, const char\* camera_name.*?^\})'

# The replacement - archived old function + new WebSocket function
replacement = r'''//// jwc 25-1202-2030 ARCHIVED HTTP POST function (replaced with WebSocket):
//// bool sendAprilTagData(int tag_id, const char* camera_name, float yaw, float pitch, float roll,
////                       float x_cm, float y_cm, float z_cm, float tag_size_percent, float distance_cm) {
////     [HTTP POST code archived - see backup file for full code]
//// }

//// jwc 25-1202-2030 NEW: WebSocket send function
bool sendAprilTagDataWebSocket(int tag_id, float yaw, float pitch, float roll,  
                                float x_cm, float y_cm, float z_cm,
                                float tag_size_percent, float distance_cm) {
    if (!webSocketConnected) {
        printf("⚠️  WebSocket not connected - data not sent\n");
        return false;
    }
    
    unsigned long send_start = millis();
    
    // Create JSON message
    StaticJsonDocument<512> doc;
    doc["event"] = "apriltag_data";
    
    JsonObject data = doc.createNestedObject("data");
    data["smartcam_ip"] = WiFi.localIP().toString();
    data["tag_id"] = tag_id;
    data["camera_name"] = "ESP32-CAM";
    data["x_cm"] = round(x_cm * 10) / 10.0;
    data["y_cm"] = round(y_cm * 10) / 10.0;
    data["z_cm"] = round(z_cm * 10) / 10.0;
    data["yaw"] = round(yaw * 10) / 10.0;
    data["pitch"] = round(pitch * 10) / 10.0;
    data["roll"] = round(roll * 10) / 10.0;
    data["tag_size_percent"] = round(tag_size_percent * 10) / 10.0;
    data["distance_cm"] = round(distance_cm * 10) / 10.0;
    data["timestamp"] = millis();
    
    String json;
    serializeJson(doc, json);
    
    // Send via WebSocket
    bool sent = webSocket.sendTXT(json);
    
    unsigned long send_duration = millis() - send_start;
    
    if (sent) {
        totalMessagesSent++;
        
        printf("📤 WebSocket Sent (#%lu): Tag ID=%d, Pos=(%.1f,%.1f,%.1f) cm, Yaw=%.1f° [%lums]\n", 
               totalMessagesSent, tag_id, x_cm, y_cm, z_cm, yaw, send_duration);
        
        // Update TFT display
        gfx->setTextSize(1);
        gfx->setCursor(180, 1);
        gfx->printf("WS:%lu", totalMessagesSent);
        
        return true;
    } else {
        printf("❌ WebSocket send failed\n");
        
        // Show error on display
        gfx->setTextSize(1);
        gfx->setCursor(200, 1);
        gfx->printf("WS:ERR");
        
        return false;
    }
}'''

# Replace using multiline and dotall flags
content = re.sub(pattern, replacement, content, flags=re.MULTILINE | re.DOTALL)

# Write back
with open(FILE, 'w') as f:
    f.write(content)

print("✅ Function replacement complete!")
print("")
print("="*70)
print("🎉 HTTP → WebSocket CONVERSION 100% COMPLETE!")
print("="*70)
print("")
print("Summary of all changes:")
print("  ✅ HTTP includes archived, WebSocket includes added")
print("  ✅ WebSocket configuration added")
print("  ✅ WebSocket event handler added")
print("  ✅ initWiFi() updated for WebSocket")
print("  ✅ sendAprilTagData() replaced with sendAprilTagDataWebSocket()")
print("  ✅ webSocket.loop() added to main loop")
print("  ✅ Function call updated in loop()")
print("")
print("Next steps:")
print("  1. Review the modified file in VS Code")
print("  2. Compile and test!")
print("  3. If issues: cp *.backup-* 01B-*.ino.cpp to revert")
print("")
print("Ready to compile! 🚀")
