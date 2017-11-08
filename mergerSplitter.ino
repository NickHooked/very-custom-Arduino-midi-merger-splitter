#define BAUDRATE 3125;
#define DEBOUNCE 1500;
   
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

void handleButton(button){
	button.pressed = false;
	button.value = digitalRead(button.pin); 
	if(button.debounce == 0){ //button has been off
		if(button.value == LOW){ //button is on now
			button.debounce = DEBOUNCE;
			button.pressed = true;
		}
		else{
			if(button.value == HIGH){ //button is off now
				--button.debounce; //handle debounce
			}
			else{ //button is still on
				button.debounce = DEBOUNCE;
			}
		}
	}

	return button;
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

void checkBassMidi(){
	do{
		if (Serial1.available()){
			return midiNote {
				Serial1.read(),//read the first byte which is the command(noteOn/noteOff)
				Serial1.read(),//read the seccond byte which is the note value
				Serial1.read()//read the last byte which is the velocity
			}
		}
	}
	while(Serial1.available() > 2)
	return midiNote {0,0,0};	
}

void sendBassMidi(midi){
	if(bassToBass){
		Serial1.write(midi.command);
		Serial1.write(midi.note);
		Serial1.write(midi.velocity);
	}

	if(bassToKey){
		Serial2.write(midi.command);
		Serial2.write(midi.note);
		Serial2.write(midi.velocity);
	}
}

void checkKeyMidi(){
	do{
		if (Serial2.available()){
			return midiNote {
				Serial2.read(),//read the first byte which is the command(noteOn/noteOff)
				Serial2.read(),//read the seccond byte which is the note value
				Serial2.read()//read the last byte which is the velocity
			}
		}
	}
	while(Serial2.available() > 2)
	return midiNote {0,0,0};	
}

void sendKeyMidi(midi){
	if(keyToBass){
		Serial1.write(midi.command);
		Serial1.write(midi.note);
		Serial1.write(midi.velocity);
	}

	if(keyToKey){
		Serial2.write(midi.command);
		Serial2.write(midi.note);
		Serial2.write(midi.velocity);
	}
}

void handleMidi(){
	//first set up the routing, we'll worry about the priority later

	bassMidi = checkBassMidi(); //check the bass midi and return it if there is a note
	if(bassMidi.command != 0){ //if there is midi comming in from bass
		sendBassMidi(bassMidi); //send it to the correct places
	}

	keyMidi = checkKeyMidi(); //check the bass midi and return it if there is a note
	if(keyMidi.command != 0){ //if there is midi comming in from bass
		sendKeyMidi(keyMidi); //send it to the correct places
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

void setup(){
	setPins();
}

void loop(){
	handleSettings(); //this handles the buttons and leds for the settings

	handleMidi(); //this handles the routing

	handleOrganAudio(); //this handles if the organ is muted or not
}
