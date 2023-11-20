import cv2
import numpy as np
import pyzbar.pyzbar as pyzbar
import urllib.request
 
 
def decode(im) :
  # Find barcodes and QR codes
  decodedObjects = pyzbar.decode(im)

  # Print results
  for obj in decodedObjects:
    print('Type : ', obj.type)
    print('Data : ', str(obj.data),'\n')

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
      hull = points;

    # Number of points in the convex hull
    n = len(hull)

    # Draw the convext hull
    for j in range(0,n):
      cv2.line(im, hull[j], hull[ (j+1) % n], (255,0,0), 3)

  # Display results
  #cv2.imshow("Results", im);
  cv2.waitKey(0);
  


#cap = cv2.VideoCapture(0)
font = cv2.FONT_HERSHEY_PLAIN
 
url='http://192.168.222.42'
cv2.namedWindow("live transmission", cv2.WINDOW_AUTOSIZE)

print("punto 1")
 
prev=""
pres=""
while True:
    print("punto 2")
    try:
        img_resp = urllib.request.urlopen(url)
    except Exception as e:
        print("Error al abrir la URL: ", e)
        
    print("punto 3")
    imgnp=np.array(bytearray(img_resp.read()),dtype=np.uint8)
    
    print("punto 4")
    frame=cv2.imdecode(imgnp,-1)
    print("punto 5")
    #_, frame = cap.read()
 
    #decodedObjects = decode(frame)
    #display(im, decodedObjects)
    
    print("esperando imagen a leer...")
    
    decodedObjects = pyzbar.decode(frame)
    
    if(decodedObjects) :
        print("Se ha leido algo")
    
    for obj in decodedObjects:
        pres=obj.data
        if prev == pres:
            pass
        else:
            print("Type:",obj.type)
            print("Data: ",obj.data)
            prev=pres
        cv2.putText(frame, str(obj.data), (50, 50), font, 2,
                    (255, 0, 0), 3)
 
    cv2.imshow("live transmission", frame)
 
    """key = cv2.waitKey(1)
    if key == 27:
        break"""
    if cv2.waitKey(1) & 0xFF == ord('q'):
          break
 
cv2.destroyAllWindows()
