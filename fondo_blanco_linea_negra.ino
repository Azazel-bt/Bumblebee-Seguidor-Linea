// =============================================================================
// BUMBLEBEE - Robot Sigue Línea con Cara Animada en LCD I2C
// =============================================================================

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ─── LCD I2C ──────────────────────────────────────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ─── Pines Motores ───────────────────────────────────────────────────────────
int VelocidadMotor1 = 6;   // ENA → PWM
int VelocidadMotor2 = 5;   // ENB → PWM

// REDUCCIÓN DE VELOCIDAD: Cambiado a float (0.7 = 70% de la potencia original)
// Puedes ajustar este valor (por ejemplo 0.6 o 0.5) para hacerlo aún más lento.
float velocidad_constante = 1; 

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

// =============================================================================
// SETUP
// =============================================================================
void setup() {
  // Inicialización del puerto Serial
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
  
  // Aplicación de la velocidad reducida desde el inicio
  analogWrite(VelocidadMotor1, 125 * velocidad_constante);
  analogWrite(VelocidadMotor2, 87 * velocidad_constante);

  detenerMotores();

  // ─── Sensores ────────────────────────────────────────────────────────
  pinMode(infraPin, INPUT);
  pinMode(infraPin1, INPUT);

  // ─── Presentación ────────────────────────────────────────────────────
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
// LOOP
// =============================================================================
void loop() {
  // ─── Lectura de sensores ────────────────────────────────────────────────
  valorInfra  = digitalRead(infraPin);
  valorInfra1 = digitalRead(infraPin1);

  // ─────────────────────────────────────────────────────────────────────
  // MONITOR SERIAL GRÁFICO 
  // ─────────────────────────────────────────────────────────────────────
  Serial.print("Ojos de Bumblebee:  ");
  
  // Sensor Izquierdo (1 = Negro, 0 = Blanco)
  if (valorInfra == 1) {
    Serial.print("[■] NEGRO (Izq) ");
  } else {
    Serial.print("[ ] BLANCO (Izq)");
  }

  Serial.print("  <=>  ");

  // Sensor Derecho (1 = Negro, 0 = Blanco)
  if (valorInfra1 == 1) {
    Serial.print("(Der) NEGRO [■]");
  } else {
    Serial.print("(Der) BLANCO [ ]");
  }

  Serial.print("   ||   Acción: ");

  // ─────────────────────────────────────────────────────────────────────
  // LÓGICA SIGUE LÍNEA (Para línea NEGRA y fondo BLANCO)
  // ─────────────────────────────────────────────────────────────────────
  if (valorInfra == 0 && valorInfra1 == 0) {
    Serial.println("▲ AVANZAR RECTO (Centrado)");
    posicionActual = 4;
    avanzar();
  }
  else if (valorInfra == 0 && valorInfra1 == 1) {
    Serial.println("► GIRAR A LA DERECHA (Se salió el izquierdo)");
    posicionActual = 8;
    girarDerecha();
  }
  else if (valorInfra == 1 && valorInfra1 == 0) {
    Serial.println("◄ GIRAR A LA IZQUIERDA (Se salió el derecho)");
    posicionActual = 1;
    girarIzquierda();
  }
  else {
    Serial.println("❌ STOP (Ambos fuera de la línea)");
    posicionActual = 4;
    detenerMotores();
  }

  // ─── Actualizar ojos en la pantalla LCD ──────────────────────────────
  if (posicionActual != ultimaPosicion) {
    mostrarOjosAbiertos(posicionActual);
    ultimaPosicion = posicionActual;
  }

  // ─── Parpadeo automático ────────────────────────────────────────────
  if (millis() - ultimoParpadeo >= intervaloParpadeo) {
    parpadearRapido(posicionActual);
    mostrarOjosAbiertos(posicionActual);
    ultimoParpadeo = millis();
    intervaloParpadeo = random(2000, 5000);
    ultimaPosicion = posicionActual;
  }
}

// =============================================================================
// FUNCIONES DE MOVIMIENTO (Sin delays vacíos)
// =============================================================================
void avanzar() {
  digitalWrite(Motor1A, HIGH);
  digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2C, LOW);
  digitalWrite(Motor2D, HIGH);
}

void girarDerecha() {
  digitalWrite(Motor1A, HIGH);
  digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2C, HIGH);
  digitalWrite(Motor2D, LOW);
}

void girarIzquierda() {
  digitalWrite(Motor1A, LOW);
  digitalWrite(Motor1B, HIGH);
  digitalWrite(Motor2C, LOW);
  digitalWrite(Motor2D, HIGH);
}

void detenerMotores() {
  digitalWrite(Motor1A, LOW);
  digitalWrite(Motor1B, LOW);
  digitalWrite(Motor2C, LOW);
  digitalWrite(Motor2D, LOW);
}

// =============================================================================
// FUNCIONES LCD
// =============================================================================
void mostrarOjosAbiertos(int colInicio) {
  lcd.clear();
  dibujarSegmentoOjo(colInicio, 0, 0);
  dibujarSegmentoOjo(colInicio + 5, 0, 0);
  dibujarSegmentoOjo(colInicio, 1, 0);
  dibujarSegmentoOjo(colInicio + 5, 1, 0);
}

void parpadearRapido(int colInicio) {
  lcd.clear();
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
