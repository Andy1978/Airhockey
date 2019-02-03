# Airhockey
Meine Ideen zu dem Section77 Projekt:

* Hardware https://www.jjrobots.com/the-open-source-air-hockey-robot/
* Source im submodule Air_Hockey_Robot_EVO

## Arduino Software

Siehe submodule Air_Hockey_Robot_EVO/Arduino
Das Board ist ein "Arduino Leonardo"

* nur noch manuelle, inkrementelle Positionsänderung über UDP
* alles andere wie newDataStrategy usw. raus

## Erkennung Puck und "Schubser"

Ich würde zwei Streifen im gleichen Material und Farbe am Rand montieren,
damit der Erkennungsalgorithmus die Farbe von dort nehmen kann und
IMHO Änderungen in der Beleuchtung besser ausregulieren kann

* Eine Anwendung, die von [Video (.mp4)], Bild (.jpg) oder über v4l2 ein Bild analysiert und die
  Position von Puck und "Schubser" vielleicht zusammen mit probability ausgibt

## Korrektur Blickwinkel

## Berechnung der Positionsänderung "Schubser"

## Eine Simulation, welches Positionsänderungen entgegen nimmt und direkt die Positionen aktualisiert

## Eine Simulation, welche Positionen entgegen nimmt und ein Bild rendert
