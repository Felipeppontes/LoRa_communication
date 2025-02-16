`A partir dos resultados obtidos nos testes anteriores, vamos dar um próximo passo para uma comunicação mais prática e eficiente, onde não será preciso definir um nó transmissor e outro receptor, pois ambos enviam e recebem informações (FULL DUPLEX), muito usado nos serviços de telefonia e redes de computadores.`

`Mas temos um problema !`   
`“Como eles compartilham a mesma antena para transmissão e recepção, existe um pequeno risco de colisão de mensagens.”`

**`![][image1]`**

`No entanto, podemos criar um protocolo que administra a comunicação entre eles: Podemos fazer com que um módulo espere a chegada de uma mensagem pré estabelecida para ser autorizado a enviar uma mensagem para o outro, e repetir esse loop infinitamente.`   
`O código do sistema ficou assim:` 

1. `Transmissor: (30/01/25):`

```c
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

	lorawan.setPinReset(5); // Define pino de reset do LoRa
	lorawan.reset(); // Reinicializa módulo LoRa

	LoRaSerial.begin(115200, SERIAL_8N1, RXD2, TXD2); // Inicia comunicação serial com LoRa

	lorawan.event_listener = &event_handler; // Define função de evento
	Serial.println(F("Handler configurado"));
}

// -----------------------------------------------
void loop() {
	lorawan.listen(); // Mantém LoRa escutando mensagens

	if (timeout < millis()) { // Verifica tempo para próxima transmissão
    	count++;
    	if (count > 255) count = 0; // Evita estouro de variável

    	char data[] = "SENHA"; // Dados a serem enviados

    	Serial.println("=======================================");
    	Serial.print(F("Mensagem Enviada (P/ NÓ B): "));
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
        	if (mensagemRecebida.equals("ACESSO LIBERADO!")) {
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
        	Serial.print(F("Mensagem Recebida (NÓ B): "));
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



```

2. `Receptor: (30/01/25):`

```c
#if !defined(ARDUINO_ESP32_DEV) // Verifica se o ESP32 está sendo usado
#error Use este exemplo com o ESP32
#endif

// --------------------------------------------------
// Bibliotecas

#include <RoboCore_SMW_SX1276M0.h> // Biblioteca do módulo LoRa
#include <string.h> // Biblioteca para manipulação de strings

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
// --------------------------------------------------
// --------------------------------------------------

void setup() {
  // Inicializa a UART para depuração
  Serial.begin(115200);
  Serial.println(F("--- SMW_SX1276M0 P2P ---"));

  // Definição do pino de reset do módulo
  lorawan.setPinReset(5);
  lorawan.reset(); // Realiza um reset no módulo
 
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


```

### **`Fluxo Geral da Comunicação LoRa P2P:`**

`1️⃣ Início`  
 `2️⃣ Configuração do ESP32 e do módulo LoRa em ambos os dispositivos`  
 `3️⃣ O Nó A envia a mensagem "SENHA" para o Nó B`  
 `4️⃣ O Nó B recebe a mensagem e verifica o conteúdo`  
 `🔹 Se a mensagem for "SENHA", ele responde enviando para Nó A: "ACESSO LIBERADO!"`  
 `5️⃣ O Nó A recebe a resposta do Nó B`  
 `🔹 Se a resposta for "ACESSO LIBERADO!", o LED é aceso`  
 `6️⃣ O processo se repete continuamente após um intervalo de tempo`

`Isso cria um sistema simples de solicitação e autorização entre os dois dispositivos via LoRa.`

* `Limitações que surgem com essa estratégia`  
1. `A segurança do sistema pode ser comprometida pois a chave de confirmação é pouco pode ser vazada ou perdida`  
2. `Se a mensagem não for entregue corretamente em ambas as direções não existe uma maneira de forçar o envio e o fluxo vai continuar sem a entrega da palavra de acesso.`

`Esses problemas podem ser superados com uma solução mais completa no futuro.`

[image1]: <data:image/png;base64,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>
