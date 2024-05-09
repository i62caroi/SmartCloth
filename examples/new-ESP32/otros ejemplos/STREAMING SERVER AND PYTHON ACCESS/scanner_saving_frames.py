from __future__ import print_function
import cv2
import pyzbar.pyzbar as pyzbar
import numpy as np


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
        
        # Guarda el frame en un archivo de imagen
        filename = "frame-test.jpg"
        cv2.imwrite(filename, frame)

        # Decodificar código de barras (si hay)
        decodedObjects = pyzbar.decode(cv2.imread(filename))
        
        # Si se decodificó algo, imprime los datos
        if decodedObjects:
            print('Se ha decodificado algo.')
            for obj in decodedObjects:
                print('Type: ', obj.type)
                print('Data: ', str(obj.data), '\n')

        cv2.imshow("Video Stream", frame)
        
        # Libera la memoria utilizada por el frame
        frame = None

        # Salir del bucle y cerrar ventana si se pulsa ESC
        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

