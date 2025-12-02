#!/usr/bin/env python3

"""
WebSocket Server for ESP32 AprilTag & GDevelop Communication
Migration from HTTP polling to real-time WebSocket communication

Architecture:
    ESP32 → WebSocket → Python Server → WebSocket → GDevelop
    
Features:
    - Real-time AprilTag data streaming (no polling!)
    - Video frame streaming via WebSocket
    - Backward compatible HTTP endpoints
    - CORS-enabled for GDevelop access
    - ngrok WebSocket Secure (WSS) support
    - Auto-reconnection handling
"""

from flask import Flask, request, jsonify, render_template_string, make_response
from flask_cors import CORS
from flask_socketio import SocketIO, emit, join_room, leave_room
import time
import json
from datetime import datetime
import threading
import logging
import base64

# ============================================================================
# CONFIGURATION
# ============================================================================
SERVER_PORT = 5000
SERVER_HOST = '0.0.0.0'  # Listen on all interfaces

# Video streaming settings
VIDEO_QUALITY = 10  # JPEG quality 1-100 (10 = low quality for speed)
VIDEO_WIDTH = 240
VIDEO_HEIGHT = 240

# ============================================================================
# DATA STRUCTURES
# ============================================================================

class TagData:
    """AprilTag data structure matching ESP32 format"""
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
        """Convert to dictionary for JSON serialization"""
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

# Legacy FIFO queue (keep for HTTP compatibility)
tagData_List_Fifo = []
tagData_List_Fifo_Count_Base1 = 0
tagData_List_Fifo_Count_MAX_Base1 = 50
tagData_List_Fifo_Index_Head_Base0 = 0

# WebSocket state
connected_clients = {
    'esp32': [],      # ESP32 camera devices
    'gdevelop': [],   # GDevelop game clients
    'viewers': []     # Web viewers
}

# Latest data for new clients
latest_apriltag_data = None
latest_video_frame = None
video_frame_lock = threading.Lock()

# Statistics
stats = {
    'total_ws_connections': 0,
    'active_ws_connections': 0,
    'apriltag_events_sent': 0,
    'video_frames_sent': 0,
    'start_time': time.time(),
    'esp32_connected': False,
    'gdevelop_clients': 0
}

# ============================================================================
# FLASK & SOCKETIO SETUP
# ============================================================================

app = Flask(__name__)
app.config['SECRET_KEY'] = 'your-secret-key-here'

# Enable CORS for all routes (WebSocket & HTTP)
CORS(app, origins="*", supports_credentials=True)

# Initialize SocketIO with CORS support
socketio = SocketIO(
    app,
    cors_allowed_origins="*",  # Allow all origins (including ngrok)
    async_mode='threading',
    logger=False,
    engineio_logger=False,
    ping_timeout=60,
    ping_interval=25
)

# ============================================================================
# WEBSOCKET EVENT HANDLERS
# ============================================================================

@socketio.on('connect')
def handle_connect():
    """Handle client connection"""
    client_id = request.sid
    stats['total_ws_connections'] += 1
    stats['active_ws_connections'] += 1
    
    print(f"\n{'='*70}")
    print(f"🔌 WebSocket CONNECTED: {client_id}")
    print(f"   Active connections: {stats['active_ws_connections']}")
    print(f"{'='*70}\n")
    
    # Send welcome message
    emit('connection_success', {
        'status': 'connected',
        'client_id': client_id,
        'server_time': time.time(),
        'message': 'WebSocket connection established'
    })

@socketio.on('disconnect')
def handle_disconnect():
    """Handle client disconnection"""
    client_id = request.sid
    stats['active_ws_connections'] -= 1
    
    # Remove from all client lists
    for client_type in connected_clients:
        if client_id in connected_clients[client_type]:
            connected_clients[client_type].remove(client_id)
            print(f"🔌 {client_type.upper()} disconnected: {client_id}")
    
    # Update ESP32 status
    if client_id in connected_clients['esp32']:
        stats['esp32_connected'] = False
    
    print(f"\n{'='*70}")
    print(f"🔌 WebSocket DISCONNECTED: {client_id}")
    print(f"   Active connections: {stats['active_ws_connections']}")
    print(f"{'='*70}\n")

@socketio.on('identify')
def handle_identify(data):
    """Client identifies itself (esp32, gdevelop, or viewer)"""
    client_id = request.sid
    client_type = data.get('type', 'unknown')
    
    if client_type in connected_clients:
        connected_clients[client_type].append(client_id)
        
        if client_type == 'esp32':
            stats['esp32_connected'] = True
            print(f"📹 ESP32 IDENTIFIED: {client_id}")
            
            # Send latest video frame to ESP32 if available
            if latest_video_frame:
                emit('latest_video_frame', {'frame': latest_video_frame})
                
        elif client_type == 'gdevelop':
            stats['gdevelop_clients'] = len(connected_clients['gdevelop'])
            print(f"🎮 GDevelop IDENTIFIED: {client_id} (Total: {stats['gdevelop_clients']})")
            
            # Send latest AprilTag data to new GDevelop client
            if latest_apriltag_data:
                emit('apriltag_data', latest_apriltag_data)
        
        emit('identify_success', {
            'client_id': client_id,
            'client_type': client_type,
            'message': f'Identified as {client_type}'
        })
    else:
        emit('identify_error', {
            'message': f'Unknown client type: {client_type}'
        })

@socketio.on('apriltag_data')
def handle_apriltag_data(data):
    """Receive AprilTag data from ESP32 and broadcast to GDevelop clients"""
    global latest_apriltag_data
    
    client_id = request.sid
    
    try:
        # Add server receive timestamp
        data['server_timestamp'] = time.time()
        
        # Store as latest
        latest_apriltag_data = data
        
        # Also push to legacy FIFO queue (for HTTP compatibility)
        tag_data = TagData()
        tag_data.smartcam_ip = data.get('smartcam_ip', '')
        tag_data.tag_id = data.get('tag_id', 0)
        tag_data.camera_name = data.get('camera_name', '')
        tag_data.yaw = data.get('yaw', 0.0)
        tag_data.pitch = data.get('pitch', 0.0)
        tag_data.roll = data.get('roll', 0.0)
        tag_data.x_cm = data.get('x_cm', 0.0)
        tag_data.y_cm = data.get('y_cm', 0.0)
        tag_data.z_cm = data.get('z_cm', 0.0)
        tag_data.tag_size_percent = data.get('tag_size_percent', 0.0)
        tag_data.distance_cm = data.get('distance_cm', 0.0)
        tag_data.timestamp = data['server_timestamp']
        AprilTag_List_Fifo_Push(tag_data)
        
        # Broadcast to ALL GDevelop clients (real-time push!)
        stats['apriltag_events_sent'] += len(connected_clients['gdevelop'])
        emit('apriltag_data', data, broadcast=True, include_self=False)
        
        print(f"📡 AprilTag Data → GDevelop ({len(connected_clients['gdevelop'])} clients): "
              f"ID={data.get('tag_id')}, "
              f"Pos=({data.get('x_cm', 0):.1f}, {data.get('y_cm', 0):.1f}, {data.get('z_cm', 0):.1f}) cm")
        
        # Send acknowledgment to ESP32
        emit('apriltag_ack', {
            'status': 'received',
            'broadcast_count': len(connected_clients['gdevelop'])
        })
        
    except Exception as e:
        print(f"❌ Error handling AprilTag data: {e}")
        emit('apriltag_error', {'error': str(e)})

@socketio.on('video_frame')
def handle_video_frame(data):
    """Receive video frame from ESP32 and store for viewers"""
    global latest_video_frame
    
    client_id = request.sid
    
    try:
        # Extract frame data (base64 encoded JPEG)
        frame_data = data.get('frame', '')
        
        if not frame_data:
            print("⚠️  Received empty video frame")
            return
        
        # Store frame with thread safety
        with video_frame_lock:
            latest_video_frame = frame_data
            stats['video_frames_sent'] += 1
        
        # Broadcast to viewers (optional - for real-time viewing)
        emit('video_frame', {'frame': frame_data}, 
             room='viewers', broadcast=True, include_self=False)
        
        print(f"📹 Video Frame → {len(connected_clients['viewers'])} viewers "
              f"(Frame #{stats['video_frames_sent']})")
        
    except Exception as e:
        print(f"❌ Error handling video frame: {e}")

@socketio.on('request_latest_data')
def handle_request_latest_data():
    """GDevelop client requests latest AprilTag data"""
    if latest_apriltag_data:
        emit('apriltag_data', latest_apriltag_data)
    else:
        emit('no_data_available', {'message': 'No AprilTag data available yet'})

@socketio.on('ping')
def handle_ping(data):
    """Handle ping/pong for connection health check"""
    emit('pong', {'timestamp': time.time()})

# ============================================================================
# LEGACY HTTP ENDPOINTS (Backward Compatibility)
# ============================================================================

def AprilTag_List_Fifo_Push(tag_data):
    """Add tag data to circular list (FIFO with oldest drop on overflow)"""
    global tagData_List_Fifo_Index_Head_Base0, tagData_List_Fifo_Count_Base1
    
    if tagData_List_Fifo_Count_Base1 < tagData_List_Fifo_Count_MAX_Base1:
        tagData_List_Fifo.append(tag_data)
        tagData_List_Fifo_Count_Base1 += 1
    else:
        tagData_List_Fifo[tagData_List_Fifo_Index_Head_Base0] = tag_data
        tagData_List_Fifo_Index_Head_Base0 = (tagData_List_Fifo_Index_Head_Base0 + 1) % tagData_List_Fifo_Count_MAX_Base1

def AprilTag_List_Fifo_Pop():
    """Remove and return oldest tag data from list"""
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
    """Legacy HTTP GET endpoint for GDevelop (backward compatibility)"""
    if request.method == 'OPTIONS':
        response = make_response('', 200)
        response.headers['Access-Control-Allow-Origin'] = '*'
        response.headers['Access-Control-Allow-Methods'] = 'GET, OPTIONS'
        response.headers['Access-Control-Allow-Headers'] = 'Content-Type'
        return response
    
    tag_data = AprilTag_List_Fifo_Pop()
    
    if tag_data is None:
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
    
    response = make_response(jsonify(tag_data.to_dict()))
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response

@app.route('/')
def home():
    """Web interface showing system status"""
    uptime_minutes = int((time.time() - stats['start_time']) / 60)
    
    html = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>WebSocket Server Status</title>
        <meta http-equiv="refresh" content="5">
        <style>
            body {{ font-family: Arial; margin: 20px; background: #2c3e50; color: white; }}
            .container {{ max-width: 1200px; margin: 0 auto; }}
            .header {{ background: #34495e; padding: 20px; border-radius: 10px; margin-bottom: 20px; }}
            .status-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 15px; margin-bottom: 20px; }}
            .status-card {{ background: #34495e; padding: 20px; border-radius: 10px; text-align: center; }}
            .status-value {{ font-size: 2.5em; font-weight: bold; color: #3498db; }}
            .status-label {{ color: #bdc3c7; margin-top: 10px; }}
            .connected {{ color: #27ae60; }}
            .disconnected {{ color: #e74c3c; }}
            .info {{ background: #34495e; padding: 20px; border-radius: 10px; margin-bottom: 20px; }}
            code {{ background: #2c3e50; padding: 5px; border-radius: 3px; }}
        </style>
    </head>
    <body>
        <div class="container">
            <div class="header">
                <h1>🚀 WebSocket Server Status</h1>
                <p>Real-time ESP32 ↔ GDevelop Communication</p>
            </div>
            
            <div class="status-grid">
                <div class="status-card">
                    <div class="status-value {'connected' if stats['esp32_connected'] else 'disconnected'}">
                        {'✅' if stats['esp32_connected'] else '❌'}
                    </div>
                    <div class="status-label">ESP32 Status</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{stats['gdevelop_clients']}</div>
                    <div class="status-label">GDevelop Clients</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{stats['active_ws_connections']}</div>
                    <div class="status-label">Active Connections</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{stats['apriltag_events_sent']}</div>
                    <div class="status-label">AprilTag Events</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{stats['video_frames_sent']}</div>
                    <div class="status-label">Video Frames</div>
                </div>
                
                <div class="status-card">
                    <div class="status-value">{uptime_minutes}</div>
                    <div class="status-label">Uptime (min)</div>
                </div>
            </div>
            
            <div class="info">
                <h3>📡 WebSocket Endpoints</h3>
                <p><strong>Server:</strong> <code>ws://localhost:{SERVER_PORT}</code></p>
                <p><strong>ngrok (WSS):</strong> <code>wss://your-domain.ngrok-free.app/socket.io/</code></p>
                <p><strong>Events:</strong></p>
                <ul>
                    <li><code>connect</code> - Client connects</li>
                    <li><code>identify</code> - Client identifies (esp32/gdevelop/viewer)</li>
                    <li><code>apriltag_data</code> - ESP32 → Server → GDevelop</li>
                    <li><code>video_frame</code> - ESP32 → Server → Viewers</li>
                    <li><code>request_latest_data</code> - Get latest data</li>
                </ul>
            </div>
            
            <div class="info">
                <h3>🔄 Migration Status</h3>
                <p><strong>HTTP Endpoints:</strong> Still active for backward compatibility</p>
                <p><strong>WebSocket:</strong> {'🟢 Active' if stats['active_ws_connections'] > 0 else '🔴 No connections'}</p>
                <p><strong>Legacy HTTP GET:</strong> <code>/client_gdevelop_to_server__smartcam_data_get</code></p>
            </div>
        </div>
    </body>
    </html>
    """
    return html

@app.route('/status')
def status():
    """JSON status endpoint"""
    return jsonify({
        'server': 'online',
        'uptime_seconds': int(time.time() - stats['start_time']),
        'websocket': {
            'active_connections': stats['active_ws_connections'],
            'esp32_connected': stats['esp32_connected'],
            'gdevelop_clients': stats['gdevelop_clients'],
            'events_sent': stats['apriltag_events_sent']
        },
        'http': {
            'legacy_endpoint': '/client_gdevelop_to_server__smartcam_data_get',
            'status': 'active'
        }
    })

# ============================================================================
# STARTUP & MAIN
# ============================================================================

def print_startup_info():
    """Print server startup information"""
    import socket
    
    hostname = socket.gethostname()
    local_ip = socket.gethostbyname(hostname)
    
    print("\n" + "="*70)
    print("🚀 WebSocket Server Starting...")
    print("="*70)
    print(f"🖥️  Hostname: {hostname}")
    print(f"🌐 Local IP: {local_ip}")
    print(f"🔌 Port: {SERVER_PORT}")
    print("="*70)
    print("📡 WebSocket Endpoints:")
    print(f"   ws://{local_ip}:{SERVER_PORT}/socket.io/")
    print(f"   wss://your-ngrok-domain.ngrok-free.app/socket.io/")
    print("="*70)
    print("🌐 HTTP Endpoints (Legacy):")
    print(f"   http://{local_ip}:{SERVER_PORT}/")
    print(f"   http://{local_ip}:{SERVER_PORT}/status")
    print(f"   http://{local_ip}:{SERVER_PORT}/client_gdevelop_to_server__smartcam_data_get")
    print("="*70)
    print("💡 Usage:")
    print("   1. Start ngrok: ngrok http 5000")
    print("   2. Update ESP32 with ngrok WSS endpoint")
    print("   3. Update GDevelop with ngrok WSS endpoint")
    print("   4. View status at http://localhost:5000/")
    print("="*70)
    print("✅ Server ready for WebSocket connections!")
    print("="*70 + "\n")

if __name__ == '__main__':
    # Disable Flask's default logging for cleaner output
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)
    
    print_startup_info()
    
    try:
        # Run Flask-SocketIO server
        socketio.run(
            app,
            host=SERVER_HOST,
            port=SERVER_PORT,
            debug=False,
            allow_unsafe_werkzeug=True  # For development only
        )
    except KeyboardInterrupt:
        print("\n🛑 Server stopped by user")
    except Exception as e:
        print(f"❌ Server error: {e}")
