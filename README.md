# Smart Glove for Speech Assistance

A low-cost wearable that helps people with speech impairments communicate. Five pressure sensors on a glove, one per finger, form a 5-bit code: a specific combination of finger presses maps to a letter or a whole phrase, which is spoken aloud in real time.

Built as a team project with Roshna George and Pranav Sajith for the Procedural Programming using C course at Amrita Vishwa Vidyapeetham.

## How it works

1. Five analog pressure sensors (A0 to A4) are read continuously by an Arduino Uno.
2. Each reading is thresholded into a 0 or 1, giving a 5-bit code (32 combinations).
3. A press must be held briefly, with debounce, before it registers, so accidental touches are not read as letters.
4. Codes 1 to 26 map to A to Z. A few reserved codes map to whole phrases (HELLO, I AM, BYE, VOICE ASSISTANT) so common things can be said in a single gesture.
5. Completed words are strung into a sentence. After a pause the Arduino prints `FINAL SENTENCE: ...` over serial.
6. A small C program on the host listens on the serial port and, when it sees a finished sentence, calls eSpeak to speak it.

## Repository structure

```
smart-glove-speech-assistance/
├── arduino/
│   └── sketch_sep30a.ino   reads sensors, builds letters, words and sentences
└── host/
    └── live_tts.c          reads the Arduino serial output, triggers eSpeak
```

## Tech stack

- Arduino Uno (ATmega328P), Arduino IDE
- C for both the embedded logic and the host program
- Linux with the eSpeak TTS engine
- Serial communication over USB at 9600 baud

## Running it

**Arduino side**

1. Wire five pressure sensors to A0-A4 on an Arduino Uno.
2. Open `arduino/sketch_sep30a.ino` in the Arduino IDE and upload it.

**Host side (Linux)**

```bash
gcc host/live_tts.c -o live_tts
./live_tts
```

Install eSpeak first (`sudo apt install espeak`) and update `SERIAL_PORT` in `live_tts.c` if your Arduino appears on a different port than `/dev/ttyACM1`.

## Notes

- The sensor threshold is tuned in software (`threshold` in the `.ino` file) and may need adjusting for different sensors.
- End-to-end delay from pressing a sensor to hearing it spoken is under a second.
