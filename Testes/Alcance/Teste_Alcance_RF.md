
# 📡 Testes de Alcance da Comunicação LoRa DIA: 14/02/2025

Realizamos testes para avaliar o alcance da comunicação entre os módulos LoRa em diferentes ambientes. Os testes consistiram no envio de comandos para acionamento de LED e no recebimento de dados de sensores.

---

## 🔬 **1º Teste - Corredor do DES**

📍 **Local:** Corredor do Departamento de Engenharia de Sistemas (DES)  
🛠 **Cenário:** Ambiente fechado, cercado por paredes  
📏 **Distância máxima testada:** ~90m

### 🔹 **Resultados:**
✅ **Acionamento do LED:** 100% de aproveitamento em toda a distância testada.  
❌ **Recebimento de dados dos sensores:**
   - Comunicação interrompida a partir de aproximadamente **90m** (entre a parede da entrada do laboratório e a torre do elevador central).

⚠ **Observação:** As paredes impactaram diretamente o alcance da transmissão dos dados dos sensores.

---

## 🚗 **2º Teste - Estacionamento do CTG**

📍 **Local:** Estacionamento do Centro de Tecnologia e Geociências (CTG)  
🛠 **Cenário:** Ambiente aberto com alguns carros e pessoas circulando  
📏 **Distância máxima testada:** ~260m (limitação do ponto de visada direta)

### 🔹 **Resultados:**
✅ **Acionamento do LED:** 100% de aproveitamento até 260m, sem oscilações.  
⚠ **Recebimento de dados dos sensores:**
   - Houve um momento de instabilidade na recepção dos dados.
   - Comunicação foi restabelecida após reiniciar o ESP32, funcionando novamente com **100% de aproveitamento**.

⚠ **Observação:**
- O ambiente aberto proporcionou um **melhor desempenho da comunicação**.
- Como não alcançamos o limite máximo de distância, o alcance pode ser ainda maior.

---

## 📊 **Conclusões Gerais**

🔹 Ambientes **fechados** (paredes) **reduzem o alcance** da comunicação, impactando principalmente o envio de dados dos sensores.  
🔹 Em ambientes **abertos**, o módulo LoRa demonstrou **excelente desempenho**, mantendo a comunicação mesmo a **distâncias superiores a 260m**.  
🔹 Pequenas **interferências temporárias** podem ocorrer, mas a comunicação pode ser restabelecida reiniciando o ESP32.  

📌 **Próximos passos:** Realizar novos testes para determinar a distância máxima efetiva em ambientes abertos.

---

✍ *Registro dos testes realizado por Felipe Pontes*


