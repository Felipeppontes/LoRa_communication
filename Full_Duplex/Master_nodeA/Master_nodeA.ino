#if !defined(ARDUINO_ESP32_DEV) // Verifica se o ESP32 está sendo usado
#error Use este exemplo com o ESP32
#endif

// --------------------------------------------------
// Bibliotecas necessárias
#include <RoboCore_SMW_SX1276M0.h>
#include <HardwareSerial.h> // Comunicação serial via hardware

// --------------------------------------------------
// Definição de variáveis
HardwareSerial LoRaSerial(2); // Configura a comunicação LoRa na UART2
#define RXD2 16 // Pino RX do LoRa
#define TXD2 17 // Pino TX do LoRa
#define LED_PIN 13 // Pino do LED de indicação
#define BUTTON_PIN 4 // Pino onde o botão está conectado

SMW_SX1276M0 lorawan(LoRaSerial); // Instância do módulo LoRa
CommandResponse response; // Armazena resposta dos comandos LoRa

// Endereços para comunicação ponto a ponto
const char DEVADDR[] = "00000001"; // Endereço do dispositivo atual
const char DEVADDR_P2P[] = "00000002"; // Endereço do destino

//const char APPSKEY[] = "0000000000000000000000000000000c"; // Chave de sessão do aplicativo
//const char NWKSKEY[] = "0000000000000000000000000000000b"; // Chave de sessão da rede

const unsigned long PAUSE_TIME = 15000; // Intervalo de envio (15s)
unsigned long timeout; // Controle de tempo
int count = 0; // Contador de transmissões

// --------------------------------------------------
// Protótipo da função de tratamento de eventos
void event_handler(Event);

// --------------------------------------------------
void setup() {
	Serial.begin(115200);
	Serial.println(F("--- SMW_SX1276M0 P2P ---"));

	pinMode(LED_PIN, OUTPUT); // Configura LED como saída
	digitalWrite(LED_PIN, LOW); // LED começa apagado
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Configura o botão como entrada com pull-up interno


	lorawan.setPinReset(5); // Define pino de reset do LoRa
	lorawan.reset(); // Reinicializa módulo LoRa

	LoRaSerial.begin(115200, SERIAL_8N1, RXD2, TXD2); // Inicia comunicação serial com LoRa

	lorawan.event_listener = &event_handler; // Define função de evento
	Serial.println(F("Handler configurado"));
}

// -----------------------------------------------
void loop() {
	lorawan.listen(); // Mantém LoRa escutando mensagens

  if (digitalRead(BUTTON_PIN) == LOW) { // Se o botão for pressionado (nível baixo)
    Serial.println("Botão pressionado! Enviando sinal para o slave...");

    char command[] = "01"; // Mensagem para o slave acender o LED

    response = lorawan.sendT(10, command); // Envia mensagem para a porta 10
    
    delay(500); // Evita múltiplas transmissões rápidas devido ao bounce do botão
}


	if (timeout < millis()) { // Verifica tempo para próxima transmissão
    	count++;
    	if (count > 255) count = 0; // Evita estouro de variável

    	char data[] = "envie os dados"; // Dados a serem enviados

    	Serial.println("=======================================");
    	Serial.print(F("Mensagem Enviada Para Slave: "));
    	Serial.println(data);
    	Serial.println("=======================================");

    	response = lorawan.sendT(10, data); // Envia mensagem para a porta 10
    	timeout = millis() + PAUSE_TIME; // Atualiza o tempo de envio
	}
}

// --------------------------------------------------
// Função de tratamento de eventos LoRa
void event_handler(Event type) {
	if (type == Event::JOINED) {
    	Serial.println(F("Conectado"));
	}
	else if (type == Event::RECEIVED) { // Mensagem recebida
    	Serial.println(F("Mensagem de texto recebida"));
    	delay(50);
    	lorawan.flush();

    	uint8_t port;
    	Buffer buffer;
    	response = lorawan.readT(port, buffer); // Lê dados recebidos
    	if (response == CommandResponse::OK) {
        	String mensagemRecebida = "";
        	while (buffer.available()) {
            	mensagemRecebida += (char)buffer.read(); // Converte dados para string
        	}
        	Serial.print(F("Mensagem: "));
        	Serial.println(mensagemRecebida);
        	Serial.print(F(" na porta "));
        	Serial.println(port);

        	// Se a mensagem for "ACESSO LIBERADO!", acende o LED
        	if (mensagemRecebida.equals("aqui os dados")) {
            	Serial.println(F("Recebido: ACESSO LIBERADO! - Ligando LED."));
            	digitalWrite(LED_PIN, HIGH);
        	}
    	}
	}
	else if (type == Event::RECEIVED_X) { // Mensagem recebida em hexadecimal
    	Serial.println(F("!Mensagem Hexadecimal recebida e convertida em texto!"));
    	delay(50);
    	lorawan.flush();

    	uint8_t port;
    	Buffer buffer;
    	response = lorawan.readX(port, buffer); // Lê dados hexadecimais
    	if (response == CommandResponse::OK) {
        	Serial.print(F("Mensagem Recebida do slave: "));
        	String mensagemHex = "";

        	while (buffer.available()) {
            	uint8_t byteHex = buffer.read();
            	mensagemHex += (char)byteHex; // Converte para string
        	}

        	String mensagemConvertida = "";
        	for (int i = 0; i < mensagemHex.length(); i += 2) {
            	String hexByte = mensagemHex.substring(i, i + 2);
            	char caractere = (char)strtol(hexByte.c_str(), NULL, 16);
            	mensagemConvertida += caractere; // Converte HEX para ASCII
        	}

        	Serial.print(mensagemConvertida);
        	Serial.print(F(" | Na porta: "));
        	Serial.println(port);

        	// Se a mensagem for "ACESSO LIBERADO!", acende e pisca o LED
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


