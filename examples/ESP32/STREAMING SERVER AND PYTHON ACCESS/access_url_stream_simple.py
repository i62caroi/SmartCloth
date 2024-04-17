# Este programa accede al stream de video de una cámara IP y muestra el video en tiempo real.
# Está hecho para acceder al stream de una cámara que ejecute el programa de ejemplo de CameraWebServer,
# por eso se especifica el puerto 81 y la ruta /stream.

import cv2


# Main
if __name__ == '__main__':
    
    # Cambiar solo la parte de la IP por la de tu cámara
    url = 'http://192.168.2.238:81/stream'
    cap = cv2.VideoCapture(url)

    while True:
        ret, frame = cap.read()
        if not ret:
            print("No se pudo leer el frame")
            break

        # Aquí puedes procesar el frame como quieras

        cv2.imshow('Video Stream', frame)

        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()
