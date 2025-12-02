//// IMPORTANT NOTES
//// jwc 25-0803-0700 * This code is for the T-CameraPlus-S3 with 1.3" TFT display
//// * 240 x 240 pixels
//// * Range for 5cm x 5cm April-Tag for Thin Rectangle-Boundary: 5cm to 23cm

//// NOTES
//// jwc 25-0411-1800 convert to April-Tag Detect 
////     * libraries\Mylibrary\pin_config.h
////     * Libraries\
////       * C:\11i-GD-C\09j-E3\20e-Arduino-Sketchbook_Default-NOW\libraries\Apriltag_library_for_Arduino_ESP32

//// jwc 25-0409-1600 April-Tag Pose
//// jwc 25-0410-1500 'C:\11i-GD-C\09j-E3\24-1226-1005-Esp32-AllInOne-Waveshare-2.0in-CameraOptrion\ESP32-S3-Touch-LCD-2-Demo\Arduino\examples\09_lvgl_camera-Z01-SimpleTagDetect_April\09_lvgl_camera-Z01-SimpleTagDetect_April.ino'
//// jwc 25-0410-1500 Convert from St7735 to St7789
//// jwc * Esp32s3 'printf' to 'Serial.printf'
//// jwc 25-0410-1500
////     * C:\11i-GD-C\09j-E3\20e-Arduino-Sketchbook_Default-NOW\libraries\TFT_eSPI\User_Setup-Esp32_Wroom-Tft_St7735-25-0410-1500-Waveshare_Esp32s3_2p0In_St7789.h
////       * #define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
////       * #define TFT_HEIGHT 240 // ST7789 240 x 320
////     * C:\11i-GD-C\09j-E3\24-1114-0752-AAA_AAa-TYJ-AprilTag--Github-Raspiduino-Apriltag\apriltag-esp32\examples\simpletagdetect_25_0409_1601_Ws_Esp32s3_2p0In_AprilTag_Pose_NOW\simpletagdetect_25_0409_1601_Ws_Esp32s3_2p0In_AprilTag_Pose_NOW.ino
////       * config.frame_size = FRAMESIZE_240X240;
////       * tft.setRotation(0);
////     * NOT WORK
////       * #define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
////       * #define TFT_HEIGHT 320 // ST7789 240 x 320
////
////       * config.frame_size = FRAMESIZE_QVGA;

//// jwc 25-0416-1400 Add 'C:\11i-GD-C\09j-E3\24-1226-1001-Esp32-Display-AllInOne-Waveshare_2.8in\Arduino-NOW-AllInOne_TftCapacitance_Waveshare\examples\250201-121953\src\main.cpp'
////     * Serial Test w/ Mb
////     * Bonus: Compass w/ I2C

    //// jwc 24-1229-1000 https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-2.8
    //// jwc 24-1230-0630 Add Serial2: Actuall works: 43, 44, but need to disconect loopback: to accurately test disconeected first, then connected
    //// * C:\09j-Esp32\24-1206-2001-Esp32-2432S024C-CheapYellowDisplay-Rzeldent\esp32-smartdisplay-demo-SNAPSHOT-24-1222--0730-ABA_Aa-TYJ-Esp_28R_24C_Serial_2__35_22__22_27_NoMbYet\src\main.cpp
    //// jwc 24-1230-0900 Add I2c:Compass_Mpu6050
    //// * C:\09j-Esp32\24-1213-1900-Esp32-Compass_Mp6050-Arduino_Mp6050-Shilleh\24-1213-1900-basic_readings-Adafruit_Mp6050-Example-Shilleh--SNAPSHOT-24-1214-0200-AAA_a-TYJ-Works-ButDegreesConfusing
    //// jwc 24-1213-2000 Adafruit Mpu6050 Library: Example: basic_reading.ino
    //// jwc https://www.youtube.com/watch?v=H9e1Up7xHjc
    //// * 'Serial.println' >> 'printf'

    //// jwc 24-1231-0420: yyy Serial2.begin(115200, SERIAL_8N1, 43, 44); //
    //// jwc 24-1231-0420: yyy Serial2.begin(115200, SERIAL_8N1, 15, 18); //

    //// jwc 25-0125-0500 Esp32 no longer return back Mb packets since seems corrupted, but sends back new, clean packets


//// jwc \/
// * simple_tag_detect_24_1115_0530_ACa_a__Commit_24_0416_Wrover_Vid
//   * Flash_Jpg_24_1115_1040_ABC_a_Esp32_FnWroverCamToTft7735.ino
//     * TFT_graphicstest_one_lib_24_1114_0800_ACA_a_FnWrover_St7735_1p4.ino

// * 24-1127-1850 Appears that at small 'FRAMESIZE_QVGA' to reduce April-Tag processing
//   * for 2.5x2.5cm Tft, need April-Tag min. 0.5x0.5cm for recognition.
//   * 2.5x2.5 = 6.25 || 0.5x0.5 = 0.25 || 0.25/6.25 = 0.04 = 4% of screen min

//// ! NOTE: jwc 25-0421-1730
//// ! NOTE: jwc yyyy -D ARDUINO_USB_CDC_ON_BOOT=0
//// * printf():        YES goes to SerialPort-4pin (to Mb) and YES UsbPort (to Computer: SerialMonitor)
//// * Serial.printf(): NOT goes to UsbPort (to Computer: SerialMonitor)
//// ! NOTE: jwc yyyy -D ARDUINO_USB_CDC_ON_BOOT=1
//// * printf():        NOT goes to SerialPort-4pin (to Mb) and YES goes to UsbPort (to Computer: SerialMonitor)
//// * Serial.printf(): YES goes to UsbPort (to Computer: SerialMonitor)
//// !! NOTE: whether -D ARDUINO_USB_CDC_ON_BOOT: =0 | =1
//// * Seems best use 'printf()' and not 'Serial.printf()' for completeness
//// * Download/Flash always via UsbPort 


//// ! NOTE: jwc 25-1121-1800
////
//// ## Complete System Configuration Status:
//// 
//// ### ✅ ESP32 Code (Ready for Deployment):
//// 
//// - __WiFi SSID__: `Chan-Comcast-xFi`
//// - __WiFi Password__: `Jesus333`
//// - __Server URL__: `http://10.0.0.150:5000/esp32_apriltag_data`
//// - __HTTP Debug Logging__: Comprehensive console output enabled
//// - __TFT Visual Feedback__: Shows "HTTP:OK" or "HTTP:ERR"
//// - __JSON Format__: `{"id": tag_id, "camera_name": camera_name, "timestamp": millis()}`
//// 
//// ### ✅ Python Server (Ready for Deployment):
//// 
//// - __Server IP__: `10.0.0.150`
//// - __Server Port__: `5000`
//// - __Endpoint__: `/esp32_apriltag_data`
//// - __Web Interface__: `http://10.0.0.150:5000/`
//// 
//// ## 🚀 Ready to Deploy!
//// 
//// __Both programs are now fully configured and ready for testing:__
//// 
//// 1. __Start Python Server on Ubuntu:__
//// 
////    ```bash
////    python3 Camera_Screen_AprilTag__Serial_With_Microbit-NOW.ino--TestServer_Receiver-E32_to_MiniPcIntelUbuntu-NOW.py
////    ```
//// 
//// 2. __Flash ESP32 Code__ to your T-CameraPlus-S3
//// 
//// 3. __Monitor Results:__
//// 
////    - ESP32 Serial Monitor: Watch WiFi connection and HTTP debug output
////    - Web Browser: Open `http://10.0.0.150:5000/` to see real-time data reception
//// 
//// __The system should work!__ Both devices will communicate via WiFi network with matching configurations.
//// 

//// jwc 25-1122-1200
////
//// ## 🎯 System Now Configured For:
//// 
//// ### ESP32:
//// 
//// - ✅ WiFi: `Jason Phone Verizon Samsung` / `Jesus333!`
//// - ✅ Server: `http://172.19.216.7:5000/esp32_apriltag_data`
//// - ✅ HTTP Rate: __500ms (2 req/sec) - stable & reliable__
//// - ✅ Complete debug logging enabled
//// 
//// ### Python Server:
//// 
//// - ✅ WiFi Band Detection working
//// - ✅ Server ready on port 5000
//// - ✅ Real-time web interface available


/*
 * AprilTag detector demo on AI Thinker ESP32-CAM
 * Created by gvl610
 * Based on https://github.com/AprilRobotics/apriltag
 * with some modifications (for adaption and performance)
 */

// ===================
// Select camera model
// ===================
//// jwc \/
//// jwc oy REPLACED BELOW \/ #define CAMERA_MODEL_WROVER_KIT // Has PSRAM
#define CAMERA_MODEL_LILYGO_T_CAMERA_PLUS_S3_ESP32S3_TFT1P3IN


/*
 * @Description(CN):
 *       这是一个将OV2640摄像头数据投影到屏幕上的例程
 *
 * @Description(EN):
 *      This is a routine that projects the OV2640 camera data onto the T-CameraPlus-S3 screen.
 *
 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-11-15 16:57:21
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-11-25 09:04:35
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include <esp_camera.h>
#include "camera_index.h"
//// jwc o NOT NEEDED?: #include "app_httpd.tpp"

//// jwc 25-1126-2200 OPTION 1: ESP32 POSTs to Python Server (ACTIVE)
#include <WiFi.h>
#include <HTTPClient.h>
//// jwc 25-1130-1100 ARCHIVED: SSL error for HTTP: #include <WiFiClientSecure.h>
#include <WiFiClient.h>

//// jwc 25-1126-2200 OPTION 2: ESP32 GET Server for direct polling (ARCHIVED)
//// jwc 25-1126-2200 #include <ESPAsyncWebServer.h>

// WiFi credentials
//
//// jwc 25-1120-0910 WiFi Configuration - hardcoded like Python code
//// jwc 25-1122-0930 const char* WIFI_SSID = "Chan-Comcast-xFi";     // Replace with your WiFi SSID
//// jwc 25-1123-0400 const char* WIFI_SSID = "Jason Phone Verizon Samsung";     // Replace with your WiFi SSID
//// jwc 25-1123-0400 const char* WIFI_PASSWORD = "Jesus333!"; // Replace with your WiFi password
//// jwc 25-1124-1400 retry Xfinity-Xb6: y const char* WIFI_SSID = "BMax_B1Pro-Wifi_Hotspot";
const char* WIFI_SSID = "Chan-Comcast";     // Replace with your WiFi SSID

const char* WIFI_PASSWORD = "Jesus333!";

//// jwc 25-1124-1630 Event Queue for GDevelop polling
// Circular buffer for tag detection events
const int tagData_MAX = 50;  // Buffer size (50 events = ~1.6KB RAM)

struct tagData_Struct {
    int tag_id;
    float yaw;
    float pitch;
    float roll;
    float x_cm;
    float y_cm;
    float z_cm;
    float tag_size_percent;
    float distance_cm;
    unsigned long timestamp;
};

tagData_Struct tagData_Queue[tagData_MAX];
int list_head = 0;   // Write position
int list_count = 0;  // Number of events in list

//// jwc 25-1126-2200 OPTION 2: GET Server (ARCHIVED)
//// jwc 25-1126-2200 AsyncWebServer server(80);

//// jwc 25-1123-1800 Network latency measurement (microseconds for precision)
// Tracks round-trip time for HTTP requests
struct NetworkLatencyStats {
    unsigned long current_us = 0;     // Most recent latency in microseconds
    unsigned long min_us = 999999;    // Minimum latency observed (us)
    unsigned long max_us = 0;         // Maximum latency observed (us)
    unsigned long total_us = 0;       // Sum of all latencies (us)
    unsigned long count = 0;          // Number of successful measurements
    
    // Calculate average latency in microseconds
    unsigned long getAverage() {
        return (count > 0) ? (total_us / count) : 0;
    }
    
    // Update statistics with new measurement (in microseconds)
    void update(unsigned long latency_us) {
        current_us = latency_us;
        if (latency_us < min_us) min_us = latency_us;
        if (latency_us > max_us) max_us = latency_us;
        total_us += latency_us;
        count++;
    }
    
    // Reset statistics
    void reset() {
        current_us = 0;
        min_us = 999999;
        max_us = 0;
        total_us = 0;
        count = 0;
    }
} network_latency;

// Add tag detection to list for later HTTP sending (non-blocking, fast)
void listTagEvent_Add(int id, float yaw, float pitch, float roll, float x_cm, float y_cm, float z_cm, 
                   float tag_size_percent, float distance_cm, const char* camera_name) {
    if (list_count < tagData_MAX) {
        unsigned long timestamp = millis();
        tagData_Queue[list_head] = {id, yaw, pitch, roll, x_cm, y_cm, z_cm, tag_size_percent, distance_cm, timestamp};
        list_head = (list_head + 1) % tagData_MAX;
        list_count++;
        
        #if DEBUG >= 1
        printf("*** ADDED TO LIST: Tag ID=%d, List=%d/%d\n", id, list_count, tagData_MAX);
        #endif
    } else {
        printf("*** LIST FULL! Dropped Tag ID=%d\n", id);
    }
}

// Remove oldest tag from list for HTTP sending
bool listTagEvent_Remove(tagData_Struct* out_tag) {
    if (list_count > 0) {
        // Calculate read position (oldest event = FIFO)
        int read_pos = (list_head - list_count + tagData_MAX) % tagData_MAX;
        *out_tag = tagData_Queue[read_pos];
        list_count--;
        return true;
    }
    return false;
}
//// jwc 25-1126-2200 
//// jwc 25-1126-2200 // Build JSON response from queue with latency stats
//// jwc 25-1126-2200 String getEventsJSON(unsigned long request_start_us) {
//// jwc 25-1126-2200     // Start JSON response
//// jwc 25-1126-2200     String json = "{";
//// jwc 25-1126-2200     
//// jwc 25-1126-2200     bool has_tag = false;
//// jwc 25-1126-2200     
//// jwc 25-1126-2200     // Only send 1 event per HTTP GET request (if available)
//// jwc 25-1126-2200     if (queue_count > 0) {
//// jwc 25-1126-2200         // Calculate read position (oldest event)
//// jwc 25-1126-2200         int read_pos = (queue_head - queue_count + tagData_MAX) % tagData_MAX;
//// jwc 25-1126-2200         
//// jwc 25-1126-2200         tagData_Struct& evt = tagData_Queue[read_pos];
//// jwc 25-1126-2200         json += "\"smartcam_ip\":\"" + WiFi.localIP().toString() + "\",";
//// jwc 25-1126-2200         json += "\"tag_id\":" + String(evt.tag_id) + ",";
//// jwc 25-1126-2200         json += "\"yaw\":" + String(evt.yaw, 1) + ",";
//// jwc 25-1126-2200         json += "\"pitch\":" + String(evt.pitch, 1) + ",";
//// jwc 25-1126-2200         json += "\"roll\":" + String(evt.roll, 1) + ",";
//// jwc 25-1126-2200         json += "\"x_cm\":" + String(evt.x_cm, 1) + ",";
//// jwc 25-1126-2200         json += "\"y_cm\":" + String(evt.y_cm, 1) + ",";
//// jwc 25-1126-2200         json += "\"z_cm\":" + String(evt.z_cm, 1) + ",";
//// jwc 25-1126-2200         json += "\"tag_size_percent\":" + String(evt.tag_size_percent, 1) + ",";
//// jwc 25-1126-2200         json += "\"distance_cm\":" + String(evt.distance_cm, 1) + ",";
//// jwc 25-1126-2200         json += "\"timestamp\":" + String(evt.timestamp) + ",";
//// jwc 25-1126-2200         
//// jwc 25-1126-2200         // Remove the sent event from queue
//// jwc 25-1126-2200         queue_count--;
//// jwc 25-1126-2200         has_tag = true;
//// jwc 25-1126-2200     }
//// jwc 25-1126-2200     
//// jwc 25-1126-2200     // Close JSON response with remaining queue count
//// jwc 25-1126-2200     json += "\"queue_remaining\":" + String(queue_count) + "}";
//// jwc 25-1126-2200     
//// jwc 25-1126-2200     // Calculate processing latency in microseconds
//// jwc 25-1126-2200     unsigned long processing_us = micros() - request_start_us;
//// jwc 25-1126-2200     
//// jwc 25-1126-2200     // Update latency statistics
//// jwc 25-1126-2200     network_latency.update(processing_us);
//// jwc 25-1126-2200     
//// jwc 25-1126-2200     // Log latency to console only if events were sent (not in JSON response)
//// jwc 25-1126-2200     if (has_tag) {
//// jwc 25-1126-2200         //// jwc 25-1124-2020 printf("\n");
//// jwc 25-1126-2200         // Convert to milliseconds for display (with decimal precision)
//// jwc 25-1126-2200         float current_ms = network_latency.current_us / 1000.0;
//// jwc 25-1126-2200         float min_ms = network_latency.min_us / 1000.0;
//// jwc 25-1126-2200         float avg_ms = network_latency.getAverage() / 1000.0;
//// jwc 25-1126-2200         float max_ms = network_latency.max_us / 1000.0;
//// jwc 25-1126-2200         
//// jwc 25-1126-2200         printf("\n>>> >>> >>> HTTP GET: Send Stats:: 1 event | Latency: %.2fms (min:%.2f avg:%.2f max:%.2f)\n", 
//// jwc 25-1126-2200                current_ms, min_ms, avg_ms, max_ms);
//// jwc 25-1126-2200         printf(">>> >>> >>> HTTP GET: Send Data:: %s\n", json.c_str());
//// jwc 25-1126-2200         printf("\n");
//// jwc 25-1126-2200     }
//// jwc 25-1126-2200     
//// jwc 25-1126-2200     return json;
//// jwc 25-1126-2200 }

//// jwc 25-1126-2300 HTTP POST Endpoint Configuration
// Server URL for real-time AprilTag data transmission
//
//// jwc 25-1130-0220 RENAMED: Clearer naming convention
//// jwc OLD: TEST_SERVER_URL
//// jwc NEW: client_e32__http_post_to_serverhub__smartcam_april_tag_URL
//// jwc 25-1120-0910 HTTP Server Configuration - matching TestServer
//// jwc 25-1122-0930 y const char* client_e32__http_post_to_serverhub__smartcam_april_tag_URL = "http://10.0.0.150:5000/client_e32_to_server__smartcam_data_post";
//// jwc 25-1123-0400 y const char* client_e32__http_post_to_serverhub__smartcam_april_tag_URL = "http://172.19.216.7:5000/client_e32_to_server__smartcam_data_post";
//// jwc 25-1124-1410 y const char* client_e32__http_post_to_serverhub__smartcam_april_tag_URL = "http://10.42.0.1:5000/client_e32_to_server__smartcam_data_post";
//// jwc 25-1130-1048 NGROK (HTTPS) - ARCHIVED 25-1130-1058: SSL error -29312 persists: const char* client_e32__http_post_to_serverhub__smartcam_april_tag_URL = "https://mallard-happy-singularly.ngrok-free.app/client_e32__http_post_to_serverhub__smartcam_april_tag";
//// jwc 25-1130-0842 CLOUDFLARE TUNNEL - ARCHIVED 25-1130-1048: const char* client_e32__http_post_to_serverhub__smartcam_april_tag_URL = "https://instrumentation-intensive-lou-specializing.trycloudflare.com/client_e32__http_post_to_serverhub__smartcam_april_tag";
//// jwc 25-1130-1058 ✅ LOCAL HTTP (Ubuntu LAN) - ACTIVE - Most reliable, no SSL issues
//// jwc 25-1130-1058 IMPORTANT: Update this IP to match your Ubuntu server's local IP
//// jwc 25-1130-1058 Run 'ip addr' on Ubuntu to find correct IP (look for wlp1s0 or enp2s0)
const char* client_e32__http_post_to_serverhub__smartcam_april_tag_URL = "http://10.0.0.149:5000/client_e32__http_post_to_serverhub__smartcam_april_tag";



//// jwc 25-1128-0100 VIDEO STREAMING - Optimized to prevent lag
// Video frame upload endpoint for human viewing
//// jwc 25-1130-0220 RENAMED: Clearer naming convention
//// jwc OLD: VIDEO_FRAME_UPLOAD_URL
//// jwc NEW: client_e32__http_post_to_serverhub__smartcam_video_stream_URL
//// jwc 25-1130-1048 NGROK (HTTPS) - ARCHIVED 25-1130-1058: SSL error -29312 persists: const char* client_e32__http_post_to_serverhub__smartcam_video_stream_URL = "https://mallard-happy-singularly.ngrok-free.app/client_e32__http_post_to_serverhub__smartcam_video_stream";
//// jwc 25-1130-0842 CLOUDFLARE TUNNEL - ARCHIVED 25-1130-1048: const char* client_e32__http_post_to_serverhub__smartcam_video_stream_URL = "https://instrumentation-intensive-lou-specializing.trycloudflare.com/client_e32__http_post_to_serverhub__smartcam_video_stream";
//// jwc 25-1130-1058 ✅ LOCAL HTTP (Ubuntu LAN) - ACTIVE - Most reliable, no SSL issues
const char* client_e32__http_post_to_serverhub__smartcam_video_stream_URL = "http://10.0.0.149:5000/client_e32__http_post_to_serverhub__smartcam_video_stream";


// Video streaming timing control - Optimized settings
unsigned long video_send_time_last = 0;
const unsigned long VIDEO_SEND_INTERVAL_MS = 3000;  // 3 seconds (0.33 FPS) - slow enough to not block AprilTag detection
const int VIDEO_JPEG_QUALITY = 10;  // Low quality (1-100, lower = smaller file, faster upload)

// Rate limiting - don't send too frequently
//
// ============================================================================
// HTTP THROTTLE CONFIGURATION - Network-Specific Settings
// ============================================================================
//
// VERIZON HOTSPOT (Mobile Network) - RECOMMENDED: 1500-2000ms
// --------------------------------------------------------
// Characteristics:
//   - Higher latency: 50-200ms (vs 1-5ms on WiFi)
//   - Variable connection quality (signal fluctuates)
//   - Data cap concerns (conserve mobile data)
//   - Higher packet loss (needs time for retries)
//   - Battery consideration (longer intervals = less power)
//
// Suggested Settings:
//   1500ms = 0.67 req/sec (CONSERVATIVE - Best for reliability)
//   2000ms = 0.50 req/sec (VERY CONSERVATIVE - Maximum stability)
//
// Benefits:
//   ✅ More reliable transmission
//   ✅ Reduces failed requests
//   ✅ Conserves mobile data
//   ✅ Better battery life
//   ✅ Less likely to overwhelm hotspot
//
// HOME ROUTER (Local WiFi Network) - RECOMMENDED: 500-1000ms
// --------------------------------------------------------
// Characteristics:
//   - Lower latency: ~1-5ms
//   - Stable connection (consistent signal)
//   - No data caps
//   - Lower packet loss
//   - Better bandwidth
//
// Suggested Settings:
//   500ms = 2.0 req/sec (GOOD - Stable and responsive)
//   1000ms = 1.0 req/sec (CONSERVATIVE - Very stable)
//
// Benefits:
//   ✅ Near real-time updates
//   ✅ Higher data throughput
//   ✅ Faster response to AprilTag detection
//   ✅ Better for interactive applications
//
// TESTING OPTIONS (All intervals in milliseconds):
//   25ms = 40 req/sec (VERY AGGRESSIVE - WiFi instability, not usable)
//   50ms = 20 req/sec (AGGRESSIVE - 50% failure rate)
//   100ms = 10 req/sec (MODERATE - Good for home router)
//   250ms = 4 req/sec (CONSERVATIVE)
//   500ms = 2 req/sec (GOOD for home router)
//   1000ms = 1 req/sec (CONSERVATIVE for home router)
//   1500ms = 0.67 req/sec (RECOMMENDED for Verizon hotspot)
//   2000ms = 0.5 req/sec (VERY CONSERVATIVE for Verizon hotspot)
//
// ============================================================================

//// jwc 25-1123-0720 Switch to Ubuntu-Hotspot// CURRENT CONFIGURATION: Verizon Hotspot (Mobile Network)
//// jwc 25-1123-0720 Switch to Ubuntu-Hotspotconst unsigned long HTTP_MIN_INTERVAL_MS = 1500; // 0.67 req/sec - Optimized for Verizon hotspot
//// jwc 25-1123-0720 Switch to Ubuntu-Hotspot
//// jwc 25-1123-0720 Switch to Ubuntu-Hotspot// For Home Router, use this instead:
//// jwc 25-1123-0720 Switch to Ubuntu-Hotspot// const unsigned long HTTP_MIN_INTERVAL_MS = 500; // 2 req/sec - Optimized for home WiFi
//// jwc 25-1123-0720 Switch to Ubuntu-Hotspot

// Replace with actual GDevelop endpoint (for future use)
const char* GDEVELOP_SERVER_URL = "https://your-gdevelop-endpoint.com/apriltag_data"; 

//// jwc 25-1120-0910 HTTP transmission control
bool wifi_connected = false;

//// jwc 25-1123-0750 Decouple screen updates from HTTP sends
// HTTP timing - separate from screen updates
unsigned long http_send_time_last = 0;

//// jwc ## Real-World Responsiveness
//// jwc 
//// jwc ### Scenario: AprilTag appears in camera view
//// jwc 
//// jwc ```javascript
//// jwc Time 0ms:     Tag detected, added to queue
//// jwc Time 250ms:   (average wait in queue)
//// jwc Time 250-280ms: HTTP POST transmitted
//// jwc Time 280ms:   Server/GDevelop receives data
//// jwc 
//// jwc Total latency: ~280ms
//// jwc 
//// jwc __This is faster than human reaction time (300ms)__, so it feels instant.

//// jwc 25-1128-0943 OLD (conservative): const unsigned long HTTP_SEND_INTERVAL_MS = 2000;  // 2s (0.5 req/sec)
const unsigned long HTTP_SEND_INTERVAL_MS = 500;  // 0.5s (2 req/sec) - 4x more responsive!

// Rate limiting for list additions - prevent list overflow
unsigned long list_add_time_last = 0;
const unsigned long LIST_ADD_INTERVAL_MS = 500;  // Only add to list every 500ms (2 per second max)

// Tag data buffer - stores latest detected tag
struct LatestTagData {
    bool has_data = false;
    int tag_id = 0;
    char camera_name[32] = "";
    unsigned long timestamp = 0;
} latest_tag;

//// jwc 25-1121-1700 Forward declaration for gfx (defined later in file)
extern Arduino_TFT *gfx;

//// jwc 25-1124-1700 WiFi and HTTP Server Setup
void initWiFi() {
    printf("*** WiFi Init: Starting...\n");
    printf("*** WiFi SSID: %s\n", WIFI_SSID);
    printf("*** WiFi Password: %s\n", WIFI_PASSWORD);
    printf("*** WiFi Band: 2.4GHz ONLY (ESP32-S3 hardware limitation)\n");
    printf("*** Note: Works with band-steering routers (e.g. Xfinity XB6) - router auto-assigns 2.4GHz band\n");
    //// jwc 25-1124-1700 printf("*** Server URL: %s\n", TEST_SERVER_URL);
    //// jwc 25-1124-1700 printf("*** HTTP Send Interval: %lu ms (%0.1f req/sec)\n", HTTP_SEND_INTERVAL_MS, 1000.0 / HTTP_SEND_INTERVAL_MS);
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        printf(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifi_connected = true;
        printf("\n*** WiFi Connected! SmartCam-IP: %s\n", WiFi.localIP().toString().c_str());
        printf("*** POST Server: %s\n", client_e32__http_post_to_serverhub__smartcam_april_tag_URL);
        
        //// jwc 25-1126-2200 OPTION 2: GET Server (ARCHIVED)
        //// jwc 25-1126-2200 // Setup HTTP GET endpoint for GDevelop to poll
        //// jwc 25-1126-2200 server.on("/smartcam_data", HTTP_GET, [](AsyncWebServerRequest *request){
        //// jwc 25-1126-2200     // Start latency measurement (using micros for precision)
        //// jwc 25-1126-2200     unsigned long request_start_us = micros();
        //// jwc 25-1126-2200     
        //// jwc 25-1126-2200     // Enable CORS for GDevelop
        //// jwc 25-1126-2200     AsyncWebServerResponse *response = request->beginResponse(200, "application/json", getEventsJSON(request_start_us));
        //// jwc 25-1126-2200     response->addHeader("Access-Control-Allow-Origin", "*");
        //// jwc 25-1126-2200     response->addHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
        //// jwc 25-1126-2200     response->addHeader("Access-Control-Allow-Headers", "Content-Type");
        //// jwc 25-1126-2200     request->send(response);
        //// jwc 25-1126-2200 });
        //// jwc 25-1126-2200 
        //// jwc 25-1126-2200 // Handle CORS preflight
        //// jwc 25-1126-2200 server.on("/smartcam_data", HTTP_OPTIONS, [](AsyncWebServerRequest *request){
        //// jwc 25-1126-2200     AsyncWebServerResponse *response = request->beginResponse(200);
        //// jwc 25-1126-2200     response->addHeader("Access-Control-Allow-Origin", "*");
        //// jwc 25-1126-2200     response->addHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
        //// jwc 25-1126-2200     response->addHeader("Access-Control-Allow-Headers", "Content-Type");
        //// jwc 25-1126-2200     request->send(response);
        //// jwc 25-1126-2200 });
        //// jwc 25-1126-2200 
        //// jwc 25-1126-2200 server.begin();
        //// jwc 25-1126-2200 printf("*** HTTP Server: Started successfully!\n");
        
        gfx->setTextSize(2);
        gfx->setCursor(1, 220);
        gfx->printf("WiFi: %s", WiFi.localIP().toString().c_str());
    } else {
        wifi_connected = false;
        printf("\n*** WiFi Failed to connect\n");
        gfx->setTextSize(2);
        gfx->setCursor(1, 220);
        gfx->printf("WiFi: FAILED");
    }
}

//// jwc 25-1201-1330 VIDEO STREAMING - Send JPEG frame to server (Fixed timeout & error handling)
bool sendVideoFrame(camera_fb_t *fb) {
    // Check WiFi connection status
    if (!wifi_connected || WiFi.status() != WL_CONNECTED) {
        printf("*** VIDEO ERROR: WiFi not connected\n");
        return false;
    }
    
    if (!fb) {
        printf("*** VIDEO ERROR: No frame buffer provided\n");
        return false;
    }
    
    // Convert grayscale to JPEG (if not already JPEG)
    uint8_t *jpg_buf = NULL;
    size_t jpg_buf_len = 0;
    bool jpg_converted = false;
    
    if (fb->format == PIXFORMAT_JPEG) {
        // Already JPEG
        jpg_buf = fb->buf;
        jpg_buf_len = fb->len;
    } else {
        // Convert grayscale to JPEG with LOW quality for smaller file size
        jpg_converted = frame2jpg(fb, VIDEO_JPEG_QUALITY, &jpg_buf, &jpg_buf_len);
        if (!jpg_converted) {
            printf("*** VIDEO ERROR: JPEG conversion failed\n");
            return false;
        }
    }
    
    printf("\n>>> VIDEO: Sending frame (Size: %d bytes, Quality: %d)...\n", jpg_buf_len, VIDEO_JPEG_QUALITY);
    
    //// jwc 25-1130-0400 OLD METHOD (SSL error -29312 with HTTPS):
    //// jwc 25-1130-0400 HTTPClient http;
    //// jwc 25-1130-0400 http.begin(client_e32__http_post_to_serverhub__smartcam_video_stream_URL);
    //// jwc 25-1130-0400 http.addHeader("Content-Type", "image/jpeg");
    
    //// jwc 25-1130-0410 ARCHIVED: WiFiClientSecure for HTTPS - caused SSL error with HTTP:
    //// jwc 25-1130-0410 WiFiClientSecure *client = new WiFiClientSecure;
    //// jwc 25-1130-0410 if (!client) {
    //// jwc 25-1130-0410     printf(">>> VIDEO ERROR: Failed to allocate WiFiClientSecure\n");
    //// jwc 25-1130-0410     if (jpg_converted && jpg_buf) {
    //// jwc 25-1130-0410         free(jpg_buf);
    //// jwc 25-1130-0410     }
    //// jwc 25-1130-0410     return false;
    //// jwc 25-1130-0410 }
    //// jwc 25-1130-0410 client->setInsecure();  // Skip SSL certificate verification for HTTPS (ngrok)
    
    //// jwc 25-1130-1100 Use WiFiClient for HTTP (not HTTPS):
    WiFiClient *client = new WiFiClient;
    if (!client) {
        printf(">>> VIDEO ERROR: Failed to allocate WiFiClient\n");
        if (jpg_converted && jpg_buf) {
            free(jpg_buf);
        }
        return false;
    }
    
    HTTPClient http;
    
    //// jwc 25-1201-1330 IMPORTANT FIX: Use begin() without client parameter for HTTP
    //// This fixes connection refused errors with local HTTP servers
    http.begin(client_e32__http_post_to_serverhub__smartcam_video_stream_URL);
    http.addHeader("Content-Type", "image/jpeg");
    http.setTimeout(5000);  // 5 second timeout - longer for video uploads
    
    int httpResponseCode = http.POST(jpg_buf, jpg_buf_len);
    
    http.end();  // End HTTP connection
    delete client;  // Clean up WiFiClient
    
    if (httpResponseCode > 0) {
        printf(">>> VIDEO SUCCESS: Frame uploaded (Code: %d)\n", httpResponseCode);
        
        // Free converted JPEG buffer if we allocated it
        if (jpg_converted && jpg_buf) {
            free(jpg_buf);
        }
        return true;
    } else {
        // Detailed error messages for common codes
        printf(">>> VIDEO FAILURE: Upload failed (Code: %d)\n", httpResponseCode);
        if (httpResponseCode == -1) {
            printf("    - Error -1: Connection refused. Server may not be running or endpoint missing.\n");
            printf("    - Check server URL: %s\n", client_e32__http_post_to_serverhub__smartcam_video_stream_URL);
        } else if (httpResponseCode == -11) {
            printf("    - Error -11: Timeout. Server not responding or network slow.\n");
        }
        
        // Free converted JPEG buffer if we allocated it
        if (jpg_converted && jpg_buf) {
            free(jpg_buf);
        }
        return false;
    }
}

//// jwc 25-1126-2200 OPTION 1: Real-time HTTP POST (ACTIVE)
bool sendAprilTagData(int tag_id, const char* camera_name, float yaw, float pitch, float roll, 
                      float x_cm, float y_cm, float z_cm, float tag_size_percent, float distance_cm) {
    // Check WiFi connection status
    if (!wifi_connected || WiFi.status() != WL_CONNECTED) {
        printf("*** HTTP ERROR: WiFi not connected (wifi_connected=%d, WiFi.status()=%d)\n", 
               wifi_connected, WiFi.status());
        return false;
    }
    
    unsigned long current_time = millis();
    
    // Start latency measurement
    unsigned long latency_start_ms = millis();
    
    printf("\nvvv HTTP POST START vvv\n");
    printf("*** HTTP: Target URL: %s\n", client_e32__http_post_to_serverhub__smartcam_april_tag_URL);
    printf("*** HTTP: Tag ID: %d, Camera: %s\n", tag_id, camera_name);
    
    //// jwc 25-1130-0400 OLD METHOD (SSL error -29312 with HTTPS):
    //// jwc 25-1130-0400 HTTPClient http;
    //// jwc 25-1130-0400 http.begin(client_e32__http_post_to_serverhub__smartcam_april_tag_URL);
    //// jwc 25-1130-0400 http.addHeader("Content-Type", "application/json");
    
    //// jwc 25-1130-0410 ARCHIVED: WiFiClientSecure for HTTPS - caused SSL error with HTTP:
    //// jwc 25-1130-0410 WiFiClientSecure *client = new WiFiClientSecure;
    //// jwc 25-1130-0410 if (!client) {
    //// jwc 25-1130-0410     printf("*** HTTP ERROR: Failed to allocate WiFiClientSecure\n");
    //// jwc 25-1130-0410     return false;
    //// jwc 25-1130-0410 }
    //// jwc 25-1130-0410 client->setInsecure();  // Skip SSL certificate verification for HTTPS (ngrok)
    
    //// jwc 25-1130-1100 Use WiFiClient for HTTP (not HTTPS):
    WiFiClient *client = new WiFiClient;
    if (!client) {
        printf("*** HTTP ERROR: Failed to allocate WiFiClient\n");
        return false;
    }
    
    HTTPClient http;
    http.begin(*client, client_e32__http_post_to_serverhub__smartcam_april_tag_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Access-Control-Allow-Origin", "*");
    http.addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    http.addHeader("Access-Control-Allow-Headers", "Content-Type");
    
        // Create JSON payload with all AprilTag data + ESP32 queue status
        String json_payload = "{";
        json_payload += "\"smartcam_ip\":\"" + WiFi.localIP().toString() + "\",";
        json_payload += "\"tag_id\":" + String(tag_id) + ",";
        json_payload += "\"yaw\":" + String(yaw, 1) + ",";
        json_payload += "\"pitch\":" + String(pitch, 1) + ",";
        json_payload += "\"roll\":" + String(roll, 1) + ",";
        json_payload += "\"x_cm\":" + String(x_cm, 1) + ",";
        json_payload += "\"y_cm\":" + String(y_cm, 1) + ",";
        json_payload += "\"z_cm\":" + String(z_cm, 1) + ",";
        json_payload += "\"tag_size_percent\":" + String(tag_size_percent, 1) + ",";
        json_payload += "\"distance_cm\":" + String(distance_cm, 1) + ",";
        json_payload += "\"esp32_queue_count\":" + String(list_count) + ",";  // ESP32 queue status for lag monitoring
        json_payload += "\"camera_name\":\"" + String(camera_name) + "\",";
        json_payload += "\"timestamp\":" + String(current_time);
        json_payload += "}";
    
    printf("*** HTTP: Sending JSON payload: %s\n", json_payload.c_str());
    printf("*** HTTP: Payload size: %d bytes\n", json_payload.length());
    
    int httpResponseCode = http.POST(json_payload);
    
    // End latency measurement
    unsigned long latency_end_ms = millis();
    unsigned long latency_ms = latency_end_ms - latency_start_ms;
    
    if (httpResponseCode > 0) {
        String response = http.getString();
        
        // Update latency statistics on successful request
        network_latency.update(latency_ms);
        
        printf("*** HTTP SUCCESS: Response Code: %d\n", httpResponseCode);
        printf("*** HTTP SUCCESS: Server Response: %s\n", response.c_str());
        printf("*** HTTP LATENCY: Current=%lums, Min=%lums, Max=%lums, Avg=%lums (Count=%lu)\n",
               latency_ms,
               network_latency.min_us / 1000,
               network_latency.max_us / 1000,
               network_latency.getAverage() / 1000,
               network_latency.count);
        printf("^^^ HTTP POST END (SUCCESS) ^^^\n\n");
        
        // Show success on display with latency
        gfx->setTextSize(1);
        gfx->setCursor(180, 1);
        gfx->printf("HTTP:%lums", latency_ms);
        
        http.end();  // End HTTP connection first
        delete client;  // Clean up WiFiClient
        return true;
    } else {
        printf("*** HTTP FAILURE: Error Code: %d\n", httpResponseCode);
        printf("*** HTTP FAILURE: Possible causes:\n");
        printf("    - Server not running on %s\n", client_e32__http_post_to_serverhub__smartcam_april_tag_URL);
        printf("    - Network connectivity issues\n");
        printf("    - Firewall blocking port 5000\n");
        printf("^^^ HTTP POST END (FAILED) ^^^\n\n");
        
        // Show error on display
        gfx->setTextSize(1);
        gfx->setCursor(200, 1);
        gfx->printf("HTTP:ERR");
        
        http.end();  // End HTTP connection first
        delete client;  // Clean up WiFiClient
        return false;
    }
}

//// \/ jwc 25-0411-1800 convert to April-Tag Detect 

// Apriltag headers
// We choose 36h11 family to use in this demo, but you can use
// any family of your choice. Note that due to memory limitation,
// you might have to reduce the number of tag in `codedata` array
// in the tag family source file.
#include "apriltag.h"
#include "tag36h11.h" // Tag family. You can change
#include "common/image_u8.h"
#include "common/zarray.h"

//// jwc o http://esp32.io/viewtopic.php?t=31461
/** jwc \/
"Failed to get the frame on time!" after a while working fine.
* I have a script that in the loop does some kind of "motion detection". Later I plan to take a high-res pic and send it to a server if a motion is detected. Beside this I want to have the possibility to connect via a Webserver. The part in the loop works fine already. When I connect via the Webserver it works fine at the beginning but after taking about 40-50 frames I get this error and the esp32 reboots:
* [E][camera.c:1483] esp_camera_fb_get(): Failed to get the frame on time!
Guru Meditation Error: Core 1 panic'ed (LoadProhibited). Exception was unhandled.
* The Webserver on its own seems to work fine. When I comment out everything in the loop function and insert the then missing "Config(LOW)" in the setup function the stream goes on and on without any problems.
**/
#include <img_converters.h>


//// jwc 25-0409-1600 April-Tag Pose
//
#include "apriltag_pose.h" // For pose estimation
#include "common/matd.h"


// Config for pose estimation
// Tag size (in meter). See original AprilTag readme for how to measure
// You have to put your value here. This value is of NO standard and
// is just my own tag size.
//// jwc Measure Full April-Tag Dimension if 100% Black
#define TAG_SIZE 0.05 // 0.05m = 5cm

// Camera calibration data for LILYGO T-CameraPlus-S3 at 240x240 resolution
// 
// OFFICIAL SPECIFICATIONS FROM LILYGO:
// =====================================
// Camera: OV2640 sensor
// Max Resolution: UXGA (1600x1200), SXGA (1280x1024), XGA (1024x768), 
//                 SVGA (800x600), VGA (640x480), QVGA (320x240), etc.
// Current Frame Size: FRAMESIZE_240X240 (240x240 pixels)
// Display: 1.3-inch 240x240 TFT (ST7789V driver)
//
// // Original values were for a different camera/resolution:
//   FX/FY = 924.7, CX = 403.8, CY = 305.1
//   This suggests original resolution was ~800x600 or higher

// CALCULATION EXPLANATION:
// ========================
// 1. PRINCIPAL POINT (CX, CY):
//    - For 240x240 image: center = (240/2, 240/2) = (120, 120)
//    - CX = 120.0, CY = 120.0
//
// 2. FOCAL LENGTH (FX, FY) - OV2640 Specific:
//    - OV2640 sensor size: ~3.6mm x 2.7mm (1/4" format)
//    - Typical focal length: ~2.8mm (varies by lens assembly)
//    - For 240x240 pixels from center crop of larger sensor:
//    - Pixel size ≈ 3.6mm / 1600px ≈ 2.25μm (at max resolution)
//    - Focal length in pixels = (focal_length_mm / pixel_size_mm)
//    - FX = FY ≈ 2.8mm / 0.00225mm ≈ 1244 pixels (at full resolution)
//    - Scaled for 240x240: 1244 × (240/1600) ≈ 186 pixels
//    - Adjusted for typical OV2640 behavior: ~200 pixels
//
// 3. FIELD OF VIEW VERIFICATION:
//    - Horizontal FOV ≈ 2×atan(240/(2×200)) ≈ 2×atan(0.6) ≈ 62°
//    - This matches OV2640 typical FOV of ~60-66°
//
// 4. MAXIMUM RESOLUTIONS SUPPORTED:
//    - FRAMESIZE_UXGA: 1600x1200 (2MP)
//    - FRAMESIZE_SXGA: 1280x1024 
//    - FRAMESIZE_XGA:  1024x768
//    - FRAMESIZE_SVGA: 800x600
//    - FRAMESIZE_VGA:  640x480
//    - FRAMESIZE_QVGA: 320x240
//    - FRAMESIZE_240X240: 240x240 (current setting)
//
// NOTE: For production use, calibrate your specific camera using:
//       - OpenCV calibration tools with checkerboard pattern
//       - Multiple images at different angles/distances
//       - Calculate distortion coefficients if needed
//
#define FX 200.0  // fx (focal length in pixels) - calculated for OV2640 at 240x240
#define FY 200.0  // fy (focal length in pixels) - calculated for OV2640 at 240x240  
#define CX 120.0  // cx (principal point X) - center of 240x240 image
#define CY 120.0  // cy (principal point Y) - center of 240x240 image

/*
 * Define this macro to enable debug mode
 * Level 0: Absolutely no debug at all. Suitable for
 *  production use.
 * Level 1: Simple debug messages , like simple events
 *  notifications.
 * Level 2: Low level debug messages
 * Level 3: Debug messages that in a loop
 */
//// jwc o #define DEBUG 2
//// jwc \/ get as much info as possible
//// jwc y #define DEBUG 1
//// jwc y #define D EBUG 3
//// jwc y cause lag when on batt? #define DEBUG 1
#define DEBUG 1

// Create tag family object
apriltag_family_t *tf = tag36h11_create();
// Create AprilTag detector object
apriltag_detector_t *td = apriltag_detector_create();

//// /\ jwc 25-0411-1800 convert to April-Tag Detect 


static bool OV2640_Initialization_Flag = false;

// fp-133h01d
Arduino_DataBus *bus = new Arduino_HWSPI(
    LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, -1 /* MISO */);

Arduino_TFT *gfx = new Arduino_ST7789(
    bus, LCD_RST /* RST */, 0 /* rotation */, true /* IPS */,
    LCD_WIDTH /* width */, LCD_HEIGHT /* height */,
    0 /* col offset 1 */, 0 /* row offset 1 */, 0 /* col_offset2 */, 0 /* row_offset2 */);

bool OV2640_Initialization(void)
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    // ============================================================================
    // CAMERA CLOCK FREQUENCY CONFIGURATION
    // ============================================================================
    // OV2640 Camera Clock (XCLK) - Controls frame capture speed
    //
    // OFFICIAL OV2640 SPECS:
    //   - Minimum: 10 MHz
    //   - Maximum: 24 MHz
    //   - Typical:  20 MHz (recommended by datasheet)
    //
    // PERFORMANCE vs STABILITY TRADE-OFF:
    //   10 MHz = CONSERVATIVE (slowest, most stable, no timing errors)
    //   15 MHz = BALANCED     (50% faster, still very stable) ✅ CURRENT
    //   20 MHz = RECOMMENDED  (full speed, within spec, may have occasional timing issues)
    //   24 MHz = MAXIMUM      (fastest, datasheet limit, prone to EV-VSYNC-OVF errors)
    //
    // TIMING ERRORS (EV-VSYNC-OVF):
    //   - Higher clock = faster frames BUT more prone to timing errors
    //   - Errors don't damage hardware, just cause frame drops
    //   - Solution: Lower clock speed if errors occur
    //
    // IMPACT ON APRILTAG PROCESSING:
    //   - 10→15 MHz: ~33% reduction in frame capture time
    //   - 15→20 MHz: ~25% reduction in frame capture time
    //   - Total system speedup: 10-20% (frame capture is only part of pipeline)
    // ============================================================================
    
    //// jwc 25-1128-0943 OLD (conservative): config.xclk_freq_hz = 10000000;  // 10 MHz
    config.xclk_freq_hz = 15000000;  // 15 MHz - 50% faster, still very stable


    // Set frame config
    // • FRAMESIZE_UXGA (1600 x 1200)
    // * FRAMESIZE_SXGA (1280 x 1024)
    // • FRAMESIZE_XGA (1024 x 768)
    // • FRAMESIZE_SVGA (800 x 600)
    // • FRAMESIZE_VGA (640 x 480)
    // • FRAMESIZE_QVGA (320 x 240)
    // • FRAMESIZE_CIF (352 x 288)
    //
    //// jwc typedef enum {
    //// jwc   FRAMESIZE_96X96,    // 96x96
    //// jwc   FRAMESIZE_QQVGA,    // 160x120
    //// jwc   FRAMESIZE_QCIF,     // 176x144
    //// jwc   FRAMESIZE_HQVGA,    // 240x176
    //// jwc   FRAMESIZE_240X240,  // 240x240
    //// jwc   FRAMESIZE_QVGA,     // 320x240
    //// jwc   FRAMESIZE_CIF,      // 400x296
    //// jwc   FRAMESIZE_HVGA,     // 480x320
    //// jwc   FRAMESIZE_VGA,      // 640x480
    //// jwc   FRAMESIZE_SVGA,     // 800x600
    //// jwc   FRAMESIZE_XGA,      // 1024x768
    //// jwc   FRAMESIZE_HD,       // 1280x720
    //// jwc   FRAMESIZE_SXGA,     // 1280x1024
    //// jwc   FRAMESIZE_UXGA,     // 1600x1200
    //// jwc   // 3MP Sensors
    //// jwc   FRAMESIZE_FHD,      // 1920x1080
    //// jwc   FRAMESIZE_P_HD,     //  720x1280
    //// jwc   FRAMESIZE_P_3MP,    //  864x1536
    //// jwc   FRAMESIZE_QXGA,     // 2048x1536
    //// jwc   // 5MP Sensors
    //// jwc   FRAMESIZE_QHD,      // 2560x1440
    //// jwc   FRAMESIZE_WQXGA,    // 2560x1600
    //// jwc   FRAMESIZE_P_FHD,    // 1080x1920
    //// jwc   FRAMESIZE_QSXGA,    // 2560x1920
    //// jwc   FRAMESIZE_INVALID
    //// jwc } framesize_t;

    //// jwc y but maybe too big? \/ config.frame_size = FRAMESIZE_VGA;
    //// jwc y From: pose_estimate.ino
    //// * config.frame_size = FRAMESIZE_SVGA; // You can change the resolution to fit your need

    //// jwc y Using a 240x240 Suqare screen \/config.frame_size = FRAMESIZE_QVGA;

    //// jwc y config.frame_size = FRAMESIZE_240X240;
    //// jwc yy config.frame_size = FRAMESIZE_QVGA;

    config.frame_size = FRAMESIZE_240X240;

    // config.pixel_format = PIXFORMAT_JPEG; // for streaming
    //// jwc 25-0411-1800 oy    config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    //// jwc ? config.pixel_format = PIXFORMAT_GRAYSCALE; // for April-Tag Detection
    //// jwc config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
    config.pixel_format = PIXFORMAT_GRAYSCALE; // for April-Tag Detection

    //// jwc 25-0411-1800 oy config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.grab_mode = CAMERA_GRAB_LATEST; // Has to be in this mode, or detection will be lag

    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 0;
    config.fb_count = 2;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        printf("Camera init failed with error 0x%x", err);
        gfx->printf("Camera init failed with error 0x%x \n", err);
        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (s)
    {
        printf("camera id:");
        printf("%d", s->id.PID);
        printf("\n");
        gfx->print("camera id:");
        gfx->println(s->id.PID);
        gfx->println();

        //// \/ jwc 25-0411-1800 convert to April-Tag Detect 

        // Custom camera configs should go here \/
        //
        s->set_brightness(s, 0);     // -2 to 2
        s->set_contrast(s, 0);       // -2 to 2
        s->set_saturation(s, 0);     // -2 to 2
        s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
        s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
        s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
        s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
        s->set_aec2(s, 1);           // 0 = disable , 1 = enable
        s->set_ae_level(s, 0);       // -2 to 2
        s->set_aec_value(s, 168);    // 0 to 1200
        s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
        s->set_agc_gain(s, 0);       // 0 to 30
        s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
        s->set_bpc(s, 0);            // 0 = disable , 1 = enable
        s->set_wpc(s, 1);            // 0 = disable , 1 = enable
        s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
        s->set_lenc(s, 1);           // 0 = disable , 1 = enable
        s->set_hmirror(s, 1);        // 0 = disable , 1 = enable
        s->set_vflip(s, 1);          // 0 = disable , 1 = enable
        s->set_dcw(s, 1);            // 0 = disable , 1 = enable
        
        //// /\ jwc 25-0411-1800 convert to April-Tag Detect 



        camera_sensor_info_t *sinfo = esp_camera_sensor_get_info(&(s->id));
        if (sinfo)
        {
            printf("camera model:");
            printf(sinfo->name);
            printf("\n");
            gfx->print("camera model:");
            gfx->println(sinfo->name);
            gfx->println();
        }
    }
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);       // flip it back
        s->set_brightness(s, 1);  // up the brightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }

    //// \/ jwc 25-0411-1800 convert to April-Tag Detect 

    //// jwc 25-0411-1800 TODO
    //// jwc 25-0410-1500 TODO #if defined(CAMERA_MODEL_WAVESHARE_ESP32S3_TFT2P0IN_CAM)
    //// jwc 25-0410-1500 TODO   //// jwc n even with '1', April-Tag Detected: s->set_vflip(s, 1);  
    //// jwc 25-0410-1500 TODO   s->set_vflip(s, 0);  
    //// jwc 25-0410-1500 TODO   //// jwc n and with '1', April-Tag Detected-NOT: s->set_hmirror(s, 1);
    //// jwc 25-0410-1500 TODO   s->set_hmirror(s, 0);
    //// jwc 25-0410-1500 TODO #endif
#if defined(CAMERA_MODEL_LILYGO_T_CAMERA_PLUS_S3_ESP32S3_TFT1P3IN)
    //// jwc n even with '1', April-Tag Detected: s->set_vflip(s, 1);  
    //// jwc y s->set_vflip(s, 0);  
    //// jwc n w/ set_rotation(0): s->set_vflip(s, 1);  
    //// jwc 25-0411 ? s->set_vflip(s, 0);  
    //// jwc n s->set_vflip(s, 1);  
    //// jwc n s->set_vflip(s, 0);  
    //// jwc n s->set_vflip(s, 0);  
    s->set_vflip(s, 1);  

    //// jwc n and with '1', April-Tag Detected-NOT: s->set_hmirror(s, 1);
    //// jwc 25-0410-1500 NOT WORK: s->set_hmirror(s, 0);
    //// jwc 25-0410-1500 WORK:
    s->set_hmirror(s, 1);
#endif

    // Done init camera
#if DEBUG >= 1
    //// jwc o \/ Serial.println("done");
    printf("*** Camera Init: End");
    printf("\n");
#endif


    // Setup AprilTag detection
#if DEBUG >= 1
    printf("Init AprilTag detector... ");
#endif

    //// jwc move to global space above: // Create tag family object
    //// jwc move to global space above: apriltag_family_t *tf = tag36h11_create();
    //// jwc move to global space above: 
    //// jwc move to global space above: // Create AprilTag detector object
    //// jwc move to global space above: apriltag_detector_t *td = apriltag_detector_create();

    // Add tag family to the detector
    apriltag_detector_add_family(td, tf);
    
    // Tag detector configs
    // quad_sigma is Gaussian blur's sigma
    // quad_decimate: small number = faster but cannot detect small tags
    //                big number = slower but can detect small tags (or tag far away)
    // With quad_sigma = 1.0 and quad_decimate = 4.0, ESP32-CAM can detect 16h5 tag
    // from the distance of about 1 meter (tested with tag on screen. not on paper)
    td->quad_sigma = 0.0;
    td->quad_decimate = 4.0;
    td->refine_edges = 0;
    //// jwc TODO From: pose_estimate.ino
    //// jwc: td->decode_sharpening = 0.25;
    //// jwc: td->nthreads = 2; // The optimal (after many tries) is 2 thread on 2 cores ESP32
    td->decode_sharpening = 0;

    //// jwc 25-0410-1300 td->nthreads = 1;
    td->nthreads = 2;

    //// jwc o Test Debug: td->debug = 0;
    //// jwc n E (903) ledc: ledc_get_duty(740): LEDC is not initialized: td->debug = 1;
    td->debug = 0;

    // Done init AprilTag detector
#if DEBUG >= 1
    printf("done");
    printf("\n");
    printf("Start detecting...");
    printf("\n");
#endif

    //// /\ jwc 25-0411-1800 convert to April-Tag Detect 

    return true;
}

void setup()
{
    Serial.begin(115200);
    //// jwc o Need English Instead: Serial.println("Ciallo");

    //// jwc ? //// jwc 'Serial2.begin(baud-rate, protocol, RX pin, TX pin);'
    //// jwc ? //// * https://www.waveshare.com/esp32-s3-touch-lcd-2.8.htm
    //// jwc y DOES SEEM IMORTANT, NOT WORK IF MISSING:   Serial2.begin(115200, SERIAL_8N1, 44, 43); //
    //// jwc ? Serial2.begin(115200, SERIAL_8N1, 43, 44); //
    //// jwc yy Serial2.begin(115200, SERIAL_8N1, 15, 18); //
    //// jwc n Serial2.begin(115200, SERIAL_8N1, 1, 2); //
    //// jwc yyy 24-1231-0420: Serial2.begin(115200, SERIAL_8N1, 43, 44); //
    //// jwc yyy 24-1231-0420: Serial2.begin(115200, SERIAL_8N1, 15, 18); //
    //// jwc o 25-0416-1500 Serial2.begin(115200, SERIAL_8N1, 43, 44); //
    
    //// jwc ? //// jwc 25-0416-1500 Lg T-CameraPlus-S3: U0RxD = 50 (Right Pin), U0TxD = 49 (Left Pin)
    //// jwc ? Serial2.begin(115200, SERIAL_8N1, 50, 49); //

  
    //// jwc ? Serial.printf("*** Serial_0 & Serial_2: Setup Done\n");
    //// jwc ? printf("*** Serial_0 & Serial_2: Setup Done\n");
    printf("*** Serial_0 & Serial_2: Setup Done\n");
    printf("\n");
    
    pinMode(LCD_BL, OUTPUT);
    ledcAttachPin(LCD_BL, 1);
    ledcSetup(1, 20000, 8);
    ledcWrite(1, 255); // brightness 0 - 255

    gfx->begin();
    gfx->fillScreen(WHITE);

    gfx->setTextColor(BLACK);

    delay(3000);

    OV2640_Initialization_Flag = OV2640_Initialization();
    
    //// jwc 25-1121-1700 Initialize WiFi after camera setup
    initWiFi();
}

int cpuCyclesDelay_Before_Esp32_Tx_Int = 0;

void loop()
{
    //// jwc 25-1128-0810 Track if new AprilTag detected in this frame (for smart video streaming)
    bool apriltag_detected_in_frame = false;
    
    if (OV2640_Initialization_Flag == true)
    {
        camera_fb_t *frame = esp_camera_fb_get();

        //// jwc old: String string_String_Tag_Data = "";
        //// // jwc crash: char string_Tag_Data_ArrayOfChar[50];

        if (frame)
        {

            //// jwc 25-0411 gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)frame->buf, frame->width, frame->height);
            //// jwc yy gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)frame->buf, frame->width, frame->height);
            //// jwc n gfx->drawGrayscaleBitmap(0, 0, (uint16_t *)frame->buf, frame->width, frame->height);
            gfx->drawGrayscaleBitmap(0, 0, (uint8_t *)frame->buf, frame->width, frame->height);

            //// \/ jwc 25-0411-1800 convert to April-Tag Detect 

            // Convert our framebuffer to detector's input format
            #if DEBUG >= 3
            printf("Converting frame to detector's input format... ");
            printf("\n");

        #endif
            image_u8_t im = {
            .width = frame->width,
            .height = frame->height,
            .stride = frame->width,
            .buf = frame->buf
            };
        #if DEBUG >= 3
            printf("done");
            printf("\n");
            printf("Detecting... ");
            printf("\n");
        #endif

            // Detect
            zarray_t *detections = apriltag_detector_detect(td, &im);
        #if DEBUG >= 3
            printf("done. Result:");
            printf("\n");
        #endif

            //// jwc ? /// jwc Clear Tft Screen
            //// jwc ? ///
            //// jwc ? /// jwc y tft.fillScreen(TFT_BLACK);  
            //// jwc ? ft.fillScreen(TFT_BLACK);  
            //// jwc ? /// jwc y Provide more upper and left margin: tft.setCursor(0,0);
            //// jwc ? ft.setCursor(10,10);
            //// jwc ? ft.setTextColor(TFT_YELLOW);  
            //// jwc ? /// jwc tft.setTextSize(3);
            //// jwc ? /// jwc y tft.setTextSize(6);
            //// jwc ? ft.setTextSize(4);

            //// jwc 25-0411-1800 tft.setCursor(1,1);
            gfx->setCursor(1,1);
            //// jwc y good for nomral, but increase for VideoMeet-Cam \/: tft.setTextSize(2); tft.setTextSize(3);
            //// jwc y tft.setTextSize(4);
            //// jwc y seems just right to fit 6 max
            //// jwc 25-0411-1800 tft.setTextSize(5);
            //// jwc smaller for more hud info: gfx->setTextSize(5);
            //// jwc y gfx->setTextSize(3);
            //// jwc y very small for alot of text: gfx->setTextSize(2);
            gfx->setTextSize(3);

            gfx->printf(".");
            printf(".");

            if(zarray_size(detections) > 0){
                //// jwc 25-1128-0810 Mark that AprilTag was detected in this frame
                apriltag_detected_in_frame = true;
            
                // Print result
                for (int i = 0; i < zarray_size(detections); i++) {
                    apriltag_detection_t *det;
                    zarray_get(detections, i, &det);
                    //// jwc \/
                    //// jwc y Serial.print(" *** ");
                    
                    if(det->id % 2 == 0){
                        // Even #
                        //// jwc 25-0411-1800 tft.setTextColor(TFT_BLUE);      
                        gfx->setTextColor(BLUE);      
                    }
                    else{
                        // Odd #
                        //// jwc 25-0411-1800 tft.setTextColor(TFT_RED);
                        gfx->setTextColor(RED);
                    }

                    //// jwc add pose: //// jwc tft.printf(" *** %d", (det->id));
                    //// jwc 25-0411-1800 tft.printf("%d ", (det->id));
                    //// jwc add pose: Serial.print(det->id);
                    //// jwc add pose: //// jwc o Serial.print(", ");

                    //// jwc yy 25-0418-1000 Add Coordinates Info \/ gfx->printf("%d ", (det->id));
                    //// jwc y convert to int: gfx->printf("%d(%f,%f)[(%f,%f)>(%f,%f)] ", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]));
                    //// jwc y convert to int: printf     ("%d(%f,%f)[(%f,%f)>(%f,%f)] ", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]));
                    //// jwc yy: gfx->printf("\n  %d (%d,%d) [(%d,%d)>(%d,%d)]", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]));
                    //// jwc yy: printf     ("\n  %d (%d,%d) [(%d,%d)>(%d,%d)]", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]));
                    //// jwc n string_String_Tag_Data = sprintf("\n %d (%d,%d) [(%d,%d)>(%d,%d)]", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]));
                    //// // jwc n crash: sprintf(string_Tag_Data_ArrayOfChar, "\n %d (%d,%d) [(%d,%d)>(%d,%d)]", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]));           
                    //// // jwc n crash: gfx->printf("%s", string_Tag_Data_ArrayOfChar);
                    //// // jwc n crash: printf("%s", string_Tag_Data_ArrayOfChar);

            #if DEBUG == 1
                    gfx->setTextSize(3);
                    gfx->printf("\n* Tag: %d", (det->id));
            #elif DEBUG >= 2
                    gfx->setTextSize(2);
                    gfx->printf("\n* Tag: %d (%d,%d) [(%d,%d)>(%d,%d)] <%d, %d>", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]), det->hamming, det->decision_margin);
            #endif

            //// jwc yy #if DEBUG >= 2
            #if DEBUG >= 2
                //// jwc yy printf     ("\n* Tag: %d (%d,%d) [(%d,%d)>(%d,%d)] <%d, %d>", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]), det->hamming, det->decision_margin);
                //// jwc yy printf     ("\n* Tag: %d (% 15d,% 15d) [(% 15d,%15d)>(% 15d,% 15d)] <% 15d, % 15d>", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]), det->hamming, det->decision_margin);
                printf     ("\n* Tag: %d center(%.0f,%.0f)px corners[(%d,%d)>(%d,%d)] qual[%d|%d]", (det->id), (det->c[0]), (det->c[1]), (det->p[0][0]), (det->p[0][1]), (det->p[2][0]), (det->p[2][1]), det->hamming, det->decision_margin);
            #endif

                    //// jwc 25-0420-0000: #if DEBUG >= 1
                    //// jwc 25-0420-0000:         // Print tag ID
                    //// jwc 25-0420-0000:         //// jwc o Serial.print("ID: ");
                    //// jwc 25-0420-0000:         //// jwc o Serial.println(det->id);
                    //// jwc 25-0420-0000:         //// jwc n Serial.println("*** *** *** ", det->id, det->family, det->decision_margin);
                    //// jwc 25-0420-0000:         // Print tag ID and decision margin
                    //// jwc 25-0420-0000:         //// jwc y Serial.printf("*** *** *** [DET]%d,%f,", det->id, det->decision_margin);
                    //// jwc 25-0420-0000:         Serial.printf("*** *** *** [DETECT ID:] %5.0d,%5.0f,", det->id, det->decision_margin);
                    //// jwc 25-0420-0000: #endif

                    // Creating detection info object to feed into pose estimator
                    apriltag_detection_info_t info;
                    info.det = det;
                    info.tagsize = TAG_SIZE;
                    info.fx = FX;
                    info.fy = FY;
                    info.cx = CX;
                    info.cy = CY;

                    // Estimate the pose
                    apriltag_pose_t pose;
                    double err = estimate_tag_pose(&info, &pose);

                    // Compute the yaw, pitch, and roll from the rotation matrix (and convert to degree)
                    double yaw = atan2(MATD_EL(pose.R, 1, 0), MATD_EL(pose.R, 0, 0)) * RAD_TO_DEG;
                    double pitch = atan2(-MATD_EL(pose.R, 2, 0), sqrt(pow(MATD_EL(pose.R, 2, 1), 2) + pow(MATD_EL(pose.R, 2, 2), 2))) * RAD_TO_DEG;
                    double roll = atan2(MATD_EL(pose.R, 2, 1), MATD_EL(pose.R, 2, 2)) * RAD_TO_DEG;
            #if DEBUG >= 2   
                    // Print the yaw, pitch, and roll of the camera
                    //// jwc y Serial.printf("y,p,r: %15f, %15f, %15f\n", yaw, pitch, roll);
                    //// jwc yy Serial.printf(" *** y,p,r: %5.0f, %5.0f, %5.0f", yaw, pitch, roll);
                    //// jwc ? Serial.printf(" *** top-down-cam (right_hand-rule): yaw (down-axis), roll (backward-axis), pitch (right-axis): %5.0f, %5.0f, %5.0f", yaw, pitch, roll);
                    printf(" *** top-down-cam (right_hand-rule): yaw (down-axis), roll (backward-axis), pitch (right-axis): % 5.0f, % 5.0f, % 5.0f", yaw, pitch, roll);
            #endif         

            #if DEBUG >= 3    
                    // Print result (position of the tag in the camera's coordinate system)
                    //matd_print(pose.R, "%15f"); // Rotation matrix
                    //matd_print(pose.t, "%15f"); // Translation matrix
                    printf("\n");
                    printf("    *** pose.R: \n");
                    //// jwc y matd_print(pose.R, "%15f"); // Rotation matrix
                    matd_print(pose.R, "%15f"); // Rotation matrix
            #endif
            #if DEBUG >= 2    
                    //// y Serial.printf("\n");
                    printf("\n");
                    //// jwc o Serial.printf("    *** pose.t: \n");
                    printf("    *** pose.t: \n");
                    //// jwc y matd_print(pose.t, "%15f"); // Translation matrix
                    matd_print(pose.t, "%15f"); // Translation matrix
            #endif

            // Compute the transpose of the rotation matrix
                    matd_t *R_transpose = matd_transpose(pose.R);

                    // Negate the translation vector
                    for (int i = 0; i < pose.t->nrows; i++) {
                        MATD_EL(pose.t, i, 0) = -MATD_EL(pose.t, i, 0);
                    }

                    // Compute the position of the camera in the tag's coordinate system
                    matd_t *camera_position = matd_multiply(R_transpose, pose.t);
            #if DEBUG >= 1    
                    // Print x,y,z (range) plus orientation/yaw at DEBUG level 1
                    // x,y,z are camera position relative to tag in centimeters (z negative = tag in front of camera)
                    // yaw is camera orientation relative to tag in degrees
                    // range is simply the absolute value of z-coordinate (direct distance along optical axis)
                    printf("*** pos(%.1fcm,%.1fcm,%.1fcm) yaw:%.1fdeg range:%.1fcm\n", 
                           MATD_EL(camera_position, 0, 0) * 100.0, 
                           MATD_EL(camera_position, 1, 0) * 100.0, 
                           MATD_EL(camera_position, 2, 0) * 100.0, 
                           yaw,
                           //// jwc 'fabs()' ensures positive value, regardless of the coordinate system orientation.
                           fabs(MATD_EL(camera_position, 2, 0)) * 100.0);
            #endif
            #if DEBUG >= 3    
                    //// jwc y Serial.printf("x,y,z: %15f, %15f, %15f\n", MATD_EL(camera_position, 0, 0), MATD_EL(camera_position, 1, 0), MATD_EL(camera_position, 2, 0));
                    //// jwc o Serial.printf("*** x,y,z: %5.0f, %5.0f, %5.0f\n", MATD_EL(camera_position, 0, 0), MATD_EL(camera_position, 1, 0), MATD_EL(camera_position, 2, 0));
                    printf(" *** x,y,z: %5.0f, %5.0f, %5.0f", MATD_EL(camera_position, 0, 0), MATD_EL(camera_position, 1, 0), MATD_EL(camera_position, 2, 0));
            #endif         
                    
                    //// jwc 25-1126-2200 OPTION 1: Real-time HTTP POST (ACTIVE)
                    // Calculate all values BEFORE freeing matrices
                    
                    // Convert meters to centimeters for x,y,z
                    float x_cm = MATD_EL(camera_position, 0, 0) * 100.0;
                    float y_cm = MATD_EL(camera_position, 1, 0) * 100.0;
                    float z_cm = MATD_EL(camera_position, 2, 0) * 100.0;
                    
                    // Calculate distance from camera to tag (absolute value of z-coordinate)
                    float distance_cm = fabs(MATD_EL(camera_position, 2, 0)) * 100.0;
                    
                    // Calculate tag size percentage of display
                    // Tag corners span from p[0] to p[2] (diagonal)
                    float tag_width_px = sqrt(pow(det->p[2][0] - det->p[0][0], 2) + pow(det->p[2][1] - det->p[0][1], 2));
                    float screen_diagonal_px = sqrt(pow(frame->width, 2) + pow(frame->height, 2));
                    float tag_size_percent = (tag_width_px / screen_diagonal_px) * 100.0;
                    
                    // Free the matrices (after extracting values)
                    matd_destroy(R_transpose);
                    matd_destroy(camera_position);
                    
                    // Queue tag data for later HTTP sending (with rate limiting to prevent list overflow)
                    unsigned long current_time_for_list = millis();
                    
                    // Only add to list if enough time has passed since last addition
                    if (current_time_for_list - list_add_time_last >= LIST_ADD_INTERVAL_MS) {
                        sensor_t *s = esp_camera_sensor_get();
                        if (s) {
                            camera_sensor_info_t *sinfo = esp_camera_sensor_get_info(&(s->id));
                            if (sinfo) {
                                listTagEvent_Add(det->id, yaw, pitch, roll, x_cm, y_cm, z_cm, 
                                            tag_size_percent, distance_cm, sinfo->name);
                                list_add_time_last = current_time_for_list;
                                
                                #if DEBUG >= 1
                                printf("*** LIST: Added tag (rate-limited to every %lums)\n", LIST_ADD_INTERVAL_MS);
                                #endif
                            }
                        }
                    }
                    #if DEBUG >= 2
                    else {
                        printf("*** LIST: Skipped addition (rate limit: %lums remaining)\n", 
                               LIST_ADD_INTERVAL_MS - (current_time_for_list - list_add_time_last));
                    }
                    #endif

                    //// jwc 25-1126-2200 OPTION 1B: Buffer for periodic POST (ARCHIVED)
                    //// jwc 25-1126-2200 // Buffer tag data instead of sending HTTP immediately
                    //// jwc 25-1126-2200 // This allows screen to update at full speed without blocking
                    //// jwc 25-1126-2200 sensor_t *s = esp_camera_sensor_get();
                    //// jwc 25-1126-2200 if (s) {
                    //// jwc 25-1126-2200     camera_sensor_info_t *sinfo = esp_camera_sensor_get_info(&(s->id));
                    //// jwc 25-1126-2200     if (sinfo) {
                    //// jwc 25-1126-2200         // Save to buffer (instant, no network delay)
                    //// jwc 25-1126-2200         latest_tag.has_data = true;
                    //// jwc 25-1126-2200         latest_tag.tag_id = det->id;
                    //// jwc 25-1126-2200         strncpy(latest_tag.camera_name, sinfo->name, sizeof(latest_tag.camera_name) - 1);
                    //// jwc 25-1126-2200         latest_tag.camera_name[sizeof(latest_tag.camera_name) - 1] = '\0';
                    //// jwc 25-1126-2200         latest_tag.timestamp = millis();
                    //// jwc 25-1126-2200         printf("*** Tag buffered: ID=%d, Camera=%s (will POST later)\n", det->id, sinfo->name);
                    //// jwc 25-1126-2200     }
                    //// jwc 25-1126-2200 }

                    //// jwc 25-1126-2200 OPTION 2: Queue for GET polling (ARCHIVED)
                    //// jwc 25-1126-2200 // Convert meters to centimeters for x,y,z
                    //// jwc 25-1126-2200 float x_cm = MATD_EL(camera_position, 0, 0) * 100.0;
                    //// jwc 25-1126-2200 float y_cm = MATD_EL(camera_position, 1, 0) * 100.0;
                    //// jwc 25-1126-2200 float z_cm = MATD_EL(camera_position, 2, 0) * 100.0;
                    //// jwc 25-1126-2200 
                    //// jwc 25-1126-2200 // Calculate distance from camera to tag (absolute value of z-coordinate)
                    //// jwc 25-1126-2200 float distance_cm = fabs(MATD_EL(camera_position, 2, 0)) * 100.0;
                    //// jwc 25-1126-2200 
                    //// jwc 25-1126-2200 // Calculate tag size percentage of display
                    //// jwc 25-1126-2200 // Tag corners span from p[0] to p[2] (diagonal)
                    //// jwc 25-1126-2200 float tag_width_px = sqrt(pow(det->p[2][0] - det->p[0][0], 2) + pow(det->p[2][1] - det->p[0][1], 2));
                    //// jwc 25-1126-2200 float screen_diagonal_px = sqrt(pow(frame->width, 2) + pow(frame->height, 2));
                    //// jwc 25-1126-2200 float tag_size_percent = (tag_width_px / screen_diagonal_px) * 100.0;
                    //// jwc 25-1126-2200 
                    //// jwc 25-1126-2200 // Queue the event (instant, no network delay)
                    //// jwc 25-1126-2200 queueTagEvent(det->id, yaw, pitch, roll, x_cm, y_cm, z_cm, tag_size_percent, distance_cm);
                    //// jwc 25-1126-2200 
            //// jwc 25-1126-2200 #if DEBUG >= 2    
                    //// jwc 25-1126-2200 printf("*** Tag queued: size=%.1f%% of screen, distance=%.1fcm\n", tag_size_percent, distance_cm);
            //// jwc 25-1126-2200 #endif
                    }
            #if DEBUG >= 2    
                    //// jwc o Serial.println("");
                    printf(";\n");
            #endif         
            } 

            // Free detection result object
            apriltag_detections_destroy(detections);

            //// /\ jwc 25-0411-1800 convert to April-Tag Detect 
            
            esp_camera_fb_return(frame);
        }
        delay(1);
    } // Close if (OV2640_Initialization_Flag == true)

    //// jwc \/ 
    ////

    
    // read from port 0, send to port 1:
    //// jwc y if (Serial.available()) {
    //// jwc yyy if (Serial.available()) {

    //// jwc ? Serial.printf("Received %d bytes\n", Serial.available());
    //// jwc ? Serial.printf("First byte is '%c' [0x%02x]\n", Serial.peek(), Serial.peek());
    uint8_t charPerLine = 0;
    String string_String_Read = "";

    //// jwc Following Serial.read causes some lag to videostream, so can stub out if need full real-time videostream   
#if DEBUG >= 1 //// test lag?
    while (Serial.available()) {
        //// jwc 25-0418-0000 char c = Serial.read();
        //// jwc 25-0418-0000 //// jwc This appears to work-NOT via Uart0 Port: Serial.printf("P:0< '%c' [0x%02x] ", c, c);
        //// jwc 25-0418-0000 //// jwc y printf("p:0< '%c' [0x%02x] ", c, c);
        //// jwc 25-0418-0000 //// jwc yy printf("0< '%c' [0x%02x] ", c, c);
        //// jwc 25-0418-0000 
        //// jwc 25-0418-0000 //// jwc COULD THIS CAUSE Mb to have BufferRx Overrun?: printf("0<'%c'", c);
        //// jwc 25-0418-0000 gfx->fillScreen(WHITE);
        //// jwc 25-0418-0000 gfx->setTextSize(5);
        //// jwc 25-0418-0000 //// jwc 1, 50, try 100
        //// jwc 25-0418-0000 gfx->setCursor(1,100);
        //// jwc 25-0418-0000 gfx->printf("0<'%c'", c);
        //// jwc 25-0418-0000 
        //// jwc 25-0418-0000 //// jwc '1000', 50 not bad, 25 too fast, try 100 nice to read, but get stuck infitely in this while loop, no more Esp_Tx
        //// jwc 25-0418-0000 //// jwc not bad, but needs slower: delay(100);
        //// jwc 25-0418-0000 delay(50);
        //// jwc 25-0418-0000 
        //// jwc 25-0418-0000 //// jwc ? Serial.write(c);
        //// jwc 25-0418-0000 
        //// jwc 25-0418-0000 //// jwc oy if (++charPerLine >= 10) {
        //// jwc 25-0418-0000 //// jwc oy     charPerLine = 0;
        //// jwc 25-0418-0000 //// jwc oy     //// jwc This appears to work-NOT via Uart0 Port: Serial.printf("P:\n");
        //// jwc 25-0418-0000 //// jwc oy     //// jwc y printf("p:\n");
        //// jwc 25-0418-0000 //// jwc oy     printf("\n");
        //// jwc 25-0418-0000 //// jwc oy }

        string_String_Read = Serial.readString();
        string_String_Read.trim();

        //// jwc COULD THIS CAUSE Mb to have BufferRx Overrun?: printf("0<'%c'", c);

        //// /// gfx->fillScreen(WHITE);
        gfx->setTextSize(3);
        //// jwc 1, 50, try 100, 200, 175
        gfx->setCursor(1,175);
        gfx->printf("\n0<'%s'", string_String_Read);
        printf     ("\n0<'%s'", string_String_Read);

        //// jwc '1000', 50 not bad, 25 too fast, try 100 nice to read, but get stuck infitely in this while loop, no more Esp_Tx
        //// jwc not bad, but needs slower: delay(100);
        //// //// jwc oy delay(50);
        //// //// jwc too laggy, try 50, 0 \/ delay(100);
        //// //// delay(50);

        //// jwc ? Serial.write(c);

        //// jwc oy if (++charPerLine >= 10) {
        //// jwc oy     charPerLine = 0;
        //// jwc oy     //// jwc This appears to work-NOT via Uart0 Port: Serial.printf("P:\n");
        //// jwc oy     //// jwc y printf("p:\n");
        //// jwc oy     printf("\n");
        //// jwc oy }
    }
#endif


    //// jwc yyy //// jwc yy int inByte = Serial.read();
    //// jwc yyy String str = Serial.readString();
    //// jwc yyy str.trim();

    //
    //// jwc n Serial.write("**Serial_0\n");
    //// jwc n Serial.write(inByte);
    //// jwc y Serial.print("**Serial_0\n");
    //// jwc n Serial.print("*** Serial_0:%d\n", inByte);
    //// jwc n Serial0.print("*** Serial_0:%d\n", inByte);
    //// jwc yy printf("0:Serial_0:%d\n", inByte);
    //// jwc yyy Serial.print  (">0");
    //// jwc yyy Serial.println(str);

    //// jwc o Serial.write(inByte);
    //// jwc yyy }
    
    //// jwc ? //// jwc y Serial2.print("Hello\n");
    //// jwc ? //// jwc yy Serial2.print("123");
    //// jwc ? //// jwc y Serial2 not received: printf("123");
    //// jwc ? //// jwc y Add '\n' for microbit \/: Serial2.print("123");
    //// jwc ? //// jwc yy Serial2.print("123\n");

    //// jwc ? const int BUFFER_SIZE = 10;
    //// jwc ? char buf[BUFFER_SIZE];
    //// jwc ? 
    //// jwc ? //// jwc yyy if (Serial2.available()) {
    //// jwc ? 
    //// jwc ? 
    //// jwc ? //// jwc ? Serial2.printf("Received %d bytes\n", Serial2.available());
    //// jwc ? //// jwc ? Serial2.printf("First byte is '%c' [0x%02x]\n", Serial2.peek(), Serial2.peek());
    //// jwc ? uint8_t charPerLine_2 = 0;
    //// jwc ? 
    //// jwc ? while (Serial2.available()) {
    //// jwc ?   char c = Serial2.read();
    //// jwc ?   //// jwc 25-0125-0500 Serial2.printf("2< '%c' [0x%02x] ", c, c);
    //// jwc ? 
    //// jwc ?   //Serial2.write(c);
    //// jwc ?   printf("2< '%c' [0x%02x] ", c, c);
    //// jwc ? 
    //// jwc ?   //// jwc 25-0125-0500 if (++charPerLine_2 >= 10) {
    //// jwc ?   //// jwc 25-0125-0500   charPerLine_2 = 0;
    //// jwc ?   //// jwc 25-0125-0500   Serial2.println();
    //// jwc ?   //// jwc 25-0125-0500 }
    //// jwc ? }
    
    //// jwc Create delay for Esp32 sending to not overwhelm micro:bit receive buffer
    ////
    cpuCyclesDelay_Before_Esp32_Tx_Int++;

    if (cpuCyclesDelay_Before_Esp32_Tx_Int >= 10){

        cpuCyclesDelay_Before_Esp32_Tx_Int = 0;

        int key_Int = random(0,4);
        //// jwc y int value_Int = random(5,9);
        //// jwc Mb> [0..2]:[3..6] | E3> [7..9]
        int value_Int = random(7,10);
      
        //// jwc oy String string_String_Write = "2>" + String(value_Int) + "\n";
        //// jwc yy remove '\n' to reduce log-space: String string_String_Write = "0>" + String(value_Int) + "\n";
        String string_String_Write = "0>" + String(value_Int) + ", ";
      
        //// jwc y Serial2.printf("2>%d\n", value_Int);
        //// jwc y printf("2>:%d\n", value_Int);
        //// jwc n Serial2.printf(string_String_Write);
        //// jwc n Serial2.printf("%S", string_String_Write);
        //// jwc oy Serial2.printf("%s", string_String_Write);
        //// jwc This appears to work-NOT via Uart0 Port: Serial.printf("P:%s", string_String_Write);
        //// jwc This appears to work via Uart0 Port \/
        //// jwc printf("%s", string_String_Write);
        //// jwc yy remove '\n' to reduce log-space: printf("\n%s", string_String_Write);
        printf("%s", string_String_Write);

        //// jwc ? \/     Serial.write(value_Int);
        
        //// jwc This appears to work-NOT via Uart0 Port
        //// jwc y Serial.printf("P: .0.\n");
        
        //// jwc ? \/     Serial.printf("#");
        //// jwc ? \/     Serial.write("-");
    
        //// jwc This appears to work via Uart0 Port \/
        //// jwc y printf("p: .0.\n");
        printf(".");

    }
    
    //// jwc 25-1127-1130 PERIODIC HTTP SENDER - Processes list one tag at a time
    // Check if it's time to send tag data from list via HTTP
    unsigned long current_time = millis();
    if (list_count > 0 && 
        (current_time - http_send_time_last >= HTTP_SEND_INTERVAL_MS)) {
        
        // Remove oldest tag from list
        tagData_Struct tag_to_send;
        if (listTagEvent_Remove(&tag_to_send)) {
            #if DEBUG >= 1
            printf("\n*** HTTP: Sending tag from list (List: %d remaining)...\n", list_count);
            #endif
            
            // Send via HTTP POST
            sensor_t *s = esp_camera_sensor_get();
            if (s) {
                camera_sensor_info_t *sinfo = esp_camera_sensor_get_info(&(s->id));
                if (sinfo) {
                    sendAprilTagData(tag_to_send.tag_id, sinfo->name, 
                                   tag_to_send.yaw, tag_to_send.pitch, tag_to_send.roll,
                                   tag_to_send.x_cm, tag_to_send.y_cm, tag_to_send.z_cm,
                                   tag_to_send.tag_size_percent, tag_to_send.distance_cm);
                }
            }
            
            // Update last send time (whether success or failure)
            http_send_time_last = current_time;
        }
    }
    
    //// jwc 25-1128-0810 VIDEO STREAMING - Smart streaming based on lag time
    // Calculate lag: time since oldest tag was added (if list has items)
    unsigned long lag_ms = 0;
    if (list_count > 0) {
        // Calculate position of oldest tag
        int oldest_pos = (list_head - list_count + tagData_MAX) % tagData_MAX;
        lag_ms = current_time - tagData_Queue[oldest_pos].timestamp;
    }
    
    // Decide whether to send video based on lag
    bool should_send_video = false;
    
    if (OV2640_Initialization_Flag && wifi_connected && 
        (current_time - video_send_time_last >= VIDEO_SEND_INTERVAL_MS)) {
        
        if (lag_ms < 3000) {
            // System processing fast (lag < 3s) - send video regularly
            should_send_video = true;
            #if DEBUG >= 1
            printf("*** VIDEO: Lag=%lums - Regular streaming mode\n", lag_ms);
            #endif
        } else if (apriltag_detected_in_frame) {
            // System is lagging (lag >= 3s) - only send frames with new AprilTag detections
            should_send_video = true;
            #if DEBUG >= 1
            printf("*** VIDEO: Lag=%lums - Smart mode: Sending frame with NEW AprilTag detection\n", lag_ms);
            #endif
        }
        #if DEBUG >= 2
        else {
            printf("*** VIDEO: Lag=%lums - Smart mode: Skipping frame (no new AprilTag)\n", lag_ms);
        }
        #endif
    }
    
    if (should_send_video) {
        // Capture and send video frame
        camera_fb_t *video_frame = esp_camera_fb_get();
        if (video_frame) {
            bool success = sendVideoFrame(video_frame);
            esp_camera_fb_return(video_frame);
            
            // Always update timestamp to prevent retry spam on failure
            video_send_time_last = current_time;
            
            if (!success) {
                printf("*** VIDEO: Upload failed - skipping to avoid blocking\n");
            }
        }
    }
    ////
    //// jwc 25-1124-1700 y //// jwc 25-1123-0750 PERIODIC HTTP SENDER - Non-blocking, separate from screen updates
    //// jwc 25-1124-1700 y // Check if it's time to send buffered tag data via HTTP
    //// jwc 25-1124-1700 y unsigned long current_time = millis();
    //// jwc 25-1124-1700 y if (latest_tag.has_data && 
    //// jwc 25-1124-1700 y     (current_time - last_http_send_time >= HTTP_SEND_INTERVAL_MS)) {
    //// jwc 25-1124-1700 y     
    //// jwc 25-1124-1700 y     printf("\n*** HTTP: Time to send buffered data (every %lu ms)...\n", HTTP_SEND_INTERVAL_MS);
    //// jwc 25-1124-1700 y     
    //// jwc 25-1124-1700 y     // Send the buffered tag data
    //// jwc 25-1124-1700 y     bool http_success = sendAprilTagData(
    //// jwc 25-1124-1700 y         latest_tag.tag_id, 
    //// jwc 25-1124-1700 y         latest_tag.camera_name
    //// jwc 25-1124-1700 y     );
    //// jwc 25-1124-1700 y     
    //// jwc 25-1124-1700 y     if (http_success) {
    //// jwc 25-1124-1700 y         printf(">>> >>> HTTP SEND: YES * Tag ID %d\n\n", latest_tag.tag_id);
    //// jwc 25-1124-1700 y     } else {
    //// jwc 25-1124-1700 y         printf(">>> >>> HTTP SEND: NOT * Tag ID %d\n\n", latest_tag.tag_id);
    //// jwc 25-1124-1700 y     }
    //// jwc 25-1124-1700 y     
    //// jwc 25-1124-1700 y     printf("\n");
//// jwc 25-1124-1700 y 
    //// jwc 25-1124-1700 y     // Update last send time (whether success or failure)
    //// jwc 25-1124-1700 y     last_http_send_time = current_time;
    //// jwc 25-1124-1700 y     
    //// jwc 25-1124-1700 y     // Clear buffer after sending
    //// jwc 25-1124-1700 y     latest_tag.has_data = false;
    //// jwc 25-1124-1700 y }   

}
