# POCLpokou
P.O.C.L. est l'acronyme de Petit Objet Connecté et Ludique. 

Les premiers prototypes ont été réalisés à l'occasion du hackathon POCL les 9 & 10 décembre à l'EDULAB REnnes 2 https://www.wiki.lesfabriquesduponant.net/index.php?title=Hackathon_POCL_2021

## Fonction du POCL pokou
Le POCL pokou est un POCL communautaire interactif. Sa fonction est d'envoyer ou de recevoir un message, nommé "pokou" vers ou provenant des autres POCL pokou. 

En émettant ou recevant le message, le POCL luit ainsi que tous les POCL pokou du monde.

Il dispose d'un portail captif permettant de le connecter à un réseau Wifi.

Lorsqu'il est connecté à internet, vous pouvez choissir sa couleur grâce à une interface web.

La première documentation du projet est dispnible sur le wiki des Fabriques du Ponant : https://www.wiki.lesfabriquesduponant.net/index.php?title=POCL_:_Poke

## Composants électroniques du POCL
* un D1 mini
* un ruban de 13 led RGB WS2812B
* Entre 1 et 4 boutons poussoirs
* un cable micro USB
* des cables Dupont

## Boitier du POCL
Sa forme lui donne l'allure d'une lanterne.
* Contre-plaqué peuplier 3mm ou (du MDF)
* une feuille de papier blanche

## Technologies du POCL pokou
### La comunication entre POCL
Elle est basée sur le protocole MQTT.

Pour cela nous avons fabriqué et mis en ligne un serveur MQTT Mosquitto. Il est installé sur un raspberry pi.

Le POCL peut avoir un nom individuel (Les D1 mini ont d'ailleurs un identifiant unique), nous n'utilisons pas cette particularité pour l'instant.

### Le portail captif
C'est une bibliothèque Arduino nommée WiFimanager (avev un W et un F majuscule !)

### Le serveur web de configuration de la couleur
c'est d'abord un site web embarqué sur le D1 mini. Il utilise javascript pour l'interface et le choix de la couleur.
La couleur choissie est stockée dans l'EEPROM du D1 mini, ce qui fait que si on le débranche, il conserve la mémoire de sa configuration.

## TODO
* vérifier la sécurité et les risques informatiques
* imaginer des usages
* imaginer d'autres types d'interactions (vibreurs, touches tactiles, son...)

## hypothèses
* Mobiliser les concepts de Many-to-many au lieu de one-to-one
* concept de POCL communautaire
* Un POCL qui a des heures d'ouverture ? (8h à 20h par exemple) ?
