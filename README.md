# 🔐 Locker Inteligente com Feedback Multissensorial

**CCM520 – Internet das Coisas | Centro Universitário FEI**  
Pedro Henrique Lima de Oliveira · Matheus Sarmento Pinto · João Vitor Maciente Rangel

---

## 📋 Descrição

Sistema de controle de acesso para um cofre usando Arduino Uno. O usuário insere uma senha binária de 4 dígitos por meio de dois botões. O display LCD guia o usuário e LEDs indicam o estado do sistema. Após 3 tentativas erradas, um alerta é acionado. Um sensor de temperatura monitora o ambiente continuamente.

---

## 🔩 Componentes

- Arduino Uno
- Display LCD 16x2
- 6 LEDs (aprovado, incorreto, incêndio, tentativas 1/2/3)
- 2 botões (alternar dígito / confirmar)
- Sensor de temperatura (A0)
- Buzzer
- Protoboard e resistores

---

## ▶️ Como usar

1. Abra `cofre.ino` na Arduino IDE e faça o upload para o Arduino Uno.
2. Monte o circuito conforme o diagrama de conexões abaixo.
3. A senha padrão é `1010`.
4. Use o **botão de senha** (esquerda) para alternar entre `0` e `1`, e o **botão de tentativa** (direita) para confirmar cada dígito.
5. Após o 4º dígito, o sistema valida a senha automaticamente.

---

## 🔌 Conexões

| Pino  | Componente       |
|-------|-----------------|
| D2    | LED Incêndio    |
| D3    | LED Aprovado    |
| D4    | LED Incorreto   |
| D5    | LED Tentativa 1 |
| D6    | LED Tentativa 2 |
| D7    | LED Tentativa 3 |
| D10   | Botão Senha     |
| D11   | Botão Tentativa |
| D12   | LCD RS          |
| D13   | LCD EN          |
| A0    | Temperatura     |
| A2–A5 | LCD D4–D7       |

---

## 📷 Foto
<img width="610" height="348" alt="image" src="https://github.com/user-attachments/assets/f13bcbc6-5d3b-42db-9cf4-2a3536ea38fb" />
