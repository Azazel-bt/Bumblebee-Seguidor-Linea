// =============================================================================
// BUMBLEBEE - Robot Sigue Línea con Cara Animada en LCD I2C (Estabilizado)
// =============================================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ─── LCD I2C ──────────────────────────────────────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ─── Pines Motores ───────────────────────────────────────────────────────────
int VelocidadMotor1 = 6;   // ENA → PWM
int VelocidadMotor2 = 5;   // ENB → PWM

int Motor1A = 13;
int Motor1B = 12;

int Motor2C = 11;
int Motor2D = 10;

// ─── Pines Sensores Infrarrojo ───────────────────────────────────────────────
int infraPin  = 4;   // Sensor izquierdo
int infraPin1 = 2;   // Sensor derecho

int valorInfra  = 0;
int valorInfra1 = 0;

// ─── Caracteres Personalizados LCD ───────────────────────────────────────────
byte ojoAbierto[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };
byte ojoParpadeoSup[8] = { B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111 };
byte ojoParpadeoInf[8] = { B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000 };

// ─── Variables de estado ─────────────────────────────────────────────────────
int posicionActual = 4;
int ultimaPosicion = -1;

unsigned long ultimoParpadeo = 0;
unsigned long intervaloParpadeo = 3000;
unsigned long ultimaActualizacionLCD = 0; // Control de estabilidad para el bus I2C

// =============================================================================
// SETUP
// =============================================================================
void setup() {
  Serial.begin(9600);
  Serial.println("--- CONEXIÓN SERIAL ESTABLECIDA CON BUMBLEBEE ---");

  // ─── LCD ─────────────────────────────────────────────────────────────
  lcd.init();
  lcd.backlight();

  randomSeed(analogRead(A0));

  lcd.createChar(0, ojoAbierto);
  lcd.createChar(1, ojoParpadeoSup);
  lcd.createChar(2, ojoParpadeoInf);

  // ─── Motores ─────────────────────────────────────────────────────────
  pinMode(Motor1A, OUTPUT);
  pinMode(Motor1B, OUTPUT);
  pinMode(Motor2C, OUTPUT);
  pinMode(Motor2D, OUTPUT);

  pinMode(VelocidadMotor1, OUTPUT);
  pinMode(VelocidadMotor2, OUTPUT);
  
  detenerMotores();

  // ─── Sensores ────────────────────────────────────────────────────────
  pinMode(infraPin, INPUT);
  pinMode(infraPin1, INPUT);

  // ─── Presentación Inicial ────────────────────────────────────────────
  escribirTextoInteligente("Bumblebee online");
  delay(20);
  escribirTextoInteligente("Hola profe a que no soy un 5 ");
  delay(20);
  
  lcd.clear();
  mostrarOjosAbiertos(posicionActual);
  delay(500);
  parpadearRapido(posicionActual);
  mostrarOjosAbiertos(posicionActual);
  delay(500);

  ultimaPosicion = posicionActual;
  ultimoParpadeo = millis();
  intervaloParpadeo = random(2000, 5000);
  
  Serial.println("--- Presentación terminada. Iniciando Loop de Control ---");
}

// =============================================================================
// LOOP PRINCIPAL
// =============================================================================
void loop() {
  valorInfra  = digitalRead(infraPin);
  valorInfra1 = digitalRead(infraPin1);

  // ─── LÓGICA DE SEGUIMIENTO ───────────────────────────────────────────
  if (valorInfra == 0 && valorInfra1 == 0) {
    avanzar();
    posicionActual = 4;
  }
  else if (valorInfra == 0 && valorInfra1 == 1) {
    girarDerecha();
    posicionActual = 8;
  }
  else if (valorInfra == 1 && valorInfra1 == 0) {
    girarIzquierda();
    posicionActual = 1;
  }
  else {
    detenerMotores();
    posicionActual = 4;
  }

  // ─── Actualizar la dirección de los ojos en la pantalla LCD (Estabilizado) ───
  // Solo se permite actualizar la pantalla si cambió la posición Y pasaron al menos 50ms desde la última escritura.
  if (posicionActual != ultimaPosicion && (millis() - ultimaActualizacionLCD >= 50)) {
    mostrarOjosAbiertos(posicionActual);
    ultimaPosicion = posicionActual;
    ultimaActualizacionLCD = millis();
  }

  // ─── Sistema de parpadeo automático no bloqueante ─────────────────────
  if (millis() - ultimoParpadeo >= intervaloParpadeo) {
    parpadearRapido(posicionActual);
    mostrarOjosAbiertos(posicionActual);
    ultimoParpadeo = millis();
    intervaloParpadeo = random(3000, 6000); // Un intervalo ligeramente mayor ayuda a la estabilidad
    ultimaPosicion = posicionActual;
    ultimaActualizacionLCD = millis();
  }
}

// =============================================================================
// FUNCIONES DE MOVIMIENTO CONFIGURABLES
// =============================================================================
void avanzar() {
  analogWrite(VelocidadMotor1, 85); // si es muy lento 95 105
  analogWrite(VelocidadMotor2, 60); // si es muy lento 70 80

  digitalWrite(Motor1A, HIGH);
  digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2C, LOW);
  digitalWrite(Motor2D, HIGH);
}

void girarDerecha() {
  analogWrite(VelocidadMotor1, 95);  
  analogWrite(VelocidadMotor2, 70); 

  digitalWrite(Motor1A, HIGH);
  digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2C, HIGH); 
  digitalWrite(Motor2D, LOW);
}

void girarIzquierda() {
  analogWrite(VelocidadMotor1, 70);  
  analogWrite(VelocidadMotor2, 95); 

  digitalWrite(Motor1A, LOW);  
  digitalWrite(Motor1B, HIGH);
  digitalWrite(Motor2C, LOW);
  digitalWrite(Motor2D, HIGH);
}

void detenerMotores() {
  analogWrite(VelocidadMotor1, 0);
  analogWrite(VelocidadMotor2, 0);
  digitalWrite(Motor1A, LOW);
  digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2C, LOW);
  digitalWrite(Motor2D, LOW);
}

// =============================================================================
// FUNCIONES LCD OPTIMIZADAS
// =============================================================================
void mostrarOjosAbiertos(int colInicio) {
  static int ultimaColBorrado = 0;
  if(colInicio != ultimaColBorrado) {
    lcd.setCursor(ultimaColBorrado, 0);     lcd.print("   ");
    lcd.setCursor(ultimaColBorrado + 5, 0); lcd.print("   ");
    lcd.setCursor(ultimaColBorrado, 1);     lcd.print("   ");
    lcd.setCursor(ultimaColBorrado + 5, 1); lcd.print("   ");
    ultimaColBorrado = colInicio;
  }
  dibujarSegmentoOjo(colInicio, 0, 0);
  dibujarSegmentoOjo(colInicio + 5, 0, 0);
  dibujarSegmentoOjo(colInicio, 1, 0);
  dibujarSegmentoOjo(colInicio + 5, 1, 0);
}

void parpadearRapido(int colInicio) {
  dibujarSegmentoOjo(colInicio, 0, 1);
  dibujarSegmentoOjo(colInicio + 5, 0, 1);
  dibujarSegmentoOjo(colInicio, 1, 2);
  dibujarSegmentoOjo(colInicio + 5, 1, 2);
  delay(140);
}

void dibujarSegmentoOjo(int colInicio, int fila, int charType) {
  for (int i = 0; i < 3; i++) {
    lcd.setCursor(colInicio + i, fila);
    lcd.write(byte(charType));
  }
}

// =============================================================================
// TEXTO INTELIGENTE LCD 
// =============================================================================
void escribirTextoInteligente(String texto) {
  lcd.clear();
  lcd.setCursor(0, 0);
  int fila = 0;
  int col = 0;
  String palabra = "";

  for (int i = 0; i <= texto.length(); i++) {
    char c = (i < texto.length()) ? texto[i] : ' ';
    if (c == ' ') {
      if (col + palabra.length() > 16) {
        fila++;
        col = 0;
        if (fila > 1) {
          delay(1800);
          lcd.clear();
          fila = 0;
        }
        lcd.setCursor(col, fila);
      }
      lcd.print(palabra);
      col += palabra.length();
      if (col < 15) {
        lcd.print(" ");
        col++;
      }
      palabra = "";
    } else {
      palabra += c;
    }
    delay(10);
  }
  delay(1000);
}
