# Bate Ponto com RFID ESP32

## 📌 Sobre o projeto

O **Bate Ponto com RFID ESP32** é um sistema desenvolvido para automatizar o controle de entrada, saída e tempo de permanência de pessoas em escolas, empresas e outros estabelecimentos.

O projeto utiliza um ESP32 WROOM como unidade principal, responsável pelo processamento, comunicação Wi-Fi e gerenciamento dos dados. A identificação dos usuários é realizada por meio de cartões ou tags RFID, utilizando o módulo RC522.

O ESP32 também atua como servidor web, permitindo cadastrar usuários, consultar registros e administrar as informações por meio de uma página acessível pelo navegador. Um display, LEDs e um buzzer fornecem informações e alertas durante a operação.

## 🎯 Objetivo

Facilitar o controle de frequência por meio da identificação RFID, automatizando o registro de horários e disponibilizando uma interface web para gerenciamento das informações.

## 🧰 Componentes

| Componente              |          Quantidade | Função                                         |
| ----------------------- | ------------------: | ---------------------------------------------- |
| ESP32 WROOM             |                   1 | Processamento, Wi-Fi e gerenciamento dos dados |
| Módulo RFID RC522       |                   1 | Leitura dos cartões e tags RFID                |
| Display alfanumérico    |                   1 | Exibição de mensagens                          |
| LED verde               |                   1 | Indicação visual                               |
| LED vermelho            |                   1 | Indicação visual                               |
| Resistores de 220 Ω     |                   2 | Limitação de corrente dos LEDs                 |
| Buzzer ativo ou passivo |                   1 | Alertas sonoros                                |
| Cartões ou tags RFID    | Conforme necessário | Identificação dos usuários                     |

## 🔌 Pinagem

### RFID RC522 — SPI

| Pino do módulo | Pino do ESP32 |
| -------------- | ------------- |
| SDA / SS       | GPIO 5        |
| SCK            | GPIO 18       |
| MOSI           | GPIO 23       |
| MISO           | GPIO 19       |
| RST            | GPIO 4        |
| 3.3V           | 3V3           |
| GND            | GND           |

### Display — I²C

| Pino do display | Pino do ESP32     |
| --------------- | ----------------- |
| SDA             | GPIO 21           |
| SCL             | GPIO 22           |
| VCC             | Conforme o módulo |
| GND             | GND               |

### LEDs e buzzer

| Componente           | Pino do ESP32 |
| -------------------- | ------------- |
| LED verde — ânodo    | GPIO 16       |
| LED vermelho — ânodo | GPIO 2        |
| Buzzer — positivo    | GPIO 15       |

Os cátodos dos LEDs e o terminal negativo do buzzer devem ser conectados ao GND. Utilize resistores em série com os LEDs e verifique os requisitos elétricos dos componentes antes da montagem.

**Atenção:** o módulo RFID RC522 deve ser alimentado com 3,3 V.

## 💻 Tecnologias e ferramentas

* **Microcontrolador:** ESP32 WROOM
* **Linguagem:** C/C++
* **IDE:** Visual Studio Code
* **Extensão de desenvolvimento:** PlatformIO
* **Identificação:** RFID com módulo RC522
* **Comunicação:** SPI, I²C e Wi-Fi
* **Interface web:** HTML, CSS e JavaScript

## 📄 Licença

Este projeto está licenciado sob a licença MIT.
Consulte o arquivo LICENSE para obter mais informações.
