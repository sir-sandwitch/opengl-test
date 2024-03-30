import struct

def convert_nt_to_windows3(input_file, output_file):
    # Read the input file
    with open(input_file, 'rb') as f:
        data = f.read()

    # Extract the necessary information from the NT BMP file
    width = struct.unpack_from('<I', data, 18)[0]
    height = struct.unpack_from('<I', data, 22)[0]
    pixel_data_offset = struct.unpack_from('<I', data, 10)[0]

    # Create the header for the Windows 3 BMP file
    windows3_header = struct.pack('<H', 19778)  # Signature
    windows3_header += struct.pack('<I', len(data))  # File size
    windows3_header += struct.pack('<H', 0)  # Reserved1
    windows3_header += struct.pack('<H', 0)  # Reserved2
    windows3_header += struct.pack('<I', pixel_data_offset)  # Pixel data offset
    windows3_header += struct.pack('<I', 12)  # DIB header size
    windows3_header += struct.pack('<I', width)  # Width
    windows3_header += struct.pack('<I', height)  # Height
    windows3_header += struct.pack('<H', 1)  # Color planes
    windows3_header += struct.pack('<H', 8)  # Bits per pixel
    windows3_header += struct.pack('<I', 0)  # Compression
    windows3_header += struct.pack('<I', 0)  # Image size
    windows3_header += struct.pack('<I', 0)  # X pixels per meter
    windows3_header += struct.pack('<I', 0)  # Y pixels per meter
    windows3_header += struct.pack('<I', 0)  # Total colors
    windows3_header += struct.pack('<I', 0)  # Important colors

    # Write the Windows 3 BMP file
    with open(output_file, 'wb') as f:
        f.write(windows3_header)
        f.write(data[pixel_data_offset:])

# Usage example
convert_nt_to_windows3('jason.bmp', 'jason2.bmp')