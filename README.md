[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/rb0M7Pn8)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=23781127&assignment_repo_type=AssignmentRepo)
# Lab07: Visualización en LCD 16x2 usando módulo I²C con microcontrolador PIC

## Integrantes

* [Laura Alejandra Fuentes Ubaque](https://github.com/LauraAlejandraFuentes)
* [Juan Sebastian Guerrero Gualteros](https://github.com/juanseguerrerogu07)
* [Pedro Felipe Jimenez Celis](https://github.com/pedrofejimenezce-ship-it)
* [Samuel Corro Pedrozo](https://github.com/SamuelCorro)

## Documentación

**1. Objetivos de aprendizaje**

1.Configurar el modulo I²C (MSSP) del PIC18F45K22 en modo maestro

2.Comunicar el PIC con una LCD 16x2 utilizando el adaptador basado en PCF8574

3.Implementar funciones para enviar comandos y caracteres vía  I²C

4.Mostrar mensajes en la pantalla LCD desde el programa principal

**2.Materiales**

1.PIC18F45K22

2.Programador/debugger PICkit 4

3.Fuente de alimentación (o PICkit 4)

4.LCD 16X2

5.Módulo I²C PCF8574

![alt text](image.png)

6.Entorno de programación MPLAB X IDE con compilador XC8

**3.Fundamento teórico**

**I²C** es un protocolo de comunicación serial de dos hilos que utiliza una línea de datos serial (SDA) y una línea de reloj (SCL).

Este protocolo permite múltiples dispositivos esclavos (o periféricos) en el mismo bus de comunicación, y también puede soportar múltiples maestros que envién y reciban comandos y datos.

**I²C** es una comunicación **half-duplex**, donde solo un controlador o un dispositivo objetivo envía datos por el bus en un momento dadoo. En comparación, la Interfaz Periférica Serial(**SPI**) es un protocolo **full-duplex**, donde se pueden enviar y recibir datos simultáneamente. SPI requiere cuatro lineas para la comunicación: dos lineas de datos utilizadas para enviar y recibir información hacia y desde el dispositivo objetivo. Además de la línea de reloj serial, se emplea una línea de chip select única para seleccionar el dispositivo con el que se desea comunicar, junto con las dos líneas de datos usadas para la entrada y salida del dispositivo.

La comunicación se transmite en paquetes de bytes, con una dirección única para cada dispositivo esclavo.

La siguiente figura muestra la estructura de una transferencia típica en el bus I²C, donde se observa la condición de inicio, el envío de la dirección del dispositivo con el bit de lectura/escritura, el bit de reconocimiento (ACK) y finalmente la transmisión de datos seguida de la condición de parada.

![alt text](image-1.png)

La pantalla LCD 16X2 se controlará en este laboratorio mediante el protocolo **I²C** utilizando un expansor de puertos **PCF8574**. Este enfoque reduce la cantidad de pines necesarios para la conexión, ya que utiliza únicamente dos líneas: SDA (datos) y ´´´SCL´´´ (reloj).

El PIC18F45K22 cuenta con el módulo **MSSP** (Master Synchronous Serial Port), capaz de trabajar en protocolos **SPI** e **I²C**

El siguiente diagrama representa el funcionamiento interno del módulo MSSP configurado en modo **I²C**

Incluye:

➤ Buffers de transmisión y recepción (SSBUF y SSPSR)

➤ Detectores de Start/Stop, ACK, colisiones

➤ Control de reloj (Clock Gen)

➤ Generador de baudios

➤ Lógica para habilitar SDA y SCL

➤ Flags y registros de control (SSPCON1, SSPCON2, SSPSTAT, PIR1bits.SSPIF)

![alt text](image-2.png)

Entonces, en este laboratorio se usa en modo **I²C** Maestro para enviar datos al módulo LCD **PCF8574**

El módulo **PCF8574** suele tener una dirección base de 7 bits igual a 0 x 27. Sin embargo, en el protocolo **I²C**, la dirección que se transmite al bus debe tener 8 bits, donde el último bit indica si se va a leer (1) o escribir (0). Como en este caso solo se realiza escritura hacia el LCD, la dirección efectiva enviada es 0 x 4E, que resulta de desplazar 0 x 27 una posición a la izquierda ( 0 x 27 << 1). Esta dirección ya se encuentra definida en el código como:

 #define LCD_ADDR 0x4E

**¿Por qué usar I²C con la LCD?** 
 
  Sin I²C → la LCD requiere 6 a 8 pines del microcontrolador.

Con I²C → solo se requieren 2 pines: SCL (RC3) y SCL (RC4).


**EXPLICACION DE LOS CODIGOS**
    static void lcd_send_nibble_cmd(unsigned char nibble)
    static void lcd_send_nibble_data(unsigned char nibble)
El LCD en modo I2C recibe datos de 4 bits (nibble) a la vez. Cada función pulsa la línea EN (Enable) poniéndola en 1 y luego en 0 para que el LCD capture el dato. La diferencia es que _cmd deja RS=0 (comando) y _data deja RS=1 (carácter a mostrar).

    void lcd_cmd(unsigned char cmd)
Divide el byte del comando en dos nibbles (alto y bajo) y los manda por I2C con start/stop. Por ejemplo, lcd_cmd(0x01) limpia la pantalla.

    void lcd_write_char(char c)
Igual que lcd_cmd pero usando lcd_send_nibble_data, lo que le indica al LCD que el byte es un carácter visible, no un comando.

    void lcd_write_string(const char *str)
Recorre el string y llama lcd_write_char por cada carácter hasta el \0.

    void lcd_set_cursor(unsigned char row, unsigned char col)
Mueve el cursor a fila y columna específicas. Fila 0 parte en dirección DDRAM 0x80, fila 1 en 0xC0. Se le suma la columna para la posición exacta.

    void lcd_clear(void)
Manda el comando 0x01 y espera 2 ms porque esta operación es más lenta que las demás en el HD44780.

    void lcd_create_char(uint8_t slot, uint8_t *data)
Permite guardar un símbolo propio (como un grado °, una flecha, etc.) en la memoria CGRAM del LCD. Hay 8 slots disponibles (0–7). Se le pasa el slot y un arreglo de 8 bytes que definen los píxeles del carácter fila por fila. Al terminar vuelve a DDRAM para no alterar la escritura normal.

    void lcd_init(void)
Secuencia de arranque obligatoria del HD44780 en modo I2C/4 bits: espera 50 ms tras encendido, manda comandos 0x33 y 0x32 para forzar el modo 4 bits, luego configura 2 líneas (0x28), display encendido sin cursor (0x0C), incremento automático del cursor (0x06) y limpia la pantalla (0x01).

    TRIS_SCL = 1;  TRIS_SDA = 1;
    ANSEL_SCL = 0; ANSEL_SDA = 0;
SCL y SDA se ponen como entradas digitales (el propio módulo MSSP las maneja internamente). Se desactiva la función analógica para que funcionen como pines digitales.

    SSPSTAT = 0x80;
    SSPCON1 = 0x28;
    SSPADD  = 39;
0x80 desactiva el slew rate (recomendado a 100 kHz). 0x28 pone el módulo MSSP en modo I2C maestro. El valor 39 en SSPADD fija la velocidad a 100 kHz con FOSC de 16 MHz usando la fórmula (FOSC / (4 * f_SCL)) - 1.

    SSPCON2bits.SEN = 1;
    while (!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
Inicia la transmisión I2C poniendo la línea SDA en bajo mientras SCL está en alto. El programa espera a que el hardware levante el flag SSPIF confirmando que terminó, luego lo limpia manualmente.

    SSPCON2bits.PEN = 1;
    while (!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
Libera el bus I2C (SDA sube mientras SCL está en alto). Mismo patrón de espera y limpieza del flag.

    SSPBUF = data;
    while (!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
Carga el byte en el buffer de transmisión y espera a que el hardware lo envíe bit a bit por SCL/SDA. Al terminar limpia el flag para dejarlo listo para la siguiente operación.

    #pragma config FOSC = INTIO67
    #define _XTAL_FREQ 16000000UL
Oscilador interno, watchdog y brownout apagados, MCLR externo. El define le indica a __delay_ms la frecuencia real para calcular los tiempos correctamente.

    uint8_t heart[8] = {0x00,0x0A,0x1F,...};
    uint8_t check[8] = {...};
    uint8_t bell[8]  = {...};
Cada arreglo de 8 bytes define un símbolo píxel a píxel (5 bits útiles por fila). Se cargan en la CGRAM del LCD en la demo 3.

    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS  = 0b10;
    I2C_init();
    lcd_init();
Fija el oscilador interno a 16 MHz, luego inicializa I2C y el LCD antes de entrar al loop.

    while (1) {
        demo_texto_estatico();   __delay_ms(3000);
        demo_desplazamiento();   __delay_ms(1000);
        demo_caracteres_especiales(); __delay_ms(3000);
    }
Cicla entre las tres demos con pausas entre ellas.

    lcd_set_cursor(0, 0);
    lcd_write_string("  PIC18F45K22   ");
    lcd_set_cursor(1, 0);
    lcd_write_string("   I2C LCD OK!  ");
Escribe dos líneas fijas centradas visualmente con espacios.

    lcd_set_cursor(1, 0);
    lcd_write_string(msg);
    for (i = 0; i < 16; i++) {
        __delay_ms(300);
        lcd_cmd(0x18);
    }
Escribe el mensaje en fila 1 y luego desplaza toda la pantalla 16 posiciones a la izquierda con pausas de 300 ms, dando efecto de scroll.

    lcd_create_char(0, heart);
    lcd_create_char(1, check);
    lcd_create_char(2, bell);
    lcd_write_char(0); // corazón
    lcd_write_char(1); // check
    lcd_write_char(2); // campana
Carga los tres símbolos en CGRAM (slots 0, 1, 2) y los muestra en posiciones separadas de la fila 1.

## Diagramas

![alt text](image-4.png)

Conexión PIC18F45K22 con PICkit 4

![alt text](image-3.png)

Diagrama de montaje 


## Evidencias de implementación

[video simulacion I2C](https://youtu.be/3he64MFGGu8)

## Preguntas

1. ¿Por qué I²C se clasifica como half-duplex mientras que SPI es full-duplex? ¿Qué implicación práctica tiene esa diferencia para el control de una LCD?.
2. En I2C_init() se asigna SSPCON1 = 0x28. Desglose ese valor bit a bit e identifique qué modo de operación del MSSP se está seleccionando y por qué se elige ese valor.
3. Las funciones I2C_start(), I2C_stop() e I2C_write() comparten el mismo patrón: activar un bit de control y luego esperar con while(!PIR1bits.SSPIF). ¿Qué representa la bandera SSPIF y por qué se limpia después de cada operación?.
4. El fuse PBADEN = OFF está presente en la configuración. ¿Qué efecto tendría dejarlo en ON sobre los pines del puerto B, y por qué podría causar problemas si se usan esos pines como salidas digitales?.
5. Compare el control de la LCD en modo paralelo (lab04) con el modo I²C de este laboratorio. Mencione ventajas y desventajas de cada enfoque en términos de: cantidad de pines usados, velocidad de actualización y complejidad del código.
6. El bus I²C permite conectar múltiples esclavos con solo dos hilos. Si se quisiera agregar un segundo módulo PCF8574 al mismo bus (por ejemplo, para controlar un segundo LCD), ¿qué cambio mínimo sería necesario en el hardware y en el código?

## Conclusiones

## Referencias