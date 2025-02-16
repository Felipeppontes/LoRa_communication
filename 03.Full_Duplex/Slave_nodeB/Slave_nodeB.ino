#if !defined(ARDUINO_ESP32_DEV) // Verifica se o ESP32 está sendo usado
#error Use este exemplo com o ESP32
#endif

// --------------------------------------------------
// Bibliotecas
#include <RoboCore_SMW_SX1276M0.h>
#include <string.h>
#include <DHT.h>// Biblioteca para o DHT11


// --------------------------------------------------
// Variáveis
#include <HardwareSerial.h> // Comunicação serial por hardware
HardwareSerial LoRaSerial(2); // Usa a UART2 para LoRa
#define RXD2 16 // Pino RX para comunicação LoRa
#define TXD2 17 // Pino TX para comunicação LoRa
#define LED_PIN 13 // Pino do LED indicador


//Declaracao das variaveis do pino de dados do DHT11
const int pinoDHT = 12;


SMW_SX1276M0 lorawan(LoRaSerial); // Objeto LoRaWAN
CommandResponse response; // Armazena a resposta dos comandos LoRa

//Criacao da instancia DHT, em funcao do pino do sensor e do tipo do DHT
DHT dht(pinoDHT, DHT11);

// Endereços para comunicação P2P
const char DEVADDR[] = "00000002"; // Endereço do dispositivo atual
const char DEVADDR_P2P[] = "00000001"; // Endereço de destino P2P

//const unsigned long PAUSE_TIME = 30000; // Intervalo de 30s entre transmissões
//unsigned long timeout; // Controle de tempo
//int count = 0; // Contador de transmissões

// --------------------------------------------------
// Protótipo da função de tratamento de eventos
void event_handler(Event);


// --------------------------------------------------
void setup() {
  Serial.begin(115200); // Inicializa a UART para debug
  Serial.println(F("--- SMW_SX1276M0 P2P ---"));

  lorawan.setPinReset(5); // Define o pino de reset do módulo
  lorawan.reset(); // Reinicializa o módulo LoRa

  LoRaSerial.begin(115200, SERIAL_8N1, RXD2, TXD2); // Configura a comunicação UART para LoRa

  lorawan.event_listener = &event_handler; // Define o handler de eventos
  Serial.println(F("Handler configurado"));

  pinMode(15, OUTPUT); //Pino usado pelo LDR
  pinMode(12, OUTPUT); //Pino usado pelo DHT11
  pinMode(LED_PIN, OUTPUT); //Led da placa que está no Pino 13 
  digitalWrite(LED_PIN, LOW); // O led 13 Começa apagado

   //Inicializamos nosso sensor DHT11
  dht.begin();
      
}

// --------------------------------------------------
void loop() {
  lorawan.listen(); // Aguarda mensagens recebidas
}

// --------------------------------------------------
// Função para tratar eventos do LoRa
void event_handler(Event type) {
  uint8_t port;
  Buffer buffer;

  if (type == Event::JOINED) { // Evento de conexão
    Serial.println(F("Conectado"));
  } 
  else if (type == Event::RECEIVED) { // Texto recebido
    Serial.println(F("Mensagem de texto recebida"));
    delay(50);
    lorawan.flush(); // Limpa buffer de comunicação

    response = lorawan.readT(port, buffer);
    if (response == CommandResponse::OK) {
      Serial.print(F("Mensagem: "));
      while (buffer.available()) {
        // Lê os dados recebidos
      }
      Serial.print(F("na porta:"));
    }
  } 
  else if (type == Event::RECEIVED_X) { // Mensagem recebida em formato hexadecimal
    delay(50);
    lorawan.flush();
    response = lorawan.readX(port, buffer);
    if (response == CommandResponse::OK) {
      String mensagemHex = "";
      while (buffer.available()) {
        uint8_t byteHex = buffer.read();
        mensagemHex += (char)byteHex;
      }

      // Converte mensagem hexadecimal para string
      String mensagemConvertida = "";
      for (int i = 0; i < mensagemHex.length(); i += 2) {
        String hexByte = mensagemHex.substring(i, i + 2);
        char caractere = (char)strtol(hexByte.c_str(), NULL, 16);
        mensagemConvertida += caractere;
      }

      Serial.println("===============================================");
      Serial.print(F("Mensagem Recebida do Master: "));
      Serial.print(mensagemConvertida);
      Serial.print(F(" | Na porta: "));
      Serial.println(port);
      Serial.println("===============================================");

      // Responde com "ACESSO LIBERADO!" caso a mensagem seja "SENHA"
      if (mensagemConvertida.equals("envie os dados")) {
        Serial.println(F("!Mensagem Hexadecimal recebida e convertida! Enviando resposta para A..."));


        // NESSE PONTO VAMOS ENVIAR OS DADOS DA PLACA ESCRAVA PARA O MASTER/COMPUTADOR
        //char resposta[] = "aqui os dados";
        int luminosidade = analogRead(15); // Lê o valor do LDR
        //Criamos duas variaveis locais para armazenar a temperatura e a umidade lidas
        float temperatura = dht.readTemperature();
        float umidade = dht.readHumidity();

        char resposta[30]; 
        sprintf(resposta, "L:%d T:%.1f U:%.1f", luminosidade, temperatura, umidade); // Converte o valor para string


        response = lorawan.sendT(port, resposta);

        if (response == CommandResponse::OK) {
          Serial.println(F("Resposta enviada com sucesso!"));
        } else {
          Serial.println(F("Erro ao enviar a resposta!"));
        }
      }
      else if(mensagemConvertida.equals("01")){
      Serial.println(F("Comando recebido: ACENDER_LED - Ligando LED."));
      digitalWrite(LED_PIN, HIGH);
      delay(2000); // Mantém o LED aceso por 2 segundos
      digitalWrite(LED_PIN, LOW);
      }
    }
  }
}
