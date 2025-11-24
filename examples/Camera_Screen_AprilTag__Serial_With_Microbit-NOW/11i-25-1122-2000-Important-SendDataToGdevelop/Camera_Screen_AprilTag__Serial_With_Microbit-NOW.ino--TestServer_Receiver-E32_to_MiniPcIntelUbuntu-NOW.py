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

from flask import Flask, request, jsonify, render_template_string
from flask_cors import CORS
import time
import json
from datetime import datetime
import threading

# Configuration
ESP32_SERVER_PORT = 5000
ESP32_SERVER_HOST = '0.0.0.0'  # Listen on all interfaces

# Global storage for received ESP32 data
esp32_data_log = []
latest_esp32_data = {
    'id': None,
    'camera_name': None,
    'timestamp': None,
    'status': 'waiting_for_data'
}

# Statistics
stats = {
    'total_requests': 0,
    'successful_requests': 0,
    'failed_requests': 0,
    'unique_tag_ids': set(),
    'start_time': time.time()
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

@app.route('/')
def home():
    """Web interface showing ESP32 data reception status"""
    uptime_minutes = int((time.time() - stats['start_time']) / 60)
    
    # Calculate data age
    data_age = "N/A"
    if latest_esp32_data['timestamp']:
        try:
            data_age = int(time.time() - latest_esp32_data['timestamp'])
        except:
            data_age = "Unknown"
    
    return render_template_string(HTML_TEMPLATE,
                                server_ip="10.0.0.150",  # Update with your server IP
                                server_port=ESP32_SERVER_PORT,
                                stats=stats,
                                uptime=uptime_minutes,
                                latest_data=latest_esp32_data,
                                data_age=data_age,
                                recent_log=esp32_data_log[-10:])

@app.route('/esp32_apriltag_data', methods=['POST', 'OPTIONS'])
def receive_esp32_apriltag_data():
    """Main endpoint to receive ESP32 AprilTag data"""
    global latest_esp32_data, stats
    
    # Handle CORS preflight
    if request.method == 'OPTIONS':
        return '', 200
    
    stats['total_requests'] += 1
    
    try:
        # Get JSON data from ESP32
        data = request.get_json()
        
        if not data:
            stats['failed_requests'] += 1
            log_esp32_data("❌ No JSON data received", "ERROR")
            return jsonify({'error': 'No JSON data provided'}), 400
        
        # Extract ESP32 AprilTag data
        tag_id = data.get('id')
        camera_name = data.get('camera_name')
        timestamp = data.get('timestamp', time.time())
        
        # Validate required fields
        if tag_id is None:
            stats['failed_requests'] += 1
            log_esp32_data("❌ Missing 'id' field in ESP32 data", "ERROR")
            return jsonify({'error': 'Missing required field: id'}), 400
        
        if not camera_name:
            stats['failed_requests'] += 1
            log_esp32_data("❌ Missing 'camera_name' field in ESP32 data", "ERROR")
            return jsonify({'error': 'Missing required field: camera_name'}), 400
        
        # Update global data
        latest_esp32_data = {
            'id': tag_id,
            'camera_name': camera_name,
            'timestamp': timestamp,
            'status': 'receiving_data'
        }
        
        # Update statistics
        stats['successful_requests'] += 1
        stats['unique_tag_ids'].add(tag_id)
        
        # Log successful reception
        log_esp32_data(f"✅ ESP32 AprilTag Data: ID={tag_id}, Camera={camera_name}", "SUCCESS")
        
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
        'esp32_communication': latest_esp32_data['status'],
        'latest_data': latest_esp32_data,
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
            'test_connection': 'GET /test_esp32_connection'
        }
    })

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
    print(f"   ESP32 Endpoint: http://{display_ip}:{ESP32_SERVER_PORT}/esp32_apriltag_data")
    print(f"   Status Check: http://{display_ip}:{ESP32_SERVER_PORT}/esp32_status")
    
    if hotspot_ip and hotspot_ip != local_ip:
        print(f"\n💡 ESP32 Configuration:")
        print(f'   const char* TEST_SERVER_URL = "http://{hotspot_ip}:{ESP32_SERVER_PORT}/esp32_apriltag_data";')
    print("=" * 70)
    print("📋 Expected ESP32 JSON Format:")
    print('   {"id": 5, "camera_name": "OV2640", "timestamp": 1234567890}')
    print("=" * 70)
    print("🚀 Ready to receive ESP32 AprilTag data!")
    print("   - Make sure ESP32 is connected to WiFi")
    print("   - ESP32 should POST to /esp32_apriltag_data endpoint")
    print("   - View real-time data at web interface")
    if "5GHz" in wifi_band:
        print("   ⚠️  WARNING: Ubuntu on 5GHz! ESP32 needs 2.4GHz network!")
    print("=" * 70)

if __name__ == '__main__':
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
