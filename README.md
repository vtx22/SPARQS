# SPARQS
SPARQ Sender Class to transmit data in SPARQ format


# SPARQ Format Documentation
## Modes
Two transmission modes are available and switchable at runtime.
### Byte Mode
With byte mode selected all values are transmitted in non human readable form as bytes. This format is preferred as it is the most efficient because it uses a minimal amount of data per message.
### ACSCII Mode (TODO)
In ASCII Mode all values are transmitted as chars for each decimal digit which is especially useful for console debugging purposes. It is slower and uses more bytes than messages in byte mode. 


## Message Structure
Each message has the same format and only differs in length based on the amount of data points transmitted per message.

| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| SIG | 1 | Signature byte for message and receiver identification |
| CNT | 1 | Control byte for configuration flags |
| NVAL | 1 | Number of values transmitted in this message |
| ID0 | 1 | ID of first value | 
| VAL0 | 4 | First value as float |
| ID1 | 1 | ID of second value |
| VAL1 | 4 | Second value as float |
| ... | ... | ... |
| CS | 2 | Message checksum bytes |

Total message length: 5 + NVAL * 5

### SIG
The signature byte is used by the receiver to identify the sender.
### CNT
The control byte contains flags for configuration.
| Bit | Function |
| ---- | ---- |
| CNT[7] | Float byte order: 0=MSB, 1=LSB first |
| CNT[6] | Checksum type: 0=XOR, 1=CRC |
| CNT[5] | reserved |
| CNT[4] | reserved |
| CNT[3] | reserved |
| CNT[2] | reserved |
| CNT[1] | reserved |
| CNT[0] | reserved |
### NVAL
Contains the total number of values transmitted.
### ID + VAL
Contains the value as float and its ID.
### CS
Configurable Checksum. Default: XOR Checksum