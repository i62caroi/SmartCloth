import cv2

url = 'http://192.168.222.42:81/stream'
cap = cv2.VideoCapture(url)

while True:
    ret, frame = cap.read()
    if not ret:
        print("No se pudo leer el frame")
        break

    # Aquí puedes procesar el frame como quieras

    cv2.imshow('Video Stream', frame)

    #if cv2.waitKey(1) & 0xFF == ord('q'):
    #    break
    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
