---

# SMW_SX1276M0 Bridge (v1.0)  

Este projeto implementa uma ponte de comunicação entre um computador e um módulo **LoRaWAN SMW SX1276M0**, utilizando um **ESP32**. O código permite que os dados recebidos do módulo LoRaWAN sejam enviados para o computador via **UART**, e vice-versa, possibilitando a comunicação transparente entre ambos.  

**OBJETIVO:** Através do programa obter informação de configuração do módulo, como: Região que ele está configurado (Precisa ser a região 1 - AU915), configurar o módulo caso necessário via comandos AT.

Baseado totalmente no código: https://github.com/RoboCore/RoboCore_SMW-SX1276M0/blob/master/examples/Bridge_ESP32/Bridge_ESP32.ino

## 📌 **Funcionalidade**  
- O ESP32 atua como intermediário entre o computador e o módulo LoRaWAN.  
- Qualquer dado enviado pelo computador via **porta serial** (`Serial`) é transmitido para o módulo **LoRaWAN** (`LoRaSerial`).  
- Qualquer dado recebido do módulo **LoRaWAN** (`LoRaSerial`) é encaminhado para o computador via **Serial**.  
- O código também realiza o reset do módulo LoRaWAN no início da execução.  

## 🛠 **Configuração de Hardware**  
### 📡 **Conexões ESP32 ↔ LoRaWAN**  
- **RXD2 (GPIO16)** → Conectado ao **TX** do módulo LoRaWAN  
- **TXD2 (GPIO17)** → Conectado ao **RX** do módulo LoRaWAN  
- **GPIO5** → Pino de **reset** do módulo LoRaWAN  

### ⚙️ **Dependências**  
Este código usa a biblioteca **RoboCore_SMW_SX1276M0** para facilitar a comunicação com o módulo LoRaWAN.  

## 🔧 **Como Usar**  
1. **Carregue o código** no ESP32 utilizando a **Arduino IDE**.  
2. **Abra o monitor serial** com baud rate **115200**.  
3. **Envie comandos** via monitor serial para interagir com o módulo LoRaWAN.  
4. **Observe as respostas** do módulo sendo enviadas de volta ao computador.  

---

#**Na prática:**
##VERIFICANDO REGIÃO:

A região é um parâmetro muito importante pois define a faixa de frequência da comunicação do módulo que por Lei no Brasil utiliza-se 915 Mhz (que pelo manual de comandos do Fabricante [pag23] é REGION_AU915 = 1).

![Uma das placas estava configurada na região 6 (Quando usado o comando AT+REGION).](Bridge_ESP32/Comando ver região ( AT+REGION).png)  

![Logo, foi possível com o comando AT+REGION 1 configurar a nossa placa para a região 1](Bridge_ESP32/Comando mudar região (AT+REGION 1).png)

Uma das placas passou a estar configurada na região 1 (Quando usado o comando AT+REGION 1).E então temos a região devidamente configurada como mostra:
![Configurada na região 1 (Quando usado o comando AT+REGION).](Bridge_ESP32/Região 1 configurada.png)

**Outra forma de configurar a regiao da placa é atravez da função fornecida pela biblioteca <set_Region()>  para fazer a definição da região:**






