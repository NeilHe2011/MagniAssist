/*

  how it works:
    1. The mic listens and when someone speaks, the question is recorded.
    2. The recording is sent to OpenAI and comes back as text.
    3. The text is sent to a chat model, which replies with a list of clip numbers.
    4. The DFPlayer plays those clips from the SD card, one after another.

  why add clips?
    The DFPlayer can only play MP3 files that already sit on its own SD card.
    The ESP32 cannot write to that card or stream sound into the DFPlayer.
    So the AI "speaks" by choosing from a list of words that YOU record
    (the CLIPS table in section 5). Add or change words to change what it can say.

  BEFORE YOU UPLOAD INSTALL THESE:
    libraries :  "DFRobotDFPlayerMini" by DFRobot
                 "ArduinoJson" by Benoit Blanchon (version 7.x)
    board package: "esp32" by Espressif, version 3.x
    board:         "ESP32C3 Dev Module", USB CDC On Boot = Enabled
    SD card:       FAT32, with a folder named  mp3  holding 0001.mp3, 0002.mp3, ...
                   File number = position in the CLIPS table (1st entry = 0001.mp3)
    INMP441:       tie its L/R pin to GND (this sketch reads the left channel)

  sections 1-5 are the only parts you will need to edit.
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ESP_I2S.h>
#include <DFRobotDFPlayerMini.h>
#include <ArduinoJson.h>


// 1) acounts and ai settings

#define WIFI_SSID        "your_wifi_name"
#define WIFI_PASSWORD    "your_wifi_password"
#define OPENAI_API_KEY   "your_openai_key"   // keep this private - don't share the file

#define STT_MODEL        "whisper-1"             // speech -> text model
#define CHAT_MODEL       "gpt-4o-mini"           // model that picks the clips
#define SPEECH_LANGUAGE  "en"                    // language you speak (ISO code)

// Many ESP32-C3 Supermini boards can't connect to Wi-Fi at full power.
// If yours connects fine without it, set this to WIFI_POWER_19_5dBm.
const wifi_power_t WIFI_TX_POWER = WIFI_POWER_8_5dBm;


// 2) pins  (match the wiring)

const int PIN_MIC_SCK = 6;    // INMP441 SCK
const int PIN_MIC_WS  = 7;    // INMP441 WS
const int PIN_MIC_SD  = 8;    // INMP441 SD
const int PIN_DF_RX   = 20;   // ESP32 RX  <- DFPlayer TX
const int PIN_DF_TX   = 21;   // ESP32 TX  -> DFPlayer RX

// Optional: push-to-talk button (one leg to this GPIO, other leg to GND).
// -1 = no button: the assistant is always listening for a voice.
const int PIN_BUTTON  = -1;


// 3) listening setting

const uint32_t SAMPLE_RATE        = 16000;
const int      RECORD_MAX_SECONDS = 4;      // longest question (lower to 3 if you get memory errors)
const int      MIC_SHIFT          = 12;     // mic gain: lower = louder, higher = quieter
const int      MIC_SLOT           = I2S_STD_SLOT_LEFT;  // try I2S_STD_SLOT_RIGHT if you hear nothing
const int      VOICE_LEVEL        = 300;    // how loud counts as "someone is talking"
const uint32_t END_SILENCE_MS     = 1000;   // silence that ends a question
const uint32_t MIN_SPEECH_MS      = 300;    // ignore blips shorter than this
const uint32_t WAIT_FOR_SPEECH_MS = 5000;   // how long to wait for speech per attempt
const bool     DEBUG_LEVELS       = false;  // true = print mic loudness, to help tune VOICE_LEVEL


// 4) speaker settings

const int      SPEAKER_VOLUME  = 20;        // 0-30. Keep moderate on a small battery.
const int      DF_EQ           = DFPLAYER_EQ_NORMAL;  // NORMAL, POP, ROCK, JAZZ, CLASSIC, BASS
const int      CLIP_BOOT       = 1;         // clip played at power-on (0 = none)
const int      CLIP_ERROR      = 6;         // clip played when something goes wrong
const int      MAX_CLIPS_PER_ANSWER = 8;
const uint32_t COOLDOWN_MS     = 1500;      // pause after speaking so it doesn't hear itself


// 5) WHAT MAGNIASSIST CAN SAY
//    word 1 = 0001.mp3, word 2 = 0002.mp3 and so on.
//    Record each word (any text-to-speech tool works), save it as an MP3
//    with the matching number, and put it in the "mp3" folder on the SD card.
//    Never reorder words without renaming the files to match.

const char* const CLIPS[] = {
  "hello",            // 0001
  "goodbye",          // 0002
  "yes",              // 0003
  "no",               // 0004
  "maybe",            // 0005
  "I don't know",     // 0006  (used for CLIP_ERROR)
  "please",           // 0007
  "thank you",        // 0008
  "sorry",            // 0009
  "I am MagniAssist", // 0010
  "the answer is",    // 0011
  "it is",            // 0012
  "today",            // 0013
  "tomorrow",         // 0014
  "good",             // 0015
  "bad",              // 0016
  "hot",              // 0017
  "cold",             // 0018
  "big",              // 0019
  "small",            // 0020
  "red",              // 0021
  "green",            // 0022
  "blue",             // 0023
  "yellow",           // 0024
  "black",            // 0025
  "white",            // 0026
  "zero",             // 0027
  "one",              // 0028
  "two",              // 0029
  "three",            // 0030
  "four",             // 0031
  "five",             // 0032
  "six",              // 0033
  "seven",            // 0034
  "eight",            // 0035
  "nine",             // 0036
  "ten"               // 0037
};


// Everything below is the engine. You shouldn't need to edit it.


const size_t CLIP_COUNT    = sizeof(CLIPS) / sizeof(CLIPS[0]);
const size_t MAX_SAMPLES   = (size_t)SAMPLE_RATE * RECORD_MAX_SECONDS;
const size_t BLOCK_SAMPLES = 256;
const size_t PRE_PAD       = 512;  
const size_t POST_PAD      = 64;    
const uint32_t MAX_CLIP_MS = 10000; 
const uint32_t MIN_CLIP_MS = 200;   

#define BOUNDARY "----MagniAssistBoundary"

I2SClass i2s;
DFRobotDFPlayerMini dfp;
uint8_t* recBuf = nullptr;  // one allocation: [PRE_PAD][audio][POST_PAD]
int16_t* pcm    = nullptr;  // start of the audio inside recBuf

// helpers

void halt(const char* msg) {
  while (true) {
    Serial.println(msg);
    delay(3000);
  }
}

bool ensureWifi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  Serial.print("Connecting to Wi-Fi");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) {
    delay(250);
    Serial.print(".");
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? " connected" : " FAILED");
  return WiFi.status() == WL_CONNECTED;
}

// microphone


size_t readMicBlock(int16_t* out, size_t maxSamples) {
  static int32_t raw[BLOCK_SAMPLES];
  if (maxSamples > BLOCK_SAMPLES) maxSamples = BLOCK_SAMPLES;
  size_t bytes = i2s.readBytes((char*)raw, maxSamples * sizeof(int32_t));
  size_t n = bytes / sizeof(int32_t);
  for (size_t i = 0; i < n; i++) {
    int32_t s = raw[i] >> MIC_SHIFT;
    if (s > 32767)  s = 32767;
    if (s < -32768) s = -32768;
    out[i] = (int16_t)s;
  }
  return n;
}

uint32_t blockLevel(const int16_t* s, size_t n) {
  if (!n) return 0;
  uint32_t sum = 0;
  for (size_t i = 0; i < n; i++) sum += abs((int)s[i]);
  return sum / n;
}

// Throw away mic audio for a while (clears old data and the assistant's own voice).
void drainMic(uint32_t ms) {
  int16_t tmp[BLOCK_SAMPLES];
  uint32_t t0 = millis();
  while (millis() - t0 < ms) readMicBlock(tmp, BLOCK_SAMPLES);
}

// Waits for someone to speak, records until they stop.
// Returns the number of samples recorded (0 = nothing usable heard).
size_t recordQuestion() {
  int16_t block[BLOCK_SAMPLES];
  size_t n = 0;
  bool speaking = false;
  uint32_t waitStart = millis(), speechStart = 0, lastVoice = 0;

  while (true) {
    size_t got = readMicBlock(block, BLOCK_SAMPLES);
    if (!got) continue;

    uint32_t level = blockLevel(block, got);
    if (DEBUG_LEVELS) Serial.println(level);
    bool loud = level > (uint32_t)VOICE_LEVEL;

    if (!speaking) {
      if (loud) {
        speaking = true;
        speechStart = lastVoice = millis();
      } else if (millis() - waitStart > WAIT_FOR_SPEECH_MS) {
        return 0;
      } else {
        continue;
      }
    }

    if (n + got > MAX_SAMPLES) break;             // buffer full
    memcpy(pcm + n, block, got * sizeof(int16_t));
    n += got;

    if (loud) lastVoice = millis();
    else if (millis() - lastVoice > END_SILENCE_MS) break;
  }

  if (lastVoice - speechStart < MIN_SPEECH_MS) return 0;  // just a click or a cough
  return n;
}

// speech -> text (OpenAI Whisper) 

void writeWavHeader(uint8_t* h, uint32_t pcmBytes) {
  auto le32 = [&](int o, uint32_t v) { h[o] = v; h[o + 1] = v >> 8; h[o + 2] = v >> 16; h[o + 3] = v >> 24; };
  auto le16 = [&](int o, uint16_t v) { h[o] = v; h[o + 1] = v >> 8; };
  memcpy(h, "RIFF", 4);      le32(4, 36 + pcmBytes);
  memcpy(h + 8, "WAVEfmt ", 8);
  le32(16, 16);  le16(20, 1);  le16(22, 1);          // PCM, mono
  le32(24, SAMPLE_RATE);  le32(28, SAMPLE_RATE * 2); // rate, bytes per second
  le16(32, 2);   le16(34, 16);                       // 16-bit samples
  memcpy(h + 36, "data", 4);   le32(40, pcmBytes);
}

// Sends the recording to Whisper and returns the text ("" if it failed).
String transcribe(size_t samples) {
  if (!ensureWifi()) return "";
  size_t pcmBytes = samples * sizeof(int16_t);

  const String pre =
    "--" BOUNDARY "\r\n"
    "Content-Disposition: form-data; name=\"model\"\r\n\r\n"
    STT_MODEL "\r\n"
    "--" BOUNDARY "\r\n"
    "Content-Disposition: form-data; name=\"language\"\r\n\r\n"
    SPEECH_LANGUAGE "\r\n"
    "--" BOUNDARY "\r\n"
    "Content-Disposition: form-data; name=\"file\"; filename=\"question.wav\"\r\n"
    "Content-Type: audio/wav\r\n\r\n";
  const String post = "\r\n--" BOUNDARY "--\r\n";

  // Trick to save memory: build the upload headers directly in front of the
  // recorded audio and the footer directly after it, so it is one solid block.
  size_t headLen = pre.length() + 44;
  if (headLen > PRE_PAD || post.length() > POST_PAD) {
    Serial.println("Upload headers too big - increase PRE_PAD");
    return "";
  }
  uint8_t* audio = (uint8_t*)pcm;
  uint8_t* start = audio - headLen;
  memcpy(start, pre.c_str(), pre.length());
  writeWavHeader(start + pre.length(), pcmBytes);
  memcpy(audio + pcmBytes, post.c_str(), post.length());
  size_t total = headLen + pcmBytes + post.length();

  WiFiClientSecure client;
  client.setInsecure();   // skips certificate checking to keep this simple
  HTTPClient http;
  http.setTimeout(30000);
  if (!http.begin(client, "https://api.openai.com/v1/audio/transcriptions")) return "";
  http.addHeader("Authorization", "Bearer " OPENAI_API_KEY);
  http.addHeader("Content-Type", "multipart/form-data; boundary=" BOUNDARY);

  int code = http.POST(start, total);
  String body = http.getString();
  http.end();

  if (code != 200) {
    Serial.printf("Transcription failed (%d): %s\n", code, body.c_str());
    return "";
  }
  JsonDocument doc;
  if (deserializeJson(doc, body)) return "";
  return doc["text"] | "";
}

// speaker (DFPlayer)

// Plays clip number `id` (1 = 0001.mp3) and waits until it finishes.
bool playClip(int id) {
  if (id < 1 || id > (int)CLIP_COUNT) {
    Serial.printf("Skipping unknown clip %d\n", id);
    return false;
  }
  Serial.printf("Playing clip %d: %s\n", id, CLIPS[id - 1]);
  while (dfp.available()) dfp.readType();   // clear old messages
  dfp.playMp3Folder(id);

  uint32_t t0 = millis();
  while (millis() - t0 < MAX_CLIP_MS) {
    if (dfp.available()) {
      uint8_t type = dfp.readType();
      if (type == DFPlayerPlayFinished && millis() - t0 > MIN_CLIP_MS) return true;
      if (type == DFPlayerError) {
        Serial.printf("DFPlayer error %d (is file %04d.mp3 in the mp3 folder?)\n", dfp.read(), id);
        return false;
      }
    }
    delay(5);
  }
  return false;
}

// text -> clips (chat model) 

String buildSystemPrompt() {
  String p =
    "You are MagniAssist, a small talking assistant. You cannot speak freely: "
    "you can only play pre-recorded clips. Available clips (id = what it says):\n";
  for (size_t i = 0; i < CLIP_COUNT; i++) {
    p += String(i + 1) + " = " + CLIPS[i] + "\n";
  }
  p += "Answer the user's question by choosing up to " + String(MAX_CLIPS_PER_ANSWER) +
       " clips, in the order they should be played. "
       "Reply ONLY with JSON like {\"clips\":[1,5,7]}. "
       "If no clips can answer the question, reply {\"clips\":[" + String(CLIP_ERROR) + "]}.";
  return p;
}

// sends the question to the chat model and plays the clips it picks.
bool askAndSpeak(const String& question) {
  if (!ensureWifi()) return false;

  JsonDocument req;
  req["model"] = CHAT_MODEL;
  req["response_format"]["type"] = "json_object";
  JsonArray msgs = req["messages"].to<JsonArray>();
  JsonObject sys = msgs.add<JsonObject>();
  sys["role"] = "system";
  sys["content"] = buildSystemPrompt();
  JsonObject usr = msgs.add<JsonObject>();
  usr["role"] = "user";
  usr["content"] = question;
  String payload;
  serializeJson(req, payload);

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setTimeout(30000);
  if (!http.begin(client, "https://api.openai.com/v1/chat/completions")) return false;
  http.addHeader("Authorization", "Bearer " OPENAI_API_KEY);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST(payload);
  String body = http.getString();
  http.end();

  if (code != 200) {
    Serial.printf("Chat request failed (%d): %s\n", code, body.c_str());
    return false;
  }

  JsonDocument resp;
  if (deserializeJson(resp, body)) return false;
  const char* content = resp["choices"][0]["message"]["content"];
  if (!content) return false;

  JsonDocument answer;
  if (deserializeJson(answer, content)) return false;
  JsonArray ids = answer["clips"];

  int played = 0;
  for (JsonVariant v : ids) {
    if (played >= MAX_CLIPS_PER_ANSWER) break;
    playClip(v.as<int>());
    played++;
    delay(60);   // tiny gap between words
  }
  return played > 0;
}
// setup and main loop 
void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("\nMagniAssist starting...");

  if (PIN_BUTTON >= 0) pinMode(PIN_BUTTON, INPUT_PULLUP);

  // Reserve the recording memory first, before Wi-Fi uses up the heap.
  recBuf = (uint8_t*)malloc(PRE_PAD + MAX_SAMPLES * sizeof(int16_t) + POST_PAD);
  if (!recBuf) halt("Not enough memory - lower RECORD_MAX_SECONDS");
  pcm = (int16_t*)(recBuf + PRE_PAD);

  // Microphone (I2S)
  i2s.setPins(PIN_MIC_SCK, PIN_MIC_WS, -1, PIN_MIC_SD);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO, MIC_SLOT)) {
    halt("Microphone (I2S) failed to start - check GPIO 6, 7, 8");
  }

  // DFPlayer Mini (serial)
  Serial1.begin(9600, SERIAL_8N1, PIN_DF_RX, PIN_DF_TX);
  while (!dfp.begin(Serial1, true, true)) {
    Serial.println("DFPlayer not responding - check TX/RX wiring, 5V power and the SD card");
    delay(2000);
  }
  dfp.setTimeOut(500);
  dfp.outputDevice(DFPLAYER_DEVICE_SD);
  dfp.volume(SPEAKER_VOLUME);
  dfp.EQ(DF_EQ);

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_TX_POWER);
  ensureWifi();

  Serial.printf("Free memory: %u bytes\n", (unsigned)ESP.getFreeHeap());
  if (CLIP_BOOT > 0) playClip(CLIP_BOOT);
  Serial.println("Ready. Ask me something.");
}

void loop() {
  // Push-to-talk mode: do nothing until the button is held down.
  if (PIN_BUTTON >= 0 && digitalRead(PIN_BUTTON) == HIGH) {
    delay(10);
    return;
  }

  drainMic(150);
  size_t samples = recordQuestion();
  if (!samples) return;          // nothing heard, listen again

  Serial.printf("Heard %.1f seconds of speech, transcribing...\n", (float)samples / SAMPLE_RATE);
  String question = transcribe(samples);
  if (question.length() == 0) {
    Serial.println("Couldn't understand that.");
    return;
  }
  Serial.println("You said: " + question);

  if (!askAndSpeak(question)) playClip(CLIP_ERROR);

  drainMic(COOLDOWN_MS);
}
