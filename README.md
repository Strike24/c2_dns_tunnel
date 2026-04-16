# C2 DNS Tunnel

A DNS-based command and control (C2) tunnel implementation in C.

## Overview

This project implements a simple DNS tunnel for command and control (C2) communication. It consists of a client that encodes commands into DNS queries and a server that decodes these queries to execute commands on the target system.
This project is a work in progress and is not complete.

## Structure

The project is seperated into two main components: A client and a server.

The client acts as a beacon. It periodically sends DNS (A) queries, telling the server its alive. data is encoded with base32 in the subdomain.
The response to that A query will be a dumby IP address.

- 1.1.1.1 - go back to sleep
- 1.1.1.2 - Prepeare for command
- 1.1.1.3 - Terminate

When the client receives the "Prepeare for command" response, it will send a TXT query, expecting to get the command in the response. after executing the command, it will send another A query to report the result, with the data encoded in the subdomain.

The server listens for incoming DNS queries, sends the appropriate responses. It binds on the DNS port and impersonates a DNS server.
