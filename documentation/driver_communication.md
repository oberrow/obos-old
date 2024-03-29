# OBOS Driver communication specification

## Commands

### Common commands

---
#### OBOS_SERVICE\_GET\_SERVICE\_TYPE<br>
- Gets the type of the driver.
- This command takes no parameters
- Returns `enum serviceType`
- Can be accessed by anyone.

---
### OBOS_SERVICE\_TYPE\_FILESYSTEM
#### OBOS_SERVICE\_QUERY\_FILE\_DATA
- Queries file properties
- Parameters:<br>
&nbsp;&nbsp;&nbsp;sizeFilepath - size\_t<br>
&nbsp;&nbsp;&nbsp;filepath - char\*<br>
&nbsp;&nbsp;&nbsp;driveId - uint64\_t<br>
&nbsp;&nbsp;&nbsp;partitionId - uint8\_t<br>
- Returns the file size in bytes, the LBA offset of the file, and `enum fileExistsReturn`
- Can only be accessed by the kernel.
#### OBOS\_SERVICE\_MAKE\_FILE\_ITERATOR
- Creates a file iterator.
- Parameters:<br>
&nbsp;&nbsp;&nbsp;driveId - uint64\_t<br>
&nbsp;&nbsp;&nbsp;partitionId - uint8\_t<br>
- Returns an `uintptr_t` that can be used with iterator commands.
- Can only be accessed by the kernel.
#### OBOS\_SERVICE\_NEXT\_FILE
- Returns the file data of the current file, then seeks to the next file in the iterator. If the end of the partition is reached, then this command must return FILE_DOESNT_EXIST.
- Parameters:<br>
&nbsp;&nbsp;&nbsp;iterator - uintptr\_t
- Returns the same as OBOS\_SERVICE\_QUERY\_FILE\_DATA + `SIZE sizeFilepath` and `char\* filepath`.
- Can only be accessed by the kernel.
#### OBOS\_SERVICE\_CLOSE\_FILE\_ITERATOR
- Closes the file iterator.
- Parameters:<br>
&nbsp;&nbsp;&nbsp;iterator - uintptr\_t
- Returns zero on success, or one if there is no such uintptr\_t (uint8_t).
- Can only be accessed by the kernel.
#### OBOS\_SERVICE\_READ\_FILE
- Reads the file specified by the file path parameter synchronously.
- Parameters:<br>
&nbsp;&nbsp;&nbsp;sizeFilepath - size\_t<br>
&nbsp;&nbsp;&nbsp;filepath - char\*<br>
&nbsp;&nbsp;&nbsp;driveId - uint64\_t<br>
&nbsp;&nbsp;&nbsp;partitionId - uint8\_t<br>
&nbsp;&nbsp;&nbsp;nToSkip - uint64\_t<br>
&nbsp;&nbsp;&nbsp;nToRead - size\_t<br>
- Returns the size of the data read (this can be greater/less than nToRead), and the file data.
- Can only be accessed by the kernel.
### OBOS\_SERVICE\_TYPE\_INITRD\_FILESYSTEM <- OBOS\_SERVICE\_TYPE\_FILESYSTEM (No unique commands yet)
### OBOS\_SERVICE\_TYPE\_STORAGE\_DEVICE, SERVICE\_TYPE\_VIRTUAL\_STORAGE\_DEVICE
#### OBOS\_SERVICE\_READ\_LBA
- Reads `nSectors` at the offset `lbaOffset` from `driverId`
- Parameters:<br>
&nbsp;&nbsp;&nbsp;driveId - uint64\_t<br>
&nbsp;&nbsp;&nbsp;lbaOffset - uint64\_t<br>
&nbsp;&nbsp;&nbsp;nSectors - size\_t<br>
- Returns a byte array with a size of nSectors * 512.
- Can be accessed by anyone.
#### OBOS\_SERVICE\_WRITE\_LBA
- Writes `nSectors` at the offset `lbaOffset` to `driverId`
- Parameters:<br>
&nbsp;&nbsp;&nbsp;driveId - uint64\_t<br>
&nbsp;&nbsp;&nbsp;lbaOffset - uint64\_t<br>
&nbsp;&nbsp;&nbsp;nSectors - size\_t<br>
&nbsp;&nbsp;&nbsp;data - byte[nSectors * 512]<br>
- Returns void.
- Can be accessed by anyone.
### OBOS\_SERVICE\_TYPE\_USER\_INPUT\_DEVICE, SERVICE\_TYPE\_VIRTUAL\_USER\_INPUT\_DEVICE
#### OBOS\_SERVICE\_READ\_CHARACTER
- Receives one byte from the device.
- This function takes no parameters.
- Returns the byte read, or `nul` if there are no bytes to read.
- Can be accessed anywhere.
### TODO: SERVICE\_TYPE\_GRAPHICS\_DEVICE, SERVICE\_TYPE\_MONITOR, SERVICE\_TYPE\_KERNEL\_EXTENSION
### OBOS\_SERVICE\_TYPE\_COMMUNICATION, OBOS\_SERVICE\_TYPE\_VIRTUAL\_COMMUNICATION
#### OBOS\_SERVICE\_CONFIGURE\_COMMUNICATION
- Configures communication on the device. This is driver specific and can do whatever it wants with the parameters.
- Parameters:<br>
&nbsp;&nbsp;&nbsp;szParameters - size\_t<br>
&nbsp;&nbsp;&nbsp;parameters - byte[szParameters]<br>
- Returns a `uintptr_t` to the connection.
- Can be accessed anywhere.
#### OBOS\_SERVICE\_RECV\_BYTE\_FROM\_DEVICE
- Receives one byte from the device.
- This function takes no parameters.
- Returns the byte read, or `nul` if there are no bytes to read.
- Can be accessed anywhere.
#### OBOS\_SERVICE\_SEND\_BYTE\_TO\_DEVICE
- Sends one byte to the device.
- Parameters:<br>
&nbsp;&nbsp;&nbsp;toSend - byte<br>
- This function returns a DWORD describing a driver-specific error code.
- Can be accessed anywhere.