// ============================================================================
// KIT DE CARRITO ARDUINO UNO - NIVEL 1
// ============================================================================

// --- 1. DEFINICIÓN DE PINES (Puente H L298N) ---
const int ENA = 10; // PWM Velocidad Motor Izquierdo (Pin con ~)
const int IN1 = 9;  // Dirección Motor Izquierdo
const int IN2 = 8;  // Dirección Motor Izquierdo

const int IN3 = 7;  // Dirección Motor Derecho
const int IN4 = 6;  // Dirección Motor Derecho
const int ENB = 5;  // PWM Velocidad Motor Derecho (Pin con ~)

// --- 2. VARIABLES DE CONTROL Y PROCESO ---
int velocidadBase = 200;       // Velocidad general (Rango: 0 a 255)
int velocidadGiro = 180;       // Velocidad reducida para giros más controlados
char comandoActual = 'S';      // Estado inicial: 'S' (Stop / Detenido)

// Variables para el Watchdog (Freno de seguridad automático)
unsigned long ultimoComandoMs = 0;
const unsigned long TIEMPO_SEGURIDAD = 1500; // Si en 1.5 seg no hay señal, se detiene
bool frenoSeguridadActivo = false;           // Actívalo en 'true' si tu app manda datos continuos

void setup() {
  // Inicializar comunicación serial (Sirve para PC y Módulo Bluetooth HC-05/HC-06)
  Serial.begin(9600);

  // Configurar todos los pines de los motores como salidas
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Asegurar que el carrito inicie completamente detenido
  detener();

  Serial.println("=== SISTEMA DE CARRITO INICIADO ===");
  Serial.println("Comandos: [W] Adelante | [S] Atras | [A] Izquierda | [D] Derecha | [X] Parar");
  Serial.println("Velocidad: [1] Baja | [2] Media | [3] Maxima");
}

void loop() {
  // 1. Escuchar órdenes del control remoto o la computadora
  leerControlRemoto();

  // 2. Verificar seguridad (evita que el auto choque si pierde conexión)
  verificarSeguridad();

  // 3. Espacio listo para futuras actualizaciones (Sensores, luces, etc.)
  actualizarSensoresFuturos();
}

// ============================================================================
// MÓDULO DE PROCESAMIENTO DE CONTROL REMOTO
// ============================================================================
void leerControlRemoto() {
  if (Serial.available() > 0) {
    char entrada = toupper(Serial.read()); // Convierte a mayúscula automáticamente
    
    // Ignorar saltos de línea del monitor serial
    if (entrada == '\n' || entrada == '\r') return;

    ultimoComandoMs = millis(); 
    comandoActual = entrada;

    switch (comandoActual) {
      case 'W': 
      case 'F':
        avanzar(velocidadBase);
        Serial.println("-> Avanzando");
        break;

      case 'S': 
      case 'B':
        retroceder(velocidadBase);
        Serial.println("-> Retrocediendo");
        break;

      case 'A': 
      case 'L':
        girarIzquierda(velocidadGiro);
        Serial.println("-> Girando a la Izquierda");
        break;

      case 'D': 
      case 'R':
        girarDerecha(velocidadGiro);
        Serial.println("-> Girando a la Derecha");
        break;

      case 'X': 
      case 'P':
        detener();
        Serial.println("-> Detenido");
        break;

      // Cambio dinámico de velocidades (Marchas)
      case '1':
        velocidadBase = 130;
        Serial.println("Velocidad: Nivel 1 (Baja)");
        break;
      case '2':
        velocidadBase = 190;
        Serial.println("Velocidad: Nivel 2 (Media)");
        break;
      case '3':
        velocidadBase = 255;
        Serial.println("Velocidad: Nivel 3 (Maxima)");
        break;
    }
  }
}

// ============================================================================
// FUNCIONES DE MOVIMIENTO
// ============================================================================

void avanzar(int vel) {
  // Motor Izquierdo hacia adelante
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, vel);

  // Motor Derecho hacia adelante
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, vel);
}

void retroceder(int vel) {
  // Motor Izquierdo hacia atrás
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, vel);

  // Motor Derecho hacia atrás
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, vel);
}

void girarIzquierda(int vel) {
  // Giro sobre su propio eje: Izquierdo atrás, Derecho adelante
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, vel);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, vel);
}

void girarDerecha(int vel) {
  // Giro sobre su propio eje: Izquierdo adelante, Derecho atrás
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, vel);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, vel);
}

void detener() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 0);
}

// ============================================================================
// FUNCIONES DE SEGURIDAD Y FUTURAS ACTUALIZACIONES
// ============================================================================

void verificarSeguridad() {
  // Si el modo seguridad está activo y no llegan órdenes nuevas a tiempo, frena solo
  if (frenoSeguridadActivo && (millis() - ultimoComandoMs > TIEMPO_SEGURIDAD)) {
    detener();
  }
}

void actualizarSensoresFuturos() {
  // ESPACIO PARA NIVEL 2 Y 3:
  // Aquí puedes agregar un sensor ultrasónico (HC-SR04) o seguidores de línea
  // sin tener que borrar ni reescribir las funciones de arriba.
  // Ejemplo:
  // if (leerDistanciaUltrasonico() < 15 && comandoActual == 'W') { detener(); }
}