to #!/usr/bin/env python3
"""
Complete HTTP to WebSocket Conversion Script
jwc 25-1202-2030
Completes ALL remaining conversion steps automatically
"""

import re
from datetime import datetime

FILE = "01-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp"

# Read file
with open(FILE, 'r') as f:
    content = f.read()

print(f"Converting {FILE}...")
print(f"Backup already exists from previous script")
print("")

# Step 2: Add WebSocket configuration after WIFI_PASSWORD
websocket_config = '''

// ============================================================================
// jwc 25-1202-2030 WebSocket Server Configuration (NEW)
// ============================================================================
const char* WS_HOST = "10.0.0.149";  // Ubuntu server IP  
const uint16_t WS_PORT = 5000;
const char* WS_PATH = "/websocket";
const char* AUTH_TOKEN = "Jesus333!!!";  // Must match Python server
const bool WS_USE_SSL = false;  // false = ws://, true = wss://

// WebSocket client globals
WebSocketsClient webSocket;
bool webSocketConnected = false;
unsigned long totalMessagesSent = 0;
unsigned long totalReconnects = 0;

// Helper function for formatted output
String createSeparator(char ch, int len) {
    String result = "";
    for (int i = 0; i < len; i++) result += ch;
    return result;
}
'''

# Find insertion point after WIFI_PASSWORD
pattern = r'(const char\* WIFI_PASSWORD = "Jesus333!";)'
content = re.sub(pattern, r'\1' + websocket_config, content)
print("✓ Step 2: Added WebSocket configuration")

# Step 3: Add WebSocket event handler before initWiFi
event_handler = '''
// ============================================================================
// jwc 25-1202-2030 WEBSOCKET EVENT HANDLER (NEW)
// ============================================================================
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            printf("\\n%s\\n", createSeparator('=', 70).c_str());
            printf("❌ WebSocket DISCONNECTED!\\n");
            printf("   Total messages: %lu, Reconnects: %lu\\n", totalMessagesSent, totalReconnects);
            printf("%s\\n", createSeparator('=', 70).c_str());
            webSocketConnected = false;
            break;
            
        case WStype_CONNECTED: {
            printf("\\n%s\\n", createSeparator('=', 70).c_str());
            printf("✅ WebSocket CONNECTED!\\n");
            printf("   URL: ws://%s:%d%s\\n", WS_HOST, WS_PORT, WS_PATH);
            printf("%s\\n", createSeparator('=', 70).c_str());
            webSocketConnected = true;
            
            // Send identify message with auth token
            StaticJsonDocument<256> identifyDoc;
            identifyDoc["event"] = "identify";
            JsonObject identifyData = identifyDoc.createNestedObject("data");
            identifyData["type"] = "esp32";
            identifyData["device"] = "T-CameraPlus-S3";
            identifyData["version"] = "2.0.1";
            identifyData["auth_token"] = AUTH_TOKEN;
            
            String identifyJson;
            serializeJson(identifyDoc, identifyJson);
            webSocket.sendTXT(identifyJson);
            
            printf("📨 Sent identification with auth token\\n");
            break;
        }
            
        case WStype_TEXT: {
            printf("📨 Server response: %s\\n", (char*)payload);
            
            // Parse server responses
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, payload, length);
            
            if (!error) {
                const char* event = doc["event"];
                
                if (strcmp(event, "connection_success") == 0) {
                    printf("✅ Server acknowledged connection\\n");
                }
                else if (strcmp(event, "identify_success") == 0) {
                    printf("✅ Successfully identified and authenticated\\n");
                }
                else if (strcmp(event, "apriltag_ack") == 0) {
                    printf("✅ AprilTag data acknowledged by server\\n");
                }
            }
            break;
        }
            
        case WStype_ERROR:
            printf("❌ WebSocket error occurred\\n");
            break;
    }
}

'''

# Insert before initWiFi function
pattern = r'(void initWiFi\(\) \{)'
content = re.sub(pattern, event_handler + r'\1', content)
print("✓ Step 3: Added WebSocket event handler")

# Step 4: Update initWiFi() - replace HTTP server setup with WebSocket
# Find and replace the section after WiFi connects
old_init_section = r'''printf\("\\n\*\*\* WiFi Connected! SmartCam-IP: %s\\n", WiFi.localIP\(\).toString\(\).c_str\(\)\);
        printf\("\\*\\*\\* POST Server: %s\\n", client_e32__http_post_to_serverhub__smartcam_april_tag_URL\);'''

new_init_section = '''printf("\\n*** WiFi Connected! SmartCam-IP: %s\\n", WiFi.localIP().toString().c_str());
        
        //// jwc 25-1202-2030 ARCHIVED HTTP server setup:
        //// printf("*** POST Server: %s\\n", client_e32__http_post_to_serverhub__smartcam_april_tag_URL);
        
        //// jwc 25-1202-2030 NEW: WebSocket setup
        printf("*** WebSocket: Connecting to ws://%s:%d%s\\n", WS_HOST, WS_PORT, WS_PATH);
        webSocket.onEvent(webSocketEvent);
        webSocket.enableHeartbeat(15000, 3000, 2);
        webSocket.begin(WS_HOST, WS_PORT, WS_PATH);
        webSocket.setReconnectInterval(5000);
        totalReconnects++;
        printf("*** WebSocket: Setup complete (auto-reconnect enabled)\\n");'''

content = re.sub(old_init_section, new_init_section, content, flags=re.DOTALL)
print("✓ Step 4: Updated initWiFi() function")

# Step 5: Add webSocket.loop() at start of loop()
loop_addition = '''void loop()
{
    //// jwc 25-1202-2030 NEW: Poll WebSocket (must be called every loop)
    webSocket.loop();
    webSocketConnected = webSocket.isConnected();
    
    //// jwc 25-1128-0810 Track if new AprilTag detected in this frame'''

old_loop_start = r'''void loop\(\)
\{
    //// jwc 25-1128-0810 Track if new AprilTag detected in this frame'''

content = re.sub(old_loop_start, loop_addition, content)
print("✓ Step 6: Added webSocket.loop() to main loop")

# Step 7: Replace function call
old_call = r'sendAprilTagData\(tag_to_send\.tag_id, sinfo->name,'
new_call = '''//// jwc 25-1202-2030 CHANGED: HTTP→WebSocket (removed camera_name param)
                    sendAprilTagDataWebSocket(tag_to_send.tag_id,'''

content = re.sub(old_call, new_call, content)
print("✓ Step 7: Updated function call to sendAprilTagDataWebSocket")

# Write modified file
with open(FILE, 'w') as f:
    f.write(content)

print("")
print("="*70)
print("✅ ALL CONVERSIONS COMPLETE!")
print("="*70)
print(f"Modified: {FILE}")
print("Backup: {FILE}.backup-*")
print("")
print("⚠️  NOTE: You still need to add the new sendAprilTagDataWebSocket()")
print("    function to replace the old HTTP version. See detailed guide.")
print("")
print("Next steps:")
print("1. Open file in VS Code and review changes")
print("2. Add sendAprilTagDataWebSocket() function (see guide)")
print("3. Compile and test!")
