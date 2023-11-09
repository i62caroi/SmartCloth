import cv2
import pyzbar.pyzbar as pyzbar

def decode(im):
    # Decodifica los códigos de barras de la imagen
    decodedObjects = pyzbar.decode(im)
    
    for obj in decoded_objects:
        print('Type: ', obj.type)
        print('Data: ', obj.data, '\n')
        
    return decoded_objects

# Abre la cámara
cap = cv2.VideoCapture(0)
# Abre el stream de vídeo del ESP32-CAM
#cap = cv2.VideoCapture('http://192.168.222.13:80/')

while True:
    # Lee una imagen del video
    ret, im = cap.read()

    if not ret:
        break

    decoded_objects = decode(im)
    
    # Muestra la imagen
    cv2.imshow("Image", im)
    
    # Si se presiona la tecla 'q', se rompe el bucle
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()