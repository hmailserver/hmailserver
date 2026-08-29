---
title: "TCP/IP Listening Port"
slug: reference_tcpipport
parent: book_configuration
index: 0
is_book: false
---

## TCP/IP Ports (listening only)

### Protocol

Choose which protocol should be used for this specific port. When a client connects to the port, hMailServer will use this protocol to parse the incoming commands from the client.

### TCP/IP address

Specify the TCP/IP address hMailServer should listen on.  The address must match an internal network card address assigned to a network card in your server. The default value, 0.0.0.0, means that hMailServer will listen on all available IP addresses. For IPv6 the equivalent default value is ::

### TCP/IP port

The port number hMailServer should listen on, on the specified IP address.

### Connection security

Select the [connection security](https://www.hmailserver.com/documentation/latest/?page=reference_ssltls) to be used for the port.

### SSL certificate

If you have chosen to use SSL/TLS or StartTLS, you must select which SSL certificate you want to use for this specific port.

## TCP/IP Sending port

For TCP/IP Sending port see [SMTP Relayer](https://www.hmailserver.com/documentation/v5.6/?page=reference_protocolsmtp).
