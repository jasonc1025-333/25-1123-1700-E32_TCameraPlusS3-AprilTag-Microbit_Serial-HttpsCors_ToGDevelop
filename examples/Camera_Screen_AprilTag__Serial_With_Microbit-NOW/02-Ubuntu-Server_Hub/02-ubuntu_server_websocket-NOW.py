#!/usr/bin/env python3

"""
Pure WebSocket Server for ESP32 AprilTag & GDevelop Communication
Uses flask-sock for reliable WebSocket support

Architecture:
    ESP32 → WebSocket → Python Server → WebSocket → GDevelop
    
Features:
    - Reliable WebSocket using flask-sock
    - Real-time AprilTag data streaming
    - GDevelop native WebSocketClient compatible
    - Lower memory usage on ESP32
    - Simple JSON message format
    - Backward compatible HTTP endpoints
    
MIGRATION NOTE (25-1202-0340):
    - Switched from simple-websocket to flask-sock
    - simple-websocket had compatibility issues (400 errors)
    - flask-sock is more reliable and works with Flask 3.x
"""

from flask import Flask, request, jsonify, make_response
from flask_cors import CORS
from flask_sock import Sock
import time
import json
from datetime import datetime
import threading
import logging

# ============================================================================
# CONFIGURATION
# ============================================================================
SERVER_PORT = 5000
SERVER_HOST = '0.0.0.0'

# ============================================================================
# DATA STRUCTURES
# ============================================================================

class TagData:
    """AprilTag data structure"""
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

    def to_dict(self):
        return {
            'smartcam_ip': self.smartcam_ip,
            'tag_id': self.tag_id,
            'camera_name': self.camera_name,
            'yaw': self.yaw,
            'pitch': self.pitch,
            'roll': self.roll,
            'x_cm': self.x_cm,
            'y_cm': self.y_cm,
            'z_cm': self.z_cm,
            'tag_size_percent': self.tag_size_percent,
            'distance_cm': self.distance_cm,
            'timestamp': self.timestamp
        }

# ============================================================================
# GLOBAL STATE
# ============================================================================

# WebSocket connections (thread-safe)
websocket_clients = {
    'esp32': [],
    'gdevelop': [],
    'viewers': []
}
websocket_lock = threading.Lock()

# Latest data
latest_apriltag_data = None
latest_video_frame = None

# Legacy FIFO queue (HTTP compatibility)
tagData_List_Fifo = []
tagData_List_Fifo_Count_Base1 = 0
tagData_List_Fifo_Count_MAX_Base1 = 50
tagData_List_Fifo_Index_Head_Base0 = 0

# Statistics
stats = {
    'total_connections': 0,
    'active_connections': 0,
    'apriltag_events': 0,
    'video_frames': 0,
    'start_time': time.time(),
    'esp32_connected': False,
    'gdevelop_clients': 0
}

# ============================================================================
# FLASK APP SETUP
# ============================================================================

app = Flask(__name__)
CORS(app, origins="*", supports_credentials=True)
sock = Sock(app)  # Initialize flask-sock

# ============================================================================
# WEBSOCKET HANDLERS
# ============================================================================

def broadcast_to_gdevelop(message_dict):
    """Broadcast message to all GDevelop clients"""
    message_json = json.dumps(message_dict)
    
    with websocket_lock:
        disconnected = []
        for ws in websocket_clients['gdevelop']:
            try:
                ws.send(message_json)
            except Exception as e:
                print(f"❌ Error sending to GDevelop client: {e}")
                disconnected.append(ws)
        
        # Remove disconnected clients
        for ws in disconnected:
            websocket_clients['gdevelop'].remove(ws)
            stats['gdevelop_clients'] = len(websocket_clients['gdevelop'])

def handle_esp32_message(ws, message):
    """Handle message from ESP32"""
    global latest_apriltag_data
    
    try:
        data = json.loads(message)
        event = data.get('event', '')
        
        if event == 'identify':
            # ESP32 identifying itself
            with websocket_lock:
                if ws not in websocket_clients['esp32']:
                    websocket_clients['esp32'].append(ws)
                    stats['esp32_connected'] = True
            
            print(f"📹 ESP32 IDENTIFIED")
            ws.send(json.dumps({'event': 'identify_success', 'client_type': 'esp32'}))
            
        elif event == 'apriltag_data':
            # Received AprilTag data from ESP32
            payload = data.get('data', {})
            payload['server_timestamp'] = time.time()
            
            # Store as latest
            latest_apriltag_data = payload
            
            # Push to legacy FIFO
            tag_data = TagData()
            tag_data.smartcam_ip = payload.get('smartcam_ip', '')
            tag_data.tag_id = payload.get('tag_id', 0)
            tag_data.camera_name = payload.get('camera_name', '')
            tag_data.yaw = payload.get('yaw', 0.0)
            tag_data.pitch = payload.get('pitch', 0.0)
            tag_data.roll = payload.get('roll', 0.0)
            tag_data.x_cm = payload.get('x_cm', 0.0)
            tag_data.y_cm = payload.get('y_cm', 0.0)
            tag_data.z_cm = payload.get('z_cm', 0.0)
            tag_data.tag_size_percent = payload.get('tag_size_percent', 0.0)
            tag_data.distance_cm = payload.get('distance_cm', 0.0)
            tag_data.timestamp = payload['server_timestamp']
            AprilTag_List_Fifo_Push(tag_data)
            
            # Broadcast to GDevelop
            broadcast_message = {
                'event': 'apriltag_data',
                'data': payload
            }
            broadcast_to_gdevelop(broadcast_message)
            
            stats['apriltag_events'] += 1
            
            print(f"📡 AprilTag → GDevelop ({stats['gdevelop_clients']} clients): "
                  f"ID={payload.get('tag_id')}, "
                  f"Pos=({payload.get('x_cm', 0):.1f}, {payload.get('y_cm', 0):.1f}, {payload.get('z_cm', 0):.1f}) cm")
            
            # Acknowledge to ESP32
            ws.send(json.dumps({'event': 'apriltag_ack', 'status': 'received'}))
            
        elif event == 'video_frame':
            # Received video frame
            stats['video_frames'] += 1
            print(f"📹 Video frame received (#{stats['video_frames']})")
            
        elif event == 'ping':
            ws.send(json.dumps({'event': 'pong', 'timestamp': time.time()}))
            
    except json.JSONDecodeError:
        print(f"❌ Invalid JSON from ESP32: {message[:100]}")
    except Exception as e:
        print(f"❌ Error handling ESP32 message: {e}")

def handle_gdevelop_message(ws, message):
    """Handle message from GDevelop"""
    try:
        data = json.loads(message)
        event = data.get('event', '')
        
        if event == 'identify':
            # GDevelop identifying itself
            with websocket_lock:
                if ws not in websocket_clients['gdevelop']:
                    websocket_clients['gdevelop'].append(ws)
                    stats['gdevelop_clients'] = len(websocket_clients['gdevelop'])
            
            print(f"🎮 GDevelop IDENTIFIED (Total: {stats['gdevelop_clients']})")
            ws.send(json.dumps({'event': 'identify_success', 'client_type': 'gdevelop'}))
            
            # Send latest data if available
            if latest_apriltag_data:
                ws.send(json.dumps({'event': 'apriltag_data', 'data': latest_apriltag_data}))
                
        elif event == 'request_latest_data':
            # GDevelop requesting latest data
            if latest_apriltag_data:
                ws.send(json.dumps({'event': 'apriltag_data', 'data': latest_apriltag_data}))
            else:
                ws.send(json.dumps({'event': 'no_data_available'}))
                
        elif event == 'ping':
            ws.send(json.dumps({'event': 'pong', 'timestamp': time.time()}))
            
    except json.JSONDecodeError:
        print(f"❌ Invalid JSON from GDevelop: {message[:100]}")
    except Exception as e:
        print(f"❌ Error handling GDevelop message: {e}")

@sock.route('/websocket')
def websocket(ws):
    """WebSocket endpoint for all clients using flask-sock"""
    stats['total_connections'] += 1
    stats['active_connections'] += 1
    
    print(f"\n{'='*70}")
    print(f"🔌 WebSocket CONNECTED (Total: {stats['active_connections']})")
    print(f"{'='*70}\n")
    
    # Send welcome message
    ws.send(json.dumps({
        'event': 'connection_success',
        'message': 'WebSocket connected - send identify event'
    }))
    
    client_type = 'unknown'
    
    try:
        while True:
            message = ws.receive()
            if message is None:
                break
            
            # Determine client type from first message
            try:
                data = json.loads(message)
                if data.get('event') == 'identify':
                    client_type = data.get('data', {}).get('type', 'unknown')
            except:
                pass
            
            # Route message based on client type
            if client_type == 'esp32' or ws in websocket_clients['esp32']:
                handle_esp32_message(ws, message)
            elif client_type == 'gdevelop' or ws in websocket_clients['gdevelop']:
                handle_gdevelop_message(ws, message)
            else:
                # Try to parse and route
                try:
                    data = json.loads(message)
                    event = data.get('event', '')
                    if event in ['apriltag_data', 'video_frame']:
                        handle_esp32_message(ws, message)
                    else:
                        handle_gdevelop_message(ws, message)
                except:
                    print(f"⚠️  Unknown message: {message[:100]}")
                    
    except Exception as e:
        print(f"❌ WebSocket error: {e}")
    finally:
        # Cleanup
        stats['active_connections'] -= 1
        
        with websocket_lock:
            for client_list_type in websocket_clients:
                if ws in websocket_clients[client_list_type]:
                    websocket_clients[client_list_type].remove(ws)
                    if client_list_type == 'esp32':
                        stats['esp32_connected'] = False
                    elif client_list_type == 'gdevelop':
                        stats['gdevelop_clients'] = len(websocket_clients['gdevelop'])
        
        print(f"🔌 Client disconnected (Active: {stats['active_connections']})")

# ============================================================================
# LEGACY HTTP ENDPOINTS
# ============================================================================

def AprilTag_List_Fifo_Push(tag_data):
    """Add tag data to FIFO queue"""
    global tagData_List_Fifo_Index_Head_Base0, tagData_List_Fifo_Count_Base1
    
    if tagData_List_Fifo_Count_Base1 < tagData_List_Fifo_Count_MAX_Base1:
        tagData_List_Fifo.append(tag_data)
        tagData_List_Fifo_Count_Base1 += 1
    else:
        tagData_List_Fifo[tagData_List_Fifo_Index_Head_Base0] = tag_data
        tagData_List_Fifo_Index_Head_Base0 = (tagData_List_Fifo_Index_Head_Base0 + 1) % tagData_List_Fifo_Count_MAX_Base1

def AprilTag_List_Fifo_Pop():
    """Remove and return oldest tag data"""
    global tagData_List_Fifo_Index_Head_Base0, tagData_List_Fifo_Count_Base1
    
    if tagData_List_Fifo_Count_Base1 == 0:
        return None
    
    if len(tagData_List_Fifo) < tagData_List_Fifo_Count_MAX_Base1:
        tag_data = tagData_List_Fifo.pop(0)
        tagData_List_Fifo_Count_Base1 -= 1
    else:
        tag_data = tagData_List_Fifo[tagData_List_Fifo_Index_Head_Base0]
        tagData_List_Fifo_Index_Head_Base0 = (tagData_List_Fifo_Index_Head_Base0 + 1) % tagData_List_Fifo_Count_MAX_Base1
        tagData_List_Fifo_Count_Base1 -= 1
    
    return tag_data

@app.route('/client_gdevelop_to_server__smartcam_data_get', methods=['GET', 'OPTIONS'])
def serve_gdevelop_data_http():
    """Legacy HTTP GET endpoint"""
    if request.method == 'OPTIONS':
        response = make_response('', 200)
        response.headers['Access-Control-Allow-Origin'] = '*'
        response.headers['Access-Control-Allow-Methods'] = 'GET, OPTIONS'
        response.headers['Access-Control-Allow-Headers'] = 'Content-Type'
        return response
    
    tag_data = AprilTag_List_Fifo_Pop()
    
    if tag_data is None:
        return jsonify({
            'smartcam_ip': '', 'tag_id': -1, 'camera_name': '',
            'yaw': 0.0, 'pitch': 0.0, 'roll': 0.0,
            'x_cm': 0.0, 'y_cm': 0.0, 'z_cm': 0.0,
            'tag_size_percent': 0.0, 'distance_cm': 0.0,
            'timestamp': 0, 'list_remaining': 0
        })
    
    response = make_response(jsonify(tag_data.to_dict()))
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response

@app.route('/')
def home():
    """Status web page"""
    uptime_min = int((time.time() - stats['start_time']) / 60)
    
    html = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>flask-sock WebSocket Server</title>
        <meta http-equiv="refresh" content="5">
        <style>
            body {{ font-family: Arial; margin: 20px; background: #2c3e50; color: white; }}
            .container {{ max-width: 1200px; margin: 0 auto; }}
            .header {{ background: #34495e; padding: 20px; border-radius: 10px; margin-bottom: 20px; }}
            .status-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }}
            .status-card {{ background: #34495e; padding: 20px; border-radius: 10px; text-align: center; }}
            .status-value {{ font-size: 2.5em; font-weight: bold; color: #3498db; }}
            .status-label {{ color: #bdc3c7; margin-top: 10px; }}
            .connected {{ color: #27ae60; }}
            .disconnected {{ color: #e74c3c; }}
            code {{ background: #2c3e50; padding: 5px; border-radius: 3px; }}
        </style>
    </head>
    <body>
        <div class="container">
            <div class="header">
                <h1>🚀 flask-sock WebSocket Server</h1>
                <p>Reliable WebSocket with Flask 3.x</p>
            </div>
            
            <div class="status-grid">
                <div class="status-card">
                    <div class="status-value {'connected' if stats['esp32_connected'] else 'disconnected'}">
                        {'✅' if stats['esp32_connected'] else '❌'}
                    </div>
                    <div class="status-label">ESP32</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{stats['gdevelop_clients']}</div>
                    <div class="status-label">GDevelop</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{stats['active_connections']}</div>
                    <div class="status-label">Connections</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{stats['apriltag_events']}</div>
                    <div class="status-label">AprilTag Events</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{uptime_min}</div>
                    <div class="status-label">Uptime (min)</div>
                </div>
            </div>
            
            <div style="background: #34495e; padding: 20px; border-radius: 10px; margin-top: 20px;">
                <h3>📡 WebSocket Endpoint</h3>
                <p><code>ws://localhost:{SERVER_PORT}/websocket</code></p>
                <p><code>wss://your-ngrok.ngrok-free.app/websocket</code></p>
                
                <h3>📨 Message Format (JSON)</h3>
                <pre style="background: #2c3e50; padding: 10px; border-radius: 5px;">
// Identify as ESP32
{{"event": "identify", "data": {{"type": "esp32"}}}}

// Send AprilTag data
{{"event": "apriltag_data", "data": {{"tag_id": 5, "x_cm": 10.5, ...}}}}

// Identify as GDevelop
{{"event": "identify", "data": {{"type": "gdevelop"}}}}
                </pre>
            </div>
        </div>
    </body>
    </html>
    """
    return html

@app.route('/status')
def status():
    """JSON status"""
    return jsonify({
        'server': 'online',
        'protocol': 'flask_sock_websocket',
        'uptime_seconds': int(time.time() - stats['start_time']),
        'websocket': {
            'active_connections': stats['active_connections'],
            'esp32_connected': stats['esp32_connected'],
            'gdevelop_clients': stats['gdevelop_clients']
        }
    })

# ============================================================================
# MAIN
# ============================================================================

def print_startup_info():
    """Print startup information"""
    import socket
    
    hostname = socket.gethostname()
    local_ip = socket.gethostbyname(hostname)
    
    print("\n" + "="*70)
    print("🚀 flask-sock WebSocket Server Starting...")
    print("="*70)
    print(f"🖥️  Hostname: {hostname}")
    print(f"🌐 Local IP: {local_ip}")
    print(f"🔌 Port: {SERVER_PORT}")
    print("="*70)
    print("📡 WebSocket Endpoint:")
    print(f"   ws://{local_ip}:{SERVER_PORT}/websocket")
    print(f"   wss://your-ngrok.ngrok-free.app/websocket")
    print("="*70)
    print("📨 Message Format: Standard JSON")
    print('   {{"event": "identify", "data": {{"type": "esp32"}}}}')
    print('   {{"event": "apriltag_data", "data": {{...}}}}')
    print("="*70)
    print("💡 Usage:")
    print("   1. pip install -r 0a-requirements.txt")
    print("   2. Start ngrok: ngrok http 5000")
    print("   3. Connect ESP32 to ws://[IP]/websocket")
    print("   4. Connect GDevelop to ws://[IP]/websocket")
    print("="*70)
    print("✅ Server ready!")
    print("="*70 + "\n")

if __name__ == '__main__':
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)
    
    print_startup_info()
    
    try:
        app.run(host=SERVER_HOST, port=SERVER_PORT, debug=False)
    except KeyboardInterrupt:
        print("\n🛑 Server stopped")
    except Exception as e:
        print(f"❌ Error: {e}")
