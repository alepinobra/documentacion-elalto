#ifdef CORE_CM7

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE
#define ENABLE_ESP_SSLCLIENT
#define SERIAL_CDC


#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <FirebaseClient.h>
#include "ExampleFunctions.h" // Provides the functions used in the examples.
#include <time.h>
#include <RPC.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MCP23X17.h>

#include <Arduino_Portenta_OTA.h>


// NUEVO
#include <FlashIAPBlockDevice.h>
#include "FlashIAPLimits.h"
//using namespace mbed;
// NUEVO

static Arduino_Portenta_OTA_QSPI ota(QSPI_FLASH_FATFS_MBR, 2);

#include <ArduinoJson.h>
StaticJsonDocument<8192> jdoc;


//#define API_KEY "AIzaSyBmPHx4Mx93qGhxQjTMLlaxXLpT7qnzCIA"
//#define DATABASE_URL "https://eggspress-dashboard-default-rtdb.firebaseio.com/"
//#define USER_EMAIL "abs@gmail.com"
//#define USER_PASSWORD "123456"

#define API_KEY "AIzaSyBb-a97yzK1G1zu7DgD-tYwUqgAvwn8exg"
#define DATABASE_URL "https://egg-counter-dashboard-default-rtdb.firebaseio.com/"
#define USER_EMAIL "desarrollo@gestionelalto.cl"
#define USER_PASSWORD "EggCounter2025@"

// Nombre del dispositivo
const char* nombreBase = "Pabellon_5";
const char* nombre[8] = {
  "SensorA",
  "SensorB",
  "SensorC",
  "SensorD",
  "SensorE",
  "SensorF",
  "SensorG",
  "SensorH"
};
char PAB_PATH[64];



// VSPI del ESP32 (hardware)
const uint8_t SCK_PIN  = 18; // CLK
const uint8_t MISO_PIN = 19; // MISO (solo ENC usa)
const uint8_t MOSI_PIN = 23; // DIN


const uint8_t DIN_PIN = 16;
const uint8_t CLK_PIN = 17;
const uint8_t CS_PIN  = 13;

void processData(AsyncResult &aResult);
void processDataRead(AsyncResult &aResult);
void processDataLog(AsyncResult &aResult);
void sendLog(String mensaje);
//void connectEthernet();

time_t getNtpTime();
void syncNTP();
time_t getSoftwareTime();

// Zona horaria (UTC−3 para Chile continental; ajustar si hiciera falta)
const long  gmtOffset_sec = -4 * 3600;
const int   daylightOffset_sec = 0;  

EthernetUDP Udp;
const int NTP_PACKET_SIZE = 48;
const int localPort = 8888;           // puerto UDP local
const char* ntpServer = "pool.ntp.org";
int lastResetMinute = -1;
// Epoch obtenido de NTP en la última sincronización
time_t lastEpoch   = 0;
// Momento (millis) en que hicimos esa sincronización
unsigned long ntpLastSyncMs = 0;
// Minuto que procesamos la última vez (0–59)
int lastProcessedMin = -1;
byte packetBuffer[NTP_PACKET_SIZE];
bool first_init = true;
int currentSecond;
int currentMinute;
int currentHour;
int lastSentMinute = 0;
int lastSentHour = 0;
int lastReceivedSecond = 0;
int lastReceivedMinute = 0;
int lastReceivedHour = 0;
int m4Time = 0;
bool minuteChanged;
bool sendFlag = true;
bool Actualizando = false;
bool firstRun = true;
bool firstTime = true;
bool M4Reboot = false;
int turnedPin = 0;

EthernetClient eth_client, stream_eth_client;

// This is a library internal SSL client.
// You can use any SSL Client that works with Ethernet library.
// The ESP_SSLClient uses PSRAM by default (if it is available), for PSRAM usage, see https://github.com/mobizt/FirebaseClient#memory-options
// For ESP_SSLClient documentation, see https://github.com/mobizt/ESP_SSLClient
ESP_SSLClient ssl_client, stream_ssl_client;

using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client), streamClient(stream_ssl_client);

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD, 3000 /* expire period in seconds (<3600) */);
FirebaseApp app;
RealtimeDatabase Database;
//AsyncResult streamResult;

unsigned long ms = 0;


// —————— Pines de interrupción ——————
const uint8_t inputPins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const uint8_t numPins     = sizeof(inputPins)/sizeof(inputPins[0]);
const uint32_t DEBOUNCE_MS = 50;
// —————— Variables volátiles ——————
volatile uint32_t counts[numPins]        = {0};
volatile uint32_t sendedcounts[numPins]  = {0};
volatile uint32_t totalCount          = 0;
volatile bool     evtFlag[numPins]    = {false};
volatile bool     alternador[numPins] = {true};
volatile bool     prevState[numPins] = {true};
volatile bool     state[numPins] = {true};

//ADS1115 ADS(0x48);
Adafruit_MCP23X17 mcp;
// PCF8575 PCF_20; // Dirección I2C del PCF8575

struct FirmwareInfo {
  String key;      // pushId del nodo (p.ej. "-OXBY1h3d...").
  int    version;  // número de versión
  String url;      // url directa al .bin (Firebase Storage con alt=media&token=...)
};

FirmwareInfo latest;
int msg_counter = 0;
// Pantalla
LiquidCrystal_I2C lcd(0x27,16,2);
char huevos_buffer[7]; 
char lvl_buffer[2]; 
int current = 0;
bool prev_btn, curr_btn = {false};

int calculateSecondDifference(int lastReceivedHour, int lastReceivedMinute, int lastReceivedSecond,
                              int currentHour, int currentMinute, int currentSecond) {
    // Convertir las horas, minutos y segundos a segundos desde la medianoche para ambos momentos
    int lastTotalSeconds = lastReceivedHour * 3600 + lastReceivedMinute * 60 + lastReceivedSecond;
    int currentTotalSeconds = currentHour * 3600 + currentMinute * 60 + currentSecond;

    // Si currentTotalSeconds es menor que lastTotalSeconds, es que probablemente ha pasado al siguiente día
    //if (currentTotalSeconds < lastTotalSeconds) {
    //    currentTotalSeconds += 24 * 3600;  // Añadimos 24 horas (en segundos)
    //}

    // La diferencia en segundos
    int totalSecondDifference = currentTotalSeconds - lastTotalSeconds;

    return totalSecondDifference;
}

void start_mcp(){
  if(mcp.begin_I2C()){
    Serial.println("Se inicializó el sensor de nivel");
  } else {
    Serial.println("Falló la conexión con el sensor de nivel");
  }
  
  for (uint8_t p = 0; p < 4; p++) {
    mcp.pinMode(p, INPUT_PULLUP);       // pull-up ~100k interna del MCP23017
  }
}

int recibirTupla(int a, int b, int c, int d, int e, int f, int g, int h) {
  // Imprime la tupla recibida
  M4Reboot = false;
  Serial.print("M7 recibió tupla: ");
  Serial.print(a); Serial.print(", ");
  Serial.print(b); Serial.print(", ");
  Serial.print(c); Serial.print(", ");
  Serial.print(d); Serial.print(", ");
  Serial.print(e); Serial.print(", ");
  Serial.print(f); Serial.print(", ");
  Serial.print(g); Serial.print(", ");
  Serial.println(h);
  counts[0] += a;
  counts[1] += b;
  counts[2] += c;
  counts[3] += d;
  counts[4] += e;
  counts[5] += f;
  counts[6] += g;
  counts[7] += h;
  totalCount += (a + b + c + d + e + f + g + h);
  lcd.setCursor(10,1);
  sprintf(huevos_buffer, "%06d", totalCount);
  lcd.printstr(huevos_buffer);
  Serial.print("Total de huevos: ");
  Serial.println(huevos_buffer);

  lastReceivedSecond = currentSecond;
  lastReceivedMinute = currentMinute;
  lastReceivedHour = currentHour;

  bool state0 = mcp.digitalRead(0);
  bool state1 = mcp.digitalRead(1);
  bool state2 = mcp.digitalRead(2);
  bool state3 = mcp.digitalRead(3);

  //if (!state0 && !state1 && !state2 && !state3) {
  //  start_mcp();
  //  lcd.init();
  //  lcd.backlight();
  //}


  if (state0){
    turnedPin = 1;
  } else if (state1){
    turnedPin = 2;
  } else if (state2){
    turnedPin = 3;
  } else if (state3){
    turnedPin = 4;
  }

  Serial.print("Nivel 0: "); Serial.println(state0);
  Serial.print("Nivel 1: "); Serial.println(state1);
  Serial.print("Nivel 2: "); Serial.println(state2);
  Serial.print("Nivel 3: "); Serial.println(state3);
  
  Serial.print("Nivel: "); Serial.println(turnedPin);

  lcd.setCursor(4, 1);
  sprintf(lvl_buffer, "%01d", turnedPin);
  lcd.print(lvl_buffer);
  // Devuelve un código de confirmación (por ejemplo 0)
  return 0;
}

void resetLcd(){
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("LVL:");
  lcd.setCursor(4, 1);
  sprintf(lvl_buffer, "%01d", turnedPin);
  lcd.print(lvl_buffer);
  lcd.setCursor(8, 1);
  lcd.printstr("H:");
  lcd.setCursor(10,1);
  sprintf(huevos_buffer, "%06d", totalCount);
  lcd.printstr(huevos_buffer);
  lcd.setCursor(0,0);
  lcd.print(nombreBase);
}

void cleanLcd(){
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("LVL:");
  lcd.setCursor(4, 1);
  sprintf(lvl_buffer, "%01d", turnedPin);
  lcd.print(lvl_buffer);
  lcd.setCursor(8, 1);
  lcd.printstr("H:");
  lcd.setCursor(10,1);
  sprintf(huevos_buffer, "%06d", totalCount);
  lcd.printstr(huevos_buffer);
  lcd.setCursor(0,0);
  lcd.print(nombreBase);
}

bool connectEthernet()
{
  delay(2000);

  Serial.println("Conectando Ethernet (DHCP)");
  lcd.setCursor(0, 0);
  lcd.print("Conectando...");

  int dhcp = Ethernet.begin();   // DHCP
  delay(1500);

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Error: cable LAN desconectado");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sin cable LAN");
    return false;
  }

  if (dhcp == 0) {
    Serial.println("Error: DHCP no responde");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHCP fallo");
    return false;
  }

  IPAddress ip = Ethernet.localIP();

  Serial.print("IP asignada: ");
  Serial.println(ip);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(nombreBase);
  return true;
}

//void(* resetFunc) (void) = 0;

void mainTask()
{
    app.loop();
    if (app.ready()){
      if (app.ttl() <= 300 - 60){
        app.authenticate();
      }
    }
    if (app.isExpired()){
      app.authenticate();
    }


    if (millis() - ntpLastSyncMs >= 3600UL * 1000) {
        // Ethernet.maintain();
        Serial.println("Sincronizando NTP cada hora");
        syncNTP();
    }
    time_t epoch = getSoftwareTime();
    struct tm timeinfo;
    if (epoch == 0 || gmtime_r(&epoch, &timeinfo) == nullptr) {
        Serial.println("Error: no se pudo obtener tiempo NTP");
        // Podemos seguir con los eventos aún sin reset de minuto    
    }
    currentSecond = timeinfo.tm_sec;
    currentMinute = timeinfo.tm_min;
    currentHour = timeinfo.tm_hour;
    if (firstTime){
      lastReceivedSecond = currentSecond;
      lastSentMinute = currentMinute;
      lastReceivedMinute = currentMinute;
      lastSentHour = currentHour;
      lastReceivedHour = currentHour;
      firstTime = false;
      //NUEVO
        // Create a block device on the available space of the flash
        auto [flashSize, startAddress, iapSize] = getFlashIAPLimits();
        FlashIAPBlockDevice blockDevice(startAddress, iapSize);
        // Initialize the Flash IAP block device and print the memory layout
        blockDevice.init();
        
        
        String newMessage = String(totalCount);

        // Calculate the amount of bytes needed to store the message
        // This has to be a multiple of the program block size
        const auto eraseBlockSize = blockDevice.get_erase_size();
        const auto programBlockSize = blockDevice.get_program_size();
        const auto messageSize = newMessage.length() + 1; // C String takes 1 byte for NULL termination
        const unsigned int requiredEraseBlocks = ceil(messageSize / (float)  eraseBlockSize);
        const unsigned int requiredProgramBlocks = ceil(messageSize / (float)  programBlockSize);
        const auto dataSize = requiredProgramBlocks * programBlockSize;  
        
        char buffer_m2[dataSize] {};
        // Read back what was stored at previous execution
        Serial.println("Reading previous message...");
        blockDevice.read(buffer_m2, 0, dataSize);
        Serial.println(buffer_m2);   // Should print something like "123.45"

        // Option B: safer (checks where parsing stopped)
        char* endPtr;
        totalCount = strtod(buffer_m2, &endPtr);
        //
    }
    if (currentHour == 0) {
      totalCount = 0;
      if (currentMinute == 0){
        //resetFunc();
        NVIC_SystemReset();
      }
      
    }
    minuteChanged = (currentMinute != lastResetMinute);
    for (uint8_t i = 0; i < numPins; i++) {
        if (evtFlag[i]) {
            evtFlag[i] = false;
            Serial.print(sendFlag);
            }
        }
    
    m4Time = calculateSecondDifference(lastReceivedHour, lastReceivedMinute, lastReceivedSecond, currentHour, currentMinute, currentSecond);
    if (m4Time > 15 && m4Time < 31){
      if (!M4Reboot){
        bootM4();
        M4Reboot = true;
        sendLog("Reinicio de Nucleo: M4");
      }
    } else if (m4Time > 30 && m4Time < 3*60 && M4Reboot){
      M4Reboot = false;
      //sendLog("No se puedo reiniciar de Nucleo: M4");
      NVIC_SystemReset();
    }

    if (minuteChanged && app.ready() && sendFlag)
    {   
        sendFlag = false;
        bool state0 = mcp.digitalRead(0);
        bool state1 = mcp.digitalRead(1);
        bool state2 = mcp.digitalRead(2);
        bool state3 = mcp.digitalRead(3);

        //if (currentMinute == 10) {
        //  resetLcd();
        //}

        //NUEVO
        // Create a block device on the available space of the flash
        auto [flashSize, startAddress, iapSize] = getFlashIAPLimits();
        FlashIAPBlockDevice blockDevice(startAddress, iapSize);
        // Initialize the Flash IAP block device and print the memory layout
        blockDevice.init();
        String newMessage = String(totalCount);

        // Calculate the amount of bytes needed to store the message
        // This has to be a multiple of the program block size
        const auto eraseBlockSize = blockDevice.get_erase_size();
        const auto programBlockSize = blockDevice.get_program_size();
        const auto messageSize = newMessage.length() + 1; // C String takes 1 byte for NULL termination
        const unsigned int requiredEraseBlocks = ceil(messageSize / (float)  eraseBlockSize);
        const unsigned int requiredProgramBlocks = ceil(messageSize / (float)  programBlockSize);
        const auto dataSize = requiredProgramBlocks * programBlockSize;  
        char buffer_m[dataSize] {};

        // Read back what was stored at previous execution
        Serial.println("Reading previous message...");
        blockDevice.read(buffer_m, 0, dataSize);
        Serial.println(buffer_m);

        // Erase a block starting at the offset 0 relative
        // to the block device start address
        blockDevice.erase(0, requiredEraseBlocks * eraseBlockSize);

        // Write an updated message to the first block
        Serial.println("Writing new message...");
        Serial.println(newMessage);  
        blockDevice.program(newMessage.c_str(), 0, dataSize);

        // Deinitialize the device
        blockDevice.deinit();
        Serial.println("Done.");
        //NUEVO

        if (state0){
          turnedPin = 1;
        } else if (state1){
          turnedPin = 2;
        } else if (state2){
          turnedPin = 3;
        } else if (state3){
          turnedPin = 4;
        }

        Serial.print("Nivel 0: "); Serial.println(state0);
        Serial.print("Nivel 1: "); Serial.println(state1);
        Serial.print("Nivel 2: "); Serial.println(state2);
        Serial.print("Nivel 3: "); Serial.println(state3);
        
        Serial.print("Nivel: "); Serial.println(turnedPin);
        
        cleanLcd();
        
        if (abs(currentMinute - lastSentMinute) >= 3){
          if (lastSentMinute == 59){}
          else{
            int lastSentSince = abs(currentMinute - lastSentMinute);
            String mesage = String("No se ha podido enviar un mensaje desde las ") + String(lastSentHour) + String(" horas con ") + String(lastSentMinute) + String(" minutos");
            sendLog(mesage);
          }
        }
        Serial.print("Minuto actual: ");
        Serial.println(currentMinute);
        Serial.print("Publicando en Firebase");
        ms = millis();

        JsonWriter writer;

        object_t json, sen1, sen2, sen3, sen4, sen5, sen6, sen7, sen8, lvl;
        
        for (uint8_t k = 0; k < numPins; k++) {
            sendedcounts[k] = counts[k]; // Acumula los contadores
        }

        writer.create(sen1, "SensorA", sendedcounts[0]);
        writer.create(sen2, "SensorB", sendedcounts[1]);
        writer.create(sen3, "SensorC", sendedcounts[2]);
        writer.create(sen4, "SensorD", sendedcounts[3]);
        writer.create(sen5, "SensorE", sendedcounts[4]);
        writer.create(sen6, "SensorF", sendedcounts[5]);
        writer.create(sen7, "SensorG", sendedcounts[6]);
        writer.create(sen8, "SensorH", sendedcounts[7]);
        writer.create(lvl, "Nivel", turnedPin);

        //CASO PABELLON 6
        //writer.create(sen1, "SensorA", sendedcounts[2]);
        //writer.create(sen2, "SensorB", sendedcounts[3]);
        //writer.create(sen3, "SensorC", sendedcounts[4]);
        //writer.create(sen4, "SensorD", sendedcounts[5]);
        //writer.create(sen5, "SensorE", sendedcounts[6]);
        //writer.create(sen6, "SensorF", sendedcounts[7]);
        //writer.create(sen7, "SensorG", sendedcounts[0]);
        //writer.create(sen8, "SensorH", sendedcounts[1]);
        //writer.create(lvl, "Nivel", turnedPin);


        writer.join(json, 9, sen1, sen2, sen3, sen4, sen5, sen6, sen7, sen8, lvl);


        ////// CALCULO DE RUTA
        time_t epoch = getSoftwareTime();
        if (epoch == 0) {
            Serial.println("Error: no se pudo obtener tiempo NTP");
            return;
        }
        struct tm timeinfo;
        gmtime_r(&epoch, &timeinfo);

        // 2) construir parte de la ruta con fecha‑hora
        char nodo[64];
        snprintf(nodo, sizeof(nodo),
                "%s-%02d-%02d-%04d-%02d-%02d",
                nombreBase,
                timeinfo.tm_mday,
                timeinfo.tm_mon + 1,
                timeinfo.tm_year + 1900,
                timeinfo.tm_hour,
                timeinfo.tm_min);

        String ruta = String("/") + String(nombreBase) + String("/") + String(nodo);
        
        Database.set<object_t>(aClient, ruta.c_str(), json, processData, "setTask");
        lastResetMinute = currentMinute;
        //checkAndUpdateFirmwareOnceFirebase();
      }
    }

// ---------- Helpers FirebaseClient (lectura/escritura) ----------

// Lee un INT de RTDB usando FirebaseClient
bool rtdbGetInt(const char* path, int &out) {
  if (!app.ready()) return false;
  int value = 0;
  value = Database.get<int>(aClient, path);
  if (value != 0) {
    out = value;
    return true;
  } else {
    return false;
  }
}

// Lee nodo completo (JSON) como String
bool rtdbGetString(const char* path, String &out) {
  if (!app.ready()) return false;
  String body = "";
  body = Database.get<String>(aClient, path);
  if (body != "") {
    out = body;
    return true;
  } else {
    return false;
  }
}

// Escribe INT
bool rtdbSetInt(const char* path, int v) {
  if (!app.ready()) return false;
  Database.set<int>(aClient, path, v, processDataRead, "setInt");
  return true; // la lib es async; si quieres, podrías chequear en processData()
}

// Escribe STRING (como JSON string)
bool rtdbSetString(const char* path, const String& s) {
  if (!app.ready()) return false;
  Database.set<String>(aClient, path, s, processDataRead, "setString");
  return true;
}

// ---------- Lógica de actualización ----------

// 1) Leer version_actual
bool getCurrentVersion(int &current) {
  return rtdbGetInt(String(PAB_PATH + String("/version_actual")).c_str(), current);
}

// 2) Buscar la versión mayor disponible bajo /pabellones/Pabellon_1
bool findHigherVersion(int current, FirmwareInfo &out) {
  out.version = current;

  String json;
  if (!rtdbGetString(PAB_PATH, json)) return false;

  DeserializationError err = deserializeJson(jdoc, json);
  if (err) {
    Serial.print("JSON error: ");
    Serial.println(err.c_str());
    return false;
  }

  JsonObject root = jdoc.as<JsonObject>();
  for (JsonPair kv : root) {
    const char* k = kv.key().c_str();
    // Ignorar campos meta
    if (strcmp(k, "version_actual") == 0 || strncmp(k, "test", 4) == 0) continue;
    JsonObject obj = kv.value().as<JsonObject>();

    if (!obj.containsKey("version") || !obj.containsKey("url")) continue;

    int v = obj["version"].as<int>();
    if (v > out.version) {
      out.version = v;
      out.key     = k;
      out.url     = obj["url"].as<const char*>();
      lcd.setCursor(0, 0);
      lcd.print("Actualizando...");
    }
  }
  return (out.version > current);
}

// 3) Marcar estado OTA en RTDB
void setOtaStatus(const char* statusStr, int prev, int target, const char* msg = "") {
  rtdbSetString(String(PAB_PATH + String("/ota_status")).c_str(), String(statusStr));
  rtdbSetInt   (String(PAB_PATH + String("/ota_prev")).c_str(),   prev);
  rtdbSetInt   (String(PAB_PATH + String("/ota_target")).c_str(), target);
  if (msg && *msg)
    rtdbSetString(String(PAB_PATH + String("/ota_msg")).c_str(), String(msg));
}

static bool copyFile(const char* src, const char* dst) {
  FILE* in  = fopen(src, "rb");
  if (!in)  { Serial.print("No pude abrir src: "); Serial.println(src); return false; }
  FILE* out = fopen(dst, "wb");
  if (!out) { Serial.print("No pude abrir dst: "); Serial.println(dst); fclose(in); return false; }

  uint8_t buf[4096];
  size_t n;
  while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
    if (fwrite(buf, 1, n, out) != n) {
      Serial.println("Error escribiendo en destino.");
      fclose(in); fclose(out);
      return false;
    }
  }
  fclose(in);
  fclose(out);
  return true;
}

static bool renameOrCopy(const char* src, const char* dst) {
  // intenta renombrar
  Serial.println("Reescribiendo " + String(src) + " a " + String(dst));
  if (rename(src, dst) == 0) return true;
  // si falla, copia y luego borra el original
  if (!copyFile(src, dst)) return false;
  remove(src);
  return true;
}

// 4) APLICAR OTA con Arduino_Portenta_OTA
bool applyFirmwareFromUrl(const String& firmwareUrl) {
  Serial.println("==> OTA (QSPI) desde URL: " + firmwareUrl);

  if (!ota.isOtaCapable())
  {
    Serial.println("Higher version bootloader required to perform OTA.");
    Serial.println("Please update the bootloader.");
    Serial.println("File -> Examples -> STM32H747_System -> STM32H747_manageBootloader");
    return false;
  }

  // 1) Inicializa almacenamiento OTA (monta FS en QSPI, etc.)
  auto err = ota.begin();
  if (err != Arduino_Portenta_OTA::Error::None) {
    Serial.print("ota.begin() fallo: ");
    Serial.println((int)err);
    return false;
  }

  // 2) Descarga .ota a QSPI (usa HTTPS si la URL empieza con https://)
  const bool is_https = firmwareUrl.startsWith("https://");
  int dl = ota.download(firmwareUrl.c_str(), is_https);
  if (dl <= 0) {
    Serial.print("download() fallo: ");
    Serial.println(dl);
    return false;
  }

  // 3) Si es .ota con LZSS, descomprime al UPDATE.BIN en QSPI
  Serial.print("Bytes descargados: ");
  Serial.println(dl);

  // Heurística rápida: si el comienzo es '<' o '{', probablemente es HTML/JSON.
  FILE* fp = fopen("/fs/UPDATE.OTA", "rb");  // nombre típico; según versión puede variar
  if (fp) {
  uint8_t head[16]; size_t n = fread(head, 1, sizeof(head), fp); fclose(fp);
  if (n && (head[0] == '<' || head[0] == '{')) {
    Serial.println("El archivo descargado no es OTA (HTML/JSON). Revisa permisos/URL alt=media.");
    return false;
  }
}
  //int dec = ota.decompress();
  //if (dec < 0) {
  //  Serial.print("decompress() fallo: ");
  //  Serial.println(dec);
  //  return false;
  //}
  if(!renameOrCopy("/fs/UPDATE.BIN.LZSS", "/fs/UPDATE.BIN")){
    Serial.println("Fallo al renombrar UPDATE.BIN.LZSS a UPDATE.BIN");
    return false;
  }; // renombra a UPDATE.BIN
  // 4) Marca update para el bootloader y, según core, puede resetear
  err = ota.update();
  if (err != Arduino_Portenta_OTA::Error::None) {
    Serial.print("update() fallo: ");
    Serial.println((int)err);
    return false;
  }

  // 5) Reinicia para que el bootloader aplique el UPDATE.BIN
  ota.reset();
  return true; // probablemente no alcance a ejecutarse por el reset
}

// 5) Orquestador completo (con rollback)
void checkAndUpdateFirmwareOnceFirebase() {
  if (firstRun) {
  firstRun = false; // no volver a ejecutar hasta el próximo reset
  current = 0;
  if (!getCurrentVersion(current)) {
    Serial.println("No pude leer version_actual.");
    return;
  }
  Serial.print("version_actual actual: ");
  Serial.println(current);

  if (!findHigherVersion(current, latest)) {
    Serial.println("No hay versiones nuevas.");
    return;
  }
  Serial.print("Nueva version: ");
  Serial.print(latest.version);
  Serial.print("(key=");
  Serial.print(latest.url);
  Serial.println(")");

  Actualizando = false;
  // Marcamos estado y (opcionalmente) adelantamos version_actual ANTES de aplicar:
  setOtaStatus("updating", current, latest.version, "descargando");
  if (!rtdbSetInt(String(PAB_PATH + String("/version_actual")).c_str(), latest.version)) {
    Serial.println("No pude escribir version_actual; aborto OTA.");
    setOtaStatus("failed", current, latest.version, "error escribiendo version_actual");
    return;
  }
  } 
  
  if (Actualizando) {
   
  bool ok = applyFirmwareFromUrl(latest.url);

  if (!ok) {
    Serial.println("OTA fallo; revirtiendo version_actual en RTDB.");
    rtdbSetInt(String(PAB_PATH + String("/version_actual")).c_str(), current);
    setOtaStatus("failed", current, latest.version, "error aplicando OTA");
    return;
  }

  // Si no hubo reinicio automático:
  setOtaStatus("done", current, latest.version, "ok");
  // NVIC_SystemReset(); // si tu flujo requiere reset manual
  } 
}

void sendLog(String mensaje){
  time_t epoch = getSoftwareTime();
  if (epoch == 0) {
    Serial.println("Error: no se pudo obtener tiempo NTP");
    return;
  }
  struct tm timeinfo;
  gmtime_r(&epoch, &timeinfo);

  // 2) construir parte de la ruta con fecha‑hora
  char nodo[64];
  snprintf(nodo, sizeof(nodo),
          "%s-%02d-%02d-%04d-%02d-%02d",
          nombreBase,
          timeinfo.tm_mday,
          timeinfo.tm_mon + 1,
          timeinfo.tm_year + 1900,
          timeinfo.tm_hour,
          timeinfo.tm_min);

  String ruta = String("/") + String("LOG") + String("/") + String(nodo);

  JsonWriter writer;

  object_t json, obj1;

  writer.create(obj1, "Log", mensaje);

  writer.join(json, 1, obj1);
  
  Database.set<object_t>(aClient, ruta.c_str(), json, processDataLog, "LOG");
}

void setup()
{
  // 1) Inicializa RPC y expone el handler en M7
  RPC.begin();
  RPC.bind("recibirTupla", recibirTupla);

  // 2) Arranca el núcleo M4
  bootM4();

  snprintf(PAB_PATH, sizeof(PAB_PATH), "/pabellones/%s", nombreBase);
  Serial.begin(9600);
  Serial.println("Exito!");
  Serial.println(F("MAX7219 listo. Mostrando total de eventos."));

  //Nuevo
  // Get limits of the the internal flash of the microcontroller
  auto [flashSize, startAddress, iapSize] = getFlashIAPLimits();

  Serial.print("Flash Size: ");
  Serial.print(flashSize / 1024.0 / 1024.0);
  Serial.println(" MB");
  Serial.print("FlashIAP Start Address: 0x");
  Serial.println(startAddress, HEX);
  Serial.print("FlashIAP Size: ");
  Serial.print(iapSize / 1024.0 / 1024.0);
  Serial.println(" MB");
  // Create a block device on the available space of the flash
  FlashIAPBlockDevice blockDevice(startAddress, iapSize);
  // Initialize the Flash IAP block device and print the memory layout
  blockDevice.init();
  
  const auto eraseBlockSize = blockDevice.get_erase_size();
  const auto programBlockSize = blockDevice.get_program_size();
  Serial.println("Block device size: " + String((unsigned int) blockDevice.size() / 1024.0 / 1024.0) + " MB");
  Serial.println("Readable block size: " + String((unsigned int) blockDevice.get_read_size())  + " bytes");
  Serial.println("Programmable block size: " + String((unsigned int) programBlockSize) + " bytes");
  Serial.println("Erasable block size: " + String((unsigned int) eraseBlockSize / 1024) + " KB");


  //Nuevo

  lcd.init();
  lcd.backlight();

  
  
  // PCF_20.begin(0x20);
  // Serial.println("PCF8575 inicializado con exito");

  // for (uint8_t p = 0; p < 4; p++) {
  //  PCF_20.pinMode(p, INPUT_PULLUP);       // pull-up ~100k interna del MCP23017
  // }

  //if(ADS.begin()){
  //  Serial.println("Se inicializó el sensor de nivel");
  //} else {
  //  Serial.println("Falló la conexión con el sensor de nivel");
  //}
  start_mcp();
  // if(mcp.begin_I2C()){
  //  Serial.println("Se inicializó el sensor de nivel");
  // } else {
  //  Serial.println("Falló la conexión con el sensor de nivel");
  // }
  
  // for (uint8_t p = 0; p < 4; p++) {
  //  mcp.pinMode(p, INPUT_PULLUP);       // pull-up ~100k interna del MCP23017
  // }

  while (!connectEthernet()){}
  lcd.setCursor(0, 1);
  lcd.print("LVL:");
  lcd.setCursor(4, 1);
  sprintf(lvl_buffer, "%01d", turnedPin);
  lcd.print(lvl_buffer);
  lcd.setCursor(8, 1);
  lcd.printstr("H:");
  lcd.setCursor(10,1);
  sprintf(huevos_buffer, "%06d", totalCount);
  lcd.printstr(huevos_buffer);

  pinMode(BTN_USER, INPUT_PULLUP); // Botón de reset

  syncNTP();
  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

  ssl_client.setClient(&eth_client);
  ssl_client.setInsecure();
  ssl_client.setBufferSizes(4096, 1024);


  Serial.println("Initializing app...");
  initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");
  
  app.autoAuthenticate(false);

  app.getApp<RealtimeDatabase>(Database);

  Database.url(DATABASE_URL);


  while (!app.ready()){
    }
  
  sendLog("El equipo se ha Re-Iniciado");
}

void loop(){
  // readGPIO();
  mainTask();
  checkAndUpdateFirmwareOnceFirebase();
  curr_btn = digitalRead(BTN_USER); // Botón de reset
  if (!curr_btn){
    totalCount = 0;
  }
}



void processData(AsyncResult &aResult)
{
    // Exits when no result available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();
        if (RTDB.isStream())
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s\n", aResult.uid().c_str());
            Firebase.printf("event: %s\n", RTDB.event().c_str());
            Firebase.printf("path: %s\n", RTDB.dataPath().c_str());
            Firebase.printf("data: %s\n", RTDB.to<const char *>());
            Firebase.printf("type: %d\n", RTDB.type());

            // The stream event from RealtimeDatabaseResult can be converted to the values as following.
            bool v1 = RTDB.to<bool>();
            int v2 = RTDB.to<int>();
            float v3 = RTDB.to<float>();
            double v4 = RTDB.to<double>();
            String v5 = RTDB.to<String>();
            sendFlag = true;
          
        }
        else
        {
            Serial.println("----------------------------");
            Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
            for (size_t j = 0; j < numPins ; j++) {
                counts[j] -= sendedcounts[j]; // Reinicia los contadores si se enviaron datos JSON
            }
            sendFlag = true;
            lastSentMinute = currentMinute;
            lastSentHour = currentHour;
        }
    }
}

void processDataLog(AsyncResult &aResult)
{
    // Exits when no result available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();
        Serial.println("----------------------------");
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
        
    }
}

void processDataRead(AsyncResult &aResult)
{
    // Exits when no result available when calling from the loop.
    if (!aResult.isResult())
        return;

    if (aResult.isEvent())
    {
        Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
        Serial.println("Lectura de datos Correcta");
        msg_counter++;
        if (msg_counter >= 5) {
            Actualizando = true;
        }
    }
}


void sendNTPpacket(const char* address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  // los demás bytes ya están a cero
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

time_t getNtpTime() {
  Udp.begin(localPort);
  sendNTPpacket(ntpServer);
  delay(1000);
  if (Udp.parsePacket() == 0) {
    Udp.stop();
    return 0;
  }
  Udp.read(packetBuffer, NTP_PACKET_SIZE);
  Udp.stop();
  // bytes 40..43 contienen TimeStamp* en big-endian
  uint32_t secsSince1900 = (
    (uint32_t)packetBuffer[40] << 24 |
    (uint32_t)packetBuffer[41] << 16 |
    (uint32_t)packetBuffer[42] << 8  |
    (uint32_t)packetBuffer[43]
  );
  const uint32_t seventyYears = 2208988800UL;
  return (time_t)(secsSince1900 - seventyYears + gmtOffset_sec);
}

void syncNTP() {
  time_t t = getNtpTime();
  if (t == 0) {
    Serial.println("syncNTP: fallo en NTP");
    return;
  }
  lastEpoch       = t;
  ntpLastSyncMs   = millis();

  // Inicializamos el último minuto procesado
  struct tm tm;
  gmtime_r(&lastEpoch, &tm);
  lastProcessedMin = tm.tm_min;
  Serial.println("syncNTP: hora inicial");
  Serial.print(tm.tm_hour);
  Serial.print(tm.tm_min);
}

time_t getSoftwareTime() {
  unsigned long deltaMs = millis() - ntpLastSyncMs;
  return lastEpoch + deltaMs / 1000;
}


#else
// código M4
#include <Arduino.h>
#include <RPC.h>
#include <mbed.h>
#include <rtos.h>
#include <cmsis_os2.h>
using rtos::Thread;

Thread hiloEnvio;
Thread hiloLectura;

int t0 = 0, t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0, t6 = 0, t7 = 0;
// —————— Pines de interrupción ——————
const uint8_t inputPins[] = {A0, A1, A2, A3, A4, A5, A6, A7};
const uint8_t numPins     = sizeof(inputPins)/sizeof(inputPins[0]);
const uint32_t DEBOUNCE_MS = 50;
// —————— Variables volátiles ——————
volatile uint32_t counts[numPins]        = {0};
volatile bool     prevState[numPins] = {false};
volatile bool     state[numPins] = {false};
const uint8_t PIN_LED_3 = LED_D3; 

void tareaEnvio() {

  while (true) {
    t0 = counts[0];
    t1 = counts[1];
    t2 = counts[2];
    t3 = counts[3];
    t4 = counts[4];
    t5 = counts[5];
    t6 = counts[6];
    t7 = counts[7];
    //int confirm = RPC.call("recibirTupla", 1, t1, t2, t3, t4, t5, t6, t7).as<int>();
    int confirm = RPC.call("recibirTupla", t0, t1, t2, t3, t4, t5, t6, t7).as<int>();
    counts[0] -= t0;
    counts[1] -= t1;
    counts[2] -= t2;
    counts[3] -= t3;
    counts[4] -= t4;
    counts[5] -= t5;
    counts[6] -= t6;
    counts[7] -= t7;
    // Espera antes de volver a enviar
    osDelay(5000);
  }
}

void tareaLectura() {
  while (true){
    for (uint8_t i = 0; i< numPins; i++){
      state[i] = digitalRead(inputPins[i]);
      if (prevState[i] && !state[i]) {
        counts[i]++; 

      }
      prevState[i] = state[i];
    }
    osDelay(1);
  } 
}
void setup() {
  // Inicializa RPC (se conecta al M7 automáticamente)
  RPC.begin();

  // Inicia el hilo que envía la tupla periódicamente
  delay(10000);
  pinMode(PIN_LED_3, OUTPUT);
  for (int i = 0; i < numPins; i++) {
    pinMode(inputPins[i], INPUT);
  }

  //Serial.println("INICIANDO M4");
  hiloEnvio.start(tareaEnvio);
  hiloLectura.start(tareaLectura);
  
}

void loop() {
   digitalWrite(PIN_LED_3, HIGH);
   delay(10000);
   digitalWrite(PIN_LED_3, LOW);
   delay(10000);
}
#endif