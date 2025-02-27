# SPARQS
SPARQ Sender Class to transmit data in SPARQ format


# SPARQ Format Documentation
All values are transmitted in non human readable form as bytes. This format is preferred as it is the most efficient because it uses a minimal amount of data per message.

## Message Structure
Each message has the same format and only differs in length based on the amount of data points transmitted per message.

### Message for ID/value pairs
| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| HDR | 4 | Message header with `0bXXXXX0XX` | 
| ID0 | 1 | ID of first value | 
| VAL0 | 4 | First value |
| ID1 | 1 | ID of second value |
| VAL1 | 4 | Second value |
| ... | ... | ... |
| CS | 2 | Message checksum bytes |

Total message length: 4 + NVAL * 5 + 2

### Message for strings
| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| HDR | 4 | Message header with `0bXXXXX1XX` | 
| CH0 | 1 | First char | 
| CH1 | 1 | Second char |
| ... | ... | ... |
| CHN | 1 | Last char |
| PD | 0 - 4 | Padding if message length is not a multiple of 5 |
| CS | 2 | Message checksum bytes |

Total message length: 4 + NVAL * 5 + 2

NVAL is the string `length / 5`. 
If the length is not divisible by 5, 1 - 4 padding bytes are added. NVAL is then `length / 5 + 1`


## Header
The header consists of the following 4 bytes
| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| SIG | 1 | Signature byte for message and receiver identification |
| CNT | 1 | Control byte for configuration flags |
| NVAL | 1 | Number of values transmitted in this message |
| HCS | 1 | Header checksum |

### SIG
The signature byte is used by the receiver to identify the sender and is `0xFF` by default.
### CNT
The control byte contains flags for configuration.
| Bit | Function | Options | Remarks |
| ---- | ---- | ---- | ---- |
| CNT[7] | Data Byte Order | 0 = MSB, 1 = LSB first | Byte order is the same as sender endianess |
| CNT[6] | Checksum Type | 0 = XOR8, 1 = CRC16 | CRC16 is not implemented yet |
| CNT[5] | reserved | - | |
| CNT[4] | reserved | - | |
| CNT[3:2] | Message Type | 00 = value/id pair, 01 = string, 10=bulk single id |  |
| CNT[1] | Data Sign | 0 = unsigned, 1 = signed | Ignored if type is float or string |
| CNT[0] | Data Type | 0 = float, 1 = integer | Ignored if type is string |
### NVAL
Contains the total number of values transmitted.
### ID + VAL
Contains the value as float, uint32 or int32 and its ID.
### CS
Configurable Checksum. Default: XOR Checksum
