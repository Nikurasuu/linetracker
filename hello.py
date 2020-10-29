print("importing..")

import numpy as np
print("imported numpy")
import cv2
print("imported OpenCV")
import imutils
print("imported imutils")
import time
print("imported time")

def nothing(x):
    pass


cap = cv2.VideoCapture(0)

cap.set(cv2.CAP_PROP_FPS, 30)

ret, frame = cap.read()

cv2.imshow('frame', frame)

print(frame.shape)

threshhold = 80

line_x_up = 0
line_x_down = 0

while True:
    noLineDown = True
    noLineUp = True

    start_time = time.time()

    ret, frame = cap.read()  # Kamerabild als frame abspeichern

    cv2.imshow('line', frame)

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  # Schwarz-weis bild als gray erstellen

    ret, line = cv2.threshold(gray, threshhold, 255, cv2.THRESH_BINARY_INV)  # Bild in zwei Farben filtern

    line = cv2.erode(line, None, iterations=2)  # Filter um rauschen rauszufiltern
    line = cv2.dilate(line, None, iterations=2)

    linedown = line[300:480,
               0:640]  # Zwei Region-of-interest regionen erstellen um den Linienverlauf zu analysieren
    lineup = line[0:200, 0:640]

    # Konturen suchen
    linedown, lineContoursDown, hirachy = cv2.findContours(linedown.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    lineup, lineContoursUp, hirachy = cv2.findContours(lineup.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    try:
        cnt = lineContoursDown[0]
        for cnt in lineContoursDown:
            area = cv2.contourArea(cnt)
            if area >= 15000:
                areaDown = area
                noLineDown = False
                try:
                    linemoment = cv2.moments(cnt)
                    line_x_down = linemoment['m10'] / linemoment['m00']
                except:
                    print("error trying finding lineposition!")
            else:
                print("some areas are not big enough!")
    except:
        linePositionDown = 0
        print("Keine Linie unten!")

    try:
        cnt = lineContoursUp[0]
        for cnt in lineContoursUp:
            area = cv2.contourArea(cnt)
            if area >= 12000:
                noLineUp = False
                try:
                    linemoment = cv2.moments(cnt)
                    line_x_up = linemoment['m10'] / linemoment['m00']
                except:
                    print("error trying finding lineposition!")
            else:
                print("some areas are not big enough!")
    except:
        linePositionUp = 0
        print("Keine Linie oben!")

    if noLineUp and noLineDown:
        line_x_up = 0
        line_x_down = 0
    elif noLineUp:
        line_x_up = line_x_down
    elif noLineDown:
        line_x_down = line_x_up

    print(line_x_up)
    print(line_x_down)

    line_rotation = line_x_up - line_x_down

    print(line_rotation)

    cv2.drawContours(frame, lineContoursDown, -1, (150, 150, 0), 0, offset=(0, 300))
    cv2.drawContours(frame, lineContoursUp, -1, (150, 150, 0), 0)

    cv2.circle(frame, (int(line_x_up),150), 20, (255,0,0), thickness=3, lineType=8, shift=0)
    cv2.circle(frame, (int(line_x_down), 450), 25, (255, 0, 0), thickness=3, lineType=8, shift=0)

    cv2.line(frame, (int(line_x_down), 450), (int(line_x_up),150), (255,0,0), thickness=10)

    cv2.imshow('line', line)
    cv2.imshow('frame', frame)

    cv2.waitKey(1)

    print("--- %s seconds ---" % (time.time() - start_time))



cap.release()  # When everything done, release the capture
cv2.destroyAllWindows()