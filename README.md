# SPARQS
SPARQ Sender Class to transmit data in SPARQ format


# SPARQ Format Documentation
All values are transmitted in non human readable form as bytes. This format is preferred as it is the most efficient because it uses a minimal amount of data per message.

## Message Structure
Each message has the same format and only differs in length based on the amount of data points transmitted per message.

| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| SIG | 1 | Signature byte for message and receiver identification |
| CNT | 1 | Control byte for configuration flags |
| NVAL | 1 | Number of values transmitted in this message |
| HCS | 1 | Header XOR Checksum |
| ID0 | 1 | ID of first value | 
| VAL0 | 4 | First value |
| ID1 | 1 | ID of second value |
| VAL1 | 4 | Second value |
| ... | ... | ... |
| CS | 2 | Message checksum bytes |

Total message length: 4 + NVAL * 5 + 2

### SIG
The signature byte is used by the receiver to identify the sender.
### CNT
The control byte contains flags for configuration.
| Bit | Function | Options | Remarks |
| ---- | ---- | ---- | ---- |
| CNT[7] | Data Byte Order | 0 = MSB, 1 = LSB first | Byte order is the same as sender endianess |
| CNT[6] | Checksum Type | 0 = XOR8, 1 = CRC16 | CRC16 is not implemented yet |
| CNT[5] | reserved | - | |
| CNT[4] | reserved | - | |
| CNT[3] | reserved | - | |
| CNT[2] | Message Type | 0 = value/id pair, 1 = string |  |
| CNT[1] | Data Sign | 0 = unsigned, 1 = signed | ignored if type is float or string |
| CNT[0] | Data Type | 0 = float, 1 = integer | |
### NVAL
Contains the total number of values transmitted.
### ID + VAL
Contains the value as float, uint32 or int32 and its ID.
### CS
Configurable Checksum. Default: XOR Checksum
