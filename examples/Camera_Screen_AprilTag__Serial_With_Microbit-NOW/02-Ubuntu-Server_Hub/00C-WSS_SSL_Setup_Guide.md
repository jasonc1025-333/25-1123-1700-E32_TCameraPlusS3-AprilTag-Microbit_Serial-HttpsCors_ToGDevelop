# 🔐 WSS (WebSocket Secure) Setup Guide

## Overview

Currently the system uses:
- **WS** (ws://) - Unencrypted WebSocket
- **HTTP** (http://) - Unencrypted HTTP

This guide shows how to upgrade to:
- **WSS** (wss://) - Encrypted WebSocket (SSL/TLS)
- **HTTPS** (https://) - Encrypted HTTP (SSL/TLS)

## ✅ Compatibility

- **GDevelop**: ✅ Fully supports WSS via WebSocketClient
- **ESP32**: ✅ Supports WSS (with certificate validation or setInsecure())
- **Python Flask**: ✅ Supports HTTPS/WSS via ssl_context

## 📋 Prerequisites

```bash
# Install OpenSSL (usually pre-installed on Ubuntu)
sudo apt update
sudo apt install openssl
```

## 🔑 Option 1: Self-Signed Certificate (Development/Testing)

### Step 1: Generate Self-Signed Certificate

```bash
cd examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW/02-Ubuntu-Server_Hub/

# Generate private key and certificate (valid for 365 days)
openssl req -x509 -newkey rsa:4096 -nodes \
  -keyout server_key.pem \
  -out server_cert.pem \
  -days 365 \
  -subj "/CN=localhost"
```

This creates:
- `server_key.pem` - Private key
- `server_cert.pem` - SSL certificate

### Step 2: Update Python Server

Create a new file `02A-ubuntu_server_websocket_SSL-NOW.py`:

```python
#!/usr/bin/env python3
"""
WebSocket Server with SSL/TLS Support (WSS)
"""

# ... (copy all code from 02-ubuntu_server_websocket-NOW.py)

# At the bottom, replace the main section with:

if __name__ == '__main__':
    import ssl
    log = logging.getLogger('werkzeug')
    log.setLevel(logging.ERROR)
    
    print_startup_info()
    
    # SSL Configuration
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ssl_context.load_cert_chain(
        certfile='server_cert.pem',
        keyfile='server_key.pem'
    )
    
    try:
        print("🔒 SSL/TLS enabled - using WSS and HTTPS")
        app.run(
            host=SERVER_HOST, 
            port=SERVER_PORT, 
            debug=False,
            ssl_context=ssl_context  # Enable SSL
        )
    except KeyboardInterrupt:
        print("\n🛑 Server stopped")
    except Exception as e:
        print(f"❌ Error: {e}")
```

### Step 3: Update ESP32 Code

In `01B-Camera_Screen_AprilTag__Serial_With_Microbit-HttpToWebsocket-NOW.ino.cpp`:

```cpp
// WebSocket Server Configuration
const char* WS_HOST = "10.0.0.149";  // Your server IP
const uint16_t WS_PORT = 5000;
const char* WS_PATH = "/websocket";
const bool WS_USE_SSL = true;  // 🔒 ENABLE SSL

// In setup():
if (wifi_connected) {
    printf("🔌 WebSocket Init | Server=%s://%s:%d%s\n", 
           WS_USE_SSL ? "wss" : "ws", WS_HOST, WS_PORT, WS_PATH);
    
    webSocket.onEvent(webSocketEvent);
    
    if (WS_USE_SSL) {
        // For self-signed certificates, skip validation (development only!)
        webSocket.beginSSL(WS_HOST, WS_PORT, WS_PATH);
        // For security, use client.setInsecure() carefully
    } else {
        webSocket.begin(WS_HOST, WS_PORT, WS_PATH);
    }
}
```

### Step 4: Update GDevelop Client

In `03A-Gdevelop-Client-Test.html`:

```javascript
// Change from ws:// to wss://
const WS_URL = 'wss://10.0.0.149:5000/websocket';  // 🔒 WSS
const VIDEO_URL = 'https://10.0.0.149:5000/video_stream';  // 🔒 HTTPS
```

### Step 5: Browser Certificate Warning

When accessing `https://10.0.0.149:5000` in browser:
1. You'll see "Your connection is not private"
2. Click "Advanced"
3. Click "Proceed to 10.0.0.149 (unsafe)" - OK for development

## 🔑 Option 2: Let's Encrypt (Production)

For production with a domain name:

### Step 1: Install Certbot

```bash
sudo apt update
sudo apt install certbot
```

### Step 2: Get Certificate

```bash
# Replace yourdomain.com with your actual domain
sudo certbot certonly --standalone -d yourdomain.com

# Certificates will be in:
# /etc/letsencrypt/live/yourdomain.com/fullchain.pem
# /etc/letsencrypt/live/yourdomain.com/privkey.pem
```

### Step 3: Update Python Server

```python
if __name__ == '__main__':
    import ssl
    
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ssl_context.load_cert_chain(
        certfile='/etc/letsencrypt/live/yourdomain.com/fullchain.pem',
        keyfile='/etc/letsencrypt/live/yourdomain.com/privkey.pem'
    )
    
    app.run(
        host=SERVER_HOST, 
        port=443,  # HTTPS standard port
        ssl_context=ssl_context
    )
```

### Step 4: Auto-Renewal

```bash
# Test renewal
sudo certbot renew --dry-run

# Add to crontab for auto-renewal
sudo crontab -e

# Add this line (renew daily at 2am)
0 2 * * * certbot renew --quiet && systemctl restart your_server_service
```

## 🔑 Option 3: Reverse Proxy (Nginx)

Use Nginx to handle SSL, forward to Python:

### Step 1: Install Nginx

```bash
sudo apt update
sudo apt install nginx
```

### Step 2: Configure Nginx

Create `/etc/nginx/sites-available/websocket-ssl`:

```nginx
server {
    listen 443 ssl;
    server_name yourdomain.com;

    ssl_certificate /etc/letsencrypt/live/yourdomain.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/yourdomain.com/privkey.pem;

    # WebSocket upgrade
    location /websocket {
        proxy_pass http://localhost:5000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }

    # Regular HTTP endpoints
    location / {
        proxy_pass http://localhost:5000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

### Step 3: Enable Site

```bash
sudo ln -s /etc/nginx/sites-available/websocket-ssl /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx
```

### Step 4: Python Server (No SSL needed)

Python server runs without SSL (Nginx handles it):

```python
# Regular Flask server - Nginx adds SSL layer
app.run(host='127.0.0.1', port=5000, debug=False)
```

## 🔐 Security Best Practices

### 1. Certificate Validation

**ESP32 - Production:**
```cpp
// Load CA certificate for validation
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
// ... rest of certificate ...
"-----END CERTIFICATE-----\n";

WiFiClientSecure client;
client.setCACert(root_ca);  // Validate certificate
```

**ESP32 - Development Only:**
```cpp
WiFiClientSecure client;
client.setInsecure();  // ⚠️  Skip validation (development only!)
```

### 2. Authentication Token

The server already uses AUTH_TOKEN - keep using it even with SSL:

```cpp
// ESP32
const char* AUTH_TOKEN = "your_secret_token_here";  // Change this!

// Python
AUTH_TOKEN = "your_secret_token_here"  # Must match ESP32
```

### 3. Firewall Rules

```bash
# Allow HTTPS/WSS
sudo ufw allow 443/tcp

# Block HTTP/WS (force encrypted connections)
sudo ufw deny 80/tcp
sudo ufw deny 5000/tcp
```

## 📊 Comparison Table

| Feature | WS (Unencrypted) | WSS (Encrypted) |
|---------|------------------|-----------------|
| **Security** | ❌ Plain text | ✅ Encrypted |
| **Setup** | ✅ Easy | ⚠️  Requires certs |
| **Performance** | ✅ Faster | ⚠️  Slight overhead |
| **Port** | 80, 5000 | 443, 5000 |
| **Browser** | ✅ No warnings | ⚠️  Cert warnings (self-signed) |
| **IoT Friendly** | ✅ Simple | ⚠️  More memory |
| **Production** | ❌ Not recommended | ✅ Required |

## 🚀 Quick Start Scripts

### Self-Signed SSL Server

```bash
#!/bin/bash
# Start WSS server with self-signed certificate

cd examples/Camera_Screen_AprilTag__Serial_With_Microbit-NOW/02-Ubuntu-Server_Hub/

# Generate certificate if not exists
if [ ! -f "server_cert.pem" ]; then
    echo "Generating self-signed certificate..."
    openssl req -x509 -newkey rsa:4096 -nodes \
      -keyout server_key.pem \
      -out server_cert.pem \
      -days 365 \
      -subj "/CN=localhost"
fi

# Start server
source venv/bin/activate
python3 02A-ubuntu_server_websocket_SSL-NOW.py
```

## 🐛 Troubleshooting

### ESP32 Can't Connect to WSS

```
Error: mbedtls_ssl_handshake returned -0x7780
```

**Solution**: Use `setInsecure()` for self-signed certificates:

```cpp
webSocket.beginSSL(WS_HOST, WS_PORT, WS_PATH);
// Library internally calls client.setInsecure() if needed
```

### GDevelop "WebSocket connection failed"

**Cause**: Browser blocks mixed content (HTTPS page loading WS)

**Solution**: Use WSS when page is HTTPS, WS when page is HTTP

```javascript
// Auto-detect protocol
const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
const WS_URL = `${protocol}//10.0.0.149:5000/websocket`;
```

### Certificate Expired

```bash
# Check expiration
openssl x509 -in server_cert.pem -noout -dates

# Regenerate
openssl req -x509 -newkey rsa:4096 -nodes \
  -keyout server_key.pem \
  -out server_cert.pem \
  -days 365 \
  -subj "/CN=localhost"
```

## 📝 Summary

**For Development/Testing:**
- Use Option 1 (Self-Signed) - Quick & Easy
- ESP32: `setInsecure()` to skip validation
- Accept browser warnings

**For Production:**
- Use Option 2 (Let's Encrypt) or Option 3 (Nginx)
- ESP32: Load proper CA certificate
- Use real domain name
- Enable firewall, strong auth tokens

**Both WS and WSS work perfectly with:**
- ✅ GDevelop WebSocketClient
- ✅ ESP32 WebSocketsClient library
- ✅ Python Flask with flask-sock

Choose based on your security requirements!
