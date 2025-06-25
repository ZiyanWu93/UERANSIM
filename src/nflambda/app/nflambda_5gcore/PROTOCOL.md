# NAS IPC Protocol Specification

## Overview

This document specifies the protocol for transporting NAS (Non-Access Stratum) messages between UERANSIM and NFLambda 5G Core over IPC (Inter-Process Communication).

## Protocol Version

Current version: **1** (0x01)

## Message Format

All messages use the `IpcMessage` structure with a 2044-byte data payload. The protocol defines how to pack NAS messages into this payload.

### Message Layout

```
┌─────────────┬──────────┬───────────┬──────────────┬────────────┬───────────┬─────────────┐
│ Offset      │ 0        │ 1         │ 2-3          │ 4-7        │ 8-9       │ 10-11       │ 12-2043     │
├─────────────┼──────────┼───────────┼──────────────┼────────────┼───────────┼─────────────┼─────────────┤
│ Field       │ Msg Type │ Version   │ PDU Length   │ Trans ID   │ Event Type│ Reserved    │ NAS PDU     │
├─────────────┼──────────┼───────────┼──────────────┼────────────┼───────────┼─────────────┼─────────────┤
│ Size (bytes)│ 1        │ 1         │ 2            │ 4          │ 2         │ 2           │ 0-2032      │
└─────────────┴──────────┴───────────┴──────────────┴────────────┴───────────┴─────────────┴─────────────┘
```

### Field Descriptions

#### Message Type (1 byte)
- `0x01` - NAS_UPLINK: Message from UE to AMF
- `0x02` - NAS_DOWNLINK: Message from AMF to UE
- `0xFF` - NAS_ERROR: Error response

#### Protocol Version (1 byte)
- `0x01` - Current protocol version
- Must match for successful communication

#### PDU Length (2 bytes)
- Length of the NAS PDU in bytes
- Network byte order (big-endian)
- Maximum value: 2032

#### Transaction ID (4 bytes)
- Unique identifier for request/response correlation
- Network byte order (big-endian)
- Echoed in responses

#### Event Type (2 bytes)
- Identifies the NAS message type
- Network byte order (big-endian)
- Values:
  - `0x0001` - Registration Request
  - `0x0002` - Authentication Response
  - `0x0003` - Security Mode Complete
  - `0x0004` - Registration Complete
  - `0x0005` - PDU Session Request

#### Reserved (2 bytes)
- Set to 0
- Reserved for future use

#### NAS PDU (variable, 0-2032 bytes)
- Actual NAS message content
- Binary data (not null-terminated)

## Message Flows

### Uplink Flow (UE → AMF)

1. UERANSIM packs NAS PDU with appropriate event type
2. Sends via IPC client to NFLambda
3. NFLambda unpacks and processes
4. NFLambda sends downlink response

### Downlink Flow (AMF → UE)

1. NFLambda generates NAS response
2. Packs with DOWNLINK message type
3. Sends via IPC response
4. UERANSIM unpacks and delivers to UE

### Error Handling

If an error occurs, an ERROR message is sent with:
- Message Type: `0xFF`
- Event Type: `0x0000`
- PDU Data: Single byte error code

## Error Codes

- `0x00` - OK (Success)
- `0x01` - Protocol version mismatch
- `0x02` - Invalid message type
- `0x03` - PDU too large
- `0x04` - Internal processing error
- `0x05` - Invalid message format

## Example Messages

### Registration Request (Uplink)

```
Offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
000000  01 01 00 15 00 00 00 01 00 01 00 00 7e 00 41 79
000010  00 0d 01 99 f9 07 00 00 00 00 00 00 00
        └─┘└─┘└────┘└──────────┘└────┘└────┘└─ NAS PDU ─┘
         │  │    │        │        │      │
         │  │    │        │        │      └─ Reserved (0)
         │  │    │        │        └─ Event Type (0x0001)
         │  │    │        └─ Transaction ID (1)
         │  │    └─ PDU Length (21 bytes)
         │  └─ Version (1)
         └─ Message Type (UPLINK)
```

### Authentication Request (Downlink)

```
Offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
000000  02 01 00 38 00 00 00 01 00 00 00 00 7e 00 56 00
000010  02 00 00 21 f8 b6 96 8f 1e 80 00 f2 2c b5 1f de
000020  9c 5f 78 c0 b6 00 10 b1 5e a7 bc 0f a9 79 88 5e
000030  58 52 c6 f0 a1 e8 e7 59 f5 32 85 68
        └─┘└─┘└────┘└──────────┘└────┘└────┘└─ NAS PDU ─┘
         │  │    │        │        │      │
         │  │    │        │        │      └─ Reserved (0)
         │  │    │        │        └─ Event Type (0)
         │  │    │        └─ Transaction ID (1)
         │  │    └─ PDU Length (56 bytes)
         │  └─ Version (1)
         └─ Message Type (DOWNLINK)
```

### Error Response

```
Offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C
000000  FF 01 00 01 00 00 00 01 00 00 00 00 03
        └─┘└─┘└────┘└──────────┘└────┘└────┘└─┘
         │  │    │        │        │      │    │
         │  │    │        │        │      │    └─ Error Code (PDU_TOO_LARGE)
         │  │    │        │        │      └─ Reserved (0)
         │  │    │        │        └─ Event Type (0)
         │  │    │        └─ Transaction ID (1)
         │  │    └─ PDU Length (1 byte)
         │  └─ Version (1)
         └─ Message Type (ERROR)
```

## Implementation Notes

1. **Byte Order**: All multi-byte fields use network byte order (big-endian)
2. **Alignment**: No padding between fields
3. **Validation**: Always validate message format before processing
4. **Zero-Copy**: The unpack function returns a pointer into the original message to avoid copying

## Security Considerations

1. This protocol does not provide encryption or authentication
2. It assumes a trusted local IPC channel (Unix domain socket)
3. NAS PDUs may contain sensitive information and should be handled accordingly

## Future Extensions

The protocol is designed to be extensible:
- Version field allows protocol evolution
- Reserved field provides space for future flags
- Event type has room for many more message types
- Error codes can be extended as needed