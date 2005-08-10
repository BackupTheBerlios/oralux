// ----------------------------------------------------------------------------
// audiouserinterface.c
// $Id: audioUserInterface.c,v 1.13 2005/08/10 21:32:13 gcasse Exp $
// $Author: gcasse $
// Description: Managing and playing the pre-recorded messages.
// $Date: 2005/08/10 21:32:13 $ |
// $Revision: 1.13 $ |
// Copyright (C) 2003, 2004, 2005 Gilles Casse (gcasse@oralux.org)
// September 2003: 
// German translations by Guenther Harrasser.
// Spanish translations by Jes�s Angulo and Fernando Pinto da Silva.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// ----------------------------------------------------------------------------
#include <stdio.h>
#include <fcntl.h>
#include "audioUserInterface.h"

/* < General variables */

struct t_translation {
  char* myEnglishSentence;
  char* myTranslatedSentence;
};
#include "messages_ru.koi8-r"

static char TheOggDirectory[BUFSIZE];
static char* ThePortName=NULL;

static enum language TheCurrentLanguage; /* menu language (so a speech synthesizer and the translations are available for this language) */

static char* TheLanguageSuffix[]={
  "", //English
  "-fr", //French
  "-de", //German
  "-es", //Spanish
  "-ru", //Russian
};
static int TheSoundIsEnable=1;

// Storing the current tty file descriptor
static int pf=0;

/* > */
/* < TheMessages */

// TheMessages: is an array of the possible said messages.
//The index of this array must match with the enum sequence 
// Warning: do not be shocked by the orthograph!
// The aim here is to improve the pronunciation.

// Keyboards to add, info required; today, they are assigned to the american keyboard.
/*   chineseSaidSimplified, */
/*   chineseSaidTraditionnal, */
/*   hebrewSaid, */
/*   japaneseSaid, */
/*   netherlandSaid, */

char * TheMessages[][4]={
  {"American",
   "Am�ricain",
   "Americanische", 
   "Americano",
  }, // same order as in enum keyboard
  {"Belgian",
   "Belge",
   "Belgische",
   "Belga",
  },
  {"British",
   "Britannique",
   "Englische",
   "Ingl�s",
  },
  {"Bulgarian",
   "Bulgare",
   "Bulgarische",
   "B�lgaro",
  },
/*   {"Simplified Chinese", */
/*    "Chinois simplifi�", */
/*    "Vereinfachte Chinesische", */
/*    "Chino Simplificado", */
/*   }, */
/*   {"Traditionnal Chinese", */
/*    "Chinois traditionnel", */
/*    "Traditionelle Chinesische", */
/*    "Chino Tradicional", */
/*   }, */
  {"Czech",
   "Tch�que",
   "Tschechische",
   "Checo",
  },
  {"Danish",
   "Danois",
   "D�nische",
   "Dan�s",
  },
  {"Finnish",
   "Finlandais",
   "Finnische",
   "Finland�s",
  },
  {"French",
   "Fran�ais",
   "Franz�sische",
   "Franc�s",
  },
  {"French Canadian",
   "Fran�ais Canadien",
   NULL,
   NULL,
  },
  {"German",
   "Allemand",
   "Deutsche",
   "Alem�n",
  },
/*   {"Hebrew", */
/*    "H�breu", */
/*    "Hebr�ische", */
/*    "Hebreo", */
/*   }, */
  {"Italian",
   "Italien",
   "Italienische",
   "Italiano",
  },
/*   {"Japanese", */
/*    "Japonais", */
/*    "Japanische", */
/*    "Japon�s", */
/*   }, */
/*   {"Netherland", */
/*    "Hollandais", */
/*    "Niederl�ndische", */
/*    "de los Pa�ses Bajos", */
/*   }, */
  {"Polish",
   "Polonais",
   "Polnische",
   "Polaco",
  },
  {"Russian",
   "Russe",
   "Russische",
   "Ruso",
  },
  {"Slovaq",
   "Slovaque",
   "Slovenische",
   "Eslovaco",
  },
  {"Spanish",
   "Espagnol",
   "Spanische",
   "Espa�ol",
  },
  {"Swiss",
   "Suisse",
   "Schweizer",
   "Suizo",
  },
  {"Turkish",
   "Turque",
   "T�rkische",
   "Turco",
  },
  {"Delete",
   "Effacement",
   "L�schen",
   "Suprimir/Borrar",
  },
  {"Welcome To Oralux!",
   "Bienvenue sur oraluxe !",
   "Willkommen bei Oralux !",
   "Bienvenido a Oralux !",
  },
  {"Do you want to install the DECtalk software (English, French, German, Spanish)?",
   "Voulez-vous installer le logiciel DECtalk (allemand, anglais, espagnol, fran�ais) ?",
   "Wollen Sie die DECtalk software installieren (Deutsche, Englische, Franz�sische, Spanische)?",
   "�Quiere instalar el programa DECtalk (Alem�n, Espa�ol, Franc�s, Ingl�s)",
  },

  {"Please press the Return key if you agree, or any other key if you disagree",
   "Si oui, appuyer sur la touche Entr�e, ou, sinon, sur n'importe quelle touche",
   "Bitte Eingabetaste dr�cken wenn Sie zustimmen, ansonsten jede andere Taste, wenn nicht",
   "Por favor, presione la tecla Enter, si esta de acuerdo, o cualquier otra tecla, si no lo est�",
  },
  {"Problem with the file PHP status",
   "Probl�me avec le fichier PHP status",
   "Probleme mit der Datei PHP Status",
   "Problema con el fichero PHP status",
  },
  {"The DECtalk package can't be found",
   "Le fichier DECtalk est introuvable",
   "Das DECtalk Paket wird nicht gefunden",
   "El paquete DECtalk no puede ser encontrado",
  },
  {"The Makefile can't be found",
   "Le Makefile est introuvable",
   "Das Makefile wird nicht gefunden",
   "El Makefile no puede ser encontrado",
  },
  {"The shared library is not built",
   "La librairie partag�e n'est pas construite",
   "Die gemeinsame Bibliothek ist nicht erzeugt",
   "La librer�a compartida no ha sido construida/compilada",
  },
  {"The Emacspeak variable is not defined",
   "La variable Emacspik n'est pas d�finie",
   "Die Emacspik Variable ist nicht definiert",
   "La variable Emacspik no ha sido definida",
  },
  {"The software-dtk directory is not found",
   "Le r�pertoire software - dtk est introuvable",
   "Das software-dtk Verzeichnis wird nicht gefunden",
   "El directorio software-dtk no se encuentra",
  },
  {"The dtk directory is not found",
   "Le r�pertoire dtk est introuvable",
   "Das dtk Verzeichnis wird nicht gefunden",
   "El directorio dtk no se encuentra",
  },
  {"Please enter your serial number, and press Return",
   "Entrez s'il vous plait votre num�ro de s�rie, et appuyer sur Entr�e",
   "Bitte Seriennummer eingeben, und die Eingabetaste dr�cken",
   "Por favor, introduzca su n�mero de serie, y presione la tecla Enter",
  },
  {"Sorry, your serial number is not correct",
   "D�sol�, votre num�ro de s�rie est incorrect",
   "Ihre Seriennummer ist nicht korrekt",
   "Lo sentimos, su n�mero de serie no es correcto",
  },
  {"If you want to try again, press Return",
   "Si vous souhaitez r�essayer, appuyer sur la touche Entr�e",
   "Zum Wiederholen Eingabetaste dr�cken",
   "Si quiere probar de nuevo presione la tecla Enter",
  },
  {"If you want to stop this stage, press any other key",
   "Si vous souhaiter terminer cette �tape, appuyez sur une autre touche",
   "Zum stoppen dieses Arbeitsganges, jede andere Taste dr�cken",
   "Si quiere parar en este paso, presione cualquier otra tecla",
  },
  {"Do you want this menu in English ?",
   "Voulez-vous ce menu en anglais ?",
   "Wollen Sie dieses Men� in Englisch ?",
   "�Quiere este men� en Ingl�s ?",
  },
  {"Do you want this menu in French ?",
   "Voulez-vous ce menu en fran�ais ?",
   "Wollen Sie dieses Men� in Franz�sisch ?",
   "�Quiere este men� en Franc�s ?",
  },
  {"y",
   "i grec",
   "y",
   "y",
  },
  {"left angle bracket",
   "inf�rieur �",
   "�ffnende spitze Klammer",
   "menor que",
  },
  {"left curly bracket",
   "accolade ouvrante",
   "�ffnende geschweifte Klammer",
   "abre par�ntesis cuadrada",
  },
  {"left parenthesis",
   "parenth�se ouvrante",
   "�ffnende runde Klammer",
   "abre par�ntesis",
  },
  {"left square bracket",
   "crochet ouvrant",
   "�ffnende eckige Klammer",
   "abre corchetes",
  },
  {"vertical line",
   "ligne verticale",
   "vertikale Linie",
   "l�nea vertical",
  },
  {"space",
   "espace",
   "Leerzeichen",
   "espacio",
  },
  {"at",
   "arobasse",
   "Klammeraffe",
   "arroba",
  },
  {"right curly bracket",
   "accolade fermante",
   "schliessende geschweifte Klammer",
   "cierre corchetes",
  },
  {"What kind of keyboard is used ?",
   "Quel est le type de clavier utilis� ?",
   "Welcher Typ von Tastatur wird benutzt ?",
   "�Cu�l es el tipo de teclado utilizado ?",
  },
  {"Press Enter to increase the volume, or Space to decrease the volume, or any other key to accept it",
   "Appuyer sur Entr�e pour augmenter le volume, ou sur Espace pour diminuer le volume, ou sur toute autre touche pour l'accepter",
   "Eingabetaste dr�cken um die Lautst�rke zu erh�hen, oder Leertaste dr�cken um die Lautst�rke zu verringern, oder jede andere Taste zum beenden",
   "Pulsar enter para subir el volumen, o sobre la barra espaciadora para bajar el volumen, o sobre cualquier teclar para salir",
  },
  {"Do you want to change the type of keyboard ?",
   "Voulez-vous modifier le type de clavier ?",
   "Wollen Sie den Typ der Tastatur �ndern ?",
   "�Desea modificar el tipo de teclado ?",
  },
  {"Your keyboard is supposed to be",
   "Votre clavier est suppos� de type",
   "Ihre Tastatur ist vermutlich",
   "Su teclado debe ser",
  },
  {"Do you want to update your preferences ?",
   "Voulez-vous modifier vos pr�f�rences ?",
   "Wollen Sie Ihre Einstellungen aktualisieren ?",
   "�Desea actualizar sus preferencias ?",
  },
  {"Do you want to eject the CD ROM ?",
   "Voulez-vous �jecter le CD ROM ?",
   "Wollen Sie der CD ROM auswerfen ?",
   "�Quiere salga el CD ROM ?",
  },
  {"Once the CD is taken, out, press Return",
   "Une fois le CD retir�, appuyer sur Entr�e",
   "Wenn die CD ausgeworfen ist, Eingabetaste dr�cken",
   "Una vez retirado el CD, pulsar enter",
  },
  {"Do you want this menu in German ?",
   "Voulez-vous ce menu en allemand ?",
   "Wollen Sie dieses Men� in Deutsch ?",
   "�Quiere este men� en Alem�n ?",
  },
  {"Do you want this menu in Spanish ?",
   "Voulez-vous ce menu en espagnol ?",
   "Wollen Sie dieses Men� in Spanisch ?",
   "�Quiere este men� en Espa�ol ?",
  },
  {"Do you want Emacspeak Festival M Brola (English and French)?",
   "Voulez-vous Emacspik Festival M Brola (anglais et fran�ais)?",
   "Wollen Sie Emacspik Festival M Brola (Englische und Franz�sische)?",
   "�Quiere Emacspik Festival M Brola (Ingl�s y Franc�s)?",
  },

  {"Do you want Flite (English) ?",
   "Voulez-vous Flite (anglais) ?",
   "Wollen Sie Flite (Englische)?",
   "�Quiere Flite (Ingl�s)?",
  },

  {"Yes",
   "Oui",
   "Ja",
   "Si",
  },
  {"No",
   "Non",
   "Nein",
   "No",
  },

  // BRLTTY: drivers
  {"Alva",
   "Alva",
   "Alva",
   "Alva",
  },
  {"BrailleLite",
   "BrailleLite",
   "BrailleLite",
   "BrailleLite",
  },
  {"BrailleNote",
   "BrailleNote",
   "BrailleNote",
   "BrailleNote",
  },
  {"CombiBraille",
   "CombiBraille",
   "CombiBraille",
   "CombiBraille",
  },
  {"EcoBraille",
   "EcoBraille",
   "EcoBraille",
   "EcoBraille",
  },
  {"EuroBraille",
   "EuroBraille",
   "EuroBraille",
   "EuroBraille",
  },
  {"HandyTech",
   "HandyTech",
   "HandyTech",
   "HandyTech",
  },
  {"Log Text",
   "Logue Texte",
   "Log Text",
   "Log Text",
  },
  {"MultiBraille",
   "MultiBraille",
   "MultiBraille",
   "MultiBraille",
  },
  {"MDV",
   "MDV",
   "MDV",
   "MDV",
  },
  {"MiniBraille",
   "MiniBraille",
   "MiniBraille",
   "MiniBraille",
  },
  {"Papen meier",
   "Papene meilleur",
   "Papen meier",
   "Papen meier",
  },
  {"Telesensory Systems Inc.",
   "T�l� sensory Systems Inc.",
   "Telesensory Systems Inc.",
   "Telesensory Systems Inc.",
  },
  {"Vario, R B T (emulation 1)",
   "Vario, R B T (emulation 1)",
   "Vario, R B T (emulation 1)",
   "Vario, R B T (emulation 1)",
  },
  {"VideoBraille",
   "Vid�o Braille",
   "VideoBraille",
   "VideoBraille",
  },
  {"Vario, R B T (emulation 2)",
   "Vario, R B T (emulation 2)",
   "Vario, R B T (emulation 2)",
   "Vario, R B T (emulation 2)",
  },
  {"Voyager",
   "Voyageur",
   "Voyager",
   "Voyager",
  },
  {"VisioBraille",
   "VisioBraille",
   "VisioBraille",
   "VisioBraille",
  },

  // BRLTTY: tables
  {"French c b i f s",
   "Fran�ais c b i f s",
   "Franz�sische cbifs",
   "Franc�s c b i f s",
  },
  {"American N A B C C",
   "Americain N A B C C",
   "Amerikanische N A B C C",
   "Americano N A B C C",
  },
  {"Norwegian and German",
   "Norv�gien et allemand",
   "Norwegere und Deutsche",
   "Noruego y Alem�n",
  },
  {"Norwegian",
   "Norv�gien",
   "Norwegere",
   "Noruego",
  },
  {"Swedish",
   "Su�dois",
   "Schwedische",
   "Sueco",
  },
  {"Swedish 2",
   "Su�dois 2",
   "Schwedische 2",
   "Sueco 2",
  },
  {"Vietnamese",
   "Vietnamien",
   "Vietnamese",
   "Vietnamita",
  },

  {"Do you want to select a braille display ?",
   "Voulez-vous s�lectionner un afficheur braille ?",
   "Wollen Sie eine Blindenschriftanzeige ausw�hlen ?",
   "�Qui�res seleccionar un dispositivo de braille?",
  },

  {"What kind of braille display do you have ?",
   "Quel est le type de votre afficheur braille ?",
   "Welche Art einer Blindenschriftanzeige haben Sie ?",
   "�Qu� tipo de dispositivo de braille tienes?",
  },

  {"First serial port",
   "Premier port s�rie",
   "Erste serielle Schnittstelle",
   "Primer puerto serie",
  },

  {"Second serial port",
   "Deuxi�me port s�rie",
   "Zweite serielle Schnittstelle",
   "Segundo puerto serie",
  },

  {"Which table do you want ?",
   "Quelle table voulez-vous ?",
   "Welche Tabelle w�nschen Sie ?",
   "�Qu� tabla quieres?",
  },

  {"Which is the port ?",
   "Quel est le port ?",
   "Welche ist die Schnittstelle ?",
   "�Cu�l es el puerto ?",
  },

  {"Do you want to change the type of braille display ?",
   "Voulez-vous modifier le type d'afficheur braille ?",
   "M�chten Sie die Art der Blindenschriftanzeige �ndern ?",
   "�Qui�res cambiar el tipo de pantalla de braille ?",
  },

  {"Your braille display is supposed to be",
   "Votre afficheur braille est suppos� de type",
   "Ihre Blindenschriftanzeige k�nnte sein",
   "Tu pantalla de braille debe ser del tipo",
  },

  {"Do you want to change the table ?",
   "Voulez-vous modifier la table ?",
   "M�chten Sie die Tabelle �ndern ?",
   "�Qui�res cambiar la tabla ?",
  },

  {"The braille table is supposed to be",
   "La table braille est suppos� de type",
   "Die Blindenschrifttabelle k�nnte sein",
   "La tabla de braille debe ser del tipo",
  },

  {"Do you want to change the port ?",
   "Voulez-vous modifier le port ?",
   "M�chten Sie die Schnittstelle �ndern ?",
   "�Qui�res cambiar el puerto ?",
  },

  {"The communication port is supposed to be",
   "Le port de communication est suppos� de type",
   "Die serielle Schnittstelle k�nnte sein",
   "El puerto de comunicaci�n debe ser del tipo",
  },

  {"Do you want to start your braille display ?",
   "Voulez-vous d�marrez votre afficheur braille ?",
   "M�chten Sie Ihre Blindenschriftanzeige beginnen ?",
   "�Qui�res iniciar tu pantalla de braille?",
  },

  {
    "Accent S A",
    "Accent S A",
    "Accent S A",
    "Accent S A",
  },

  {
    "Braille Lite",
    "Braille Lite",
    "Braille Lite",
    "Braille Lite",
  },

  {
    "Braille 'n Speak",
    "Braille 'n Spik",
    "Braille 'n Spik",
    "Braille 'n Spik",
  },

  {
    "Ciber 232",
    "Ciber 232",
    "Ciber 232",
    "Ciber 232",
  },

  {
    "Ciber 232 Plus",
    "Ciber 232 Plus",
    "Ciber 232 Plus",
    "Ciber 232 Plus",
  },

  {
    "DECtalk Express",
    "DECtalk Express",
    "DECtalk Express",
    "DECtalk Express",
  },

  {
    "DECtalk Multivoice",
    "DECtalk Multivoice",
    "DECtalk Multivoice",
    "DECtalk Multivoice",
  },

  {
    "DECtalk 3",
    "DECtalk 3",
    "DECtalk 3",
    "DECtalk 3",
  },

  {
    "Dolphin Apollo",
    "Dolphin Apollo",
    "Dolphin Apollo",
    "Dolphin Apollo",
  },

  {
    "DoubleTalk LT",
    "DoubleTalk LT",
    "DoubleTalk LT",
    "DoubleTalk LT",
  },

  {
    "DoubleTalk PC",
    "DoubleTalk PC",
    "DoubleTalk PC",
    "DoubleTalk PC",
  },

  {
    "LiteTalk",
    "LiteTalk",
    "LiteTalk",
    "LiteTalk",
  },

  {
    "PC Hablado notebook",
    "PC Hablado notebook",
    "PC Hablado notebook",
    "PC Hablado notebook",
  },

  {
    "Type 'n Speak",
    "Type 'n Spik",
    "Type 'n Spik",
    "Type 'n Spik",
  },

  {"Do you want to select an external voice synthesizer ?",
   "Voulez-vous s�lectionner une synth�se vocale externe ?",
   "Wollen Sie einen externen Sprachsynthesizer ausw�hlen?",
   "�Quieres seleccionar un sintetizador de voz externo?",
  },

  {"What kind of synthesizer do you have ?",
   "Quelle type de synth�se avez-vous ?",
   "Was f�r einen Sprachsynthesizer haben Sie?",
   "�Qu� tipo de sintetizador tienes?",
  },

  {"Your external synthesizer is supposed to be",
   "Votre synth�se externe est suppos� de type",
   "Ihr externer Sprachsynthesizer sollte sein:",
   "Se supone que tu sintetizador externo es",
  },

  {"Do you want to change the type of external synthesizer ?",
   "Voulez-vous modifier le type de synth�se externe ?",
   "Wollen Sie einen anderen externen Sprachsynthesizer ausw�hlen?",
   "�Quieres cambiar el tipo de sintetizador externo?",
  },

  {
    "Emacspeak",
    "Emacspik",
    "Emacspik",
    "Emacspik",
  },

  {
    "Yasr",
    "Yasse R",
    "Yasr",
    "Yasr",
  },

  {"Your desktop is supposed to be",
   "Votre bureau est suppos� �tre",
   "Ihr Desktop sollte sein:",
   "Se supone que tu escritorio es",
  },

  {"Do you want to change the kind of desktop ?",
   "Voulez-vous modifier le type de bureau ?",
   "Wollen Sie einen anderen Desktop ausw�hlen?",
   "�Quieres cambiar el tipo de escritorio?",
  },

  {"Which desktop do you want ?",
   "Quel bureau voulez-vous ?",
   "Was f�r einen Desktop wollen Sie?",
   "�Qu� escritorio quieres?",
  },

  {"Do you want ParleMax (French) ?",
   "Voulez-vous ParleMax (Fran�ais) ?",
   "Wollen Sie ParleMax (Franz�sische)?",
   "�Quieres ParleMax (Franc�s)?",
  },

  {"Do you want to reboot your PC?",
   "Voulez-vous red�marrez votre PC ?",
   "Wollen Sie den Computer neu starten?",
   "�Quieres reiniciar tu PC?",
  },

  {"Do you want to shutdown your PC?",
   "Voulez-vous arr�tez votre PC ?",
   "Wollen Sie den Computer herunterfahren?",
   "�Quieres apagar tu PC?",
  },

  {"Do you want MultiSpeech (English and Russian) ?",
   "Voulez-vous MultiSpeech (anglais et russe) ?",
   "Wollen Sie MultiSpeech (Englische und Russische) ?",
   "�Quieres MultiSpeech (Ingl�s y Ruso) ?",
  },

  {"Do you really want to stop the Braille driver ?",
   "Voulez-vous vraiment arr�tez le pilote Braille ?",
   "Wollen Sie wirklich den Braille-Treiber beenden?",
   "�Quieres realmente parar el driver del Braille?",
  },

  {"Do you want this menu in Russian ?",
   "Voulez-vous ce menu en russe ?",
   "Wollen Sie dieses Men� in Russische ?",
   "�Quiere este men� en Ruso ?",
  },

  {"0.6 beta", // "[:pu all] 0.6 beta",
   "0.6 beta",
   "0.6 beta",
   "0.6 beta",
  },

  {"Do you want to change the keyboard features?",
   "Voulez-vous changer les caract�ristiques du clavier ?",
   "M�chten sie die Tastatureinstellungen �ndern?",
   "�Quiere cambiar la configuraci�n del teclado?",
  },

  {"You can hold three keys at a time, for example Control Alt Del",
   "Pouvez-vous appuy�es sur trois touches en m�me temps, par exemple Control Alt Del",
   "Sie k�nnen drei Tasten gleichzeitig halten; zum Beispiel Strg Alt Entf",
   "Puede pulsar tres teclas a la vez, por ejemplo Control Alt Supr.",
  },

  {"You may prefer to hold one key at a time",
   "Vous pr�f�rez appuyer sur une touche � la fois",
   "Sie wollen nur eine Taste gedr�ckt halten",
   "Prefiere pulsar una tecla cada vez",
  },

  {"Automatically repeat the pressed key",
   "R�p�ter automatiquement la touche appuy�e",
   "Automatisch die gedr�ckte Taste wiederholen",
   "Repetir Autom�ticamente la tecla pulsada",
  },

  {"Do not automatically repeat the pressed key",
   "Ne pas r�p�ter automatiquement la touche appuy�e",
   "Die Taste nicht automatisch wiederholen",
   "No Repetir Autom�ticamente la tecla pulsada",
  },

  {"Do you want to configure your internet connection?",
   "Voulez-vous configurer votre connexion internet ?",
   "M�chten sie den Internetzugang konfigurieren?",
   "�Desea configurar su conexi�n a Internet?",
  },

  {"Configure an external serial modem",
   "Configurer un modem externe s�rie ",
   "Konfigurieren eines externen Modems an der seriellen Schnittstelle.",
   "Configurar un modem externo en un puerto serie",
  },

  {"Install an A D S L  U S B Modem",
   "Installer un MODEM A D S L  U S B",
   "Ein ADSL-Modem mit USB-Schnittstelle einrichten.",
   "Instalar un Modem A D S L  U S B",
  },

  {"Set up e-mail",
   "Configurer le courriel",
   "E-Mail einrichten",
   "Configurar Correo",
  },

  {"Do you want ViaVoice?",
   "Voulez-vous ViaVoice ?",
   "Wollen Sie ViaVoice?",
   "�Quiere ViaVoice?",
  },

  {"The ViaVoice package can't be found",
   "Le fichier ViaVoice est introuvable",
   "Das ViaVoice Paket wird nicht gefunden",
   "El paquete ViaVoice no puede ser encontrado",
  },

  {"The IBMECI directory is not found",
   "Le r�pertoire IBMECI est introuvable",
   "Das IBMECI Verzeichnis wird nicht gefunden",
   "El directorio IBMECI no se encuentra",
  },

  {"Please enter your password, and press Return",
   "Entrez s'il vous plait votre mot de passe, et appuyer sur Entr�e",
   "Bitte Passwort eingeben, und die Eingabetaste dr�cken",
   "Por favor, introduzca su contrase�a, y presione la tecla Enter",
  },

  {"Do you want to install another language?",
   "Voulez-vous installer une autre langue ?",
   "M�chten Sie eine weitere Sprache installieren?",
   "�Desea instalar otro idioma?",
  },

  {"Sorry, your password is not correct",
   "D�sol�, votre mot de passe est incorrect",
   "Ihr Passwort ist nicht korrekt",
   "Lo sentimos, su contrase�a no es correcto",
  },

  {"Previous",
   "Pr�c�dant",
   "Zur�ck/vorherige Seite",
   "Anterior",
  },

  {"Next",
   "Suivant",
   "Weiter/n�chste Seite",
   "Siguiente",
  },

  {"Welcome To OraluxGold!",
   "Bienvenue sur oraluxGold!",
   "Willkommen bei OraluxGold!",
   "Bienvenido a OraluxGold !",
  },

  {"Do you want to save the configuration files?",
   "Souhaitez-vous sauvegarder les fichiers de configuration ?",
   "Wollen Sie Konfigurationsdateien speichern?",
   "�Desea guardar los archivos de configuraci�n�?",
  },

  {"Albatross",
   "Albatross",
   "Albatross",
   "Albatross",
  },

  {"FreedomScientific",
   "FreedomScientific",
   "FreedomScientific",
   "FreedomScientific",
  },

  {"B R F",
   "B R F",
   "B R F",
   "B R F",
  },

  {"Finnish 2",
   "Finlandais 2",
   "Finnische 2",
   "Finland�s 2",
  },

  {"U S B",
   "U S B",
   "U S B",
   "U S B",
  },

  {"Which contraction table do you want?",
   "Quelle table abr�g�e voulez-vous ?",
   "Welche Kurzschrifttabelle m�chten Sie nutzen?",
   "�Qu� tabla de contracci�n quieres?",
  },

  {"No contraction table",
   "Pas de table abr�g�e",
   "Keine Kurzschrifttabelle",
   "No contraction table",
  },

  {"big 5",
   "big 5",
   "big 5",
   "big 5",
  },

  {"compress",
   "compress",
   "compress",
   "compress",
  },

  {"Grade 2 US English",
   "Grade 2 US English",
   "Grade 2 US English",
   "Grade 2 US English",
  },

  {"FR abrege",
   "Fran�ais abr�g�",
   "FR abrege",
   "FR abrege",
  },

  {"FR integral",
   "Fran�ais int�gral",
   "FR integral",
   "FR integral",
  },

  {"The contraction braille table is supposed to be",
   "La table abr�g�e braille est suppos�e de type",
   "Die Kontraktionsbrailletabelle ist bestimmt f�r",
   "La tabla de contracci�n de braille debe ser del tipo",
  },

  {"Do you want to change the contraction table?",
   "Voulez-vous modifier la table abr�g�e ?",
   "Wollen Sie die Kontraktionstabelle �ndern?",
   "�Qui�res cambiar la tabla de contracci�n?",
  },

  {"Which contraction table do you want?",
   "Quelle table abr�g�e voulez-vous ?",
   "Welche KontraktionTabelle wollen sie?",
   "�Qu� tabla de contracci�n quieres?",
  },

  {"Do you want Cicero T T S (French)?",
   "Voulez-vous Cic�ro T T S (fran�ais) ?",
   "Wollen Sie Cicero T T S (Franz�sische)?",
   "�Quieres Cicero T T S (Franc�s)?",
  },

  {"The audio menu is going to start Yasr",
   "Le menu audio va lancer Yasr",
   NULL,
   "El audio menu ejecutar� el programa Yasr",
  },

  {"If the voice synthesizer stops, please press Control Alt i to start it again",
   "Si la synth�se vocale s'arr�te, appuyez s'il vous plait sur Control Alt i pour la relancer",
   NULL,
   "Si el sintetizador de voz se detiene, por favor pulse CTRL+Alt+I para
ejecutarlo de nuevo",
  },
};

/* > */

/* < getTTY */

int getTTY()
{
  return pf;
}

/* > */
/* < initAUI */

int initAUI(char* theOggDirectory, enum language theLanguage, char* thePortName)
{
  ENTER("initAUI");

  ThePortName=thePortName;
  if (thePortName)
    {
      pf = open(thePortName, O_RDWR);
      if (pf < 0)
	{
	  pf=0;
	  fprintf(stderr, "pb when opening tty %s\n", thePortName);
	  exit(1);
	}
    }
  
  TheCurrentLanguage=theLanguage;

  strcpy(TheOggDirectory, theOggDirectory);
  audioFifoInit();
  setConsoleFont(theLanguage);
  return 1;
}

/* > */
/* < stopAUI */

void stopAUI(int theSoundMustBeFinished)
{
  ENTER("stopAUI");
  SHOW2("theSoundMustBeFinished=%d",theSoundMustBeFinished);
  audioFifoQuit(theSoundMustBeFinished);
  if (pf)
    {
      close(pf);
    }
}

/* > */
/* < restartAUI */

int restartAUI()
{
  ENTER("restartAUI");
  initAUI( TheOggDirectory, TheCurrentLanguage, ThePortName);
}

/* > */
/* < beep */

void beep()
{
  ENTER("beep");
  static char buf[BUFSIZE];
  if (!TheSoundIsEnable) 
    {
      return;
    }
  //  sprintf(buf,"stty -isig;ogg123 -q %s/beep.ogg", TheOggDirectory);
  sprintf(buf,"%s/beep.ogg", TheOggDirectory);
  playOggVorbisFile(buf,1,0);
}

/* > */
/* < enableSound */

void enableSound(int the_sound_is_enable)
{
  ENTER("enableSOUND");
  TheSoundIsEnable=the_sound_is_enable;
}

/* > */
/* < getTextForThisLanguage */

char* getTextForThisLanguage(enum sentence theSentence, enum language theLanguage)
{
  ENTER("getText");  
  char *aText=NULL;
  if (theLanguage==Russian)
    {
      aText=TheRussianMessages[theSentence].myTranslatedSentence;
    }
  else
    {
      aText=TheMessages[theSentence][theLanguage];
    }
  return aText;
}

/* > */
/* < getText */

char* getText(enum sentence theSentence)
{
  ENTER("getText");  
  return getTextForThisLanguage(theSentence, TheCurrentLanguage);
}

/* > */
/* < saySentence */

static void saySentence(enum sentence theSentence, int theCallIsSynchronous)
{
  ENTER("saySentence");
  static char buf[BUFSIZE];

  SHOW2("the sentence=%d\n",theSentence);
  if (!TheSoundIsEnable) 
    {
      return;
    }
  char *aText=getText(theSentence);
  if (aText==NULL)
    {
      aText=getTextForThisLanguage(theSentence, English);
    }
  fprintf(stdout, "%s\n", aText);
  playOggVorbisFile(getOggFilename(theSentence),0,theCallIsSynchronous);
}

/* > */
/* < sayChar */

void sayChar(char c)
{
  ENTER("sayChar");
  static char buf[BUFSIZE];
  if (!TheSoundIsEnable) return;
  printf("%c",c);

  switch(c)
  {
    case 'y' :
    case 'Y' :
      saySentence(y,0);
      break;
    case '<' :
      saySentence(leftAngleBracket,0);
      break; 
    case '{' :
      saySentence(leftCurlyBracket,0);
      break; 
    case '(' :
      saySentence(leftParenthesis,0);
      break; 
    case '[' :
      saySentence(leftSquareBracket,0);
      break; 
    case '|' :
      saySentence(verticalLine,0);
      break; 
    case ' ' :
      saySentence(space,0);
      break; 
    case '@' :
      saySentence(at,0);
      break; 
    case '}' :
      saySentence(rightCurlyBracket,0);
      break; 
    default:
      playOggVorbisFile(getOggFilenameChar(c),0,0);
      break;
  }
}

/* > */
/* < say */

void say(enum sentence theSentence)
{
  ENTER("say");
  saySentence(theSentence,0);
}

/* > */
/* < sayForce */

void sayForce(enum sentence theSentence)
{
  ENTER("sayForce");
  int aFlag=TheSoundIsEnable;
  TheSoundIsEnable=1;
  clearAll();
  saySentence(theSentence, 1);
  clearAll();
  TheSoundIsEnable=aFlag;
}

/* > */
/* < sayAgain */

void sayAgain()
{
  ENTER("sayAgain");
  resayAll();
}

/* > */
/* < clearStoredSentences */

void clearStoredSentences()
{
  ENTER("clearStoredSentences");
  clearAll();
}

/* > */
/* < setLanguage */

void setLanguage( enum language theLanguage)
{
  ENTER("setLanguage");
  TheCurrentLanguage=theLanguage;
  setConsoleFont(theLanguage);
}

/* > */
/* < getOggFilename */

char* getOggFilename(enum sentence theSentence)
{
  ENTER("getOggFilename");
  static char buf[BUFSIZE];
  sprintf(buf,"%s/s%02x%s.ogg", TheOggDirectory, (unsigned int)theSentence, TheLanguageSuffix[TheCurrentLanguage]);
  SHOW(buf);
  return buf;
}

/* > */
/* < getOggFilenameForThisLanguage */

char* getOggFilenameForThisLanguage(enum sentence theSentence, enum language theLanguage)
{
  ENTER("getOggFilenameForThisLanguage");
  static char buf[BUFSIZE];
  sprintf(buf,"%s/s%02x%s.ogg", TheOggDirectory, (unsigned int)theSentence, TheLanguageSuffix[theLanguage]);
  SHOW(buf);
  return buf;
}

/* > */
/* < getOggFilenameBeep */

char* getOggFilenameBeep()
{
  ENTER("getOggFilenameBeep");
  return NULL;
}

/* > */
/* < getOggFilenameChar */

char* getOggFilenameChar(char c)
{
  ENTER("getOggFilenameChar");
  static char buf[BUFSIZE];

  SHOW2("char=%c\n",c);

  if (c<0x20)
  {
    return NULL;
  }

  if ((c>=0x41) && (c<=0x5a))
  {
    c+=0x20; // lower case only
  }

  enum language aLanguage=TheCurrentLanguage;
  if (aLanguage==Russian)
    { // At the moment, there is no specific file for the letters in Russian
      // because the characters must be said when the DECtalk serial number 
      // is typed and this is composed of latin characters.
      aLanguage=English;
    }

  sprintf(buf,"%s/c%2x%s.ogg", TheOggDirectory, (unsigned int)c, TheLanguageSuffix[aLanguage]);
  SHOW(buf);
  return buf;
}

/* > */
/* < playOGG */

void playOGG(char *theFilename)
{
  ENTER("playOGG");
  static char buf[1024];
  sprintf(buf,"%s/%s", TheOggDirectory, theFilename);
  playOggVorbisFile(buf,0,0);
}

/* > */
/* < beepLoudSpeaker */

void beepLoudSpeaker()
{
  ENTER("beepLoudSpeaker");
  system("echo -e \"\007\"");
}

/* > */
