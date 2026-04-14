# ft_ping

A production-quality C reimplementation of the Unix `ping` command using raw sockets and ICMP. This project aims to match the behavior and output format of `inetutils-2.0`.

## 🧱 Features

- **Raw Sockets:** Uses `SOCK_RAW` and `IPPROTO_ICMP` for low-level network communication.
- **Manual ICMP:** Manually constructs Echo Request packets and implements the ICMP checksum algorithm.
- **RTT Statistics:** Calculates accurate Round-Trip Time statistics including min, average, max, and `mdev` (mean deviation).
- **Graceful Termination:** Handles `SIGINT` (Ctrl+C) to print final statistics before exiting.
- **Modular Architecture:** Clean, production-ready code organized into functional modules.

## 🛠️ Installation

Compile the project using the provided `Makefile`:

```bash
make
```

This will generate the `ft_ping` binary in the root directory.

## 🚀 Usage

Raw sockets require root privileges. Run the program with `sudo`:

```bash
sudo ./ft_ping [OPTIONS] <destination>
```

### Options
- `-v`: Verbose output. Displays additional information on unexpected ICMP packets.
- `-?`: Displays the help message and usage instructions.

### Example
```bash
sudo ./ft_ping google.com
```

## 📊 Output Format

The output is designed to closely match the format of `inetutils-2.0`:

```text
PING google.com (142.250.200.14): 56 data bytes
64 bytes from 142.250.200.14: icmp_seq=0 ttl=117 time=13.42 ms
64 bytes from 142.250.200.14: icmp_seq=1 ttl=117 time=13.25 ms
^C
--- google.com ping statistics ---
2 packets transmitted, 2 received, 0% packet loss
rtt min/avg/max/mdev = 13.250/13.335/13.420/0.085 ms
```

## 🏗️ Project Structure

- `include/`: Header files defining core structures and function prototypes.
- `src/`: Source files containing the implementation:
  - `main.c`: Entry point and orchestration.
  - `ping.c`: Main send/receive loop.
  - `icmp.c`: ICMP packet construction and parsing.
  - `socket.c`: Raw socket setup.
  - `parse.c`: Argument parsing and DNS resolution.
  - `stats.c`: Statistics tracking and reporting.
  - `signal.c`: Signal handling logic.
  - `utils.c`: Timing and generic helpers.

## ⚖️ License

Developed as part of the 42 Barcelona curriculum.
