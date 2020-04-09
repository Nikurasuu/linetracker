# Installation Raspberry Pi
Auf dem Raspberry Pi müssen alle benutzten Packages für Python 3.7 installiert sein!
Dies ist einfach mit `pip install` möglich.

Danach `LinieErkennen.py` auf den Raspberry Pi ziehen und mit `python3 LinieErkennen.py` ausführen.
Nun sendet der Pi die Position der Linie über I²C an den Arduino. 

**WICHTIG: Logic Level Shifter nicht vergessen!**

Um das Programm zu verändern und zu aktualisieren, wäre es ratsam einen FTP-Server auf dem Raspberry Pi einzurichten. Dadurch lässt sich das Programm über das Netzwerk hochladen und verändern.

Einige Programmierumgebungen erlauben es über die FTP Verbindung direkt den Code hochzuladen. Sollte dies nicht möglich sein klappt dies aber mit kostenlosen FTP Clients wie FileZilla.

# Installation Arduino
`Sketch.ino` muss auf den Arduino installiert werden, dabei sind keine Libraries wichtig, außer Wire.h.

Der Arduino muss einfach nur die gesendeten Codes (siehe Unten) lesen und danach Entscheidungen treffen.

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
207 | Zwei grüne Punkte
