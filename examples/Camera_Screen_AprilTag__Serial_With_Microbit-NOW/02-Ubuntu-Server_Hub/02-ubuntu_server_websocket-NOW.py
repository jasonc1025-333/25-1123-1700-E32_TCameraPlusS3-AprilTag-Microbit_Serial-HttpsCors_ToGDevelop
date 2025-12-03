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
import re

# ============================================================================
# CONFIGURATION
# ============================================================================
SERVER_PORT = 5000
SERVER_HOST = '0.0.0.0'

# ============================================================================
# SECURITY CONFIGURATION
# ============================================================================
# Authentication token - MUST MATCH ESP32's AUTH_TOKEN
# Generate new token: python3 -c "import secrets; print(secrets.token_urlsafe(32))"
#### jwc 25-1202-1120 o AUTH_TOKEN = "your_secret_token_change_this_12345"  # TODO: Change this!
AUTH_TOKEN = "Jesus333!!!"  # jwc 25-1202-1120 Matches ESP32

# WARNING: This token protects against unauthorized access, but data is still
# transmitted in plain text over ws://. For production, use VPN or wss://
# SECURITY NOTE: Change this to a unique random token for production!

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
# UTILITY FUNCTIONS
# ============================================================================

def fix_json_quotes(message):
    """
    Fix JSON with single quotes to use double quotes.
    GDevelop may send JSON with single quotes, which is invalid JSON.
    This converts it to valid JSON format.
    """
    # Simply replace single quotes with double quotes
    # This handles the common case where GDevelop sends {'key':'value'}
    # and converts it to {"key":"value"}
    fixed = message.replace("'", '"')
    return fixed

# ============================================================================
# SECURITY FUNCTIONS
# ============================================================================

def validate_auth_token(data):
    """
    Validate authentication token from client
    
    Returns: (is_valid, error_message)
    """
    token = data.get('data', {}).get('auth_token', '')
    
    if not token:
        return (False, 'Missing auth_token')
    
    if token != AUTH_TOKEN:
        return (False, 'Invalid auth_token')
    
    return (True, '')

# ============================================================================
# WEBSOCKET HANDLERS
# ============================================================================

def broadcast_to_gdevelop(message_dict):
    """Broadcast message to all GDevelop clients"""
    message_json = json.dumps(message_dict)
    
    # DEBUG: Print what we're broadcasting
    print(f"🔊 BROADCAST to GDevelop ({len(websocket_clients['gdevelop'])} clients):")
    print(f"   Event: {message_dict.get('event', 'unknown')}")
    if message_dict.get('event') == 'apriltag_data':
        data = message_dict.get('data', {})
        print(f"   Data: tag_id={data.get('tag_id')}, x={data.get('x_cm'):.1f}, y={data.get('y_cm'):.1f}, z={data.get('z_cm'):.1f}")
    print(f"   Full JSON: {message_json[:200]}..." if len(message_json) > 200 else f"   Full JSON: {message_json}")
    
    with websocket_lock:
        disconnected = []
        for ws in websocket_clients['gdevelop']:
            try:
                ws.send(message_json)
                print(f"   ✅ Sent to GDevelop client")
            except Exception as e:
                print(f"   ❌ Error sending to GDevelop client: {e}")
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
            # ESP32 identifying itself - validate auth token
            is_valid, error_msg = validate_auth_token(data)
            
            if not is_valid:
                auth_fail_msg = {
                    'event': 'auth_failed',
                    'error': error_msg,
                    'message': 'Authentication required - check your auth_token'
                }
                print(f"❌ ESP32 AUTH FAILED: {error_msg}")
                print(f"📤 SEND to ESP32: {json.dumps(auth_fail_msg)}")
                ws.send(json.dumps(auth_fail_msg))
                return  # Don't add to clients list
            
            with websocket_lock:
                if ws not in websocket_clients['esp32']:
                    websocket_clients['esp32'].append(ws)
                    stats['esp32_connected'] = True
            
            esp32_name = data.get('data', {}).get('camera_name', 'ESP32-Unknown')
            
            identify_success_msg = {'event': 'identify_success', 'client_type': 'esp32'}
            print(f"\n{'='*70}")
            print(f"📹 ESP32 CLIENT IDENTIFIED ✅")
            print(f"{'='*70}")
            print(f" Camera Name: {esp32_name}")
            print(f"🔐 Authentication: PASSED")
            print(f"📊 ESP32 Clients: {len(websocket_clients['esp32'])}")
            print(f"{'='*70}\n")
            print(f"📤 SEND to ESP32: {json.dumps(identify_success_msg)}")
            ws.send(json.dumps(identify_success_msg))
            
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
            ack_msg = {'event': 'apriltag_ack', 'status': 'received'}
            print(f"📤 SEND to ESP32 (ACK): {json.dumps(ack_msg)}")
            ws.send(json.dumps(ack_msg))
            
        elif event == 'video_frame':
            # Received video frame
            stats['video_frames'] += 1
            print(f"📹 Video frame received (#{stats['video_frames']})")
            
        elif event == 'ping':
            pong_msg = {'event': 'pong', 'timestamp': time.time()}
            print(f"📤 SEND to ESP32 (PONG): {json.dumps(pong_msg)}")
            ws.send(json.dumps(pong_msg))
            
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
            
            gdevelop_name = data.get('data', {}).get('name', 'GDevelop-Client')
            
            identify_success_msg = {'event': 'identify_success', 'client_type': 'gdevelop'}
            print(f"\n{'='*70}")
            print(f"🎮 GDEVELOP CLIENT IDENTIFIED ✅")
            print(f"{'='*70}")
            print(f"🎮 Client Name: {gdevelop_name}")
            print(f"📊 GDevelop Clients: {stats['gdevelop_clients']}")
            print(f"{'='*70}\n")
            print(f"📤 SEND to GDevelop: {json.dumps(identify_success_msg)}")
            ws.send(json.dumps(identify_success_msg))
            
            # Send latest data if available
            if latest_apriltag_data:
                latest_data_msg = {'event': 'apriltag_data', 'data': latest_apriltag_data}
                print(f"📤 SEND to GDevelop (Latest Data): {json.dumps(latest_data_msg)[:200]}...")
                ws.send(json.dumps(latest_data_msg))
                
        elif event == 'request_latest_data':
            # GDevelop requesting latest data
            if latest_apriltag_data:
                latest_data_msg = {'event': 'apriltag_data', 'data': latest_apriltag_data}
                print(f"📤 SEND to GDevelop (Requested Data): {json.dumps(latest_data_msg)[:200]}...")
                ws.send(json.dumps(latest_data_msg))
            else:
                no_data_msg = {'event': 'no_data_available'}
                print(f"📤 SEND to GDevelop: {json.dumps(no_data_msg)}")
                ws.send(json.dumps(no_data_msg))
                
        elif event == 'ping':
            pong_msg = {'event': 'pong', 'timestamp': time.time()}
            print(f"📤 SEND to GDevelop (PONG): {json.dumps(pong_msg)}")
            ws.send(json.dumps(pong_msg))
            
    except json.JSONDecodeError:
        print(f"❌ Invalid JSON from GDevelop: {message[:100]}")
    except Exception as e:
        print(f"❌ Error handling GDevelop message: {e}")

@sock.route('/websocket')
def websocket(ws):
    """WebSocket endpoint for all clients using flask-sock"""
    stats['total_connections'] += 1
    stats['active_connections'] += 1
    
    # Get client connection info
    client_ip = request.remote_addr
    client_user_agent = request.headers.get('User-Agent', 'Unknown')
    client_origin = request.headers.get('Origin', 'Unknown')
    client_host = request.headers.get('Host', 'Unknown')
    
    print(f"\n{'='*70}")
    print(f"🔌 NEW WebSocket CONNECTION")
    print(f"{'='*70}")
    print(f"📍 Client IP: {client_ip}")
    print(f"🌐 Origin: {client_origin}")
    print(f"🖥️  Host: {client_host}")
    print(f"🔍 User-Agent: {client_user_agent[:80]}..." if len(client_user_agent) > 80 else f"🔍 User-Agent: {client_user_agent}")
    print(f"📊 Total Connections: {stats['total_connections']}")
    print(f"📊 Active Connections: {stats['active_connections']}")
    print(f"{'='*70}\n")
    
    # Send welcome message
    welcome_msg = {
        'event': 'connection_success',
        'message': 'WebSocket connected - send identify event'
    }
    print(f"📤 SEND Welcome to {client_ip}: {json.dumps(welcome_msg)}")
    ws.send(json.dumps(welcome_msg))
    
    client_type = 'unknown'
    client_identifier = f"{client_ip}:unknown"
    
    try:
        while True:
            message = ws.receive()
            if message is None:
                break
            
            # Determine client type from first message
            try:
                # Fix single quotes to double quotes if needed
                fixed_message = fix_json_quotes(message)
                data = json.loads(fixed_message)
                if data.get('event') == 'identify':
                    client_type = data.get('data', {}).get('type', 'unknown')
            except:
                pass
            
            # Fix single quotes to double quotes if needed (do this once for all paths)
            fixed_message = fix_json_quotes(message)
            
            # Route message based on client type
            if client_type == 'esp32' or ws in websocket_clients['esp32']:
                handle_esp32_message(ws, fixed_message)
            elif client_type == 'gdevelop' or ws in websocket_clients['gdevelop']:
                handle_gdevelop_message(ws, fixed_message)
            else:
                # Try to parse and route (fixed_message already created above)
                try:
                    data = json.loads(fixed_message)
                    event = data.get('event', '')
                    
                    # Auto-detect and register GDevelop clients based on event patterns
                    if event in ['request_latest_data', 'event_FromGDevelop'] or (event not in ['apriltag_data', 'video_frame', 'identify']):
                        # This looks like a GDevelop client - auto-register it
                        if ws not in websocket_clients['gdevelop']:
                            with websocket_lock:
                                websocket_clients['gdevelop'].append(ws)
                                stats['gdevelop_clients'] = len(websocket_clients['gdevelop'])
                            
                            print(f"\n{'='*70}")
                            print(f"🎮 AUTO-DETECTED GDevelop CLIENT")
                            print(f"{'='*70}")
                            print(f"📍 Client IP: {client_ip}")
                            print(f"📨 First Event: {event}")
                            print(f"📊 GDevelop Clients: {stats['gdevelop_clients']}")
                            print(f"{'='*70}\n")
                        
                        client_type = 'gdevelop'
                        handle_gdevelop_message(ws, fixed_message)
                    elif event in ['apriltag_data', 'video_frame']:
                        handle_esp32_message(ws, fixed_message)
                    else:
                        handle_gdevelop_message(ws, fixed_message)
                except Exception as e:
                    print(f"⚠️  Unknown message (parse error): {message[:100]}")
                    print(f"    Error: {e}")
                    print(f"    Attempted fix: {fix_json_quotes(message)[:100]}")
                    
    except Exception as e:
        print(f"❌ WebSocket error: {e}")
    finally:
        # Cleanup
        stats['active_connections'] -= 1
        
        disconnected_type = 'Unknown'
        with websocket_lock:
            for client_list_type in websocket_clients:
                if ws in websocket_clients[client_list_type]:
                    websocket_clients[client_list_type].remove(ws)
                    disconnected_type = client_list_type
                    if client_list_type == 'esp32':
                        stats['esp32_connected'] = False
                    elif client_list_type == 'gdevelop':
                        stats['gdevelop_clients'] = len(websocket_clients['gdevelop'])
        
        print(f"\n{'='*70}")
        print(f"❌ CLIENT DISCONNECTED")
        print(f"{'='*70}")
        print(f"📍 Client IP: {client_ip}")
        print(f"🏷️  Client Type: {disconnected_type.upper()}")
        print(f"🆔 Identifier: {client_identifier}")
        print(f"📊 Remaining Active: {stats['active_connections']}")
        print(f"📊 ESP32: {len(websocket_clients['esp32'])}, GDevelop: {stats['gdevelop_clients']}")
        print(f"{'='*70}\n")

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
    print("🌐 HTTP Endpoints:")
    print(f"   http://{local_ip}:{SERVER_PORT}/")
    print(f"      └─ Server status dashboard (auto-refresh)")
    print(f"   http://{local_ip}:{SERVER_PORT}/status")
    print(f"      └─ JSON status API")
    print(f"   http://{local_ip}:{SERVER_PORT}/client_gdevelop_to_server__smartcam_data_get")
    print(f"      └─ Legacy HTTP polling endpoint for GDevelop")
    print("="*70)
    print("📨 WebSocket Message Format (JSON):")
    print('   ESP32 Identify:')
    print('      {{"event": "identify", "data": {{"type": "esp32", "auth_token": "..."}}}}')
    print('   GDevelop Identify:')
    print('      {{"event": "identify", "data": {{"type": "gdevelop"}}}}')
    print('   AprilTag Data:')
    print('      {{"event": "apriltag_data", "data": {{"tag_id": 5, ...}}}}')
    print('   Request Latest:')
    print('      {{"event": "request_latest_data"}}')
    print("="*70)
    print("💡 Quick Start:")
    print("   1. pip install -r 0a-requirements.txt")
    print("   2. (Optional) Start ngrok: ngrok http 5000")
    print("   3. Connect ESP32 to ws://[IP]:5000/websocket")
    print("   4. Connect GDevelop to ws://[IP]:5000/websocket")
    print(f"   5. View dashboard: http://{local_ip}:{SERVER_PORT}/")
    print("="*70)
    print("✅ Server ready and listening!")
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
