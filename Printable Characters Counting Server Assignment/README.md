# Printable Characters Counting Server Assignment
### goal
The goal of this assignment is to gain experience with sockets and network programming. In this
assignment, I was needed to implement a toy client/server architecture: a printable characters counting
(PCC) server. Clients connect to the server and send it a stream of bytes. The server counts how
many of the bytes are printable and returns that number to the client. The server also maintains
overall statistics on the distribution of printable characters it has received from all clients. When
the server terminates, it prints these statistics to standard output.

## Assignment description

You need to implement two programs, a server and a client:
1. **Server** (`pcc_server`): The server accepts `TCP` connections from clients. A client that connects
sends the server a stream of *N* bytes (*N* is determined by the client and is not a global constant).
The server counts the number of printable characters in the stream (a *printable character* is a
byte b whose value is 32 ≤ b ≤ 126). Once the stream ends, the server sends the count back to
the client over the same connection. In addition, the server maintains a data structure in which it
counts the number of times each printable character was observed in all the connections. When
the server receives a `SIGINT`, it prints these counts and exits.
2. **Client** (`pcc_client`): The client creates a `TCP` connection to the server and sends it the contents
of a user-supplied file. The client then reads back the count of printable characters from the
server, prints it, and exits.

## Client specification:
Implement the following program in a file named `pcc_client.c`. The following details the specification of the program.
### Command line arguments:
* `argv[1]`: server’s IP address (assume a valid IP address).
* `argv[2]`: server’s port (assume a 16-bit unsigned integer).
* `argv[3]`: path of the file to send.

You should validate that the correct number of command line arguments is passed, and detect errors
while opening the file (e.g., if it doesn’t exist) and reading it.

### The flow:
1. Create a `TCP` connection to the specified server port on the specified server IP.
2. Open the specified file for reading.
3. Transfer the contents of the file to the server over the `TCP` connection and receive the count of
printable characters computed by the server, using the following protocol:
    - The client sends the server N, the number of bytes that will be transferred (i.e., the file
size). The value N is a 32-bit unsigned integer in network byte order.
    - The client sends the server N bytes (the file’s content).
    - The server sends the client C, the number of printable characters. The value C is a 32-bit
unsigned integer in network byte order.
4. Print the number of printable characters obtained to stdout using exactly the following `printf()`
format string:

*"# of printable characters: %u\n"*

5. Exit with exit code 0.

## Server specification
Implement the following program in a file named `pcc_server.c`. The following details the specification of the program.
### Command line arguments:
* `argv[1]`: server’s port (assume a 16-bit unsigned integer).
### The flow:
1. Initialize a data structure ``pcc_total`` that will count how many times each printable character
was observed in all client connections. The counts are 32-bits unsigned integers.
2. Listen to incoming `TCP` connections on the specified server port. Use a `listen()` queue of size
10.
3. Enter a loop, in which each iteration:
    - Accepts a `TCP` connection.
    - When a connection is accepted, reads a stream of bytes from the client, computes its
printable character count and writes the result to the client over the `TCP` connection (using
the protocol described above). After sending the result to the client, updates the `pcc_total`
global data structure. You don’t need to handle overflow of the `pcc_total` counters.
4. If the server receives a `SIGINT` signal (e.g., user hits Ctrl-C) perform the following actions:
    - If the server is processing a client when `SIGINT` is delivered, finish handling this client
(including updating the `pcc_total` global data structure).
    - Print the number of times each printable character was observed to standard output. Use
exactly the following `printf()` format string to print the count of each character:

        *"char ’%c’ : %u times\n"*
    - Exit with exit code 0.

### compile:
```cmd
gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 pcc_server.c (or pcc_client.c)
```
