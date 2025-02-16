#if !defined(ARDUINO_ESP32_DEV) // Verificação para garantir que o ESP32 está sendo usado
#error Use este exemplo com o ESP32
#endif

// --------------------------------------------------
// Bibliotecas
#include <RoboCore_SMW_SX1276M0.h>
#include <HardwareSerial.h> // Biblioteca para comunicação serial por hardware

// --------------------------------------------------
// Variáveis
HardwareSerial LoRaSerial(2); // Define o canal serial 2 para comunicação LoRa
#define RXD2 16 // Define o pino RX para a comunicação serial LoRa
#define TXD2 17 // Define o pino TX para a comunicação serial LoRa
#define LED_PIN 13 // Define o pino do LED

SMW_SX1276M0 lorawan(LoRaSerial); // Cria o objeto LoRaWAN para o módulo SMW_SX1276M0
CommandResponse response; // Variável para armazenar a resposta dos comandos LoRa

// Endereços e chaves para configuração do dispositivo e comunicação
const char DEVADDR[] = "00000001"; // Endereço do dispositivo atual
const char DEVADDR_P2P[] = "00000002"; // Endereço de destino para comunicação P2P

//const char APPSKEY[] = "0000000000000000000000000000000c"; // Chave de sessão do aplicativo 
//const char NWKSKEY[] = "0000000000000000000000000000000b"; // Chave de sessão da rede

const unsigned long PAUSE_TIME = 15000; // Pausa de 10 segundos entre transmissões (em ms)
unsigned long timeout; // Variável para controle de tempo
int count = 0; // Contador para número de transmissões

// --------------------------------------------------
// Protótipos
void event_handler(Event); // Função para gerenciar eventos LoRa

// --------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.println(F("--- SMW_SX1276M0 P2P ---"));

    pinMode(LED_PIN, OUTPUT); // Configura o LED como saída
    digitalWrite(LED_PIN, LOW); // Garante que o LED começa desligado

    lorawan.setPinReset(5);
    lorawan.reset();

    LoRaSerial.begin(115200, SERIAL_8N1, RXD2, TXD2);

    lorawan.event_listener = &event_handler;
    Serial.println(F("Handler configurado"));
}

// --------------------------------------------------
void loop() {
    lorawan.listen();

    if (timeout < millis()) {
        count++;
        if (count > 255) count = 0;

        char data[] = "SENHA";

        Serial.println("=======================================");
        Serial.print(F("Mensagem Enviada (P/ NÓ B): "));
        Serial.println(data);
        Serial.println("=======================================");

        response = lorawan.sendT(10, data);
        timeout = millis() + PAUSE_TIME;
    }
}

// --------------------------------------------------
// Função para tratar os eventos do módulo
void event_handler(Event type) {
    if (type == Event::JOINED) {
        Serial.println(F("Conectado"));
    } 
    else if (type == Event::RECEIVED) {
        Serial.println(F("Mensagem de texto recebida"));
        delay(50);
        lorawan.flush();

        uint8_t port;
        Buffer buffer;
        response = lorawan.readT(port, buffer);
        if (response == CommandResponse::OK) {
            String mensagemRecebida = "";
            while (buffer.available()) {
                mensagemRecebida += (char)buffer.read();
            }
            Serial.print(F("Mensagem: "));
            Serial.println(mensagemRecebida);
            Serial.print(F(" na porta "));
            Serial.println(port);

            // Se a mensagem recebida for "ACESSO LIBERADO!", liga o LED
            if (mensagemRecebida.equals("ACESSO LIBERADO!")) {
                Serial.println(F("Recebido: ACESSO LIBERADO! - Ligando LED."));
                digitalWrite(LED_PIN, HIGH);
            }
        }
    } 
    else if (type == Event::RECEIVED_X) {
        Serial.println(F("!Mensagem Hexadecimal recebida e convertida em texto!"));
        delay(50);
        lorawan.flush();

        uint8_t port;
        Buffer buffer;
        response = lorawan.readX(port, buffer);
        if (response == CommandResponse::OK) {
            Serial.print(F("Mensagem Recebida (NÓ B): "));
            String mensagemHex = "";

            while (buffer.available()) {
                uint8_t byteHex = buffer.read();
                mensagemHex += (char)byteHex;
            }

            String mensagemConvertida = "";
            for (int i = 0; i < mensagemHex.length(); i += 2) {
                String hexByte = mensagemHex.substring(i, i + 2);
                char caractere = (char)strtol(hexByte.c_str(), NULL, 16);
                mensagemConvertida += caractere;
            }

            Serial.print(mensagemConvertida);
            Serial.print(F(" | Na porta: "));
            Serial.println(port);

            // Se a mensagem recebida for "ACESSO LIBERADO!", liga o LED
            if (mensagemConvertida.equals("ACESSO LIBERADO!")) {
                Serial.println(F("Recebido: ACESSO LIBERADO! - Ligando LED."));
                digitalWrite(LED_PIN, HIGH);
                delay(1000);
                digitalWrite(LED_PIN, LOW);
                delay(1000);

            }
        }
    }
}