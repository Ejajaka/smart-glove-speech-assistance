const int sensorPins[5] = {A0, A1, A2, A3, A4};
int threshold = 800;
int bits[5];

String currentWord = "";
String sentence = "";

unsigned long zeroStart = 0;
unsigned long activeStart = 0;
unsigned long lastAction = 0;

bool zeroState = false;
bool gestureLatched = false;
int currentCode = 0;

const unsigned long gestureHold = 1000; 
const unsigned long wordPause  = 3000; 
const unsigned long finalDelay = 2000; 

void setup() {
  Serial.begin(9600);
  delay(2000); // wait for serial to stabilize
  Serial.println("Sentence Builder Ready");
}

void loop() {
  int code = 0;
  bool allZero = true;

  for (int i = 0; i < 5; i++) {
    int val = analogRead(sensorPins[i]);
    bits[i] = (val > threshold) ? 1 : 0;
    if (bits[i] == 1) allZero = false;
  }

  for (int i = 0; i < 5; i++) {
    code = (code << 1) | bits[i];
  }

  unsigned long now = millis();

  if (!allZero && code > 0 && code <= 32) {
    if (code != currentCode) {
      currentCode = code;
      activeStart = now;
      gestureLatched = false;
    } 
    else if (!gestureLatched && (now - activeStart >= gestureHold)) {
      handleGesture(code);
      gestureLatched = true;
      lastAction = now;
    }
    zeroState = false;
  }

  // ---- Word completion on inactivity ----
  if (allZero) {
    if (!zeroState) {
      zeroStart = now;
      zeroState = true;
      currentCode = 0;
    }

    if ((now - zeroStart >= wordPause) && currentWord.length() > 0) {
      sentence += currentWord + " ";
      Serial.println("Word ended automatically (space added).");
      currentWord = "";
      lastAction = now;
    }

    if (sentence.length() > 0 && (now - lastAction >= finalDelay)) {
      Serial.print("FINAL SENTENCE: ");
      Serial.println(sentence);
      sentence = "";
    }
  }

  delay(50);
}

void handleGesture(int code) {
  if (code >= 1 && code <= 26) {
    // Skip H entirely (code 8)
    if (code == 8) return;

    char letter = 'A' + (code - 1);
    currentWord += letter;
    Serial.print("Letter Added: ");
    Serial.println(letter);
  } 
  else if (code == 27) {  
    sentence += currentWord + " ";
    currentWord = "";
    Serial.println("Space Added (Word Ended)");
  } 
  else if (code == 28) {
    sentence += currentWord + " HELLO ";
    currentWord = "";
    Serial.println("Special Word Inserted: HELLO");
  } 
  else if (code == 29) { 
    sentence += currentWord + " I AM ";
    currentWord = "";
    Serial.println("Special Word Inserted: I AM");
  } 
  else if (code == 30) {  
    sentence += currentWord + " BYE ";
    currentWord = "";
    Serial.println("Special Word Inserted: BYE");
  } 
  else if (code == 31) {  
    sentence += currentWord + " VOICE ASSISTANT ";
    currentWord = "";
    Serial.println("Special Word Inserted: VOICE ASSISTANT");
  }
}
