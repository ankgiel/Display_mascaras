/*  Asignación de cada segmento en mis pines digitales
 *       5==A
 *  6==F    4==B
 *      7==G
 *  0==E    1==C
 *      2==D        DP: No asignado
 *      
 *  ORDEN MI PUERTO D: 
 *       PIN: 7 6 5 4 3 2 1 0
 *  SEGMENTO: G F A B P D C E    
 *  
 *  Número 0: 0 1 1 1 0 1 1 1 
 *  Número 1: 0 0 0 1 0 0 1 0
 *  Número 2: 1 0 1 1 0 1 0 1 
 *  Número 3: 1 0 1 1 0 1 1 0 
 *  Número 4: 1 1 0 1 0 0 1 0 
 *  Número 5: 1 1 1 0 0 1 1 0 
 *  Número 6: 1 1 1 0 0 1 1 1 
 *  Número 7: 0 0 1 1 0 0 1 0 
 *  Número 8: 1 1 1 1 0 1 1 1
 *  Número 9: 1 1 1 1 0 1 1 0 
 *  
 *  Valores a dar a cada pin para mostrar cada dígito
 */

 #include <TimerOne.h> //Libreria para la interrupción software
 
 volatile int espera = 5000; //Tiempo de espera - inversamente proporcional a la velocidad de aparición de los números
 const int potenciometro = A5;  //Pin donde conecto la patilla de entrada del potenciometro
 const int pinBoton = 3;        //Pin donde conecto la patilla de lectura del botón
 volatile bool parar = false;   //Variable para pausar el dado
 
 byte numeros[10] = { B01110111, //0
                      B00010010, //1
                      B10110101, //2
                      B10110110, //3
                      B11010010, //4
                      B11100110, //5
                      B11100111, //6
                      B00110010, //7
                      B11110111, //8
                      B11110110  //9
                      };    //Creación de un array con el byte que se debe asignar al puerto D
                            //Para obtener la representación del número en el visualizador
                            
 byte elementos = sizeof(numeros)/sizeof(numeros[0]);
 byte n_actual = 0;   //Variable donde almacenaremos el valor actual del dado
                      
                      
void setup() {
  DDRD = B11111111; //Digital [0 - 7] como pines de salida del puerto D. {3} asociado a una interrupcion hardware
  DDRC = DDRC & B11011111; //Analógico 5 como entrada del puerto C sin dañar los valores de otros pines del puerto C
  
  //Función pararDado saltará cuandose detecte un cambio en la señal recibida por el pin: pinBoton (3)
  //Cambio: CHANGE = flanco de subida + flanco de bajada (o viceversa)
  attachInterrupt(digitalPinToInterrupt(pinBoton), pararDado, CHANGE); 

  //Libreria Timer1 para crear una interrupción software que actuará como temporizador para llamar a la función
  Timer1.initialize(500000); //Se activará la interrupción cada 0.5 segundos
  Timer1.attachInterrupt(leerPotenciometro); //Función asociada a la interrupción software
}

void loop() {
  while(!parar){                      //Mientras la variable no indique que se debe parar el dado
      n_actual++;                     //Aumentamos el valor de n_actual en 1
      n_actual %= elementos;          //Siempre en los márgenes del vector
      PORTD = numeros[n_actual];      //Actualizamos los bits del puerto D con el número actual a representar
      delay(espera);                  //Esperamos el valor marcado con el potenciometro
  }
                                      //Sale del bucle cuando se ha pulsado el botón
  for(byte i=0 ; i<3 ; i++){          //Muestra el último valor del dado 3 veces
    PORTD = B00001000;                //Apaga todos los segmentos del visualizador
    delay(100);
    PORTD = numeros[n_actual]; 
    delay(200);
    parar = false;                    //Actualiza la variable para tirar de nuevo el dado
  }
}

void leerPotenciometro(){             //Función asociada a la interrupción software
  espera = analogRead(potenciometro); //Lectura analógica del potenciómetro 
}

void pararDado(){                     //Función asociada a la interrupción hardware
  if(!parar)                          //Esto evita que con el rebote de la pulsación si se generan dos interrupciones
    parar = true;                     //consecutivas, se actualice la variable dos veces 
}
