# Arduino: Display, máscaras e interrupciones
Display de 7 segmentos controlado aplicando máscaras a los puertos y utilizando interrupciones software y hardware para ampliar su funcionalidad como dado electrónico
<br />
![](funcionando.gif)

![Ensamblaje del circuito](https://raw.githubusercontent.com/ankgiel/Display_mascaras/master/esquema_circuito.png)

Este proyecto puede ser fácil de programar con una programación básica, pero me he basado en las alternativas mostradas en el curso de Arduino Avanzado para crear este dado electrónico optimizado y de mayor eficacia que el que se podría obtener con una programación más sencilla.

Materiales utilizados:
* Placa de desarrollo Arduino Uno
* Display o visualizador de 7 segmentos
* Potenciómetro
* Botón o pulsador
* Protoboard
* 8 Resistencias de 220 Ohmios
* Cables macho-macho 

Las características a resaltar en este proyecto son:
* Máscaras para control de puertos
* Interrupciones software
* Interrupciones hardware  
<br />

## Máscaras 
Para comenzar conectaremos el display de 7 segmentos a los puertos digitales numerados de 0-7 en nuestro Arduino.
Mis conexiones son las siguientes:

             PIN: 7 6 5 4 - 2 1 0       El punto del display no lo utilizaré y no está conectado
        SEGMENTO: G F A B P D C E  
 
 
 ![Etiquetado display](https://raw.githubusercontent.com/ankgiel/Display_mascaras/master/etiquetado_display_7_segmentos.png)  [Pines del display](https://raw.githubusercontent.com/ankgiel/Display_mascaras/master/pines_display.PNG)
 
 Con mi esquema y la configuración de pines del datasheet, basta con construir una tabla con los valores que debemos dar a los pines para encender y apagar los leds de cada segmento según lo que queramos mostrar, ya sean dígitos o letras, en mi caso, los dígitos del 0 al 9.
 
         PIN: 7 6 5 4 - 2 1 0 
    SEGMENTO: G F A B P D C E 
 
    Número 0: 0 1 1 1 0 1 1 1 
    Número 1: 0 0 0 1 0 0 1 0
    Número 2: 1 0 1 1 0 1 0 1 
    Número 3: 1 0 1 1 0 1 1 0 
    Número 4: 1 1 0 1 0 0 1 0 
    Número 5: 1 1 1 0 0 1 1 0 
    Número 6: 1 1 1 0 0 1 1 1 
    Número 7: 0 0 1 1 0 0 1 0 
    Número 8: 1 1 1 1 0 1 1 1
    Número 9: 1 1 1 1 0 1 1 0 
 
 Para facilitar la utilización de estos valores binarios de 8bits los almaceno en un vector de byte y así podremos acceder a cada número indicando su posición en el vector.
 
    byte numeros[10] = { B01110111, //0
                         B00010010, //1
                         B10110101, //2
                         ...
                      
Para empezar a utilizar estas máscaras que aplicaremos al puerto D, donde hemos conectado los 6 segmentos de nuestro display en los valores 0, 1, 2, 4, 5, 6 y 7, es imprescindible declarar si utilizaremos estos pines como entrada o como salida. 

    DDRD = B11111111;

Las principales instrucciones para mostrar una secuencia de nuestros dígitos serán las siguientes:
    
    n_actual++;                    
    n_actual %= elementos;          
    PORTD = numeros[n_actual];      

A partir de aquí sólo queda programar la funcionalidad que deseemos ya sea utilizando bucles while, for, aumentando un valor por cada pulsación del botón... Otra opción sería que los números aparezcan de forma aleatoria en lugar de secuencial, esto se podría obtener guardando en *n_actual* un valor aleatorio entre 0 y el número de elementos de nuestro vector.
<br />

## Interrupción software 
Otra de las funcionalidades implementadas es la variación en la velocidad con la que se cambian los números que aparecen en el display. Para ello utilizo un potenciometro conectado a la entrada analógica A5 (sexto pin del puerto C) declarado de la siguiente forma:

    DDRC = DDRC & B11011111;* 
Con el uso del operador lógico *&* evitamos destruir los valores que estuviesen en el resto de pines del puerto C y conseguimos poner a 0 (entrada) el pin A5.

Como me gustaría leer el valor del potenciometro de forma casi continua, sin interferir demasiado en la ejecución del programa y sin tener que esperar a que termine de mostrarse la secuencia o sin necesidad de realizar lecturas después de mostrar cada valor, la mejor opción que hemos aprendido en este curso son las interrupciones software que podemos utilizar como temporizador para realizar algunas instrucciones fuera del flujo de ejecución normal de nuestro programa. 

Para la creación de esta interrupción software he utilizado la librería TimerOne.h que he inicializado a medio segundo y a la que he asignado la funcionalidad de leer el valor del potenciometro. Las siguientes instrucciones son necesarias para el uso de esta librería.

    #include <TimerOne.h> 
    Timer1.initialize(500000);
    Timer1.attachInterrupt(leerPotenciometro);

    void leerPotenciometro(){         
      espera = analogRead(potenciometro); 
    }

Anotar que las variables que cambian su valor dentro de las interrupciones deben ser declaradas como volátiles
    
    volatile int espera;    
<br />

# Interrupción hardware
Por último, la tercera característica destacable es el uso de un pulsador para parar el dado. Si queremos que nuestro programe reaccione a las pulsaciones de forma instantánea, sin tener que mantener pulsado el botón durante unos instantes para que cuando llegue el programa al punto de lectura del valor del botón éste lo detecte como pulsado, la única y mejor opción que tenemos es utilizar una interrupción hardware. 
Con el uso de este tipo de interrupciones conseguimos detectar cualquier cambio en la señal de entrada de los pines con acceso a interrupciones hardware. En el caso de mi placa de desarrollo Arduino Uno, los pines con capacidad para usar interrupciones hardware son los pines 2 y 3. Para este proyecto he decidido usar el pin 3 como interrupción hardware destinada a controlar los pulsos eléctricos recibidos del botón. 

 ![Interrupciones hardware](https://raw.githubusercontent.com/ankgiel/Display_mascaras/master/modos-interrupciones.png)

Para declarar el uso de este tipo de interrupciones no es necesitamos el uso de ninguna librería, con la siguiente instrucción las declaramos:
      
      attachInterrupt(digitalPinToInterrupt(pinBoton), pararDado, CHANGE);
                 Asociamos el pin 3 a la interrupción   Función    Detección de la Interrupción
           
 La secuencia de instrucciones dentro de cualquier interrupción debe ser lo más sencilla y corta posible, así que en esta interrupción me limito a cambiar el valor de una variable booleana que en el programa principal utilizaré para parar el dado y mostrar el número correspondiente al momento de la pulsación. <br />
 
Y esto es todo, en el código <*matriz_mascaras.ino*> podéis encontrar otros comentarios más detallados acerca del programa. 
