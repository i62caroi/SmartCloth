import gzip

# Leer el HTML desde un archivo
with open('index_ov2640_simple.html', 'r') as f:
    html_ov2640 = f.read()

# Comprimir el HTML
index_ov2640_gzip = gzip.compress(html_ov2640.encode())

# Imprimir los datos comprimidos como una matriz de bytes
#print(', '.join(f'0x{byte:02x}' for byte in index_ov2640_gzip))

# Escribir los datos comprimidos en un archivo
with open('index_ov2640_gzip.txt', 'w') as f:
    f.write(f"// {len(index_ov2640_gzip)} bytes\n") # Comentario con número de bytes
    f.write(', '.join(f'0x{byte:02x}' for byte in index_ov2640_gzip))
    
print("\nFichero html comprimido en gzip\n")