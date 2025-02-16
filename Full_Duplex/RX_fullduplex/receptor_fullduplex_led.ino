#if !defined(ARDUINO_ESP32_DEV) // Verificação para garantir que o ESP32 está sendo usado
#error Use este exemplo com o ESP32
#endif

// --------------------------------------------------
// Bibliotecas

#include <RoboCore_SMW_SX1276M0.h>
#include <string.h> 

// --------------------------------------------------
// Variáveis
#include <HardwareSerial.h> // Biblioteca para comunicação serial por hardware
HardwareSerial LoRaSerial(2); // Define o canal serial 2 para comunicação LoRa
#define RXD2 16 // Define o pino RX para a comunicação serial LoRa
#define TXD2 17 // Define o pino TX para a comunicação serial LoRa

SMW_SX1276M0 lorawan(LoRaSerial); // Cria o objeto LoRaWAN para o módulo SMW_SX1276M0

CommandResponse response; // Variável para armazenar a resposta dos comandos LoRa

// Endereços e chaves para configuração do dispositivo e comunicação
const char DEVADDR[] = "00000002"; // Endereço do dispositivo atual
const char DEVADDR_P2P[] = "00000001"; // Endereço de destino para comunicação P2P
//const char APPSKEY[] = "0000000000000000000000000000000c"; // Chave de sessão do aplicativo
//const char NWKSKEY[] = "0000000000000000000000000000000b"; // Chave de sessão da rede

const unsigned long PAUSE_TIME = 30000; // Pausa de 30 segundos entre transmissões (em ms)
unsigned long timeout; // Variável para controle de tempo
int count = 0; // Contador para número de transmissões

// --------------------------------------------------
// Protótipos
void event_handler(Event); // Função para gerenciar eventos LoRa
// -------------------------------------------------
// -------------------------------------------------

void setup() {
  // Inicializa a UART para depuração
  Serial.begin(115200);
  Serial.println(F("--- SMW_SX1276M0 P2P ---"));

  // definicao do pino de reset do modulo
  lorawan.setPinReset(5);
  lorawan.reset(); // realiza um reset no modulo
  
  // Inicializa a UART para o módulo LoRa
  LoRaSerial.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // Define a função de tratamento de eventos
  lorawan.event_listener = &event_handler;
  Serial.println(F("Handler configurado"));
}
// --------------------------------------------------
// --------------------------------------------------

void loop() {
  // Escuta dados recebidos do módulo
  lorawan.listen();
}
// --------------------------------------------------
// --------------------------------------------------

// Função para tratar os eventos do módulo
//  @param (type) : o tipo do evento [Event]
void event_handler(Event type){
  // Lê a mensagem
  uint8_t port;
  Buffer buffer;
  char lido;
  // Verifica se o evento é de conexão
  if(type == Event::JOINED){
    Serial.println(F("Conectado"));
  } 
  // Verifica se um texto foi recebido
  else if(type == Event::RECEIVED){
    Serial.println(F("Mensagem de texto recebida"));

    // Aguarda um tempo para limpar dados restantes do evento
    delay(50);
    lorawan.flush();

    response = lorawan.readT(port, buffer);
    if(response == CommandResponse::OK){
      Serial.print(F("Mensagem: "));
      while(buffer.available()){
        //lido = buffer.read();
        //Serial.println(lido);
        //Serial.println('\n');
      }
      Serial.print(F("na porta:"));
      //Serial.println(port);
    }
  } 
  else if (type == Event::RECEIVED_X) {
    delay(50);
    lorawan.flush();

    uint8_t port;
    Buffer buffer;
    response = lorawan.readX(port, buffer);
    if (response == CommandResponse::OK) {
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

      Serial.println("===============================================");
      Serial.print(F("Mensagem Recebida (Do NÓ A): "));
      Serial.print(mensagemConvertida);
      Serial.print(F(" | Na porta: "));
      Serial.println(port);
      Serial.println("===============================================");

      if (mensagemConvertida.equals("SENHA")) {
        Serial.println(F("!Mensagem Hexadecimal recebida e convertida em texto! Enviando resposta para A..."));
        char resposta[] = "ACESSO LIBERADO!";
        response = lorawan.sendT(port, resposta);

        if (response == CommandResponse::OK) {
          Serial.println(F("Resposta enviada com sucesso!"));
        } else {
          Serial.println(F("Erro ao enviar a resposta!"));
        }
      }
    }
  }
}
