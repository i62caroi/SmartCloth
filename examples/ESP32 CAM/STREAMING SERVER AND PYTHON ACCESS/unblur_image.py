import cv2
import numpy as np
import pyzbar.pyzbar as pyzbar

"""
# Cargar la imagen
img = cv2.imread('frame-ov2460.jpg', cv2.IMREAD_COLOR)

# Aplicar un desenfoque gaussiano
blurred = cv2.GaussianBlur(img, (0, 0), 5)

# Aumentar el contraste en la imagen desenfocada
sharpened = cv2.addWeighted(img, 1.5, blurred, -0.5, 0)

# Guardar la imagen mejorada
cv2.imwrite('imagen_mejorada_ov2460.jpg', sharpened)
"""



# Main
if __name__ == '__main__':

    # Cargar la imagen
    img = cv2.imread('frame-autofocus.jpg', cv2.IMREAD_GRAYSCALE)
    cv2.imshow("GRIS", img)

    # Aplicar un desenfoque gaussiano
    blurred = cv2.GaussianBlur(img, (0, 0), 5)
    cv2.imshow("DESENFOQUE", blurred)

    # Aumentar el contraste en la imagen desenfocada
    sharpened = cv2.addWeighted(img, 2.5, blurred, -1.5, 0)
    cv2.imshow("CONTRASTE", sharpened)

    # Aplicar un umbral para resaltar el código de barras
    _, thresholded = cv2.threshold(sharpened, 100, 255, cv2.THRESH_BINARY)
    cv2.imshow("UMBRAL", thresholded)

    
    cv2.waitKey(0);

    cv2.destroyAllWindows()