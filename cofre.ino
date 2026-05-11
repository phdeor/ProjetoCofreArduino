// Locker Inteligente com Feedback Multissensorial
// CCM520 – Internet das Coisas | Centro Universitário FEI
// Pedro Henrique Lima de Oliveira, Matheus Sarmento Pinto, João Victor Maciente

#include <LiquidCrystal.h>
#include <Servo.h>

Servo motor;

// --- Pinos dos LEDs indicadores ---
#define ledIncendio 2
#define ledAprovado 3
#define ledIncorreto 4
#define ledTentativa1 5
#define ledTentativa2 6
#define ledTentativa3 7

// --- Pinos do buzzer e botões de entrada ---
#define buzzer 8
#define botaoSenha 10
#define botaoTentativa 11

// --- Sensor de temperatura e display LCD ---
const int leituraTemperatura = A0;
int temperatura;

const int rs = 12, en = 13, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// --- Variáveis de controle da senha e estado do sistema ---
int estadoBotaoSenha = 0;
int botaoSenhaVar = 0;
int botaoSenhaVar2 = 0;
int tentativa = 0;
char senha[5] = "1010";
char senhaDigitada[5] = "    ";
int indiceSenha = 0;
int nrTentativas = 1;

// =====================================================================
// SETUP: configura pinos, inicializa periféricos e exibe animação de boot
// =====================================================================
void setup()
{
  pinMode(ledIncendio, OUTPUT);
  pinMode(ledAprovado, OUTPUT);
  pinMode(ledIncorreto, OUTPUT);
  pinMode(ledTentativa1, OUTPUT);
  pinMode(ledTentativa2, OUTPUT);
  pinMode(ledTentativa3, OUTPUT);

  pinMode(botaoSenha, INPUT);
  pinMode(botaoTentativa, INPUT);
  pinMode(buzzer, OUTPUT);
  
  Serial.begin(9600);
  
  motor.attach(9);
  motor.write(0); // Servo em 0°: cofre trancado

  // Animação de inicialização no LCD com barra de progresso
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Inicializando o");
  lcd.setCursor(0, 1);
  lcd.print("cofre...");
  delay (2000);
  lcd.clear();
  for (int i = 0; i <= 15; i++) {
    lcd.setCursor(0, 0);
    lcd.print("Carregando...");
    lcd.setCursor(i, 1);
    lcd.print("*");
    delay (200);
    lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print("Sucesso!");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Digite a senha:");
  delay(2000);
  senhaDigitada[indiceSenha] = estadoBotaoSenha == 1 ? '1' : '0';
  montarTelaSenha(senhaDigitada);
  acenderTentativas();
}

// =====================================================================
// LOOP: lê botões, verifica temperatura e gerencia a lógica de senha
// =====================================================================
void loop()
{
  botaoSenhaVar = digitalRead(botaoSenha);
  tentativa = digitalRead(botaoTentativa);
  checkIncendio();
  
  // --- Confirmação de dígito ou submissão da senha completa ---
  if (tentativa == 1) {
    if (indiceSenha == 3) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("VALIDANDO");
      delay(2000);

      // Senha correta: aciona LED verde, abre o servo e aguarda fechamento
      if (strcmp(senha, senhaDigitada) == 0) {
        digitalWrite(ledAprovado, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Senha correta!");
        motor.write(90); // Servo em 90°: cofre aberto
        lcd.setCursor(8, 1);
        lcd.print("ABERTO!");
        delay(300);
        while (digitalRead(botaoTentativa) == 0) {
          digitalWrite(ledAprovado, LOW);
          delay(200);
          digitalWrite(ledAprovado, HIGH);
          delay(200);
          checkIncendio();
        }
        motor.write(0);
        digitalWrite(ledAprovado, LOW);

      // Senha incorreta: aciona LED vermelho e gerencia tentativas restantes
      } else {
        digitalWrite(ledIncorreto, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Senha incorreta!");
        lcd.setCursor(8, 1);
        lcd.print("TRANCADO!");
        delay(2000);
        if (nrTentativas < 3) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Tentativas restantes:");
          lcd.setCursor(0, 1);
          lcd.print(3 - nrTentativas);
          nrTentativas++;
          acenderTentativas();
          digitalWrite(ledIncorreto, LOW);
          delay(2000);
        } else {
          // Esgotou as tentativas: entra em loop de alerta até reset
          digitalWrite(ledIncendio, HIGH);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Alerta!");
          delay(2000);
          while (digitalRead(botaoTentativa) == 0) {
            delay(600);
            checkIncendio();
            alertaErro();
          }
        }
      }

      // Reinicia os dados da senha para nova tentativa
      lcd.clear();
      indiceSenha = 0;
      senhaDigitada[0] = estadoBotaoSenha == 1 ? '1' : '0';
      montarTelaSenha(senhaDigitada);
    } else {
      delay(300);
      indiceSenha++;
      senhaDigitada[indiceSenha] = estadoBotaoSenha == 1 ? '1' : '0';
      montarTelaSenha(senhaDigitada);
    }
  }

  // --- Alternância do dígito atual ao pressionar o botão de senha ---
  if (botaoSenhaVar == 1 && botaoSenhaVar2 == 0) {
    estadoBotaoSenha = 1 - estadoBotaoSenha;
    senhaDigitada[indiceSenha] = estadoBotaoSenha == 1 ? '1' : '0';
    montarTelaSenha(senhaDigitada);
    delay(200);
  }
  botaoSenhaVar2=botaoSenhaVar;
  
  temperatura = analogRead(leituraTemperatura);
  Serial.println(temperatura);
  delay(100);
}

// --- Atualiza o display com os dígitos digitados até o momento ---
void montarTelaSenha(char *senhaDigitada) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Digite a senha:");
  lcd.setCursor(0, 1);
  for (int i = 0; i <= indiceSenha; i++) {
    lcd.print(senhaDigitada[i]);
    if (i < indiceSenha) {
      lcd.print("-");
    }
  }
}

// --- Lê o sensor e dispara alerta de incêndio se a temperatura for crítica ---
void checkIncendio() {
  temperatura = analogRead(leituraTemperatura);
  if (temperatura >= 237) {
    alertaIncendio();
  }
}

// --- Acende o LED correspondente ao número de tentativas já realizadas ---
void acenderTentativas(){
  if (nrTentativas == 1) {
    digitalWrite(ledTentativa1, HIGH);
  } else if (nrTentativas == 2) {
    digitalWrite(ledTentativa2, HIGH);
  } else if (nrTentativas == 3) {
    digitalWrite(ledTentativa3, HIGH);
  }
}

// --- Loop de alerta de incêndio: pisca LED, exibe mensagem e aciona buzzer ---
void alertaIncendio() {
  digitalWrite(ledTentativa1, LOW);
  digitalWrite(ledTentativa2, LOW);
  digitalWrite(ledTentativa3, LOW);
  digitalWrite(ledIncendio, LOW);
  digitalWrite(ledAprovado, LOW);
  digitalWrite(ledIncorreto, LOW);
  while (digitalRead(botaoTentativa) == 0) {
    lcd.clear();
    delay(50);
    digitalWrite(ledIncendio, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("FOGO!! INCENDIO!!");
    tone(buzzer, 100, 200);
    delay(200);
    lcd.clear();
    delay(50);
    digitalWrite(ledIncendio, LOW);
    lcd.setCursor(0, 1);
    lcd.print("INCENDIO!! FOGO!!");
    tone(buzzer, 900, 200);
    delay(200);
  }
  lcd.clear();
}

// --- Animação de erro: percorre os LEDs e aciona buzzer ao esgotar tentativas ---
void alertaErro(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ERROOO!");
  tone(buzzer, 100, 200);
  digitalWrite(ledIncorreto, HIGH);
  delay(100);
  digitalWrite(ledIncorreto, LOW);
  delay(100);
  digitalWrite(ledTentativa2, HIGH);
  delay(100);
  digitalWrite(ledTentativa2, LOW);
  delay(100);
  digitalWrite(ledTentativa1, HIGH);
  lcd.clear();
  delay(100);
  tone(buzzer, 300, 200);
  digitalWrite(ledTentativa1, LOW);
  delay(100);
  digitalWrite(ledTentativa3, HIGH);
  delay(100);
  digitalWrite(ledTentativa3, LOW);
  delay(100);
}