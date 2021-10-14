print("importing...")

import numpy as np
print("imported numpy")
import cv2
print("imported OpenCV")
import imutils
print("imported imutils")
import time
print("imported time")
import smbus
print("imported smbus")

runProgramm = True

import RPi.GPIO as GPIO  # Import Raspberry Pi GPIO library

GPIO.setwarnings(False)  # Ignore warning for now
GPIO.setmode(GPIO.BOARD)  # Use physical pin numbering
GPIO.setup(10, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)  # Set pin 10 to be an input pin and set initial value to be pulled low (off)

GPIO.setup(12, GPIO.OUT)
print("imported and setup GPIO!")

bus = smbus.SMBus(1)
address = 0x04


def writeNumber(value):
    global doneSending
    try:
        bus.write_byte(address, value)
        print("übertrage: " + str(value))
        doneSending = True
        # bus.write_byte_data(address, 0, value)
        return -1
    except:
        print("Übertragung von " + str(value) + " fehlgeschlagen!")


def nothing(x):
    pass


cap = cv2.VideoCapture(0)

cap.set(cv2.CAP_PROP_FPS, 30)

ret, frame = cap.read()

print(frame.shape)

threshhold = 80

greenMin = 69
greenMax = 74

createTrackbars = False

linePositionDown = False
linePositionUp = False
greenPosition = 0
isGreen = False

areaDown = 0

cornerLine = False


print("waiting for button!")

while True:

    # Auf Knopfdruck warten
    # LED Blinken lassen
    GPIO.output(12, GPIO.HIGH)
    writeNumber(215)
    time.sleep(0.1)
    GPIO.output(12, GPIO.LOW)
    time.sleep(0.4)

    if GPIO.input(10) == GPIO.HIGH:
        runProgramm = True
        print("starting..")
        time.sleep(1)

    if cv2.waitKey(20) & 0xFF == ord('q'):
        print("starting..")
        runProgramm = True
        time.sleep(1)

    while runProgramm:

        start_time = time.time()

        GPIO.output(12, GPIO.HIGH)
        
        noLineDown = True
        noLineUp = True
        isGreen = False
        cornerLine = False

        doneSending = False

        ret, frame = cap.read()  # Kamerabild als frame abspeichern

        # Liniefahren
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  # Schwarz-weis bild als gray erstellen

        ret, line = cv2.threshold(gray, threshhold, 255, cv2.THRESH_BINARY_INV)  # Bild in zwei Farben filtern

        line = cv2.erode(line, None, iterations=2)  # Filter um rauschen rauszufiltern
        line = cv2.dilate(line, None, iterations=2)

        linedown = line[300:480,0:640]  # Zwei Region-of-interest regionen erstellen um den Linienverlauf zu analysieren
        lineup = line[0:200, 0:640]

        # Grüner Punkt
        green = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)  # frame im HSV Farbraum abspeichern, um besser zu filtern

        green_roi = green[300:480, 0:640]

        h, s, v = cv2.split(green_roi)  # Aufspalten in einzelne Kanäle

        gruenThreshold = cv2.inRange(h, greenMin, greenMax)  # inRange um grün rauszufiltern
        gruenThreshold = cv2.erode(gruenThreshold, None, iterations=2)
        gruenThreshold = cv2.dilate(gruenThreshold, None, iterations=2)

        # Konturen suchen
        linedown, lineContoursDown, hirachy = cv2.findContours(linedown.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        lineup, lineContoursUp, hirachy = cv2.findContours(lineup.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        gruenThreshold, greenContours, hirachy = cv2.findContours(gruenThreshold.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        # Jede gefundene Kontur anhand der Größe rausfiltern, um die richtige zu finden
        try:
            cnt = greenContours[0]
            for cnt in greenContours:
                area = cv2.contourArea(cnt)
                if area >= 8000:
                    try:
                        linemoment = cv2.moments(cnt)  # moments methode zum speichern der Eigenschaften für die Kontur in linemoment
                        green_x = linemoment['m10'] / linemoment['m00']  # X Position der Kontur aus linemoment ziehen
                    except:
                        print("error trying finding green!")
                    greenPosition = round(green_x)
                    isGreen = True
                else:
                    print("some areas in green are not big enough!")
        except:
            print("no green!")

        try:
            cnt = lineContoursDown[0]
            for cnt in lineContoursDown:
                area = cv2.contourArea(cnt)
                if area >= 15000:
                    areaDown = area
                    noLineDown = False
                    try:
                        linemoment = cv2.moments(cnt)
                        line_x = linemoment['m10'] / linemoment['m00']
                    except:
                        print("error trying finding lineposition!")
                    linePositionDown = round(line_x) - 20
                else:
                    print("some areas are not big enough!")
        except:
            linePositionDown = 0
            print("no Contours Down!")

        try:
            cnt = lineContoursUp[0]
            for cnt in lineContoursUp:
                area = cv2.contourArea(cnt)
                if area >= 12000:
                    noLineUp = False
                    try:
                        linemoment = cv2.moments(cnt)
                        line_x = linemoment['m10'] / linemoment['m00']
                    except:
                        print("error trying finding lineposition!")
                    linePositionUp = round(line_x) - 20
                else:
                    print("some areas are not big enough!")
        except:
            linePositionUp = 0
            print("noLineUp True")

        print(linePositionUp)
        print(linePositionDown)
        print(areaDown)

        line_rotation = linePositionUp - linePositionDown

        print(line_rotation)

        # Wenn unten die Linie ist und der Grüne Punkt erkannt wurde:
        if isGreen and noLineDown == False and doneSending == False:
            print(greenPosition)
            if greenPosition < linePositionDown:
                print("Gruener Punkt Links!")
                cv2.putText(frame, "Gruener Punkt Links", (100, 300), cv2.QT_FONT_NORMAL, 1, 150)
                writeNumber(206)
            elif greenPosition > linePositionDown:
                print("Gruener Punkt Rechts!")
                cv2.putText(frame, "Gruener Punkt Rechts", (100, 300), cv2.QT_FONT_NORMAL, 1, 150)
                writeNumber(207)

        # Wenn die Linie unten nach Rechts zeigt und eine sehr große Fläche hat (Scharfe Kurven)
        if linePositionDown > 300 and areaDown > 40000 and noLineUp and isGreen == False and doneSending == False:
            print("Rechts")
            writeNumber(202)
            cornerLine = True
        elif linePositionDown < 300 and areaDown > 40000 and noLineUp and isGreen == False and doneSending == False:
            print("Links")
            writeNumber(201)
            cornerLine = True

        # Wenn oben und unten keine Linie ist
        if noLineDown == True and noLineUp == True and cornerLine == False and isGreen == False and doneSending == False:
            linePositionDown = 0
            print("searching Line..")
            cv2.putText(frame, "searching Line..", (150, 300), cv2.QT_FONT_NORMAL, 1, 150)
            writeNumber(225)

        #Wenn die Linie unten nach Rechts zeigt
        if 250 < linePositionDown < 350 and isGreen == False and cornerLine == False and doneSending == False:
            print("sending linePositionDown!")
            cv2.putText(frame, "down", (0, 400), cv2.QT_FONT_NORMAL, 1, 150)
            cv2.putText(frame, str(linePositionDown), (0, 460), cv2.QT_FONT_NORMAL, 1, 150)
            value = round(linePositionDown / 3)
            writeNumber(value)

        # Wenn die Position der Linie unten einen Schwellwert erreicht, wird mit der oberen Linie kontrolliert
        if 250 > linePositionDown >= 1 and isGreen == False and cornerLine == False and doneSending == False:
            # Position stimmt mit der oberen überein
            if 250 > linePositionUp > 1:
                cv2.putText(frame, "down", (0, 400), cv2.QT_FONT_NORMAL, 1, 150)
                cv2.putText(frame, str(linePositionDown), (0, 460), cv2.QT_FONT_NORMAL, 1, 150)
                value = round(linePositionDown / 3)
                writeNumber(value)
                print("sending linePositionDown!")
            # Position stimmt nicht überein
            elif linePositionUp > 250:
                cv2.putText(frame, "up", (0, 400), cv2.QT_FONT_NORMAL, 1, 150)
                cv2.putText(frame, str(linePositionUp), (0, 460), cv2.QT_FONT_NORMAL, 1, 150)
                value = round(linePositionUp / 3)
                writeNumber(value)
                print("sending linePositionUp!")

        # Wenn die untere Linie einen Schwellwert erreicht, wird mit der oberen Linie kontrolliert
        if linePositionDown > 350 and isGreen == False and cornerLine == False and doneSending == False:
            # Position stimmt überein
            if linePositionUp > 350:
                cv2.putText(frame, "down", (0, 400), cv2.QT_FONT_NORMAL, 1, 150)
                cv2.putText(frame, str(linePositionDown), (0, 460), cv2.QT_FONT_NORMAL, 1, 150)
                value = round(linePositionDown / 3)
                writeNumber(value)
                print("sending linePositionDown!")
            # Position stimmt nicht überein
            elif 350 > linePositionUp > 1:
                cv2.putText(frame, "up", (0, 400), cv2.QT_FONT_NORMAL, 1, 150)
                cv2.putText(frame, str(linePositionUp), (0, 460), cv2.QT_FONT_NORMAL, 1, 150)
                value = round(linePositionUp / 3)
                writeNumber(value)
                print("sending linePositionUp!")

        # Wenn keine Linie oben ist, dann Linie unten nehmen
        if linePositionUp == 0 and noLineDown == False and cornerLine == False and isGreen == False and doneSending == False:
            cv2.putText(frame, "down", (0, 400), cv2.QT_FONT_NORMAL, 1, 150)
            cv2.putText(frame, str(linePositionDown), (0, 460), cv2.QT_FONT_NORMAL, 1, 150)
            value = round(linePositionDown / 3)
            writeNumber(value)
            print("sending linePositionDown!")

        # Wenn keine Linie unten ist, dann Linie oben nehmen
        if noLineDown == 1 and noLineUp == 0 and cornerLine == False and isGreen == False and doneSending == False:
            cv2.putText(frame, "up", (0, 400), cv2.QT_FONT_NORMAL, 1, 150)
            cv2.putText(frame, str(linePositionUp), (0, 460), cv2.QT_FONT_NORMAL, 1, 150)
            value = round(linePositionUp / 3)
            writeNumber(value)
            print("sending linePositionUp!")

        cv2.drawContours(frame, lineContoursDown, -1, (150, 150, 0), 0, offset=(0, 300))
        cv2.drawContours(frame, lineContoursUp, -1, (150, 150, 0), 0)
        cv2.drawContours(frame, greenContours, -1, (150, 150, 0), 0, offset=(0, 300))

        cv2.circle(frame, (int(linePositionUp), 150), 20, (255, 0, 0), thickness=3, lineType=8, shift=0)
        cv2.circle(frame, (int(linePositionDown), 450), 25, (255, 0, 0), thickness=3, lineType=8, shift=0)

        cv2.line(frame, (int(linePositionDown), 450), (int(linePositionUp), 150), (255, 0, 0), thickness=10)
        cv2.putText(frame, str(line_rotation), (100, 460), cv2.QT_FONT_NORMAL, 1, 150)

        # Bilder anzeigen
        cv2.imshow('frame', frame)
        cv2.imshow('line', line)
        cv2.imshow('gruen', gruenThreshold)

        if createTrackbars == 0:
            cv2.createTrackbar('threshhold', 'line', threshhold, 255, nothing)
            cv2.createTrackbar('greenMin', 'gruen', greenMin, 255, nothing)
            cv2.createTrackbar('greenMax', 'gruen', greenMax, 255, nothing)
            createTrackbars = 1

        threshhold = cv2.getTrackbarPos('threshhold', 'line')
        greenMin = cv2.getTrackbarPos('greenMin', 'gruen')
        greenMax = cv2.getTrackbarPos('greenMax', 'gruen')

        if GPIO.input(10) == GPIO.HIGH:
            runProgramm = False
            print("stopping..")
            print("waiting for button!")
            time.sleep(1)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("closing..")
            runProgramm = False
            break

        print("--- %s seconds ---" % (time.time() - start_time))

cap.release()  # When everything done, release the capture
cv2.destroyAllWindows()
