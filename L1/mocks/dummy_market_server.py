# market_data_server.py
import socket
import struct
import time

HOST = 'localhost'
PORT = 5555

def generate_market_data():
    """Generates infinite market data packets"""
    timestamp = int(time.time() * 1e9)  # nanosecond precision
    price = 100.0 + (time.time() % 10)  # oscillating price
    volume = 100
    return struct.pack('QdI', timestamp, price, volume)  # 8+8+4 = 20 bytes

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        print(f"Python Market Data Server running on {HOST}:{PORT}")
        
        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")
            try:
                while True:
                    data = generate_market_data()
                    conn.sendall(data)
            except (ConnectionResetError, BrokenPipeError):
                print("Client disconnected")

if __name__ == "__main__":
    main()
