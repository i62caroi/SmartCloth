from __future__ import print_function
import cv2
import pyzbar.pyzbar as pyzbar
import numpy as np

# Decodificar código de barras en imagen (im)
def decode(im):
    # Decodifica los códigos de barras de la imagen
    decodedObjects = pyzbar.decode(im)
    
    for obj in decodedObjects:
        print('Type: ', obj.type)
        print('Data: ', str(obj.data), '\n')
        
    return decodedObjects


# Display barcode and QR code location 
def display(im, decodedObjects):

  # Loop over all decoded objects
  for decodedObject in decodedObjects:
    points = decodedObject.polygon

    # If the points do not form a quad, find convex hull
    if len(points) > 4 :
      hull = cv2.convexHull(np.array([point for point in points], dtype=np.float32))
      hull = list(map(tuple, np.squeeze(hull)))
    else :
      hull = points

    # Number of points in the convex hull
    n = len(hull)

    # Draw the convext hull
    for j in range(0,n):
      cv2.line(im, hull[j], hull[ (j+1) % n], (255,0,0), 3)

    # Display results
    cv2.imshow("Video Stream", im)


# Guardar código decodificado en un fichero sin repetirlo
def saveDecodeObject(decodedObjects):
    with open('decoded_objects.txt', 'a') as f:
        for obj in decodedObjects:
            # Si no se ha guardado ya ese objeto, se guarda en fichero y en el conjunto
            if obj.data not in saved_objects: 
                print("\n\n--------- Guardando objeto -------------\n\n")
                f.write('Type: ' + str(obj.type) + '\n' + 'Data: ' + str(obj.data) + '\n\n')
                #f.write(str(obj.data) + '\n')
                saved_objects.add(obj.data)


# Conjunto vacío (los conjuntos no permiten duplicados)
saved_objects = set() # Global

# Main
if __name__ == '__main__':
    
    # Abre el stream de vídeo del ESP32-CAM
    url = 'http://192.168.73.74:81/stream'
    cap = cv2.VideoCapture(url)

    #cap = cv2.VideoCapture(0)

    cv2.namedWindow("Video Stream", cv2.WINDOW_AUTOSIZE)

    while True:
        # Lee un frame del video
        ret, frame = cap.read()
        
        if not ret:
            print("No se pudo leer el frame")
            break

        # Decodificar código de barras (si hay)
        decodedObjects = decode(frame)
        
        # Guardar código si no se ha guardado ya
        saveDecodeObject(decodedObjects)

        cv2.imshow("Video Stream", frame)
        
        # Libera la memoria utilizada por el frame
        frame = None

        # Salir del bucle y cerrar ventana si se pulsa ESC
        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

