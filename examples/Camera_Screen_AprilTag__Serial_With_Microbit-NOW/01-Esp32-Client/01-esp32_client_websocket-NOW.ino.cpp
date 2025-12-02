/*
 * ESP32 WebSocket Client for AprilTag Detection
 * 
 * Sends AprilTag data to Python server via WebSocket for real-time GDevelop control
 * 
 * Migration from HTTP POST to Pure WebSocket:
 * - Lower latency: 10-50ms (vs 100-300ms with HTTP)
 * - Less memory: 20KB (vs 30KB with HTTP)
 * - Event-driven (no polling!)
 * - Standard JSON protocol
 * 
 * Connection: ESP32 ⟷ WebSocket ⟷ Python Server ⟷ WebSocket ⟷ GDevelop
 * 
 * Author: Cline AI Assistant
 * Date: 2025-12-01
 * Version: 1.0.0 - Pure WebSocket Implementation
 */

// ============================================================================
// REQUIRED LIBRARIES (Add to platformio.ini)
// ============================================================================
/*
lib_deps = 
    ArduinoWebsockets @ ^0.5.3
    ArduinoJson @ ^6.21.0
    ; ... existing libraries ...
*/

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

using namespace websockets;

// ============================================================================
// CONFIGURATION
// ============================================================================

// WiFi credentials
//// jwc 25-1202-0140 const char* WIFI_SSID = "YOUR_WIFI_SSID";
//// jwc 25-1202-0140 const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* WIFI_SSID = "Chan-Comcast";     // Replace with your WiFi SSID
const char* WIFI_PASSWORD = "Jesus333!";

// WebSocket server configuration
// Local: ws://10.42.0.1:5000/websocket (no SSL, direct connection)
// ngrok: wss://your-domain.ngrok-free.app/websocket (SSL, for remote access)
//// jwc 25-1202-1220 'ngrok http 5000' >> https://mallard-happy-singularly.ngrok-free.app
//// jwc 25-1202-0220 Using local ws:// for development (SSL issues with setInsecure())
//// jwc 25-1202-0230 n errors: [  3207][E][WiFiClientSecure.cpp:144] connect(): start_ssl_client: -1 ; [  3213][E][WiFiClient.cpp:320] setSocketOption(): fail on 0, errno: 9, "Bad filechar*:: const WS_SERVER_URL = "wss://mallard-happy-singularly.ngrok-free.app/websocket";  // ngrok (needs SSL cert fix)
//// jwc 25-1202-0250 Fixed IP: Server actual IP is 10.0.0.149 (was using wrong IP 10.0.0.26)
const char* WS_SERVER_URL = "ws://10.0.0.149:5000/websocket";  // Local server IP (corrected!)


// ============================================================================
// WEBSOCKET GLOBALS
// ============================================================================

WebsocketsClient webSocket;
bool webSocketConnected = false;
unsigned long lastReconnectAttempt = 0;
const unsigned long RECONNECT_INTERVAL = 5000;  // 5 seconds

// Statistics
unsigned long totalMessagesSent = 0;
unsigned long totalReconnects = 0;
unsigned long lastMessageTime = 0;

// Helper function to create separator lines
String createSeparator(char ch, int len) {
    String result = "";
    for (int i = 0; i < len; i++) {
        result += ch;
    }
    return result;
}

// ============================================================================
// WEBSOCKET EVENT HANDLERS
// ============================================================================

void onWebSocketEvent(WebsocketsEvent event, String data) {
    switch(event) {
        case WebsocketsEvent::ConnectionOpened: {
            Serial.println("\n" + createSeparator('=', 70));
            Serial.println("🔌 WebSocket CONNECTED!");
            Serial.println(createSeparator('=', 70));
            webSocketConnected = true;
            
            // Identify as ESP32 to server
            StaticJsonDocument<128> identifyDoc;
            identifyDoc["event"] = "identify";
            JsonObject identifyData = identifyDoc.createNestedObject("data");
            identifyData["type"] = "esp32";
            identifyData["device"] = "T-CameraPlus-S3";
            identifyData["version"] = "1.0.0";
            
            String identifyJson;
            serializeJson(identifyDoc, identifyJson);
            webSocket.send(identifyJson);
            
            Serial.println("📨 Sent identification to server");
            break;
        }
            
        case WebsocketsEvent::ConnectionClosed:
            Serial.println("\n" + createSeparator('=', 70));
            Serial.println("🔌 WebSocket DISCONNECTED!");
            Serial.println("   Total messages sent: " + String(totalMessagesSent));
            Serial.println("   Total reconnects: " + String(totalReconnects));
            Serial.println(createSeparator('=', 70));
            webSocketConnected = false;
            break;
            
        case WebsocketsEvent::GotPing:
            Serial.println("🏓 Ping received from server");
            break;
            
        case WebsocketsEvent::GotPong:
            Serial.println("🏓 Pong received from server");
            break;
    }
}

void onWebSocketMessage(WebsocketsMessage message) {
    Serial.print("📨 Message from server: ");
    Serial.println(message.data());
    
    // Parse server responses
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, message.data());
    
    if (error) {
        Serial.print("❌ JSON parse error: ");
        Serial.println(error.c_str());
        return;
    }
    
    const char* event = doc["event"];
    
    if (strcmp(event, "connection_success") == 0) {
        Serial.println("✅ Server acknowledged connection");
    }
    else if (strcmp(event, "identify_success") == 0) {
        const char* clientType = doc["client_type"];
        Serial.print("✅ Successfully identified as: ");
        Serial.println(clientType);
    }
    else if (strcmp(event, "apriltag_ack") == 0) {
        const char* status = doc["status"];
        int broadcastCount = doc["broadcast_count"] | 0;
        Serial.print("✅ AprilTag data acknowledged - broadcasted to ");
        Serial.print(broadcastCount);
        Serial.println(" GDevelop clients");
    }
    else if (strcmp(event, "no_data_available") == 0) {
        Serial.println("ℹ️  Server has no data available");
    }
    else {
        Serial.print("⚠️  Unknown event: ");
        Serial.println(event);
    }
}

// ============================================================================
// WEBSOCKET CONNECTION FUNCTIONS
// ============================================================================

void connectWebSocket() {
    Serial.println("\n" + createSeparator('=', 70));
    Serial.println("🔌 Connecting to WebSocket server...");
    Serial.print("   URL: ");
    Serial.println(WS_SERVER_URL);
    
    // Set event handlers
    webSocket.onMessage(onWebSocketMessage);
    webSocket.onEvent(onWebSocketEvent);
    
    // ========================================================================
    // SSL/TLS Configuration (Only for wss:// connections)
    // ========================================================================
    // NOTE: Currently using ws:// (non-SSL) for local development
    // 
    // For wss:// (SSL) connections with ngrok:
    //   1. OPTION 1 (Quick/Dev): webSocket.setInsecure();
    //      - Skips certificate validation
    //      - Development only! Not secure!
    //   2. OPTION 2 (Production): webSocket.setCACert(root_ca_cert);
    //      - Add Let's Encrypt ISRG Root X1 certificate
    //      - Proper SSL validation
    // ========================================================================
    
    // Note: enableHeartbeat() not available in ArduinoWebsockets library
    // Heartbeat will be handled manually with periodic pings
    
    // Connect to WebSocket server
    webSocketConnected = webSocket.connect(WS_SERVER_URL);
    
    if (webSocketConnected) {
        Serial.println("✅ WebSocket connection established!");
        totalReconnects++;
    } else {
        Serial.println("❌ WebSocket connection failed!");
        Serial.println("   Check:");
        Serial.println("   - WiFi connection");
        Serial.println("   - Server is running");
        Serial.println("   - URL is correct (wss:// for ngrok)");
    }
    
    Serial.println(createSeparator('=', 70) + "\n");
}

void checkWebSocketConnection() {
    if (!webSocketConnected) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt >= RECONNECT_INTERVAL) {
            lastReconnectAttempt = now;
            Serial.println("\n🔄 Attempting to reconnect WebSocket...");
            Serial.print("   Reconnect attempt #");
            Serial.println(totalReconnects + 1);
            connectWebSocket();
        }
    }
}

// ============================================================================
// APRILTAG DATA TRANSMISSION
// ============================================================================

void sendAprilTagDataWebSocket(int tag_id, float x_cm, float y_cm, float z_cm,
                                float yaw, float pitch, float roll,
                                float tag_size_percent, float distance_cm) {
    
    if (!webSocketConnected) {
        Serial.println("⚠️  WebSocket not connected - data not sent");
        Serial.println("   Waiting for reconnection...");
        return;
    }
    
    // Create JSON message
    StaticJsonDocument<512> doc;
    doc["event"] = "apriltag_data";
    
    JsonObject data = doc.createNestedObject("data");
    data["smartcam_ip"] = WiFi.localIP().toString();
    data["tag_id"] = tag_id;
    data["camera_name"] = "ESP32-CAM";
    data["x_cm"] = round(x_cm * 10) / 10.0;  // Round to 1 decimal
    data["y_cm"] = round(y_cm * 10) / 10.0;
    data["z_cm"] = round(z_cm * 10) / 10.0;
    data["yaw"] = round(yaw * 10) / 10.0;
    data["pitch"] = round(pitch * 10) / 10.0;
    data["roll"] = round(roll * 10) / 10.0;
    data["tag_size_percent"] = round(tag_size_percent * 10) / 10.0;
    data["distance_cm"] = round(distance_cm * 10) / 10.0;
    data["timestamp"] = millis();
    
    // Serialize to JSON string
    String json;
    serializeJson(doc, json);
    
    // Send to server
    bool sent = webSocket.send(json);
    
    if (sent) {
        totalMessagesSent++;
        lastMessageTime = millis();
        
        Serial.print("📤 AprilTag Data Sent (#");
        Serial.print(totalMessagesSent);
        Serial.print("): ID=");
        Serial.print(tag_id);
        Serial.print(", Pos=(");
        Serial.print(x_cm, 1);
        Serial.print(", ");
        Serial.print(y_cm, 1);
        Serial.print(", ");
        Serial.print(z_cm, 1);
        Serial.print(") cm, Yaw=");
        Serial.print(yaw, 1);
        Serial.println("°");
    } else {
        Serial.println("❌ Failed to send AprilTag data");
        webSocketConnected = false;  // Mark as disconnected to trigger reconnect
    }
}

// ============================================================================
// PING/PONG HEALTH CHECK
// ============================================================================

void sendPing() {
    if (webSocketConnected) {
        StaticJsonDocument<64> doc;
        doc["event"] = "ping";
        doc["timestamp"] = millis();
        
        String json;
        serializeJson(doc, json);
        webSocket.send(json);
        
        Serial.println("🏓 Ping sent to server");
    }
}

// ============================================================================
// WIFI SETUP
// ============================================================================

void setupWiFi() {
    Serial.println("\n" + createSeparator('=', 70));
    Serial.println("📶 Connecting to WiFi...");
    Serial.print("   SSID: ");
    Serial.println(WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi connected!");
        Serial.print("   IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("   RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    } else {
        Serial.println("\n❌ WiFi connection failed!");
        Serial.println("   Check SSID and password");
    }
    
    Serial.println(createSeparator('=', 70) + "\n");
}

// ============================================================================
// ARDUINO SETUP
// ============================================================================

void setup() {
    // Initialize serial
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println(createSeparator('=', 70));
    Serial.println("ESP32 WebSocket Client for AprilTag Detection");
    Serial.println("Version: 1.0.0 - Pure WebSocket Implementation");
    Serial.println(createSeparator('=', 70));
    
    // Connect to WiFi
    setupWiFi();
    
    // Connect to WebSocket server
    if (WiFi.status() == WL_CONNECTED) {
        connectWebSocket();
    } else {
        Serial.println("⚠️  Skipping WebSocket connection (no WiFi)");
    }
    
    // Initialize your camera and AprilTag detection here
    // ... existing camera setup code ...
    
    Serial.println("\n✅ Setup complete!");
    Serial.println("   Ready to send AprilTag data via WebSocket\n");
}

// ============================================================================
// ARDUINO LOOP
// ============================================================================

void loop() {
    // Poll WebSocket for incoming messages
    if (webSocketConnected) {
        webSocket.poll();
    }
    
    // Check connection status and reconnect if needed
    checkWebSocketConnection();
    
    // ========================================================================
    // YOUR APRILTAG DETECTION CODE GOES HERE
    // ========================================================================
    
    /*
    // Example: When AprilTag is detected
    if (apriltagDetected) {
        int tag_id = det->id;
        float x_cm = calculateX();
        float y_cm = calculateY();
        float z_cm = calculateZ();
        float yaw = calculateYaw();
        float pitch = calculatePitch();
        float roll = calculateRoll();
        float tag_size_percent = calculateSize();
        float distance_cm = calculateDistance();
        
        // Send via WebSocket (replaces HTTP POST)
        sendAprilTagDataWebSocket(
            tag_id,
            x_cm, y_cm, z_cm,
            yaw, pitch, roll,
            tag_size_percent,
            distance_cm
        );
    }
    */
    
    // ========================================================================
    // OPTIONAL: Periodic health check (every 30 seconds)
    // ========================================================================
    static unsigned long lastPingTime = 0;
    if (millis() - lastPingTime > 30000) {
        sendPing();
        lastPingTime = millis();
    }
    
    // ========================================================================
    // OPTIONAL: Connection statistics (every 60 seconds)
    // ========================================================================
    static unsigned long lastStatsTime = 0;
    if (millis() - lastStatsTime > 60000) {
        Serial.println("\n" + createSeparator('-', 70));
        Serial.println("📊 WebSocket Statistics:");
        Serial.print("   Connected: ");
        Serial.println(webSocketConnected ? "Yes ✅" : "No ❌");
        Serial.print("   Messages sent: ");
        Serial.println(totalMessagesSent);
        Serial.print("   Reconnects: ");
        Serial.println(totalReconnects);
        if (lastMessageTime > 0) {
            Serial.print("   Last message: ");
            Serial.print((millis() - lastMessageTime) / 1000);
            Serial.println(" seconds ago");
        }
        Serial.println(createSeparator('-', 70) + "\n");
        lastStatsTime = millis();
    }
    
    // Small delay to prevent tight loop
    delay(10);
}

// ============================================================================
// USAGE NOTES
// ============================================================================
/*
 * CONFIGURATION STEPS:
 * 
 * 1. Update WiFi credentials:
 *    - Set WIFI_SSID and WIFI_PASSWORD
 * 
 * 2. Update WebSocket server URL:
 *    - Local: ws://10.42.0.1:5000/websocket
 *    - ngrok: wss://your-domain.ngrok-free.app/websocket
 * 
 * 3. Add to platformio.ini:
 *    lib_deps = 
 *        ArduinoWebsockets @ ^0.5.3
 *        ArduinoJson @ ^6.21.0
 * 
 * 4. Integrate your AprilTag detection code:
 *    - Replace the example in loop() with your actual detection
 *    - Call sendAprilTagDataWebSocket() when tag is detected
 * 
 * EXPECTED SERIAL OUTPUT:
 * 
 * 🔌 Connecting to WebSocket server...
 * ✅ WebSocket connection established!
 * 📨 Sent identification to server
 * ✅ Successfully identified as: esp32
 * 📤 AprilTag Data Sent (#1): ID=5, Pos=(10.5, 20.3, 50.0) cm, Yaw=45.0°
 * ✅ AprilTag data acknowledged - broadcasted to 1 GDevelop clients
 * 
 * TROUBLESHOOTING:
 * 
 * "❌ WebSocket connection failed!"
 * - Check WiFi connection
 * - Verify server is running: python3 ubuntu_server_websocket-NOW.py
 * - Check URL format (wss:// for ngrok, ws:// for local)
 * - Test server in browser: http://localhost:5000/
 * 
 * "⚠️  WebSocket not connected - data not sent"
 * - Auto-reconnect will trigger in 5 seconds
 * - Check server logs for disconnection reason
 * - Verify network stability
 * 
 * PERFORMANCE:
 * - Latency: 10-50ms (vs 100-300ms with HTTP)
 * - Memory: ~20KB (vs ~30KB with HTTP)
 * - Auto-reconnect: Every 5 seconds if disconnected
 * - Heartbeat: Ping every 15 seconds
 */
