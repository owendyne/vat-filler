from PIL import Image

import numpy as np
import typing

def pack(value: bool, count: int) -> int:
    return (value << 7) | count

def unpack(byte : int) -> typing.Tuple[bool, int]:
    return bool(byte >> 7), byte & 0b01111111


def encode(data : np.array) -> bytearray:
    assert data.dtype == bool, "Only boolean arrays are supported (1-bit monochrome images)"
    assert data.ndim == 1, "Only 1D arrays are supported"

    encoded = bytearray()

    value = data[0]
    count = 1
    for i in range(1, data.size):
        if data[i] == value:
            count += 1
        else:
            # handle case where run length is more than 7 bits
            while count > 127:
                encoded.append(pack(value, 127))
                count -= 127
            encoded.append(pack(value, count))
            value = data[i]
            count = 1

    # handle last run
    while count > 127:
        encoded.append(pack(value, 127))
        count -= 127
    
    encoded.append(pack(value, count))

    return encoded

def decode(src: bytearray, size : int) -> np.array:
    buffer = np.zeros(size, dtype=bool)
    i = 0 
    for byte in src:
        value, count = unpack(byte)
        buffer[i:i+count] = value
        i += count

    return buffer

def verify(original: np.array, decoded: np.array) -> bool:
    return np.array_equal(original, decoded)

if __name__ == "__main__":
    data = np.array([0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1], dtype=bool)
    enc = encode(data)
    dec = decode(enc, data.size)

    enc_hexstring = " ".join(f"{byte:02X}" for byte in enc)

    print(f"Original data:\n\t{data}\n")
    print(f"Encoded data ({len(enc)} bytes):\n\t{enc_hexstring}\n")
    print(f"Decoded data:\n\t{dec}\n")

    assert np.array_equal(data, dec), "Decoded data does not match original data"