#define BAUDRATE 3125;
#define DEBOUNCE 1500;

//midi settings
int noteOn = 144; //channel 1 note on
int noteOff = 128; //channel 1 note off

int bassMidiCh = 1;
int keyMidiCh = 2;
   
//display leds
int bassToBassLedPin = 5;
int bassToKeyLedPin = 6;

int organToBassLedPin = 7;
int organToKeyLedPin = 8;
int organAudioLedPin = 9;

int bassLowNoteLedPin = 10;
int keyHighNoteLedPin = 23;
int keyHighThreeLedPin = 23

//buttons
struct button{
  int pin;
  int debounce;
  int value;
  bool pressed;
};

struct button bassToBassBtn = {23, 0, 0, false};
struct button bassToKeyBtn = {23, 0, 0, false};

struct button organToBassBtn = {23, 0, 0, false};
struct button organToKeyBtn = {23, 0, 0, false};
struct button organAudioBtn = {23, 0, 0, false};

struct button bassLowNoteBtn = {23, 0, 0, false};
struct button keyNoteBtn = {23, 0, 0, false};

//stored values
bool bassToBass = true;
bool bassToKey = false;
bool organToBass = false;
bool organToKey = false;
bool organAudio = true;

int bassPrioritySetting = 0;
int keyPrioritySetting = 0;

struct midiNote{
  int command;
  int note;
  int velocity;
};

struct midiNote lowestNote = {0, 0, 0};
int bassPrioCycles = 12;
int bassPrioCycle = 0;

void setPins(){
	pinMode(bassToBassLedPin, OUTPUT);
	pinMode(bassToKeyLedPin, OUTPUT);

	pinMode(organToBassLedPin, OUTPUT);
	pinMode(organToKeyLedPin, OUTPUT);
	pinMode(organAudioLedPin, OUTPUT);

	pinMode(bassLowNoteLedPin, OUTPUT);
	pinMode(keyHighNoteLedPin, OUTPUT);
	pinMode(keyHighThreeLedPin, OUTPUT);

	pinMode(bassToBassBtn.pin, INPUT_PULLUP);
	pinMode(bassToKeyBtn.pin, INPUT_PULLUP);

	pinMode(organToBassBtn.pin, INPUT_PULLUP);
	pinMode(organToKeyBtn.pin, INPUT_PULLUP);
	pinMode(organAudioBtn.pin, INPUT_PULLUP);

	pinMode(bassLowNoteBtn.pin, INPUT_PULLUP);
	pinMode(keyNoteBtn.pin, INPUT_PULLUP);
}

void setup(){
	Serial1.begin(31250);
	Serial2.begin(31250);
	setPins();
}

void changeBassPriority(){
	if(++bassPrioritySetting == 2){//set the current bassPrioritySetting + 1 and if that is 2 then reset to 0 because this only has 2 states
		bassPrioritySetting = 0;
	}
	handleBassPriorityLed();
}

void changeKeyPriority(){
	if(++keyPrioritySetting == 3){//set the current keyPrioritySetting + 1 and if that is 3 then reset to 0 because this only has 3 states 
		keyPrioritySetting = 0;
	}
	handleKeyPriorityLeds();
}

void handleBassPriorityLed(){
	switch(bassPrioritySetting){
		case 0:
			digitalWrite(bassLowNoteLedPin, LOW);
		break;
		case 1:
			digitalWrite(bassLowNoteLedPin, HIGH);
		break;
	}
}

void handleKeyPriorityLeds(){
	switch(bassPrioritySetting){
		case 0:
			digitalWrite(keyHighNoteLedPin, LOW);
			digitalWrite(keyHighThreeLedPin, LOW);
		break;
		case 1:
			digitalWrite(keyHighNoteLedPin, HIGH);
			digitalWrite(keyHighThreeLedPin, LOW);
		break;
		case 2:
			digitalWrite(keyHighNoteLedPin, LOW);
			digitalWrite(keyHighThreeLedPin, HIGH);
		break;
	}
}

void handleButton(struct button btn){
	btn.pressed = false;
	btn.value = digitalRead(btn.pin); 
	if(btn.debounce == 0){ //button has been off
		if(btn.value == LOW){ //button is on now
			btn.debounce = DEBOUNCE;
			btn.pressed = true;
		}
		else{
			if(btn.value == HIGH){ //button is off now
				--btn.debounce; //handle debounce
			}
			else{ //button is still on
				btn.debounce = DEBOUNCE;
			}
		}
	}

	return btn;
}

void handleLed(pin, state){
	if(state){
		digitalWrite(pin, HIGH);
	}
	else{
		digitalWrite(pin, LOW);
	}
}

void handleSettings(){ //handle what buttons are pressed and what settings should change according
	bassToBassBtn = handleButton(bassToBassBtn);
	if(bassToBassBtn.pressed){ //if the button is pressed
		bassToBass = !bassToBass; //flip the setting boolean
		handleLed(bassToBassLedPin, bassToBass); //set the led to the correct state
	}
	
	
	bassToKeyBtn = handleButton(bassToKeyBtn);
	if(bassToKeyBtn.pressed){ //if the button is pressed
		bassToKey = !bassToKey; //flip the setting boolean
		handleLed(bassToKeyLedPin, bassToKey); //set the led to the correct state
	}

	
	organToBassBtn = handleButton(organToBassBtn);
	if(organToBassBtn.pressed){ //if the button is pressed
		organToBass = !organToBass; //flip the setting boolean
		handleLed(organToBassLedPin, organToBass); //set the led to the correct state
	}

	
	organToKeyBtn = handleButton(organToKeyBtn);
	if(organToKeyBtn.pressed){ //if the button is pressed
		organToKey = !organToKey; //flip the setting boolean
		handleLed(organToKeyLedPin, organToKey); //set the led to the correct state
	}

	
	organAudioBtn = handleButton(organAudioBtn);
	if(organAudioBtn.pressed){ //if the button is pressed
		organAudio = !organAudio; //flip the setting boolean
		handleLed(organAudioLedPin, organAudio); //set the led to the correct state
	}

	
	bassLowNoteBtn = handleButton(bassLowNoteBtn);
	if(bassLowNoteBtn.pressed){ //if the button is pressed
		changeBassPriority(); //change the priority setting, which also sets the led
	}

	
	keyNoteBtn = handleButton(keyNoteBtn);
	if(keyNoteBtn.pressed){ //if the button is pressed
		changeKeyPriority(); //change the priority setting, which also sets the led
	}
}

void checkMidi(HardwareSerial &serial){
	do{
		if (serial.available()){
			return midi {
				serial.read(),//read the first byte which is the command(noteOn/noteOff)
				serial.read(),//read the seccond byte which is the note value
				serial.read()//read the last byte which is the velocity
			}
		}
	}
	while(serial.available() > 2)
	return midi {0,0,0};	
}

void sendMidi(HardwareSerial &serial, struct midiNote midi){
	serial.write(midi.command);
	serial.write(midi.note);
	serial.write(midi.velocity);
}

void handleMidiNote(struct midiNote midi, int channel){
	if(midi.command >= 144 && midi.command <= 159){ //if it is a midi on note on any of the channels
		midi.command = noteOn  + (channel - 1);
	}

	if(midi.command >= 128 && midi.command <= 143){ //if it is a midi off note on any of the channels
		midi.command = noteOff  + (channel - 1);
	}

	return midi;
}

void handleBassMidi(struct midiNote midi){
	if(bassPrioritySetting){
		if(++bassPrioCycle =< bassPrioCycles){
			if(midi.note < lowestNote.note || lowestNote.note == 0){
				lowestNote = midi;
			}
		}
		else{
			sendMidi(Serial1, handleMidiNote(lowestNote, bassMidiCh));
			lowestNote = {0, 0, 0};
			bassPrioCycle = 0;
		}
	}
	else{
		sendMidi(Serial1, handleMidiNote(midi, bassMidiCh));
	}
}

void handleMidi(){
	//first set up the routing, we'll worry about the priority later

	bassMidi = checkMidi(Serial1); //check the bass midi and return it if there is a note
	if(bassMidi.command != 0){ //if there is midi comming in from bass

		if(bassToBass){
			handleBassMidi(bassMidi);
		}

		if(bassToKey){
			sendMidi(Serial2, handleMidiNote(bassMidi, keyMidiCh));
		}


	}

	keyMidi = checkMidi(Serial2); //check the bass midi and return it if there is a note
	if(keyMidi.command != 0){ //if there is midi comming in from bass
		if(keyToBass){
			sendMidi(Serial1, handleMidiNote(keyMidi, bassMidiCh)); //send the key midi to the bass
		}

		if(keyToKey){
			sendMidi(Serial2, handleMidiNote(keyMidi, keyMidiCh)); // send de key midi to the keys
		}
	}
}

void handleOrganAudio(){
	if(organAudio){
		//set something that lets the audio of the organ trough. Maybe a transistor or something
	}
	else{
		//the opposite of the if, stop the audio from getting trough	
	}
}

void loop(){
	handleSettings(); //this handles the buttons and leds for the settings

	handleMidi(); //this handles the routing

	handleOrganAudio(); //this handles if the organ is muted or not
}
