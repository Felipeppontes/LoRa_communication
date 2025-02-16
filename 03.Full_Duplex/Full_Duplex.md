# **Comunicação FULLDUPLEX com envio de comandos**
Nessa etapa foi desenvolvido um código para a o recebimento dos dados do slave (Sensores) e envio de comandos do master (Slave ascender LED).


## Node Master

Este código implementa a funcionalidade de um nó master na comunicação ponto a ponto (P2P). O master é responsável por enviar comandos ao nó slave para acionar um LED remotamente e também por receber mensagens do slave.

### 📌 **Descrição do Código**
- Coleta dados de sensores ambientais:
  - **DHT11**: Temperatura e umidade.
  - **LDR**: Luminosidade.
- Responde ao nó master enviando as leituras dos sensores.
- Ativa um LED indicador ao receber comandos específicos ("01").

### ⚙️ **Funcionamento**
1. O ESP32 inicia a comunicação serial e configura o módulo LoRa.
2. O master fica escutando transmissões do slave.
3. Quando o botão é pressionado, o master envia um comando para o slave acionar o LED.
4. A cada 15 segundos, o master também envia uma mensagem de solicitação de dados para o slave.
5. Caso receba uma resposta do slave contendo "aqui os dados", o master aciona o LED indicando que recebeu a mensagem corretamente.
6. Se uma mensagem hexadecimal for recebida, ela é convertida para texto e interpretada.

### 🔧 **Hardware Utilizado**
- **ESP32 DevKit**
- **Módulo LoRaWAN (SX1276M0)**
- **Push Button (Pino 04 - com pull-up interno ativado)**
### 📜 **Dependências**
Instale as seguintes bibliotecas no Arduino IDE:
- `RoboCore_SMW_SX1276M0`
- `HardwareSerial.h`

### 📡 **Especificações da Comunicação LoRa**
- **Modo:** P2P (Ponto a Ponto)
- **Endereço do Master:** `00000001`
- **Endereço do Slave:** `00000002`


## Node Slave

Este código implementa o **nó slave**. O nó slave recebe comandos do nó master e responde com dados ambientais coletados por sensores e com o acionamento do led, quando o master mandar.
Basicamente, o Mater pede os dados >> O slave recebe o pedido e envia (Temperatura, umidade e luminosidade) >> O master apenas recebe os dados e se o usuário quiser envia um comando (Apertando o Push-button) para o slave ascender o LED  >> se o slave receber "01" da comunicação, ele liga o LED (pin 13).

### 📌 **Descrição do Código**
- Coleta dados de sensores ambientais:
  - **DHT11**: Temperatura e umidade.
  - **LDR**: Luminosidade.
- Responde ao nó master enviando as leituras dos sensores.
- Ativa um LED indicador ao receber comandos específicos ("01").

### ⚙️ **Funcionamento**
1. Inicializa a comunicação serial e configura os pinos de conexão.
2. Aguarda mensagens do nó master via LoRa.
3. Se receber o comando **"envie os dados"**, lê os sensores e transmite os valores ao master.
4. Se receber o comando **"01"**, acende um LED por 2 segundos.

### 🔧 **Hardware Utilizado**
- **ESP32 DevKit**
- **Módulo LoRaWAN (SX1276M0)**
- **Sensor DHT11** (Temperatura e umidade)
- **LDR** (Sensor de luminosidade)
- **LED indicador**

### 📜 **Dependências**
Instale as seguintes bibliotecas no Arduino IDE:
- `RoboCore_SMW_SX1276M0`
- `DHT.h`

### 📡 **Especificações da Comunicação LoRa**
- **Modo:** P2P (Ponto a Ponto)
- **Endereço do Slave:** `00000002`
- **Endereço do Master:** `00000001`

### 🚀 **Execução**
1. Carregue o código no **ESP32** slave.
2. Certifique-se de que o nó master está enviando comandos corretamente.
3. Monitore a comunicação via Serial Monitor.

### **Possíveis Expansões**

1. Implementação de criptografia na comunicação.

2. Uso de sensores adicionais para transmissão de dados do slave para o master.

3. Integração com uma interface web para monitoramento remoto.

---
Projeto desenvolvido como parte da **Iniciação Científica em Agricultura Inteligente**, utilizando **redes de sensores sem fio (WSN)** e **LoRaWAN** para monitoramento ambiental. 🌱📡


