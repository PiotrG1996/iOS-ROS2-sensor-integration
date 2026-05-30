#!/usr/bin/env python3
import http.server
import socket
import os

PORT = 8000

def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    except Exception:
        ip = '127.0.0.1'
    finally:
        s.close()
    return ip

def run():
    webdir = os.path.join(os.getcwd(), 'qr_web')
    os.makedirs(webdir, exist_ok=True)
    with open(os.path.join(webdir, 'index.html'), 'w') as f:
        f.write('<html><body>SensorStream QR server</body></html>')
    os.chdir(webdir)
    http.server.test(HandlerClass=http.server.SimpleHTTPRequestHandler, port=PORT)

if __name__ == '__main__':
    print('Starting QR server on', get_local_ip(), PORT)
    run()
