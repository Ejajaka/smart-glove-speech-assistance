# Smart Glove for Speech Assistance

A wearable, low-cost communication device for people with speech impairments. Five pressure sensors on a glove (one per finger) form a 5-bit binary code — a specific combination of finger presses maps to a letter or a whole phrase, which gets spoken out loud in real time.

Built as a team project (with Roshna George and Pranav Sajith) for the *Procedural Programming using C* course, Amrita Vishwa Vidyapeetham.

## How it works

1. Five analog pressure sensors (A0–A4) are read continuously by an Arduino Uno.
2. Each sensor's reading is thresholded into a 0 or 1, giving a 5-bit code (32 possible combinations).
3. A press has to be held briefly (gesture-hold + debounce) before it registers, so accidental touches don't get read as letters.
4. Codes 1–26 map to A–Z; a few reserved codes map to whole phrases (`HELLO`, `I AM`, `BYE`, `VOICE ASSISTANT`) so common things can be said in a single gesture instead of spelling them out.
5. Completed words are strung into a sentence. After a pause, the Arduino prints `FINAL SENTENCE: ...` over serial.
6. A small C program on the host (Linux) listens on the serial port, and when it sees a finished sentence, calls `eSpeak` to speak it out loud.

## Repo structure

```
smart-glove-speech-assistance/
├── arduino/
│   └── sketch_sep30a.ino   — reads sensors, builds letters/words/sentences
└── host/
    └── live_tts.c          — reads Arduino's serial output, triggers eSpeak
```

## Tech stack

- Arduino Uno (ATmega328P), Arduino IDE
- C (both the embedded logic and the host-side program)
- Linux, eSpeak TTS engine
- Serial communication over USB (9600 baud)

## Running it

**Arduino side**
1. Wire five pressure sensors to A0–A4 on an Arduino Uno.
2. Open `arduino/sketch_sep30a.ino` in the Arduino IDE and upload it.

**Host side (Linux)**
```bash
gcc host/live_tts.c -o live_tts
./live_tts
```
Make sure `espeak` is installed (`sudo apt install espeak`) and update `SERIAL_PORT` in `live_tts.c` if your Arduino shows up on a different port than `/dev/ttyACM1`.

## Notes

- Sensor threshold is tuned in software (`threshold` in the `.ino` file) — this may need adjusting depending on the specific sensors used.
- End-to-end delay from pressing a sensor to hearing it spoken is under a second.
