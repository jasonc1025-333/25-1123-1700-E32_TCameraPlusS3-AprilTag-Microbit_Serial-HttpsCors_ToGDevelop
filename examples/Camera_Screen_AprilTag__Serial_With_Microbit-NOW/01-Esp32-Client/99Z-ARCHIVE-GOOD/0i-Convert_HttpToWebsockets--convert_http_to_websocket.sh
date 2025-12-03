#!/bin/bash

# HTTP to WebSocket Conversion Script
# jwc 25-1202-2030
# Converts 01B file from HTTP to WebSocket communication

FILE="01B-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp"

# Create backup
BACKUP="${FILE}.backup-$(date +%Y%m%d-%H%M%S)"
echo "Creating backup: $BACKUP"
cp "$FILE" "$BACKUP"

echo "Converting HTTP to WebSocket..."
echo "This will modify: $FILE"
echo ""

# Note: This script makes basic replacements
# You'll need to manually add:
# 1. WebSocket configuration constants after WIFI_PASSWORD
# 2. WebSocket event handler before initWiFi()
# 3. Complete sendAprilTagDataWebSocket() function
# 4. webSocket.loop() at start of loop()

# Change 1: Archive HTTP includes
sed -i 's|^//// jwc 25-1126-2200 OPTION 1: ESP32 POSTs to Python Server (ACTIVE)|//// jwc 25-1126-2200 OPTION 1 ARCHIVED 25-1202-2030: ESP32 POSTs to Python Server via HTTP|' "$FILE"
sed -i 's|^#include <HTTPClient\.h>|//// #include <HTTPClient.h>|' "$FILE"
sed -i 's|^#include <WiFiClient\.h>|//// #include <WiFiClient.h>|' "$FILE"

# Add note about WebSocket includes (you'll add these manually)
sed -i '/^\/\/\/\/ jwc 25-1126-2200 OPTION 1 ARCHIVED/a \/\/\/ #include <WiFiClient.h>\n\n\/\/\/\/ jwc 25-1202-2030 OPTION 1 NEW: ESP32 sends to Python Server via WebSocket (ACTIVE)\n#include <WebSocketsClient.h>  \/\/ Links2004 library\n#include <ArduinoJson.h>' "$FILE"

echo "✅ Basic conversions applied!"
echo ""
echo "⚠️  IMPORTANT: You still need to manually:"
echo "1. Add WebSocket config after WIFI_PASSWORD (see guide)"
echo "2. Add webSocketEvent() handler before initWiFi()"  
echo "3. Replace sendAprilTagData() function"
echo "4. Update initWiFi() WebSocket setup"
echo "5. Add webSocket.loop() at start of loop()"
echo "6. Update function call from sendAprilTagData to sendAprilTagDataWebSocket"
echo ""
echo "Backup saved to: $BACKUP"
echo "To revert: cp $BACKUP $FILE"
