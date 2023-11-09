

# Lee QR y barcode de una foto guardada en la misma carpeta (zbar-test.jpg)


from __future__ import print_function
import pyzbar.pyzbar as pyzbar
import numpy as np
import cv2

def decode(im) :
  # Find barcodes and QR codes
  decodedObjects = pyzbar.decode(im)

  # Print results
  for obj in decodedObjects:
    print('Type : ', obj.type)
    print('Data : ', str(obj.data),'\n')

  return decodedObjects


# Display barcode and QR code location©
def display(im, decodedObjects):

  # Loop over all decoded objects
  for decodedObject in decodedObjects:
    points = decodedObject.polygon

    # If the points do not form a quad, find convex hull
    if len(points) > 4 :
      hull = cv2.convexHull(np.array([point for point in points], dtype=np.float32))
      hull = list(map(tuple, np.squeeze(hull)))
    else :
      hull = points;

    # Number of points in the convex hull
    n = len(hull)

    # Draw the convext hull
    for j in range(0,n):
      cv2.line(im, hull[j], hull[ (j+1) % n], (255,0,0), 3)

  # Display results
  cv2.imshow("Results", im);
  cv2.waitKey(0);


# Main
if __name__ == '__main__':
  
  # Read image
  #im = cv2.imread('zbar-test.jpg')

  #decodedObjects = decode(im)
  #display(im, decodedObjects)
  
  
  print("punto 1")
  
  # Abre el stream de vídeo del ESP32-CAM
  url = 'http://192.168.222.13'
  cap = cv2.VideoCapture(url)
  #cap = cv2.VideoCapture(0)
  
  cv2.namedWindow("Video Stream", cv2.WINDOW_AUTOSIZE)
  
  print("punto 2")

  while True:
      # Lee una imagen del stream de vídeo
      ret, frame = cap.read()
      print("punto 3")

      if not ret:
          print("punto 4")
          break

      
      decodedObjects = decode(im)
      print("punto 5")
      display(im, decodedObjects)
      
      # Libera la memoria utilizada por la imagen
      im = None
      
      cv2.imshow('Video Stream', frame)

      if cv2.waitKey(1) & 0xFF == ord('q'):
          break

  cap.release()
  cv2.destroyAllWindows()
  
