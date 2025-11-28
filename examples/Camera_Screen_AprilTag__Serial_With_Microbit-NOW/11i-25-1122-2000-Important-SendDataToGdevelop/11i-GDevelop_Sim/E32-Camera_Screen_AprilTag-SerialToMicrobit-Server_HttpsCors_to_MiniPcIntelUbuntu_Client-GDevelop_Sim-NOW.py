### jwc 25-1123-0700 ## 📊 What Each Interface Means
### jwc 25-1123-0700 
### jwc 25-1123-0700 Based on your output:
### jwc 25-1123-0700 
### jwc 25-1123-0700 | IP Address | Interface | Type | Purpose | ESP32 Can Use? | |------------|-----------|------|---------|----------------| | `10.0.0.220` | `enp2s0` | Ethernet | Wired network (router) | Maybe - if same network | | `10.42.0.1` | `wlp1s0` | WiFi | __Hotspot__ | ✅ __YES - Use this!__ |
### jwc 25-1123-0700 
### jwc 25-1123-0700 ---
### jwc 25-1123-0700 
### jwc 25-1123-0700 ## 🔍 Interface Name Breakdown
### jwc 25-1123-0700 
### jwc 25-1123-0700 ### __enp2s0 = Ethernet__
### jwc 25-1123-0700 
### jwc 25-1123-0700 - `en` = Ethernet (wired)
### jwc 25-1123-0700 - `p2` = PCI bus 2
### jwc 25-1123-0700 - `s0` = Slot 0
### jwc 25-1123-0700 - __Connected to:__ Your router via Ethernet cable
### jwc 25-1123-0700 - __IP:__ 10.0.0.220 (from your home network)
### jwc 25-1123-0700 
### jwc 25-1123-0700 ### __wlp1s0 = WiFi (Hotspot)__
### jwc 25-1123-0700 
### jwc 25-1123-0700 - `wl` = Wireless LAN (WiFi)
### jwc 25-1123-0700 - `p1` = PCI bus 1
### jwc 25-1123-0700 - `s0` = Slot 0
### jwc 25-1123-0700 - __Purpose:__ WiFi hotspot for ESP32
### jwc 25-1123-0700 - __IP:__ 10.42.0.1 ⭐ (ESP32 gateway)
### jwc 25-1123-0700 
### jwc 25-1123-0700 ---
### jwc 25-1123-0700 
### jwc 25-1123-0700 ## 🎯 For Your ESP32
### jwc 25-1123-0700 
### jwc 25-1123-0700 __Use the wlp1s0 (WiFi hotspot) IP:__
### jwc 25-1123-0700 
### jwc 25-1123-0700 ```cpp
### jwc 25-1123-0700 const char* WIFI_SSID = "ESP32_Local_Network";
### jwc 25-1123-0700 const char* WIFI_PASSWORD = "YourSecurePassword123";
### jwc 25-1123-0700 const char* TEST_SERVER_URL = "http://10.42.0.1:5000/esp32_apriltag_data";
### jwc 25-1123-0700 ```
### jwc 25-1123-0700 
### jwc 25-1123-0700 __Why:__
### jwc 25-1123-0700 
### jwc 25-1123-0700 - `10.0.0.220` (enp2s0) - Your ethernet connection to router
### jwc 25-1123-0700 - `10.42.0.1` (wlp1s0) - __Your hotspot that ESP32 connects to__ ⭐
### jwc 25-1123-0700 
### jwc 25-1123-0700 ---
### jwc 25-1123-0700 
### jwc 25-1123-0700 ## 📊 Your Complete Network Setup
### jwc 25-1123-0700 
### jwc 25-1123-0700 ```javascript
### jwc 25-1123-0700 [Internet] ←→ [Router 10.0.0.1]
### jwc 25-1123-0700                     ↓
### jwc 25-1123-0700             (Ethernet Cable)
### jwc 25-1123-0700                     ↓
### jwc 25-1123-0700               [Ubuntu Server]
### jwc 25-1123-0700          ┌──────────────────────┐
### jwc 25-1123-0700          │ enp2s0: 10.0.0.220  │ ← Wired to router
### jwc 25-1123-0700          │                      │
### jwc 25-1123-0700          │ wlp1s0: 10.42.0.1   │ ← WiFi hotspot
### jwc 25-1123-0700          └──────────────────────┘
### jwc 25-1123-0700                     │
### jwc 25-1123-0700             (WiFi 2.4GHz)
### jwc 25-1123-0700                     │
### jwc 25-1123-0700               [ESP32 Device]
### jwc 25-1123-0700               10.42.0.x
### jwc 25-1123-0700 ```
### jwc 25-1123-0700 
### jwc 25-1123-0700 

#!/usr/bin/env python3

"""
ESP32 AprilTag Test Server - Modified for ESP32 Communication Testing
Based on TestServer but specifically designed to receive ESP32 AprilTag data
Receives det->id and sinfo->name from ESP32 T-CameraPlus-S3
"""

from flask import Flask, request, jsonify, render_template_string, make_response
from flask_cors import CORS
import time
import json
from datetime import datetime
import threading
import logging

# Configuration
ESP32_SERVER_PORT = 5000
ESP32_SERVER_HOST = '0.0.0.0'  # Listen on all interfaces

# TagData class to match ESP32's tagData_Struct
class TagData:
    def __init__(self):
        self.smartcam_ip = ""
        self.tag_id = 0
        self.camera_name = ""
        self.yaw = 0.0
        self.pitch = 0.0
        self.roll = 0.0
        self.x_cm = 0.0
        self.y_cm = 0.0
        self.z_cm = 0.0
        self.tag_size_percent = 0.0
        self.distance_cm = 0.0
        self.timestamp = 0

# Global list for AprilTag data (mirrors ESP32 structure)
tagData_List_Fifo = []
tagData_List_Fifo_Count_Base1 = 0
tagData_List_Fifo_Count_MAX_Base1 = 50
tagData_List_Fifo_Index_Head_Base0 = 0

# Global storage for received ESP32 data
esp32_data_log = []
esp32_data_latest = {
    'id': None,
    'camera_name': None,
    'timestamp': None,
    'status': 'waiting_for_data'
}

# jwc 25-1128-0820 Smart empty response tracking - print once on entry, print total on exit
empty_response_count = 0
empty_response_mode = False  # Track if we're in "empty mode"

# jwc 25-1128-0100 VIDEO STREAMING - Storage for latest video frame
video_frame_latest = None
video_frame_lock = threading.Lock()
video_frame_count = 0

# Statistics
stats = {
    'total_requests': 0,
    'successful_requests': 0,
    'failed_requests': 0,
    'unique_tag_ids': set(),
    'start_time': time.time(),
    'list_overflows': 0
}

# Create Flask app
app = Flask(__name__)

# Enable CORS for all routes (ESP32 cross-origin requests)
CORS(app, 
     origins="*",  # Allow all origins for ESP32 testing
     methods=["GET", "POST", "PUT", "DELETE", "OPTIONS"],
     allow_headers=["Content-Type", "Authorization", "X-Requested-With"],
     supports_credentials=True)

# HTML template for web interface
HTML_TEMPLATE = """
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 AprilTag Test Server</title>
    <meta http-equiv="refresh" content="2">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background-color: #f0f0f0; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { background-color: #2c3e50; color: white; padding: 20px; border-radius: 10px; margin-bottom: 20px; }
        .status-box { background-color: white; padding: 20px; border-radius: 10px; margin-bottom: 20px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        .data-log { background-color: #2c3e50; color: #00ff00; padding: 20px; border-radius: 10px; font-family: monospace; max-height: 400px; overflow-y: auto; }
        .stats { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; margin-bottom: 20px; }
        .stat-item { background-color: white; padding: 15px; border-radius: 10px; text-align: center; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        .stat-value { font-size: 2em; font-weight: bold; color: #3498db; }
        .stat-label { color: #7f8c8d; margin-top: 5px; }
        .success { color: #27ae60; }
        .error { color: #e74c3c; }
        .waiting { color: #f39c12; }
        .endpoint-info { background-color: #ecf0f1; padding: 15px; border-radius: 10px; margin-bottom: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🎯 ESP32 AprilTag Test Server</h1>
            <p>Receiving AprilTag data from ESP32 T-CameraPlus-S3</p>
            <p><strong>Server:</strong> http://{{ server_ip }}:{{ server_port }}</p>
        </div>

        <div class="endpoint-info">
            <h3>📡 ESP32 Endpoint Information</h3>
            <p><strong>POST Endpoint:</strong> <code>http://{{ server_ip }}:{{ server_port }}/esp32_apriltag_data</code></p>
            <p><strong>Expected JSON Format:</strong></p>
            <pre>{"id": 5, "camera_name": "OV2640", "timestamp": 1234567890}</pre>
        </div>

        <div class="stats">
            <div class="stat-item">
                <div class="stat-value">{{ stats.total_requests }}</div>
                <div class="stat-label">Total Requests</div>
            </div>
            <div class="stat-item">
                <div class="stat-value success">{{ stats.successful_requests }}</div>
                <div class="stat-label">Successful</div>
            </div>
            <div class="stat-item">
                <div class="stat-value error">{{ stats.failed_requests }}</div>
                <div class="stat-label">Failed</div>
            </div>
            <div class="stat-item">
                <div class="stat-value">{{ stats.unique_tag_ids|length }}</div>
                <div class="stat-label">Unique Tag IDs</div>
            </div>
            <div class="stat-item">
                <div class="stat-value">{{ uptime }}</div>
                <div class="stat-label">Uptime (min)</div>
            </div>
        </div>

        <div class="status-box">
            <h3>📊 Latest ESP32 Data</h3>
            {% if latest_data.status == 'waiting_for_data' %}
                <p class="waiting">⏳ Waiting for ESP32 data...</p>
                <p>Make sure your ESP32 is:</p>
                <ul>
                    <li>Connected to WiFi</li>
                    <li>Running AprilTag detection</li>
                    <li>Sending POST requests to this server</li>
                </ul>
            {% else %}
                <p class="success">✅ Receiving ESP32 data!</p>
                <p><strong>AprilTag ID:</strong> {{ latest_data.id }}</p>
                <p><strong>Camera Model:</strong> {{ latest_data.camera_name }}</p>
                <p><strong>Last Update:</strong> {{ latest_data.timestamp }}</p>
                <p><strong>Age:</strong> {{ data_age }} seconds ago</p>
            {% endif %}
        </div>

        <div class="status-box">
            <h3>📝 Recent ESP32 Transmissions (Last 10)</h3>
            <div class="data-log">
                {% for entry in recent_log %}
                <div>{{ entry }}</div>
                {% endfor %}
                {% if not recent_log %}
                <div class="waiting">No ESP32 data received yet...</div>
                {% endif %}
            </div>
        </div>
    </div>
</body>
</html>
"""

def log_esp32_data(message, level="INFO"):
    """Log ESP32 data with timestamp"""
    timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
    log_entry = f"[{timestamp}] {level}: {message}"
    esp32_data_log.append(log_entry)
    
    # Keep only last 50 entries
    if len(esp32_data_log) > 50:
        esp32_data_log.pop(0)
    
    print(log_entry)

def AprilTag_List_Fifo_Push(tag_data):
    """Add tag data to circular list (FIFO with oldest drop on overflow)"""
    global tagData_List_Fifo_Index_Head_Base0, tagData_List_Fifo_Count_Base1, stats
    
    if tagData_List_Fifo_Count_Base1 < tagData_List_Fifo_Count_MAX_Base1:
        # List not full, just append
        tagData_List_Fifo.append(tag_data)
        tagData_List_Fifo_Count_Base1 += 1
    else:
        # List full, drop oldest (FIFO)
        tagData_List_Fifo[tagData_List_Fifo_Index_Head_Base0] = tag_data
        tagData_List_Fifo_Index_Head_Base0 = (tagData_List_Fifo_Index_Head_Base0 + 1) % tagData_List_Fifo_Count_MAX_Base1
        stats['list_overflows'] += 1
        log_esp32_data(f"⚠️  List overflow! Dropped oldest event. Total overflows: {stats['list_overflows']}", "WARN")

def AprilTag_List_Fifo_Pop():
    """Remove and return oldest tag data from list (FIFO - First In, First Out)"""
    global tagData_List_Fifo_Index_Head_Base0, tagData_List_Fifo_Count_Base1
    
    if tagData_List_Fifo_Count_Base1 == 0:
        return None
    
    # For a proper FIFO that grows with append():
    # - When not full: items are at [0, 1, 2, ...], oldest is always at index 0
    # - When full: using circular buffer, oldest is at head position
    
    if len(tagData_List_Fifo) < tagData_List_Fifo_Count_MAX_Base1:
        # List hasn't wrapped yet - simple FIFO from front
        tag_data = tagData_List_Fifo.pop(0)  # Remove from front (oldest)
        tagData_List_Fifo_Count_Base1 -= 1
    else:
        # List is full - circular buffer mode
        tag_data = tagData_List_Fifo[tagData_List_Fifo_Index_Head_Base0]
        tagData_List_Fifo_Index_Head_Base0 = (tagData_List_Fifo_Index_Head_Base0 + 1) % tagData_List_Fifo_Count_MAX_Base1
        tagData_List_Fifo_Count_Base1 -= 1
    
    return tag_data

@app.route('/')
def home():
    """Web interface showing ESP32 data reception status"""
    uptime_minutes = int((time.time() - stats['start_time']) / 60)
    
    # Calculate data age
    data_age = "N/A"
    if esp32_data_latest['timestamp']:
        try:
            data_age = int(time.time() - esp32_data_latest['timestamp'])
        except:
            data_age = "Unknown"
    
    return render_template_string(HTML_TEMPLATE,
                                server_ip="10.0.0.150",  # Update with your server IP
                                server_port=ESP32_SERVER_PORT,
                                stats=stats,
                                uptime=uptime_minutes,
                                latest_data=esp32_data_latest,
                                data_age=data_age,
                                recent_log=esp32_data_log[-10:])

@app.route('/client_e32_to_server__smartcam_data_post', methods=['POST', 'OPTIONS'])
def receive_esp32_apriltag_data():
    """Main endpoint to receive ESP32 SmartCam AprilTag data"""
    global esp32_data_latest, stats
    
    # Handle CORS preflight
    if request.method == 'OPTIONS':
        return '', 200
    
    stats['total_requests'] += 1
    
    try:
        # Get JSON data from ESP32
        data = request.get_json()
        
        # DEBUG: Print raw received data
        print("\n" + "="*70)
        print("📥 POST RECEIVED - Raw Data:")
        print("="*70)
        if data:
            import json
            print(json.dumps(data, indent=2))
        else:
            print("  ⚠️  No data received (empty)")
        print("="*70 + "\n")
        
        if not data:
            stats['failed_requests'] += 1
            log_esp32_data("❌ No JSON data received", "ERROR")
            return jsonify({'error': 'No JSON data provided'}), 400
        
        # Extract all AprilTag fields
        smartcam_ip = data.get('smartcam_ip', '')
        tag_id = data.get('tag_id')
        camera_name = data.get('camera_name', '')
        yaw = data.get('yaw', 0.0)
        pitch = data.get('pitch', 0.0)
        roll = data.get('roll', 0.0)
        x_cm = data.get('x_cm', 0.0)
        y_cm = data.get('y_cm', 0.0)
        z_cm = data.get('z_cm', 0.0)
        tag_size_percent = data.get('tag_size_percent', 0.0)
        distance_cm = data.get('distance_cm', 0.0)
        timestamp = data.get('timestamp', int(time.time()))
        
        # Validate required fields
        if tag_id is None:
            stats['failed_requests'] += 1
            log_esp32_data("❌ Missing 'tag_id' field in ESP32 data", "ERROR")
            return jsonify({'error': 'Missing required field: tag_id'}), 400
        
        # Use server's current time as timestamp (ESP32's millis() is not compatible with Unix time)
        server_receive_time = time.time()
        
        # Create TagData object
        tag_data = TagData()
        tag_data.smartcam_ip = smartcam_ip
        tag_data.tag_id = tag_id
        tag_data.camera_name = camera_name
        tag_data.yaw = yaw
        tag_data.pitch = pitch
        tag_data.roll = roll
        tag_data.x_cm = x_cm
        tag_data.y_cm = y_cm
        tag_data.z_cm = z_cm
        tag_data.tag_size_percent = tag_size_percent
        tag_data.distance_cm = distance_cm
        tag_data.timestamp = server_receive_time  # Use server time, not ESP32 millis()
        
        # Push to list
        AprilTag_List_Fifo_Push(tag_data)
        
        # Calculate ESP32->Server latency (if ESP32 timestamp was in millis, we can't accurately calculate)
        # For now, just log that data was received
        network_latency_ms = 0  # Can't calculate accurately without synced clocks
        
        # Update global data for web interface
        esp32_data_latest = {
            'id': tag_id,
            'camera_name': smartcam_ip,
            'timestamp': timestamp,
            'status': 'receiving_data'
        }
        
        # Update statistics
        stats['successful_requests'] += 1
        stats['unique_tag_ids'].add(tag_id)
        
        # Log successful reception with network latency and data content
        log_esp32_data(
            f"📥 RECEIVED: Tag ID={tag_id}, IP={smartcam_ip}, "
            f"Yaw={yaw:.1f}°, Pitch={pitch:.1f}°, Roll={roll:.1f}°, "
            f"Pos=({x_cm:.1f},{y_cm:.1f},{z_cm:.1f})cm, "
            f"Dist={distance_cm:.1f}cm, Size={tag_size_percent:.1f}%, "
            f"Latency={network_latency_ms:.1f}ms, "
            f"List={tagData_List_Fifo_Count_Base1}/{tagData_List_Fifo_Count_MAX_Base1}",
            "SUCCESS"
        )
        
        # Return success response to ESP32
        response_data = {
            'status': 'success',
            'message': 'AprilTag data received successfully',
            'received_data': {
                'id': tag_id,
                'camera_name': camera_name,
                'timestamp': timestamp
            },
            'server_time': time.time()
        }
        
        return jsonify(response_data), 200
        
    except json.JSONDecodeError:
        stats['failed_requests'] += 1
        log_esp32_data("❌ Invalid JSON format from ESP32", "ERROR")
        return jsonify({'error': 'Invalid JSON format'}), 400
        
    except Exception as e:
        stats['failed_requests'] += 1
        log_esp32_data(f"❌ Server error: {str(e)}", "ERROR")
        return jsonify({'error': f'Server error: {str(e)}'}), 500

@app.route('/esp32_status')
def esp32_status():
    """Get current ESP32 communication status"""
    uptime_seconds = int(time.time() - stats['start_time'])
    
    return jsonify({
        'server_status': 'online',
        'esp32_communication': esp32_data_latest['status'],
        'latest_data': esp32_data_latest,
        'statistics': {
            'total_requests': stats['total_requests'],
            'successful_requests': stats['successful_requests'],
            'failed_requests': stats['failed_requests'],
            'unique_tag_ids': list(stats['unique_tag_ids']),
            'uptime_seconds': uptime_seconds
        },
        'endpoints': {
            'post_data': '/esp32_apriltag_data',
            'get_status': '/esp32_status',
            'web_interface': '/'
        }
    })

@app.route('/test_esp32_connection')
def test_esp32_connection():
    """Test endpoint for ESP32 to verify connectivity"""
    log_esp32_data("🔍 ESP32 connection test requested", "INFO")
    
    return jsonify({
        'status': 'server_online',
        'message': 'ESP32 test server is running and ready to receive data',
        'server_time': time.time(),
        'endpoints': {
            'send_apriltag_data': 'POST /esp32_apriltag_data',
            'check_status': 'GET /esp32_status',
            'test_connection': 'GET /test_esp32_connection',
            'gdevelop_get_data': 'GET /apriltag_data_get'
        }
    })

# jwc 25-1128-0100 VIDEO STREAMING - Upload endpoint for ESP32 to send JPEG frames
@app.route('/video_frame_upload', methods=['POST', 'OPTIONS'])
def video_frame_receive():
    """Receive JPEG video frames from ESP32"""
    global video_frame_latest, video_frame_count
    
    # Handle CORS preflight
    if request.method == 'OPTIONS':
        return '', 200
    
    try:
        # Get raw JPEG data from ESP32
        frame_data = request.data
        
        if not frame_data or len(frame_data) == 0:
            print("⚠️  VIDEO: Received empty frame")
            return jsonify({'error': 'Empty frame data'}), 400
        
        # Store frame with thread safety
        with video_frame_lock:
            video_frame_latest = frame_data
            video_frame_count += 1
        
        print(f"📹 VIDEO: Frame received ({len(frame_data)} bytes, total frames: {video_frame_count})")
        
        return jsonify({
            'status': 'success',
            'frame_size': len(frame_data),
            'frame_count': video_frame_count
        }), 200
        
    except Exception as e:
        print(f"❌ VIDEO ERROR: {str(e)}")
        return jsonify({'error': str(e)}), 500

# jwc 25-1128-0100 VIDEO STREAMING - MJPEG stream endpoint for browser viewing
@app.route('/video_stream')
def video_stream():
    """Stream video frames as MJPEG for browser viewing"""
    def generate():
        """Generate MJPEG stream"""
        while True:
            with video_frame_lock:
                if video_frame_latest:
                    # Send frame as part of multipart stream
                    yield (b'--frame\r\n'
                           b'Content-Type: image/jpeg\r\n\r\n' + 
                           video_frame_latest + b'\r\n')
                else:
                    # No frame yet, send placeholder or wait
                    pass
            time.sleep(0.1)  # 10 FPS display rate (even if receiving 1 FPS)
    
    from flask import Response
    return Response(generate(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

# jwc 25-1128-0100 VIDEO STREAMING - Simple HTML viewer page
@app.route('/video_viewer')
def video_viewer():
    """Simple HTML page to view video stream"""
    html = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>ESP32 Video Stream - Proof of Concept</title>
        <style>
            body { 
                font-family: Arial, sans-serif; 
                margin: 20px; 
                background-color: #2c3e50;
                color: white;
                text-align: center;
            }
            .container { max-width: 800px; margin: 0 auto; }
            h1 { color: #3498db; }
            img { 
                border: 3px solid #3498db; 
                border-radius: 10px;
                max-width: 100%;
                background-color: #34495e;
            }
            .info {
                background-color: #34495e;
                padding: 15px;
                border-radius: 10px;
                margin-top: 20px;
            }
            a { color: #3498db; text-decoration: none; }
            a:hover { text-decoration: underline; }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>📹 ESP32 Live Video Stream</h1>
            <p><strong>Optimized: Low-Quality Stream (Reduced Lag)</strong></p>
            
            <img src="/video_stream" alt="ESP32 Camera Stream" width="240" height="240">
            
            <div class="info">
                <h3>ℹ️ Stream Information</h3>
                <p><strong>Resolution:</strong> 240x240 pixels (grayscale)</p>
                <p><strong>JPEG Quality:</strong> 10/100 (optimized for minimal lag)</p>
                <p><strong>Frame Rate:</strong> 0.33 FPS (1 frame every 3 seconds)</p>
                <p><strong>Purpose:</strong> View what AprilTag detector sees</p>
                <p><strong>AprilTag Data:</strong> <a href="/client_gdevelop_to_server__smartcam_data_get">View JSON</a></p>
                <p><strong>Server Status:</strong> <a href="/">Main Dashboard</a></p>
            </div>
        </div>
    </body>
    </html>
    '''
    return html

@app.route('/client_gdevelop_to_server__smartcam_data_get', methods=['GET', 'OPTIONS'])
def serve_gdevelop_data():
    """GET endpoint for GDevelop to retrieve SmartCam AprilTag data from list"""
    global empty_response_count, empty_response_mode
    
    # Handle CORS preflight
    if request.method == 'OPTIONS':
        return '', 200
    
    # Pop oldest event from list
    tag_data = AprilTag_List_Fifo_Pop()
    
    if tag_data is None:
        # No data in list - smart empty mode tracking
        if not empty_response_mode:
            # First empty response - enter empty mode
            empty_response_mode = True
            empty_response_count = 1
            print(f"⚪ EMPTY MODE: Started (no tags in list)")
        else:
            # Already in empty mode - count silently
            empty_response_count += 1
        
        # Return empty flat structure
        return jsonify({
            'smartcam_ip': '',
            'tag_id': -1,
            'camera_name': '',
            'yaw': 0.0,
            'pitch': 0.0,
            'roll': 0.0,
            'x_cm': 0.0,
            'y_cm': 0.0,
            'z_cm': 0.0,
            'tag_size_percent': 0.0,
            'distance_cm': 0.0,
            'timestamp': 0,
            'list_remaining': 0
        })
    
    # Exit empty mode if we were in it
    if empty_response_mode:
        print(f"⚪ EMPTY MODE: Ended (Total empty responses: {empty_response_count}x)")
        empty_response_mode = False
        empty_response_count = 0
    
    # Calculate retrieval latency (time from ESP32 send to GDevelop retrieval)
    retrieval_time = time.time()
    total_latency_ms = (retrieval_time - tag_data.timestamp) * 1000.0
    
    # Prepare response data (flat structure)
    response_data = {
        'smartcam_ip': tag_data.smartcam_ip,
        'tag_id': tag_data.tag_id,
        'camera_name': tag_data.camera_name,
        'yaw': tag_data.yaw,
        'pitch': tag_data.pitch,
        'roll': tag_data.roll,
        'x_cm': tag_data.x_cm,
        'y_cm': tag_data.y_cm,
        'z_cm': tag_data.z_cm,
        'tag_size_percent': tag_data.tag_size_percent,
        'distance_cm': tag_data.distance_cm,
        'timestamp': tag_data.timestamp,
        'list_remaining': tagData_List_Fifo_Count_Base1
    }
    
    # DEBUG: Print data being sent to GDevelop
    print("\n" + "="*70)
    print("📤 GET RESPONSE - Sending Data to GDevelop:")
    print("="*70)
    import json
    print(json.dumps(response_data, indent=2))
    print("="*70 + "\n")
    
    # Log data being sent to GDevelop
    log_esp32_data(
        f"📤 SENT: Tag ID={tag_data.tag_id}, IP={tag_data.smartcam_ip}, "
        f"Total_Latency={total_latency_ms:.1f}ms, "
        f"List={tagData_List_Fifo_Count_Base1}/{tagData_List_Fifo_Count_MAX_Base1}",
        "SUCCESS"
    )
    
    # Create response with explicit CORS headers
    response = make_response(jsonify(response_data))
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Methods'] = 'GET, OPTIONS'
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type'
    response.headers['Content-Type'] = 'application/json'
    
    return response

def print_startup_info():
    """Print server startup information"""
    import socket
    import subprocess
    
    # Get local IP address from hostname
    hostname = socket.gethostname()
    try:
        local_ip = socket.gethostbyname(hostname)
    except:
        local_ip = "Unknown"
    
    # Get all network interface IPs with interface names
    all_ips = []
    ip_interface_map = {}  # Map IP to interface name
    hotspot_ip = None
    hotspot_interface = None
    
    try:
        # Get all IPs using 'ip addr' command
        result = subprocess.run(['ip', '-4', 'addr', 'show'], capture_output=True, text=True)
        current_interface = None
        
        for line in result.stdout.split('\n'):
            # Check for interface name line (e.g., "2: enp2s0: <BROADCAST...")
            if ': <' in line and not line.startswith(' '):
                # Extract interface name
                parts = line.split(':')
                if len(parts) >= 2:
                    current_interface = parts[1].strip()
            
            # Check for IP address line
            if 'inet ' in line and 'scope global' in line and current_interface:
                # Extract IP address
                ip = line.strip().split()[1].split('/')[0]
                all_ips.append(ip)
                ip_interface_map[ip] = current_interface
                
                # Check if this is likely the hotspot IP (10.42.0.x)
                if ip.startswith('10.42.0.'):
                    hotspot_ip = ip
                    hotspot_interface = current_interface
    except:
        pass
    
    # Try to detect WiFi band (2.4GHz vs 5GHz) using multiple methods
    wifi_band = "Unknown"
    wifi_interface = "Unknown"
    debug_lines = []
    
    # Method 1: Try iwconfig
    debug_lines.append("\n🔍 DEBUG: Method 1 - iwconfig")
    try:
        result = subprocess.run(['iwconfig'], capture_output=True, text=True)
        debug_lines.append(f"   Return code: {result.returncode}")
        debug_lines.append(f"   Raw stdout length: {len(result.stdout)} chars")
        
        line_num = 0
        for line in result.stdout.split('\n'):
            line_num += 1
            if line.strip():  # Only show non-empty lines
                debug_lines.append(f"   Line {line_num}: '{line}'")
                
            if 'IEEE 802.11' in line or 'ESSID' in line:
                wifi_interface = line.split()[0]
                debug_lines.append(f"   → Found interface: {wifi_interface}")
                
            if 'Frequency:2.' in line or 'Frequency: 2.' in line:
                wifi_band = "2.4GHz ✅ (ESP32 Compatible)"
                debug_lines.append(f"   ✅ DETECTED 2.4GHz from line: '{line}'")
                break
            elif 'Frequency:5.' in line or 'Frequency: 5.' in line:
                wifi_band = "5GHz ⚠️ (ESP32 NOT Compatible - ESP32 needs 2.4GHz!)"
                debug_lines.append(f"   ⚠️  DETECTED 5GHz from line: '{line}'")
                break
                
        if wifi_band == "Unknown":
            debug_lines.append("   ❌ No frequency found in iwconfig output")
    except Exception as e:
        debug_lines.append(f"   ❌ Exception: {e}")
    
    # Method 2: Try nmcli if iwconfig didn't work
    if wifi_band == "Unknown":
        debug_lines.append("\n🔍 DEBUG: Method 2 - nmcli")
        try:
            result = subprocess.run(['nmcli', '-t', '-f', 'ACTIVE,SSID,CHAN,FREQ', 'dev', 'wifi'], 
                                  capture_output=True, text=True)
            debug_lines.append(f"   Return code: {result.returncode}")
            debug_lines.append(f"   Raw stdout length: {len(result.stdout)} chars")
            
            line_num = 0
            for line in result.stdout.split('\n'):
                line_num += 1
                if line.strip():  # Only show non-empty lines
                    debug_lines.append(f"   Line {line_num}: '{line}'")
                    
                if line.startswith('yes:'):
                    debug_lines.append(f"   → Active connection found: '{line}'")
                    parts = line.split(':')
                    debug_lines.append(f"   → Split into {len(parts)} parts: {parts}")
                    
                    if len(parts) >= 4:
                        freq = parts[3]
                        debug_lines.append(f"   → Frequency field (parts[3]): '{freq}'")
                        
                        if '2.' in freq or freq.startswith('2') or '24' in freq:
                            wifi_band = "2.4GHz ✅ (ESP32 Compatible)"
                            debug_lines.append(f"   ✅ DETECTED 2.4GHz from frequency: '{freq}'")
                        elif '5.' in freq or freq.startswith('5'):
                            wifi_band = "5GHz ⚠️ (ESP32 NOT Compatible - ESP32 needs 2.4GHz!)"
                            debug_lines.append(f"   ⚠️  DETECTED 5GHz from frequency: '{freq}'")
                        break
                        
            if wifi_band == "Unknown":
                debug_lines.append("   ❌ No active connection with frequency found")
        except Exception as e:
            debug_lines.append(f"   ❌ Exception: {e}")
    
    # Method 3: Try iw if still unknown
    if wifi_band == "Unknown":
        try:
            result = subprocess.run(['iw', 'dev'], capture_output=True, text=True)
            if result.returncode == 0:
                for line in result.stdout.split('\n'):
                    if 'Interface' in line:
                        wifi_interface = line.split()[-1]
                        break
                
                if wifi_interface != "Unknown":
                    result2 = subprocess.run(['iw', 'dev', wifi_interface, 'info'], 
                                           capture_output=True, text=True)
                    if 'channel' in result2.stdout.lower():
                        for line in result2.stdout.split('\n'):
                            if 'channel' in line.lower():
                                # Channels 1-14 are 2.4GHz, 36+ are 5GHz
                                if any(f'channel {i}' in line.lower() for i in range(1, 15)):
                                    wifi_band = "2.4GHz ✅ (ESP32 Compatible)"
                                elif any(f'channel {i}' in line.lower() for i in range(36, 200)):
                                    wifi_band = "5GHz ⚠️ (ESP32 NOT Compatible - ESP32 needs 2.4GHz!)"
                                break
        except:
            pass
    
    # Print debug information
    print("=" * 70)
    print("🐛 WiFi Band Detection Debug Output:")
    print("=" * 70)
    for debug_line in debug_lines:
        print(debug_line)
    print("=" * 70)
    
    print("\n" + "=" * 70)
    print("🎯 ESP32 AprilTag Test Server Starting...")
    print("=" * 70)
    print(f"🖥️  Hostname: {hostname}")
    print(f"🌐 Hostname IP: {local_ip}")
    
    # Display hotspot IP with interface name
    if hotspot_ip:
        if hotspot_interface:
            print(f"📡 Hotspot IP: {hotspot_ip} ({hotspot_interface}) ⭐ (Use this for ESP32!)")
        else:
            print(f"📡 Hotspot IP: {hotspot_ip} ⭐ (Use this for ESP32!)")
    
    # Display all IPs with their interface names
    if all_ips:
        print(f"🔗 All Network Interfaces:")
        for ip in all_ips:
            interface = ip_interface_map.get(ip, 'unknown')
            is_hotspot = ' ⭐ HOTSPOT' if ip == hotspot_ip else ''
            print(f"   • {ip:15s} ({interface}){is_hotspot}")
    print(f"📶 WiFi Band: {wifi_band}")
    print(f"🔌 Server Port: {ESP32_SERVER_PORT}")
    print("=" * 70)
    
    # Display URLs with the most appropriate IP
    display_ip = hotspot_ip if hotspot_ip else local_ip
    
    print(f"📍 Server URLs:")
    print(f"   Web Interface: http://{display_ip}:{ESP32_SERVER_PORT}/")
    print(f"   ESP32 POST Endpoint: http://{display_ip}:{ESP32_SERVER_PORT}/client_e32_to_server__smartcam_data_post")
    print(f"   GDevelop GET Endpoint: http://{display_ip}:{ESP32_SERVER_PORT}/client_gdevelop_to_server__smartcam_data_get")
    print(f"   Status Check: http://{display_ip}:{ESP32_SERVER_PORT}/esp32_status")
    
    if hotspot_ip and hotspot_ip != local_ip:
        print(f"\n💡 ESP32 Configuration:")
        print(f'   const char* TEST_SERVER_URL = "http://{hotspot_ip}:{ESP32_SERVER_PORT}/client_e32_to_server__smartcam_data_post";')
    print("=" * 70)
    print("📋 Expected ESP32 JSON Format:")
    print('   {"tag_id": 5, "camera_name": "OV2640", "timestamp": 1234567890}')
    print("=" * 70)
    print("🚀 Ready to receive ESP32 SmartCam AprilTag data!")
    print("   - Make sure ESP32 is connected to WiFi")
    print("   - ESP32 should POST to /client_e32_to_server__smartcam_data_post endpoint")
    print("   - GDevelop should GET from /client_gdevelop_to_server__smartcam_data_get endpoint")
    print("   - View real-time data at web interface")
    if "5GHz" in wifi_band:
        print("   ⚠️  WARNING: Ubuntu on 5GHz! ESP32 needs 2.4GHz network!")
    print("=" * 70)

if __name__ == '__main__':
    # Disable Flask's default request logging to reduce console clutter
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)  # Only show errors, not every HTTP request
    
    print_startup_info()
    
    # Start Flask server
    try:
        app.run(
            host=ESP32_SERVER_HOST,
            port=ESP32_SERVER_PORT,
            debug=False,  # Disable debug for cleaner output
            threaded=True
        )
    except KeyboardInterrupt:
        print("\n🛑 Server stopped by user")
    except Exception as e:
        print(f"❌ Server error: {e}")
