# POCLpokou
P.O.C.L. est l'acronyme de Petit Objet Connecté et Ludique. 

Les premiers prototypes ont été réalisés à l'occasion du hackathon POCL les 9 & 10 décembre à l'EDULAB REnnes 2 https://www.wiki.lesfabriquesduponant.net/index.php?title=Hackathon_POCL_2021

C'est un projet que se déroule dans le cadre de l'appel à projet "Rennes urban data interface" (RUDI) de Rennes Métropole : https://rudi.datarennes.fr/

## Données et usages de la donnée
La donnée en jeu dans ce POCL c'est le pokou.

Il s'agit d'une donnée éphémère, comme un salut, un signe, un cin d'oeil. Lorsqu'un POCL reçoit cette donnée, la personne possède le POCL est libre d'en faire ce qu'elle souhaite.

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

## Comment ça marche ?
### Pour démarrer votre POCL et le connecter à Internet
* Branchez le POCL pokou.
* En quelques secondes, il déploie un réseau wifi "Wifi POCL pokou".
* Connectez-vous au réseau wifi du POCL
* Ouvrez un navigateur et allez à l'adresse : 192.168.4.1 - Parfois le portail captif s'ouvre automatiquement.
* Cliquez sur "Configuration".
* Le POCL scanne les réseaux Wifi à proximité et les affiches.
* Choisissez le votre et entrez votre mot de passe
* Cliquez sur "Save".
* Le POCL redémarre, il connecté à votre réseau wifi.
### Pour configurer la couleur de votre POCL
Actuellement il y a 2 options :

1 - Simple : Ouvrez un navigateur et allez à l'adresse pokou.local

2 - Complexe : Repérez l'IP de votre POCL : 
* sur le moniteur série du logiciel Arduino, oubien
* sur votre box internet (il porte un numéro de série commençant par ESP)

Ouvrez un navigateur et allez à l'adresse IP que vous venez de récupérer

Suivez les instructions pour configurer la couleur de votre POCL


## Technologies du POCL pokou
### La comunication entre POCL
Elle est basée sur le protocole MQTT.

Pour cela nous avons fabriqué et mis en ligne un serveur MQTT Mosquitto. Il est installé sur un raspberry pi.

Le POCL peut avoir un nom individuel (Les D1 mini ont d'ailleurs un identifiant unique), nous n'utilisons pas cette particularité pour l'instant.

### Le portail captif
C'est une bibliothèque Arduino nommée WiFimanager (avec un W et un F majuscule !)

### Le serveur web de configuration de la couleur
c'est d'abord un site web embarqué sur le D1 mini. Il utilise javascript pour l'interface et le choix de la couleur.

La couleur choissie est stockée dans l'EEPROM du D1 mini, ce qui fait que si on le débranche, il conserve la mémoire de sa configuration.

## Quelles données sont transmises ?
L'appuis sur le bouton déclenche l(envois un message (pokou), sur le sujet /Poke du serveur MQTT

Tous les services abonnés au sujet /Poke recevrons ce message : pokou !

Le POCL ne possède aucun autre capteur, il ne peut donc transmettre aucune autre information. Vous pouvez le vérifier !

## TODO
* Vérifier la sécurité et les risques informatiques
* Imaginer d'autres usages
* Imaginer d'autres types d'interactions (vibreurs, touches tactiles, son...).
* Trouver un moyen pour accéder au configurateur couleur plus facilement.

## Pistes de réflexion
* Mobiliser les concepts de Many-to-many au lieu de one-to-one
* Concept de POCL communautaire
* Un POCL qui a des heures d'ouverture ? (8h à 20h par exemple) ?
