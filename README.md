# SPARQS
SPARQ Sender Class to transmit data in SPARQ format


# SPARQ Format Documentation
All values are transmitted in non human readable form as bytes. This format is preferred as it is the most efficient because it uses a minimal amount of data per message.

## Message Structure
Each message has the same format and only differs in length based on the amount of data points transmitted per message.

### Message for ID/value pairs
| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| HDR | 5 | Message header with `CNT=0bXXXX00XX` | 
| ID0 | 1 | ID of first value | 
| VAL0 | 4 | First value |
| ID1 | 1 | ID of second value |
| VAL1 | 4 | Second value |
| ... | ... | ... |
| CS | 2 | Message checksum bytes |

Total message length: `5 + NVAL + 2`

### Message for single ID bulk
| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| HDR | 5 | Message header with `CNT=0bXXXX10XX` | 
| ID | 1 | ID | 
| VAL0 | 4 | First value |
| VAL1 | 4 | Second value |
| ... | ... | ... |
| VALn | 4 | Last value |
| CS | 2 | Message checksum bytes |

Total message length: `5 + NVAL + 2` where NVAL is the number of payload bytes INCLUDING the ID byte

### Message for strings
| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| HDR | 5 | Message header with `CNT=0bXXXX01XX` | 
| CH0 | 1 | First char | 
| CH1 | 1 | Second char |
| ... | ... | ... |
| CHN | 1 | Last char |
| CS | 2 | Message checksum bytes |

Total message length: `5 + NVAL + 2` where NVAL is the string `length`. 

## Header
The header consists of the following 5 bytes
| Name | Length [Byte] | Description |
| ---- | ---- | ---- |
| SIG | 1 | Signature byte for message and receiver identification |
| CNT | 1 | Control byte for configuration flags |
| NVAL | 2 | Number of payload bytes transmitted in this message |
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
Contains the total number of payload bytes transmitted.
### ID + VAL
Contains the value as float, uint32 or int32 and its ID.
### CS
Configurable Checksum. Default: XOR Checksum
