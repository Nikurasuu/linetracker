# Installation Raspberry Pi
Auf dem Raspberry Pi müssen alle benutzten Libraries für Python 3.7 installiert sein!
Danach einfach 'LinieErkennen.py' auf den Raspberry Pi ziehen und ausführen.
Nun sendet der Pi die Position der Linie über i2C an den Arduino. 
WICHTIG: Logic Level Shifter nicht vergessen!

# Installation Arduino
Sketch.ino muss auf den Arduino installiert werden, dabei sind keine Libraries wichtig, außer Wire.h


# I²C Kommunikation
Gerät | Rolle | Aufgabe
----- | ----- | -------
Raspberry Pi | master | Bilder Auswerten und Daten senden
Arduino | slave | Empfangene Daten auswerten und Motoren Steuern


# Daten vom Pi
Code | Beschreibung 
----- | ----- 
0 - 200 | Normale Position der Linie 
225 | Keine Linie gefunden
215 | Programm pausiert auf dem Pi
205 | Grüner Punkt Links
206 | Grüner Punkt Rechts
