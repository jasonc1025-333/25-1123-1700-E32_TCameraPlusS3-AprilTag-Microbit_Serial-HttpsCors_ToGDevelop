#!/bin/bash

# ============================================================================
# ESP32 Smart Camera System - Auto Startup Script
# ============================================================================
#
# REVISION HISTORY:
# ============================================================================
# 
# jwc 25-1207-1800 - Startup Order Fix
# ------------------------------------------------------
# 
# Primary Issue: ESP32 must be flashed BEFORE server starts
# 
# Script Sequence Now:
# 
# Step 1: Check Prerequisites
# Step 2: Check Ports
# Step 3: Setup Virtual Environments
# Step 4: Flash ESP32 FIRST ⬅️ MOVED UP
#         └─ Wait 10 seconds for stabilization
# Step 5: Start Ubuntu Server ⬅️ MOVED DOWN (after ESP32)
# Step 6: Start GDevelop Server
# Step 7: Display Summary
# 
# Why This Matters:
#   - Prevents old ESP32 code from connecting to new server
#   - Eliminates "Connection closed: 1005" WebSocket errors
#   - Ensures server sees only NEW ESP32 code on first connection
#   - Maximum stability with proper initialization sequence
# 
# ============================================================================
# 
# jwc 25-1205-0200 - Major Script Fixes & Enhancements
# ------------------------------------------------------
# 
# Primary Issue: Script Fixes
# 
# 1. Fixed gnome-terminal errors:
#    - Added DBUS_SESSION_BUS_ADDRESS and XDG_RUNTIME_DIR environment variables
#    - Resolved "Failed to execute child process dbus-launch" error
#    - Terminals now open successfully
# 
# 2. Implemented Method 3: Isolated Virtual Environments:
#    - Created ESP32/PlatformIO venv at 01-Esp32-Client/venv
#    - Installs compatible click==7.1.2 (fixes PlatformIO version error)
#    - No system-wide changes - everything isolated and portable
#    - Auto-creates venvs on first run (works on any Ubuntu 22 machine)
# 
# Benefits:
#    ✅ No system-wide package modifications
#    ✅ Fully portable project (copy to any Ubuntu 22 machine)
#    ✅ Auto-setup on first run (~2-3 minutes first time, instant after)
#    ✅ Fixes PlatformIO compatibility issues with Python 3.10
#    ✅ ~550MB disk space in project folder for venvs
#
# ============================================================================
# 
# This script automatically starts:
# 1. Python Ubuntu Server (with venv) - in separate terminal
# 2. ESP32 Client upload & serial monitor - in separate terminal
#
# Requirements:
# - gnome-terminal (for Ubuntu/GNOME)
# - Python 3 with venv
# - PlatformIO CLI (pio command)
#
# Usage:
#   chmod +x 00-start_system.sh
#   ./00-start_system.sh
#
# ============================================================================

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get script directory (where this script is located)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo -e "${BLUE}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║      ESP32 Smart Camera System - Auto Startup Script        ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Display system information
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}  WHAT YOU'LL SEE${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo ""
echo "After running this script, you'll have 4 windows:"
echo ""
echo "  1. Main terminal - Script status and summary"
echo "  2. Ubuntu WebSocket Server - WebSocket debug prints (port 5000)"
echo "  3. GDevelop Game Server - HTTP server for game (port 5100)"
echo "  4. ESP32 Serial Monitor - ESP32 upload and debug prints"
echo ""

echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}  SYSTEM ARCHITECTURE${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo ""
echo "┌─────────────────┐      WebSocket       ┌─────────────────┐"
echo "│   ESP32 Client  │ ◄──────────────────► │  Ubuntu Server  │"
echo "│  (T-Camera S3)  │  ws://10.0.0.149:5000│   (WebSocket)   │"
echo "└─────────────────┘                      └─────────────────┘"
echo "        │                                         │"
echo "        │ HTTP (Video)                            │ WebSocket"
echo "        │ http://10.0.0.149:5000/video            │"
echo "        └─────────────────────────────────────────┤"
echo "                                                  │"
echo "                                          ┌───────▼────────┐"
echo "                                          │ GDevelop Game  │"
echo "                                          │ localhost:5100 │"
echo "                                          └────────────────┘"
echo ""

echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}  QUICK REFERENCE${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo ""
echo "Port Configuration:"
echo "  • Port 5000: Ubuntu WebSocket Server (AprilTag + video)"
echo "  • Port 5100: GDevelop Game Server"
echo "  • USB: ESP32 Serial Monitor"
echo ""
echo "Access URLs:"
echo "  • WebSocket: ws://localhost:5000/websocket"
echo "  • Video Stream: http://localhost:5000/video_stream"
echo "  • GDevelop Game: http://localhost:5100"
echo ""

echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}  CONFIGURATION FILES${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo ""
echo "WiFi Settings (ESP32):"
echo "  File: 01-Esp32-Client/01B-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp"
echo "  • WIFI_SSID = \"Chan-Comcast\""
echo "  • WIFI_PASSWORD = \"Jesus333!\""
echo ""
echo "Server IP (ESP32):"
echo "  • WS_HOST = \"10.0.0.149\""
echo ""
echo "Timing (ESP32):"
echo "  • AprilTag_Send_INTERVAL_MS = 1500 (1.5s, 0.67 msg/sec)"
echo "  • VideoFrame_Send_INTERVAL_MS = 3000 (3s, 0.33 FPS)"
echo ""

echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}  COMMON ISSUES & SOLUTIONS${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo ""
echo "ESP32 Upload Fails:"
echo "  • Check USB connection"
echo "  • Verify COM port in PlatformIO"
echo "  • Try pressing BOOT button during upload"
echo ""
echo "Port 5000 Already in Use:"
echo "  • sudo lsof -i :5000"
echo "  • sudo kill -9 <PID>"
echo ""
echo "WiFi Connection Failed:"
echo "  • Verify SSID and password"
echo "  • Ensure 2.4GHz WiFi band (ESP32-S3 limitation)"
echo "  • Check router is not blocking device"
echo ""
echo "WebSocket Connection Refused:"
echo "  • Verify Ubuntu server is running"
echo "  • Check firewall settings"
echo "  • Confirm IP address is correct"
echo ""

echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}  PERFORMANCE TIPS${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo ""
echo "  • Lower latency: AprilTag_Send_INTERVAL_MS = 500ms"
echo "  • More stability: AprilTag_Send_INTERVAL_MS = 2000ms"
echo "  • Faster video: VideoFrame_Send_INTERVAL_MS = 1000ms (may lag)"
echo "  • Better quality: Increase VIDEO_JPEG_QUALITY (1-100)"
echo ""

echo -e "${BLUE}═══════════════════════════════════════════════════════════════${NC}"
echo ""
echo -e "${YELLOW}Press Enter to start system setup...${NC}"
read

# ============================================================================
# Step 1: Check Prerequisites
# ============================================================================
echo -e "${YELLOW}[1/6] Checking prerequisites...${NC}"

# Check for gnome-terminal
if ! command -v gnome-terminal &> /dev/null; then
    echo -e "${RED}ERROR: gnome-terminal not found${NC}"
    echo "Install it with: sudo apt install gnome-terminal"
    exit 1
fi

# Check for Python 3
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}ERROR: python3 not found${NC}"
    exit 1
fi

echo -e "${GREEN}✓ All prerequisites found${NC}"
echo ""

# ============================================================================
# Step 2: Check and Clear Ports
# ============================================================================
echo -e "${YELLOW}[2/7] Checking ports availability...${NC}"

# Function to check if a port is in use and offer to kill the process
check_and_clear_port() {
    local port=$1
    local service_name=$2
    
    # Check if port is in use
    local pid=$(lsof -ti :$port 2>/dev/null)
    
    if [ -n "$pid" ]; then
        echo -e "${YELLOW}⚠️  Port $port is already in use by PID $pid ($service_name)${NC}"
        
        # Get process info
        local process_info=$(ps -p $pid -o comm= 2>/dev/null)
        echo "   Process: $process_info"
        
        # Ask user what to do
        echo -e "${YELLOW}   Options:${NC}"
        echo "   1) Kill the process and continue"
        echo "   2) Skip and continue anyway (may cause conflicts)"
        echo "   3) Exit script"
        read -p "   Choose (1/2/3): " choice
        
        case $choice in
            1)
                echo "   Killing process $pid..."
                kill -9 $pid 2>/dev/null
                sleep 1
                
                # Verify port is now free
                local check_pid=$(lsof -ti :$port 2>/dev/null)
                if [ -z "$check_pid" ]; then
                    echo -e "${GREEN}   ✓ Port $port is now free${NC}"
                else
                    echo -e "${RED}   ERROR: Failed to free port $port${NC}"
                    exit 1
                fi
                ;;
            2)
                echo -e "${YELLOW}   ⚠️  Continuing anyway - this may cause conflicts${NC}"
                ;;
            3)
                echo "Exiting script..."
                exit 0
                ;;
            *)
                echo -e "${RED}   Invalid choice. Exiting...${NC}"
                exit 1
                ;;
        esac
    else
        echo -e "${GREEN}✓ Port $port is available ($service_name)${NC}"
    fi
}

# Check port 5000 (Ubuntu WebSocket Server)
check_and_clear_port 5000 "Ubuntu WebSocket Server"

# Check port 5100 (GDevelop Game Server)
check_and_clear_port 5100 "GDevelop Game Server"

echo ""

# ============================================================================
# Step 3: Setup Python Virtual Environments
# ============================================================================
echo -e "${YELLOW}[3/7] Setting up Python virtual environments...${NC}"

# Server venv
VENV_DIR="$SCRIPT_DIR/02-Ubuntu-Server_Hub/venv"

if [ ! -d "$VENV_DIR" ]; then
    echo "Creating Python server virtual environment..."
    python3 -m venv "$VENV_DIR"
    
    echo "Installing server dependencies..."
    source "$VENV_DIR/bin/activate"
    ### jwc 25-1205-0020 o pip install --upgrade pip
    ### jwc 25-1205-0020 o pip install flask flask-cors simple-websocket
    ### jwc 25-1205-0020 o deactivate
    
    echo -e "${GREEN}✓ Server virtual environment created${NC}"
else
    echo -e "${GREEN}✓ Server virtual environment already exists${NC}"
fi

# ESP32/PlatformIO venv
ESP32_VENV_DIR="$SCRIPT_DIR/01-Esp32-Client/venv"

if [ ! -d "$ESP32_VENV_DIR" ]; then
    echo "Creating ESP32/PlatformIO virtual environment..."
    echo "(This will take 2-3 minutes on first run - downloading ~500MB)"
    python3 -m venv "$ESP32_VENV_DIR"
    
    echo "Installing PlatformIO with compatible dependencies..."
    source "$ESP32_VENV_DIR/bin/activate"
    pip install --upgrade pip
    pip install click==7.1.2  # Compatible with PlatformIO 4.x
    pip install platformio
    deactivate
    
    echo -e "${GREEN}✓ ESP32 virtual environment created${NC}"
    
    # Wait a moment to ensure all installations complete
    echo "Waiting 2 seconds for venv to stabilize..."
    sleep 2
else
    echo -e "${GREEN}✓ ESP32 virtual environment already exists${NC}"
fi

echo ""

# ============================================================================
# Step 4: Upload ESP32 Code FIRST (Before Server Starts)
# ============================================================================
echo -e "${YELLOW}[4/7] Flashing ESP32 Client FIRST...${NC}"
echo -e "${YELLOW}⚠️  IMPORTANT: ESP32 must be flashed with new code BEFORE server starts${NC}"
echo -e "${YELLOW}   This prevents old ESP32 code from connecting with invalid data${NC}"
echo ""

ESP32_ENV="Camera_Screen_AprilTag__Serial_With_Microbit-NOW__Esp32_Client_Websocket"

# Create a launcher script for ESP32 (uses venv PlatformIO)
ESP32_LAUNCHER="$SCRIPT_DIR/01-Esp32-Client/.launch_esp32.sh"
cat > "$ESP32_LAUNCHER" << EOF
#!/bin/bash
PROJECT_ROOT="$PROJECT_ROOT"
ESP32_ENV="$ESP32_ENV"
SCRIPT_DIR="$SCRIPT_DIR"

# Activate PlatformIO virtual environment
source "\$SCRIPT_DIR/01-Esp32-Client/venv/bin/activate"

cd "\$PROJECT_ROOT"

echo "=========================================="
echo "   ESP32 Client - Upload & Monitor"
echo "=========================================="
echo ""
echo "Environment: \$ESP32_ENV"
echo "Project Root: \$PROJECT_ROOT"
echo "Using: venv PlatformIO"
echo ""

# Upload code
echo ">>> Uploading code to ESP32..."
pio run -e "\$ESP32_ENV" --target upload

if [ \$? -eq 0 ]; then
    echo ""
    echo ">>> Upload successful! Starting serial monitor..."
    echo ">>> ESP32 will wait for server to start..."
    echo ">>> Press Ctrl+C to stop monitoring"
    echo ""
    sleep 2
    
    # Start serial monitor
    pio device monitor -e "\$ESP32_ENV"
else
    echo ""
    echo ">>> Upload failed!"
    echo ">>> Make sure ESP32 is connected via USB"
fi

echo ""
echo "Monitoring stopped. Press Enter to close..."
read
EOF

chmod +x "$ESP32_LAUNCHER"

# Launch ESP32 in new terminal (with clean environment to avoid snap library conflicts)
env -i HOME="$HOME" USER="$USER" PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
    DBUS_SESSION_BUS_ADDRESS="$DBUS_SESSION_BUS_ADDRESS" \
    XDG_RUNTIME_DIR="$XDG_RUNTIME_DIR" \
    DISPLAY="$DISPLAY" XAUTHORITY="$XAUTHORITY" \
    gnome-terminal --title="ESP32 Client - Serial Monitor" \
                   --geometry=120x40+600+0 \
                   -- bash -c "$ESP32_LAUNCHER" &

echo -e "${GREEN}✓ ESP32 flash started in separate terminal${NC}"
echo ""
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}⚠️  IMPORTANT: DO NOT START SERVER YET!${NC}"
echo -e "${YELLOW}═══════════════════════════════════════════════════════════════${NC}"
echo ""
echo -e "${YELLOW}Please check the ESP32 serial monitor terminal and verify:${NC}"
echo ""
echo "  1. ✅ ESP32 upload completed successfully"
echo "  2. ✅ ESP32 connected to WiFi"
echo "  3. ✅ Serial monitor shows: 'Waiting for server...' or similar message"
echo ""
echo -e "${YELLOW}Only when you see the ESP32 is ready and waiting, press Enter to start the server.${NC}"
echo ""
read -p "Press Enter when ESP32 serial monitor confirms it's waiting for server... "

echo ""
echo -e "${GREEN}✓ User confirmed ESP32 is ready and waiting${NC}"
echo ""

# ============================================================================
# Step 5: Start Ubuntu Server AFTER ESP32 is Flashed
# ============================================================================
echo -e "${YELLOW}[5/7] Starting Ubuntu Server (AFTER ESP32 flash)...${NC}"

SERVER_SCRIPT="$SCRIPT_DIR/02-Ubuntu-Server_Hub/02-ubuntu_server_websocket-NOW.py"

if [ ! -f "$SERVER_SCRIPT" ]; then
    echo -e "${RED}ERROR: Server script not found at $SERVER_SCRIPT${NC}"
    exit 1
fi

# Create a launcher script for the server
SERVER_LAUNCHER="$SCRIPT_DIR/02-Ubuntu-Server_Hub/.launch_server.sh"
cat > "$SERVER_LAUNCHER" << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/venv/bin/activate"
echo "=========================================="
echo "   Ubuntu WebSocket Server - Starting"
echo "=========================================="
echo ""
python3 "$SCRIPT_DIR/02-ubuntu_server_websocket-NOW.py"
echo ""
echo "Server stopped. Press Enter to close..."
read
EOF

chmod +x "$SERVER_LAUNCHER"

# Launch server in new terminal (with clean environment to avoid snap library conflicts)
# Fix for: "symbol lookup error: /snap/core20/current/lib/x86_64-linux-gnu/libpthread.so.0: undefined symbol: __libc_pthread_init"
env -i HOME="$HOME" USER="$USER" PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
    DBUS_SESSION_BUS_ADDRESS="$DBUS_SESSION_BUS_ADDRESS" \
    XDG_RUNTIME_DIR="$XDG_RUNTIME_DIR" \
    DISPLAY="$DISPLAY" XAUTHORITY="$XAUTHORITY" \
    gnome-terminal --title="Ubuntu WebSocket Server" \
                   --geometry=100x30+0+0 \
                   -- bash -c "$SERVER_LAUNCHER" &

echo -e "${GREEN}✓ Ubuntu Server launched (AFTER ESP32 flash)${NC}"
echo ""

# Wait for server to start
echo "Waiting 3 seconds for server to start..."
sleep 3

# ============================================================================
# Step 6: Start GDevelop Game Server in New Terminal
# ============================================================================
echo -e "${YELLOW}[6/7] Starting GDevelop Game Server...${NC}"

GDEVELOP_DIR="$PROJECT_ROOT/11k-25-1202-1330--25-1127-0950-E32_SmartCam-ToUbuntuServerHub-ToGdevelop-WebSocket-NOW/export-Jwc--Gdevelop_Html_Server-NOW"

if [ ! -d "$GDEVELOP_DIR" ]; then
    echo -e "${RED}ERROR: GDevelop directory not found at $GDEVELOP_DIR${NC}"
    echo "Skipping GDevelop server..."
else
    # Create a launcher script for GDevelop game
    GDEVELOP_LAUNCHER="$GDEVELOP_DIR/.launch_gdevelop.sh"
    cat > "$GDEVELOP_LAUNCHER" << 'EOF'
#!/bin/bash
GDEVELOP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$GDEVELOP_DIR"

echo "=========================================="
echo "   GDevelop Game Server - Starting"
echo "=========================================="
echo ""
echo "Directory: $GDEVELOP_DIR"
echo "Port: 5100"
echo ""
echo "Access game at: http://localhost:5100"
echo ""

python3 -m http.server 5100

echo ""
echo "Server stopped. Press Enter to close..."
read
EOF

    chmod +x "$GDEVELOP_LAUNCHER"

    # Launch GDevelop in new terminal
    env -i HOME="$HOME" USER="$USER" PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
        DBUS_SESSION_BUS_ADDRESS="$DBUS_SESSION_BUS_ADDRESS" \
        XDG_RUNTIME_DIR="$XDG_RUNTIME_DIR" \
        DISPLAY="$DISPLAY" XAUTHORITY="$XAUTHORITY" \
        gnome-terminal --title="GDevelop Game Server" \
                       --geometry=80x20+0+400 \
                       -- bash -c "$GDEVELOP_LAUNCHER" &

    echo -e "${GREEN}✓ GDevelop Game Server launched in separate terminal${NC}"
    echo -e "${GREEN}  Access game at: http://localhost:5100${NC}"
    echo ""
    
    # Wait a moment for game server to start
    sleep 1
fi

# ============================================================================
# Step 7: Summary
# ============================================================================
echo -e "${BLUE}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                    System Started!                           ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${GREEN}✓ Python Server venv:${NC} $VENV_DIR"
echo -e "${GREEN}✓ PlatformIO venv:${NC} $ESP32_VENV_DIR"
echo -e "${GREEN}✓ Ubuntu WebSocket Server:${NC} Running on port 5000"
echo -e "${GREEN}✓ GDevelop Game Server:${NC} Running on port 6000 (http://localhost:5100)"
echo -e "${GREEN}✓ ESP32 Client:${NC} Uploading code & starting monitor"
echo ""
echo -e "${YELLOW}Next Steps:${NC}"
echo "1. Check Ubuntu Server terminal for WebSocket server startup"
echo "2. Open browser: http://localhost:5100 to play GDevelop game"
echo "3. Check ESP32 terminal for upload progress and serial output"
echo "4. All terminals will show debug prints in real-time"
echo ""
echo -e "${YELLOW}Troubleshooting:${NC}"
echo "- If ESP32 upload fails, check USB connection"
echo "- If server fails, check port 5000 is not in use"
echo "- Close both terminals to stop the system"
echo ""
echo -e "${YELLOW}Isolated Virtual Environments:${NC}"
echo "- No system-wide changes made"
echo "- Each component has its own isolated dependencies"
echo "- Safe to copy project to another Ubuntu 22 machine"
echo "- Script will auto-recreate venvs on new machine"
echo ""
echo -e "${BLUE}Script complete! Terminals will keep running.${NC}"
echo -e "${BLUE}Press Ctrl+C to exit this window.${NC}"
echo ""

# Keep script running so user can see the summary
# They can Ctrl+C when ready
trap 'echo ""; echo "Script exited. Terminals are still running."; exit 0' INT

# Wait indefinitely (user exits with Ctrl+C)
while true; do
    sleep 1
done
