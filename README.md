# linetracker
Auf dem Raspberry Pi müssen alle benutzten Libraries für Python 3.7 installiert sein!
Danach einfach 'LinieErkennen.py' auf den Raspberry Pi ziehen und ausführen.
Nun sendet der Pi die Position der Linie über i2C an den Arduino. 
WICHTIG: Logic Level Shifter nicht vergessen!

Sketch.ino muss auf den Arduino installiert werden, dabei sind keine Libraries wichtig, außer Wire.h
