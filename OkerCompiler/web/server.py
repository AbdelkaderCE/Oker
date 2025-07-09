#!/usr/bin/env python3
"""
Simple HTTP server for the Oker compiler web interface.
Provides a REST API to compile and run Oker code.
"""

import json
import os
import subprocess
import tempfile
import threading
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import socketserver

class OkerCompilerHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        """Handle GET requests for static files."""
        parsed_path = urlparse(self.path)
        path = parsed_path.path
        
        # Serve static files
        if path == '/' or path == '/index.html':
            self.serve_file('index.html', 'text/html')
        elif path == '/style.css':
            self.serve_file('style.css', 'text/css')
        elif path == '/app.js':
            self.serve_file('app.js', 'application/javascript')
        else:
            self.send_error(404, "File not found")
    
    def do_POST(self):
        """Handle POST requests for compilation."""
        if self.path == '/compile':
            self.handle_compile()
        else:
            self.send_error(404, "Endpoint not found")
    
    def serve_file(self, filename, content_type):
        """Serve a static file."""
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                content = f.read()
            
            self.send_response(200)
            self.send_header('Content-Type', content_type)
            self.send_header('Content-Length', str(len(content.encode('utf-8'))))
            self.end_headers()
            self.wfile.write(content.encode('utf-8'))
        except FileNotFoundError:
            self.send_error(404, f"File {filename} not found")
        except Exception as e:
            self.send_error(500, f"Error serving file: {str(e)}")
    
    def handle_compile(self):
        """Handle compilation requests."""
        try:
            # Read request body
            content_length = int(self.headers['Content-Length'])
            body = self.rfile.read(content_length).decode('utf-8')
            request_data = json.loads(body)
            
            code = request_data.get('code', '')
            action = request_data.get('action', 'run')
            
            if not code.strip():
                self.send_json_response({
                    'success': False,
                    'error': 'No code provided'
                })
                return
            
            # Compile the code
            result = self.compile_oker_code(code, action)
            self.send_json_response(result)
            
        except json.JSONDecodeError:
            self.send_json_response({
                'success': False,
                'error': 'Invalid JSON in request'
            })
        except Exception as e:
            self.send_json_response({
                'success': False,
                'error': f'Server error: {str(e)}'
            })
    
    def compile_oker_code(self, code, action):
        """Compile and optionally run Oker code."""
        try:
            # Create temporary file for source code
            with tempfile.NamedTemporaryFile(mode='w', suffix='.oker', delete=False) as f:
                f.write(code)
                temp_file = f.name
            
            try:
                # Build command based on action
                if action == 'tokens':
                    cmd = ['./oker', '-t', temp_file]
                elif action == 'ast':
                    cmd = ['./oker', '-p', temp_file]
                elif action == 'bytecode':
                    cmd = ['./oker', '-b', temp_file]
                else:  # run
                    cmd = ['./oker', temp_file]
                
                # Execute compiler with timeout
                result = subprocess.run(
                    cmd,
                    capture_output=True,
                    text=True,
                    timeout=10,
                    cwd=os.path.dirname(os.path.abspath(__file__)) + '/..'
                )
                
                if result.returncode == 0:
                    return {
                        'success': True,
                        'output': result.stdout
                    }
                else:
                    return {
                        'success': False,
                        'error': result.stderr or 'Compilation failed'
                    }
                    
            finally:
                # Clean up temporary file
                try:
                    os.unlink(temp_file)
                except:
                    pass
                    
        except subprocess.TimeoutExpired:
            return {
                'success': False,
                'error': 'Compilation timeout (10 seconds)'
            }
        except FileNotFoundError:
            return {
                'success': False,
                'error': 'Oker compiler not found. Please build the project first.'
            }
        except Exception as e:
            return {
                'success': False,
                'error': f'Compilation error: {str(e)}'
            }
    
    def send_json_response(self, data):
        """Send a JSON response."""
        response = json.dumps(data).encode('utf-8')
        
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Content-Length', str(len(response)))
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()
        self.wfile.write(response)
    
    def do_OPTIONS(self):
        """Handle CORS preflight requests."""
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()
    
    def log_message(self, format, *args):
        """Custom log message format."""
        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
        print(f"[{timestamp}] {format % args}")

def run_server(port=5000):
    """Run the web server."""
    server_address = ('0.0.0.0', port)
    httpd = HTTPServer(server_address, OkerCompilerHandler)
    
    print(f"Oker Compiler Web Server starting on port {port}")
    print(f"Open http://localhost:{port} in your browser")
    print("Press Ctrl+C to stop the server")
    
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down server...")
        httpd.server_close()

if __name__ == '__main__':
    import sys
    
    port = 5000
    if len(sys.argv) > 1:
        try:
            port = int(sys.argv[1])
        except ValueError:
            print("Invalid port number. Using default port 5000.")
    
    run_server(port)
