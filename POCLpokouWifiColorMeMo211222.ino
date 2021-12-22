////////////////////////
//     POCL Pokou     //
////////////////////////
/*
 * Un POCL imaginé et réalisé au Hackathon POCL à l'édulab Rennes 2 les 9 et 10 décembre 2021.
 * 
 * Difficultés :
 * Configurez le logiciel Arduino pour gérer la carte D1 mini (ESP8266).
 * Intégrer les biobliothèues requises par le projet :
 ** #include <MQTT.h> -> pour gérer le protocole de communication, c'est la bibliothèque MQTT de Joël Gähwiler : https://github.com/256dpi/arduino-mqtt
 ** #include <Adafruit_NeoPixel.h> -> pour gérer les rubans de led
 ** #include <WiFiManager.h> -> Bibliothèque WiFiManager pour configurer automatiquement le réseau wifi et le mot de passe.
 ** #include <ESP_EEPROM.h> // Bibliothèque de gestion de la mémoire
 *  Serveur web accessible à l'URL http://pokou.local  grace à mDNS.

  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Point your browser to  http://pokou.local you should see a response.

 * 
 *                                      BROCHAGE                            
                                _________________                        
                               /     D1 mini     \                       
                           -  |[ ]RST        TX[ ]| -                    
                           -  |[ ]A0  -GPIO  RX[ ]| -                    
                              |[ ]D0-16    5-D1[ ]| -                    
                              |[ ]D5-14    4-D2[ ]| -                    
                    Bouton -  |[X]D6-12    0-D3[X]| - ruban de leds             
                           -  |[ ]D7-13    2-D4[ ]| LED_BUILTIN          
                           -  |[ ]D8-15     GND[X]| - GND (Boutons, ruban de leds)             
                           -  |[ ]3V3 .      5V[X]| - ruban de Led        
                              |       +---+       |                     
                              |_______|USB|_______|                      

Matériel :
- des fils dupont.
- un ruban de led RGB WS28B12
- D1 mini (Wemos, LOLIN,...)
- bouton poussoir
- une alimentation 5V
*/
/*
 * Un travail d'équipe de :
 * Alma Oskouei
 * Gaëlle Bescond
 * Tony Vanpoucke
 * Wing-Anh Luy
 * Antony Le Goïc-Auffret
   ___
 / ___ \
|_|   | |
     /_/ 
     _   ___   _ 
    |_| |___|_| |_
         ___|_   _|
        |___| |_|
Les petits Débrouillards - décembre 2021 CC-By-Sa http://creativecommons.org/licenses/by-nc-sa/3.0/
// Programme nourri de https://randomnerdtutorials.com/esp32-esp8266-rgb-led-strip-web-server/
// Programme inspiré de celui par Joël Gähwiler
// https://github.com/256dpi/arduino-mqtt
*/
#include <ESP8266WiFi.h>
#include <MQTT.h> // Bibliothèque MQTT par Joël Gaehwiler
#include <Adafruit_NeoPixel.h> // Bibliothèque NeoPixel d'Adafruit
#include <WiFiManager.h> // Bibliothèque WiFiManager pour configurer automatiquement le réseau wifi et le mot de passe.
#include <ESP_EEPROM.h> // Bibliothèque de gestion de la mémoire
#include <ESP8266mDNS.h> // Cette bibliothèque est incluse dans les Bibliothèques ESP8266

WiFiClient net; //on crée l'objet WiFiClient "Net"
WiFiServer serveurPOCL(80);
MQTTClient clientMQTT;
#define BROKER_IP "debrouillards.ddns.net" //IP du serveur sur lequel est installé le Broker MQTT

unsigned long lastMillis = 0;

//=============Eléments pour le ruban de led et le bouton===============

// Broche de connexion du ruban de LED
#define PIN D3 // 

int brocheBouton = 12; //GPIO de la broche D6
int rouge = 0;
int bleu = 255;
int vert = 0;

// Nombre de Led RDGB dans votre ruban
#define NUMPIXELS 13 

// on configurer un riban nommé "pixels"
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define T 1 // temps pour gérer le fade du ruban
//==============================================================

//===============Eléments pour le choix de la couleur du Ruban===============
// Pour décoder les valeurs de HTTP GET
String redString = "0";
String greenString = "0";
String blueString = "0";
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;

// Variable to store the HTTP req  uest
String header;

// Setting PWM bit resolution
// const int resolution = 256;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//==============================================================================
void connect() {
  Serial.print("Vérification de la connexion Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nPOCL Connecté au Wifi :-) ");
  Serial.print("\nconnexion au serveur MQTT en cours...");
  //connection au serveur MQTT : identifiant, User, mot de passe
  while (!clientMQTT.connect("Antony", "poclpokou", "pokou")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnecté à MQTT !");

// on s'abonne au sujet (topic) "/Poke" 
  clientMQTT.subscribe("/Poke"); // Attention à la casse !! La casse c'est maj ou minuscule
  Serial.println("Abonné à /Poke");
  // client.unsubscribe("/Poke");// Pour se désinscrire
}

void messageReceived(String &topic, String &payload) {
  digitalWrite(LED_BUILTIN, HIGH);
  fade();
  Serial.println("incoming: " + topic + " - " + payload);
  if (payload == "pokou"){
    Serial.println("clic !");
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void fade() {
  EEPROM.get(0, rouge); Serial.print("Valeur de rouge EEPROM = "); Serial.println(rouge);
  EEPROM.get(4, vert); Serial.print("Valeur de vert EEPROM = "); Serial.println(vert);
  EEPROM.get(8, bleu); Serial.print("Valeur de bleu EEPROM = "); Serial.println(bleu);
  
  for (int sig=0; sig<3; sig++){
  for (int b=0; b<255; b++){
  pixels.setBrightness(b);
  
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(rouge, vert, bleu));
  }
  pixels.show();   // On allume les rubans
  delay(T);
  }

  for (int b=255; b>1; b--){ //boucle de gestion de la brillance
  pixels.setBrightness(b);
  
  // n'oubliez pas que le premier pixel porte le Numéro 0
  for(int i=0; i<NUMPIXELS; i++) { // Pour chaque pixel...

    // pixels.Color() prends les valeurs RGB de 0,0,0 jusqu'à 255,255,255
    pixels.setPixelColor(i, pixels.Color(rouge, vert, bleu));
  }
  pixels.show();   // on affiche les pixels
  delay(T);
  }
  }
}

void testEEPROM(){ // On test si la mémoire de l'ESP contient déjà des infos d'une configuration précédente
   if(EEPROM.percentUsed()>=0) { // Si il y en a on ne fait rien
    Serial.println("Il y a des infos dans la mémoire");
    Serial.print(EEPROM.percentUsed());
    Serial.println("% de l'espace mémoire flash de l'ESP flash actuellement utilisé.");
    EEPROM.get(0, rouge); Serial.print("Valeur de rouge EEPROM = "); Serial.println(rouge);
    EEPROM.get(4, vert); Serial.print("Valeur de vert EEPROM = "); Serial.println(vert);
    EEPROM.get(8, bleu); Serial.print("Valeur de bleu EEPROM = "); Serial.println(bleu);
  } else {       // Si il n'y en a pas on stocke la couleur pas défaut
    EEPROM.put(0, rouge);
    EEPROM.put(4, vert);
    EEPROM.put(8, bleu);
    boolean ok = EEPROM.commit();
    Serial.println((ok) ? "Commit initialisation couleur OK" : "Commit initialisation couleur raté");  
  }
}
void choixCouleur(){
  WiFiClient client = serveurPOCL.available(); // On écoute si il y a des connexions sur le site web du POCL (des clients)

  if (client) {                                // Si il y a une nouvelle connexion,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("Nouveau client connecté");          // print a message out in the serial port
    String currentLine = "";                // créer une chaîne pour contenir les données entrantes du client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {            // Boucle "while", tant que le client est connecté
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                   
            // Affiche la page web
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>");
            client.println("</head><body><div class=\"container\"><div class=\"row\"><h1>Choisi la couleur de ton POCL</h1></div>");
            client.println("<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_color\" role=\"button\">Change de Couleur</a> ");
            client.println("<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\"></div>");
            client.println("<script>function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);");
            client.println("document.getElementById(\"change_color\").href=\"?r\" + Math.round(picker.rgb[0]) + \"g\" +  Math.round(picker.rgb[1]) + \"b\" + Math.round(picker.rgb[2]) + \"&\";}</script></body></html>");
            // The HTTP response ends with another blank line
            client.println();

            // Request sample: /?r201g32b255&
            // Red = 201 | Green = 32 | Blue = 255
            if(header.indexOf("GET /?r") >= 0) {
              pos1 = header.indexOf('r'); // pos1 est la position du caractère r dans la chaine de caractères.
              pos2 = header.indexOf('g'); // idem
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('&');
              redString = header.substring(pos1+1, pos2); // la chaine de caractère juste après pos1 et jusqu'à pos2 est placée dans la variable redString.
              greenString = header.substring(pos2+1, pos3); // ...
              blueString = header.substring(pos3+1, pos4);
              Serial.println(redString.toInt()); // je publie sur le moniteur série
              Serial.println(greenString.toInt());
              Serial.println(blueString.toInt());
              rouge = redString.toInt(); // je transforme la chaine de caractère en nombre entier que je met dans la variable rouge
              vert = greenString.toInt();
              bleu = blueString.toInt();
              // Je stocke la configuration dans la mémoire du D1 mini
              EEPROM.put(0, rouge);
              EEPROM.put(4, vert);
              EEPROM.put(8, bleu);
              boolean ok1 = EEPROM.commit();
              Serial.println((ok1) ? "Commit couleur OK" : "Commit couleur raté");
            }
            // On sort ("break") de la boucle "while"
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
     fade();
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void setup() {
  Serial.begin(115200); // Ouverture de communication série
  EEPROM.begin(16);
  testEEPROM(); 
  
  WiFi.mode(WIFI_STA); // la carte D1 mini est mise en mode STATION
  WiFiManager MonReseauWifi; // on crée l'objet "MonReseauWifi"
   
  bool res;
  res = MonReseauWifi.autoConnect("Wifi POCL Pokou"); // le POCL diffuse un réseau wifi en accès libre nommé "Wifi POCL Pokou"
     
  if(!res) {
    Serial.println("La connexion n'a pas fonctionnée...");
    } 
  else {
    Serial.println("Vous êtes connecté au Wifi... :-)");
  
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address : ");
    Serial.println(WiFi.localIP());
    
    }
  // demarrage du mDNS
  if (!MDNS.begin("pokou")) {
    Serial.println("Erreur d'initialisation MDNS !");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS démarré");

  serveurPOCL.begin(); //démarrage du serveur Web du POCL

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  
  pixels.begin(); //on initialise le ruban "pixels"
  pinMode(brocheBouton,INPUT_PULLUP);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  // préparation de la connexion au serveur MQTT
  clientMQTT.begin(BROKER_IP, 1883, net); // (IP, port, Client Wifi défini plus haut)
  clientMQTT.onMessage(messageReceived); // Si on reçoit un message MQTT, la fonction "messageReceived" est appelée.

  connect(); // On se connecte
}

void loop() {
  pixels.clear(); // éteint tout les pixels
  pixels.show(); // affiches pixels
  
  MDNS.update();
  
  clientMQTT.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!clientMQTT.connected()) {
    connect();
  }

  // Si on appuie sur le bouton, on envoie un messagge MQTT "pokou", sur le topic "/Poke"
  if ( digitalRead(brocheBouton) == LOW ){
  Serial.println("Appuis Bouton"); 
  clientMQTT.publish("/Poke", "pokou");
  delay(250); // Filtre anti-rebond du bouton
  }

  choixCouleur(); // appel de la fonction qui génère la page web de configuration de la couler du POCL
}
