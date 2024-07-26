from io import BytesIO
import base64
import typing
import os
import numpy as np

import lxml.etree as ET
import PIL.Image as Image
import subprocess

import rle

SVG_DIR = "../res/art_svg"
RLE_DIR = "../res/art_rle"
HEADER_FILE = "../art_rle.h"
IMG_SIZE = (128, 64)

def extract_svg(svg_file : str) -> Image:
    tree = ET.parse(svg_file)
    image_node = tree.find(".//{http://www.w3.org/2000/svg}image")
    image_data = image_node.attrib["{http://www.w3.org/1999/xlink}href"]
    image_data = image_data.split(",")[1]
    return Image.open(BytesIO(base64.b64decode(image_data)))

def preprocess(img : Image, size : tuple) -> Image:
    if img.mode == "RGBA":
        # Convert alpha channel to white
        alpha = img.split()[3]
        white_alpha = Image.new("L", alpha.size, color=255)
        img = Image.composite(img, white_alpha, alpha)

    # Convert image to 1-bit monochrome
    img = img.convert("1")

    # Resize image while preserving aspect ratio    
    img.thumbnail(size)
    background = Image.new("1", size, color=1)
    background.paste(img, ((size[0] - img.size[0]) // 2, (size[1] - img.size[1]) // 2))
    img = background

    return img


def write_encoded_to_header(f, name, encoded):
    rle_size = len(encoded)



    f.write(f"const rle_t {name} = {{\n")
    f.write(f"\t{rle_size},\n")
    f.write("\t(const uint8_t[]){\n\t\t")
    for byte in encoded:
        f.write(f"0x{byte:02X}, ")

    f.write("\n\t}\n};\n\n")

    return rle_size

def write_header_start(f):
    f.write(
"""
#pragma once

#include <stdint.h>
        
typedef struct {
    const uint16_t size;
    const uint8_t *data;
} rle_t;

"""
    )

def main():
    total_size = 0

    f = open(HEADER_FILE, "w")

    write_header_start(f)


    for svg_file in os.listdir(SVG_DIR):
        if svg_file.endswith(".svg"):

            try:
                img_original = extract_svg(os.path.join(SVG_DIR, svg_file))
                img_preprocessed = preprocess(img_original, IMG_SIZE)

                # flatten to a boolean array
                data = np.array(img_preprocessed, dtype=bool).flatten()

                enc = rle.encode(data)
                dec = rle.decode(enc, data.size)

                assert rle.verify(data, dec), "Decoded image does not match original image"

                # write encoded data to a file
                rle_file = os.path.join(RLE_DIR, os.path.splitext(svg_file)[0] + ".bin")
                with open(rle_file, "wb") as bin_file:
                    bin_file.write(enc)
                

                rle_size = write_encoded_to_header(f, os.path.splitext(svg_file)[0], enc)
            
                total_size += rle_size
            
            except Exception as e:
                print(f"Error processing {svg_file}: {e}")
                continue
    
    f.write("// total size of art arrays : {};\n".format(total_size))
    f.close()

if __name__ == "__main__":
    main()