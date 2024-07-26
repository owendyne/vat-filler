from PIL import Image
import os

def convert_png_to_array(png_path, output_path):
    # Check if the file exists
    if not os.path.exists(png_path):
        print(f"File not found: {png_path}")
        return
    
    # Open the image and convert to black and white
    image = Image.open(png_path).convert('1')  # Convert image to black and white (1-bit pixels)
    image = image.resize((128, 32))  # Ensure the image fits within the 128x32 resolution
    pixels = image.load()
    width, height = image.size
    
    byte_array = []
    
    for y in range(height):
        for x in range(0, width, 8):
            byte = 0
            for bit in range(8):
                if x + bit < width and pixels[x + bit, y] == 255:  # White pixel (inverted to black)
                    byte |= (1 << (7 - bit))
            byte_array.append(byte)
    
    with open(output_path, 'w') as f:
        f.write('#ifndef LOGO_DATA_H\n')
        f.write('#define LOGO_DATA_H\n\n')
        f.write('#define LOGO_WIDTH 128\n')
        f.write('#define LOGO_HEIGHT 32\n\n')
        f.write('const unsigned char monocure3d_logo[] PROGMEM = {\n')
        for i, byte in enumerate(byte_array):
            if i % 12 == 0:
                f.write('\n')
            f.write(f'0x{byte:02X}, ')
        f.write('\n};\n\n')
        f.write('#endif\n')

# Make sure the file name and location are correct
png_file = '/Users/owendyne/Documents/GitHub/vat-filler/testing/MOTOR_CONTROLER_TEST/output-onlinepngtools-2.png'
output_file = '/Users/owendyne/Documents/GitHub/vat-filler/testing/MOTOR_CONTROLER_TEST/logo_data_dark.h'

convert_png_to_array(png_file, output_file)