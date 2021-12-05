# Changelog
mdr1986x-small-lib changelog

## [0.4] - 2021-12-05

### Added

- Separate module for CRC-32 with pre-calculated table.

### Deleted

- CRC-32 calculation with table calculation from system module.

## [0.3] - 2019-11-26
### Added
- Using of malloc to allocate memory for queue buffer
- Using of malloc to allocate memory for message buffer
- Init function for set up callbacks and inner buffer and dispatcher function
for proccess message and callbacks for byte write, read and message parse
in simple_exchange
- EXCH_Write function for send messages in simple_exchange
- Acknowledge callback and parsing of acknowledge bytes in simple_exchange
- Acknowledge and not acknowledge send functions in simple_exchange

### Fixed
- Using of incorrect function names in queue module
- Fixed IRQ definition in 1986ve1t startup file for and without RAM boot.

### Changed
- Type of data in queue module to uint8_t except of return values, because
functions returns -1 when queue has no data
- Message structure in simple_exchange
- Behavior of exchange all inner variables moved into EXCH_InstTypedef
- Order of CRC-16 bytes in message

### Deleted
- EXCH_PrepareMsg function as obsolete in simple_exchange
- EXCH_Crc8 function as obsolete in simple_exchange

## [0.2] - 2019-07-09
### Added
- HSE and clock source type parameters in SYS_ClkInit for flexible clock
setup in system.c (v0.4)
- Some frequency values in SYS_Freq_Enum
- Function for calculating CRC-16 in simple_exchange (v0.3)
- Simple queue module for exchange purposes (v0.1)

## [0.1] - 2019-06-25
### Added
- Initial lib files
- This CHANGELOG file
