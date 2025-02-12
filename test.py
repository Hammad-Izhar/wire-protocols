import socket

# Define server host and port
HOST = '0.0.0.0'  # Listen on all available network interfaces
PORT = 12345      # Choose a port number

def start_server():
    # Create a TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Allow quick reuse of port
    server_socket.bind((HOST, PORT))  # Bind to address
    server_socket.listen(1)  # Listen for incoming connections (max 1 client in queue)

    print(f"Server listening on {HOST}:{PORT}...")

    # Accept connection immediately
    client_socket, client_address = server_socket.accept()
    print(f"Connection accepted from {client_address}")

    # Send a welcome message to the client
    client_socket.sendall(b"Welcome to the TCP server!\n")

    # Echo received messages back to the client
    while True:
        data = client_socket.recv(1024)
        if not data:
            break
        print(f"Received: {data.decode().strip()}")
        client_socket.sendall(data)  # Echo back

    # Clean up
    print("Closing connection...")
    client_socket.close()
    server_socket.close()

if __name__ == "__main__":
    while True:
        try:
            start_server()
        except e:
            print("Error: " + e)
