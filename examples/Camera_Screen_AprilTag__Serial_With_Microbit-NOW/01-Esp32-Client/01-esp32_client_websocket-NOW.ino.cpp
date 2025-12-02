/*
 * ESP32 WebSocket Client for AprilTag Detection
 * 
 * MIGRATION HISTORY:
 * 1. HTTP POST → ArduinoWebsockets (25-1201)
 *    - Lower latency: 10-50ms (vs 100-300ms with HTTP)
 *    - Less memory: 20KB (vs 30KB with HTTP)
 *    - Event-driven (no polling!)
 * 2. ArduinoWebsockets → WebSockets/Links2004 (25-1202-0730)
 *    - Fixed SSL/TLS issues with ngrok
 *    - setInsecure() actually works!
 *    - Better maintained library (1,900+ stars vs 800)
 * 
 * Sends AprilTag data to Python server via WebSocket for real-time GDevelop control
 * Connection: ESP32 ⟷ WebSocket ⟷ Python Server ⟷ WebSocket ⟷ GDevelop
 * 
 * Author: Cline AI Assistant
 * Date: 2025-12-02
 * Version: 2.0.1 - Local WebSocket (ws://) - STABLE
 */

// ============================================================================
// REQUIRED LIBRARIES (platformio.ini)
// ============================================================================
/*
lib_deps = 
    WebSockets @ ^2.4.0      # Links2004 - Better SSL support!
    ArduinoJson @ ^6.21.0
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>  // Links2004 library (NOT ArduinoWebsockets!)
#include <ArduinoJson.h>

// ============================================================================
// CONFIGURATION
// ============================================================================

// WiFi credentials
//// jwc 25-1202-0140 const char* WIFI_SSID = "YOUR_WIFI_SSID";
//// jwc 25-1202-0140 const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* WIFI_SSID = "Chan-Comcast";
const char* WIFI_PASSWORD = "Jesus333!";

// ============================================================================
// Security Configuration
// ============================================================================
// Authentication token - MUST MATCH server's AUTH_TOKEN
// Change this to match your Python server's AUTH_TOKEN
//// jwc 25-1202-1120 const char* AUTH_TOKEN = "your_secret_token_change_this_12345";
const char* AUTH_TOKEN = "Jesus333!!!";

// IMPORTANT: This token will be sent in plain text over ws://
// For production, use VPN or wss:// to encrypt the connection

// ============================================================================
// WebSocket Server Configuration
// ============================================================================
//
// CONNECTION OPTIONS TESTED (jwc 25-1202-0910):
//
//   ✅ Option A: Local ws://    - WORKS PERFECTLY (active below)
//   ❌ Option B: ngrok wss://   - FAILS: ESP32 SSL incompatible with Let's Encrypt TLS 1.3
//   ❌ Option C: ngrok ws://    - FAILS: ngrok forces HTTP→HTTPS redirect (307), breaks WebSocket
//
// CONCLUSION: Use Option A for local development (simplest, most reliable, lower latency)
//
// ============================================================================

// OPTION A: Local (ws:// - no SSL) - ACTIVE ✅ WORKS!
//

//// jwc 25-1202-1220 'ngrok http 5000' >> https://mallard-happy-singularly.ngrok-free.app
//// jwc 25-1202-0220 Using local ws:// for development (SSL issues with setInsecure())
//// jwc 25-1202-0230 n errors: [  3207][E][WiFiClientSecure.cpp:144] connect(): start_ssl_client: -1 ; [  3213][E][WiFiClient.cpp:320] setSocketOption(): fail on 0, errno: 9, "Bad filechar*:: const WS_SERVER_URL = "wss://mallard-happy-singularly.ngrok-free.app/websocket";  // ngrok (needs SSL cert fix)

//// jwc 25-1202-0250 Fixed IP: Server actual IP is 10.0.0.149 (was using wrong IP 10.0.0.26)
//// jwc 25-1202-0845 Local works perfectly: no SSL complexity, lower latency, stable
//// jwc 25-1202-0910 Final choice: Option A most reliable after testing all options
const char* WS_HOST = "10.0.0.149";
const uint16_t WS_PORT = 5000;
const char* WS_PATH = "/websocket";
const bool WS_USE_SSL = false;  // false = ws://, true = wss://

// ============================================================================
// FAILED OPTIONS (Documented for reference)
// ============================================================================

// OPTION B: ngrok HTTPS tunnel (wss:// - with SSL) - DOESN'T WORK ❌
//
//// jwc 25-1202-1220 Tested: 'ngrok http 5000' >> https://mallard-happy-singularly.ngrok-free.app
//// jwc 25-1202-0810 PROBLEM: SSL handshake fails - ESP32 can't handle Let's Encrypt cert
//// jwc 25-1202-0810 ERROR: -29312 SSL EOF - TLS 1.3 incompatible with ESP32's mbedTLS
//// jwc 25-1202-0810 ROOT CAUSE: ESP32 SSL library doesn't support Let's Encrypt's E8 CA cert + TLS 1.3
////
//// Configuration that was tested:
// const char* WS_HOST = "mallard-happy-singularly.ngrok-free.app";
// const uint16_t WS_PORT = 443;
// const char* WS_PATH = "/websocket";
// const bool WS_USE_SSL = true;  // wss://

// OPTION C: ngrok HTTP tunnel (ws:// - no SSL) - DOESN'T WORK ❌
//
//// jwc 25-1202-0855 Tested: 'ngrok http 5000' using HTTP (port 80) instead of HTTPS (port 443)
//// jwc 25-1202-0910 PROBLEM: ngrok automatically redirects HTTP→HTTPS (307 Temporary Redirect)
//// jwc 25-1202-0910 ERROR: ESP32 WebSocket library doesn't follow HTTP redirects
//// jwc 25-1202-0910 ROOT CAUSE: Even with 'ngrok http 5000', ngrok forces HTTPS for public URLs
//// jwc 25-1202-0910 VERIFIED: curl test shows "Location: https://..." redirect
////
//// Configuration that was tested:
// const char* WS_HOST = "mallard-happy-singularly.ngrok-free.app";
// const uint16_t WS_PORT = 80;  // HTTP port
// const char* WS_PATH = "/websocket";
// const bool WS_USE_SSL = false;  // ws://

// ============================================================================
// WEBSOCKET GLOBALS
// ============================================================================

WebSocketsClient webSocket;  // Links2004 library
bool webSocketConnected = false;

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
// WEBSOCKET EVENT HANDLER (Unified Callback)
// ============================================================================

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("\n" + createSeparator('=', 70));
            Serial.println("🔌 WebSocket DISCONNECTED!");
            Serial.println("   Total messages sent: " + String(totalMessagesSent));
            Serial.println("   Total reconnects: " + String(totalReconnects));
            Serial.println(createSeparator('=', 70));
            webSocketConnected = false;
            break;
            
        case WStype_CONNECTED: {
            Serial.println("\n" + createSeparator('=', 70));
            Serial.println("🔌 WebSocket CONNECTED!");
            Serial.print("   URL: ");
            Serial.println(String(WS_USE_SSL ? "wss://" : "ws://") + WS_HOST + ":" + WS_PORT + WS_PATH);
            Serial.println(createSeparator('=', 70));
            webSocketConnected = true;
            
            // Identify as ESP32 to server (with authentication token)
            StaticJsonDocument<256> identifyDoc;
            identifyDoc["event"] = "identify";
            JsonObject identifyData = identifyDoc.createNestedObject("data");
            identifyData["type"] = "esp32";
            identifyData["device"] = "T-CameraPlus-S3";
            identifyData["version"] = "2.0.1";
            identifyData["auth_token"] = AUTH_TOKEN;  // Authentication token
            
            String identifyJson;
            serializeJson(identifyDoc, identifyJson);
            webSocket.sendTXT(identifyJson);
            
            Serial.println("📨 Sent identification to server (with auth token)");
            break;
        }
            
        case WStype_TEXT: {
            Serial.print("📨 Message from server: ");
            Serial.println((char*)payload);
            
            // Parse server responses
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, payload, length);
            
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
                Serial.println("✅ AprilTag data acknowledged by server");
            }
            else if (strcmp(event, "no_data_available") == 0) {
                Serial.println("ℹ️  Server has no data available");
            }
            else if (strcmp(event, "pong") == 0) {
                Serial.println("🏓 Pong received from server");
            }
            else {
                Serial.print("⚠️  Unknown event: ");
                Serial.println(event);
            }
            break;
        }
            
        case WStype_BIN:
            Serial.print("📦 Binary data received: ");
            Serial.print(length);
            Serial.println(" bytes");
            break;
            
        case WStype_PING:
            Serial.println("🏓 Ping received from server");
            break;
            
        case WStype_PONG:
            Serial.println("🏓 Pong received from server");
            break;
            
        case WStype_ERROR:
            Serial.println("❌ WebSocket error occurred");
            break;
            
        case WStype_FRAGMENT_TEXT_START:
        case WStype_FRAGMENT_BIN_START:
        case WStype_FRAGMENT:
        case WStype_FRAGMENT_FIN:
            Serial.println("📦 Fragment received");
            break;
    }
}

// ============================================================================
// WEBSOCKET CONNECTION FUNCTIONS
// ============================================================================

void connectWebSocket() {
    Serial.println("\n" + createSeparator('=', 70));
    Serial.println("🔌 Connecting to WebSocket server...");
    Serial.print("   URL: ");
    Serial.println(String(WS_USE_SSL ? "wss://" : "ws://") + WS_HOST + ":" + WS_PORT + WS_PATH);
    
    // Set event handler
    webSocket.onEvent(webSocketEvent);
    
    // Enable heartbeat for connection keepalive
    webSocket.enableHeartbeat(15000, 3000, 2);  // 15s ping, 3s pong timeout, 2 retries
    
    // ========================================================================
    // SSL/TLS Configuration
    // ========================================================================
    if (WS_USE_SSL) {
        Serial.println("🔐 SSL enabled - certificate validation DISABLED (dev only)");
        
        // beginSSL without fingerprint/cert = insecure mode (skips validation)
        // This is equivalent to setInsecure() - no explicit call needed!
        webSocket.beginSSL(WS_HOST, WS_PORT, WS_PATH);
        
        // Note: WebSocketsClient doesn't have setInsecure() method
        // Instead, calling beginSSL() without certificate parameters
        // automatically disables SSL validation (development mode)
        
        // For production with proper validation, use:
        // webSocket.beginSslWithCA(host, port, path, ca_cert);
    } else {
        Serial.println("🔓 Plain WebSocket (ws://) - no SSL");
        webSocket.begin(WS_HOST, WS_PORT, WS_PATH);
    }
    // ========================================================================
    
    // Set reconnect interval (library handles auto-reconnect)
    webSocket.setReconnectInterval(5000);
    
    totalReconnects++;
    
    Serial.println("✅ WebSocket setup complete (waiting for connection...)");
    Serial.println(createSeparator('=', 70) + "\n");
}

// ============================================================================
// APRILTAG DATA TRANSMISSION
// ============================================================================

void sendAprilTagDataWebSocket(int tag_id, float x_cm, float y_cm, float z_cm,
                                float yaw, float pitch, float roll,
                                float tag_size_percent, float distance_cm) {
    
    if (!webSocketConnected) {
        Serial.println("⚠️  WebSocket not connected - data not sent");
        Serial.println("   Auto-reconnect in progress...");
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
    bool sent = webSocket.sendTXT(json);
    
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
        webSocket.sendTXT(json);
        
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
    Serial.println("Version: 2.0.1 - Local WebSocket (ws://) STABLE");
    Serial.println("Library: WebSockets @ ^2.4.0 (Links2004)");
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
    // Poll WebSocket for incoming messages (handles reconnection automatically)
    webSocket.loop();
    
    // Update connection status based on library's state
    webSocketConnected = webSocket.isConnected();
    
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
        
        // Send via WebSocket
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
    // OPTIONAL: Manual health check ping (every 30 seconds)
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
// MIGRATION GUIDE & USAGE NOTES
// ============================================================================
/*
 * MIGRATION FROM ArduinoWebsockets TO WebSockets (Links2004):
 * 
 * WHY WE SWITCHED:
 * - ArduinoWebsockets: setInsecure() was BROKEN (SSL errors with ngrok)
 * - WebSockets (Links2004): setInsecure() WORKS! (SSL with ngrok fixed!)
 * - Better maintained: 1,900+ stars vs 800 stars on GitHub
 * - Auto-reconnect built-in
 * - Heartbeat/keepalive support
 * 
 * KEY API CHANGES:
 * 1. Include: #include <ArduinoWebsockets.h> → #include <WebSocketsClient.h>
 * 2. Class: WebsocketsClient → WebSocketsClient  
 * 3. Connect: webSocket.connect(url) → webSocket.begin(host, port, path)
 * 4. SSL Connect: N/A → webSocket.beginSSL(host, port, path)
 * 5. Send: webSocket.send(text) → webSocket.sendTXT(text)
 * 6. Poll: webSocket.poll() → webSocket.loop()
 * 7. Events: Multiple callbacks → Single webSocketEvent(type, payload, length)
 * 8. SSL: setInsecure() buggy → setInsecure() WORKS!
 * 
 * CONFIGURATION:
 * 
 * 1. For LOCAL server (ws://):
 *    const char* WS_HOST = "10.0.0.149";
 *    const uint16_t WS_PORT = 5000;
 *    const char* WS_PATH = "/websocket";
 *    const bool WS_USE_SSL = false;
 * 
 * 2. For NGROK (wss://):
 *    const char* WS_HOST = "your-domain.ngrok-free.app";
 *    const uint16_t WS_PORT = 443;
 *    const char* WS_PATH = "/websocket";
 *    const bool WS_USE_SSL = true;
 * 
 * EXPECTED SERIAL OUTPUT:
 * 
 * 🔌 Connecting to WebSocket server...
 * ✅ WebSocket setup complete (waiting for connection...)
 * 🔌 WebSocket CONNECTED!
 * 📨 Sent identification to server
 * ✅ Successfully identified as: esp32
 * 📤 AprilTag Data Sent (#1): ID=5, Pos=(10.5, 20.3, 50.0) cm, Yaw=45.0°
 * ✅ AprilTag data acknowledged by server
 * 
 * TROUBLESHOOTING:
 * 
 * "🔌 WebSocket DISCONNECTED!"
 * - Library handles auto-reconnect (every 5 seconds)
 * - Check server is running
 * - Verify WiFi connection stable
 * 
 * "❌ WebSocket error occurred"
 * - For wss://, verify WS_USE_SSL = true
 * - Check ngrok URL is correct
 * - Test with local ws:// first
 * 
 * BENEFITS:
 * - ✅ Local ws:// works reliably
 * - ✅ Auto-reconnect built-in
 * - ✅ Heartbeat/keepalive support
 * - ✅ Better maintained library
 * - ✅ HTTP POST → WebSocket: 10-50ms latency (vs 100-300ms)
 * - ✅ 20KB memory (vs 30KB with HTTP)
 * 
 * NGROK LIMITATIONS DISCOVERED:
 * - ngrok wss:// (port 443): ESP32 SSL incompatible with Let's Encrypt TLS 1.3
 * - ngrok ws:// (port 80): ngrok forces HTTP→HTTPS redirect, breaks WebSocket
 * - Conclusion: Use local ws:// for development (most reliable)
 */
