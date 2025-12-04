#!/bin/bash

# ============================================================================
# ESP32 Smart Camera System - Auto Startup Script
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

# ============================================================================
# Step 1: Check Prerequisites
# ============================================================================
echo -e "${YELLOW}[1/4] Checking prerequisites...${NC}"

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

# Check for PlatformIO CLI
if ! command -v pio &> /dev/null; then
    echo -e "${RED}ERROR: PlatformIO CLI (pio) not found${NC}"
    echo "Install it with: pip install platformio"
    exit 1
fi

echo -e "${GREEN}✓ All prerequisites found${NC}"
echo ""

# ============================================================================
# Step 2: Setup Python Virtual Environment (if needed)
# ============================================================================
echo -e "${YELLOW}[2/4] Setting up Python virtual environment...${NC}"

VENV_DIR="$SCRIPT_DIR/02-Ubuntu-Server_Hub/venv"

if [ ! -d "$VENV_DIR" ]; then
    echo "Creating virtual environment..."
    python3 -m venv "$VENV_DIR"
    
    echo "Installing dependencies..."
    source "$VENV_DIR/bin/activate"
    pip install --upgrade pip
    pip install flask flask-cors simple-websocket
    deactivate
    
    echo -e "${GREEN}✓ Virtual environment created${NC}"
else
    echo -e "${GREEN}✓ Virtual environment already exists${NC}"
fi

echo ""

# ============================================================================
# Step 3: Start Ubuntu Server in New Terminal
# ============================================================================
echo -e "${YELLOW}[3/4] Starting Ubuntu Server...${NC}"

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

# Launch server in new terminal
gnome-terminal --title="Ubuntu WebSocket Server" \
               --geometry=100x30+0+0 \
               -- bash -c "$SERVER_LAUNCHER" &

echo -e "${GREEN}✓ Ubuntu Server launched in separate terminal${NC}"
echo ""

# Wait for server to start
echo "Waiting 3 seconds for server to start..."
sleep 3

# ============================================================================
# Step 4: Upload ESP32 Code & Start Serial Monitor
# ============================================================================
echo -e "${YELLOW}[4/4] Starting ESP32 Client...${NC}"

ESP32_ENV="lilygo-t-camera-plus-s3"

# Create a launcher script for ESP32
ESP32_LAUNCHER="$SCRIPT_DIR/01-Esp32-Client/.launch_esp32.sh"
cat > "$ESP32_LAUNCHER" << EOF
#!/bin/bash
PROJECT_ROOT="$PROJECT_ROOT"
ESP32_ENV="$ESP32_ENV"

cd "\$PROJECT_ROOT"

echo "=========================================="
echo "   ESP32 Client - Upload & Monitor"
echo "=========================================="
echo ""
echo "Environment: \$ESP32_ENV"
echo "Project Root: \$PROJECT_ROOT"
echo ""

# Upload code
echo ">>> Uploading code to ESP32..."
pio run -e "\$ESP32_ENV" --target upload

if [ \$? -eq 0 ]; then
    echo ""
    echo ">>> Upload successful! Starting serial monitor..."
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

# Launch ESP32 in new terminal
gnome-terminal --title="ESP32 Client - Serial Monitor" \
               --geometry=120x40+600+0 \
               -- bash -c "$ESP32_LAUNCHER" &

echo -e "${GREEN}✓ ESP32 Client launcher started in separate terminal${NC}"
echo ""

# ============================================================================
# Summary
# ============================================================================
echo -e "${BLUE}╔══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║                    System Started!                           ║${NC}"
echo -e "${BLUE}╚══════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${GREEN}✓ Ubuntu Server:${NC} Running in separate terminal"
echo -e "${GREEN}✓ ESP32 Client:${NC} Uploading code & starting monitor"
echo ""
echo -e "${YELLOW}Next Steps:${NC}"
echo "1. Check Ubuntu Server terminal for WebSocket server startup"
echo "2. Check ESP32 terminal for upload progress and serial output"
echo "3. Both terminals will show debug prints in real-time"
echo ""
echo -e "${YELLOW}Troubleshooting:${NC}"
echo "- If ESP32 upload fails, check USB connection"
echo "- If server fails, check port 5000 is not in use"
echo "- Close both terminals to stop the system"
echo ""
echo -e "${BLUE}Press Ctrl+C to exit this script (terminals will keep running)${NC}"
echo ""

# Keep script running so user can see the summary
# They can Ctrl+C when ready
trap 'echo ""; echo "Script exited. Terminals are still running."; exit 0' INT

# Optional: Wait for user input to close
read -p "Press Enter to exit this script (terminals will continue running)..."
