

# Lee QR y barcode de una foto guardada en la misma carpeta (zbar-test.jpg)


from __future__ import print_function
import pyzbar.pyzbar as pyzbar
import numpy as np
import cv2


def decode(im) :
  # Find barcodes and QR codes
  decodedObjects = pyzbar.decode(im)

  if decodedObjects:
        print('Se ha decodificado algo.')
        for obj in decodedObjects:
            print('Type: ', obj.type)
            print('Data: ', str(obj.data), '\n')

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



    
def image_to_grey():
    # Cargar la imagen
    grey = cv2.imread('frame-autofocus.jpg', cv2.IMREAD_GRAYSCALE)
    cv2.imshow("GRIS", grey)

    return grey
  

def image_blurred(grey):
    # Aplicar un desenfoque gaussiano
    blurred = cv2.GaussianBlur(grey, (0, 0), 5)
    cv2.imshow("DESENFOQUE", blurred)

    return blurred
  
  
def image_contrast(grey, blurred):
    # Aumentar el contraste en la imagen desenfocada
    sharpened = cv2.addWeighted(grey, 2.5, blurred, -1.5, 0)
    cv2.imshow("CONTRASTE", sharpened)

    return sharpened
  
  
def image_umbral(sharpened):
    # Aplicar un umbral para resaltar el código de barras
    _, thresholded = cv2.threshold(sharpened, 100, 255, cv2.THRESH_BINARY)
    cv2.imshow("UMBRAL", thresholded)

    return thresholded




# Main
if __name__ == '__main__':
    

    # Imagen a gris
    grey = image_to_grey()
    # Intentar decodificar código de barras (si hay)
    decodedObjects = decode(grey)
    
    
    # Desenfoque gaussiano
    blurred = image_blurred(grey)
    # Intentar decodificar código de barras (si hay)
    decodedObjects = decode(blurred)
    
    
    # Contraste
    contrast = image_contrast(grey, blurred)
    # Intentar decodificar código de barras (si hay)
    decodedObjects = decode(contrast)
    
    
    # Umbral
    umbral = image_umbral(contrast)
    # Intentar decodificar código de barras (si hay)
    decodedObjects = decode(umbral)


        
    cv2.waitKey(0);

    cv2.destroyAllWindows()



  
  
