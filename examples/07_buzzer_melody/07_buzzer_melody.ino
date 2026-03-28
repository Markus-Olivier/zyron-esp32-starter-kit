/*
 * 07 — Buzzer Melody
 * Zyron ESP32 Starter Kit | https://zyron.co.za
 *
 * Play melodies and tones using an active or passive buzzer.
 * Demonstrates tone generation with the ESP32's LEDC PWM peripheral.
 *
 * Wiring:
 *   Buzzer (+) → GPIO 25
 *   Buzzer (-) → GND
 *
 * Note: Active buzzers beep at a fixed frequency — use them for
 * simple alerts. Passive buzzers can play tones — use ledcWriteTone().
 *
 * Skills: LEDC PWM, tone generation, note arrays, timing
 */

#define BUZZER_PIN    25
#define BUZZER_CHAN   0     // LEDC channel (0–15)

// Musical note frequencies (Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_REST  0

// Happy Birthday melody
int melody[] = {
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
    NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_B4, NOTE_B4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4
};

int noteDurations[] = {
    4, 8, 4, 4, 4, 2,
    4, 8, 4, 4, 4, 2,
    4, 8, 4, 4, 4, 4, 2,
    4, 8, 4, 4, 4, 2
};

const int TEMPO = 120; // BPM

void playTone(int freq, int durationMs) {
    if (freq == NOTE_REST) {
        ledcWriteTone(BUZZER_CHAN, 0);
    } else {
        ledcWriteTone(BUZZER_CHAN, freq);
    }
    delay(durationMs);
    ledcWriteTone(BUZZER_CHAN, 0);
    delay(durationMs * 0.1); // Brief pause between notes
}

void playMelody() {
    int noteCount = sizeof(melody) / sizeof(melody[0]);
    for (int i = 0; i < noteCount; i++) {
        int wholeDuration = 60000 / TEMPO;
        int duration      = wholeDuration / noteDurations[i] * 4;
        playTone(melody[i], duration);
    }
}

void setup() {
    Serial.begin(115200);
    ledcSetup(BUZZER_CHAN, 2000, 8); // channel, initial freq, 8-bit resolution
    ledcAttachPin(BUZZER_PIN, BUZZER_CHAN);
    Serial.println("Zyron ESP32 Starter Kit — 07 Buzzer Melody");
}

void loop() {
    Serial.println("Playing: Happy Birthday");
    playMelody();
    Serial.println("Done. Waiting 5 seconds...");
    delay(5000);
}
