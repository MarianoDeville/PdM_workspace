Trabajo práctico 5 de Programación de Microcontroladores para la Carrera de Especialización en Sistemas Embebidos

Autor: Mariano Ariel Deville

Objetivo:
Implementar un módulo de software para utilizar la UART.

Punto 1

Implementar un módulo de software en un archivos fuente API_uart.c con su correspondiente archivo de cabecera API_uart.h y ubicarlos en el proyecto dentro de  las carpetas /API/src y /API/inc, respectivamente.

Punto 2

Sobre el ejercicio de la práctica 4, implementar un programa que utilice el módulo de uart implementado por ustedes para informar al usuario cuando se producen los flancos ascendentes y descendentes.

Punto 3 (optativo)

Implementar una función para borrar todo el contenido de la terminal.
Cada vez que se envíe un caracter ‘c’ desde la terminal de la PC, primero se debe borrar todo el contenido de la terminal.  Luego, el microcontrolador debe responder con el envío de los parámetros de inicialización de la UART en un formato que sea legible claramente desde la terminal. 

Resolución:

Se utilizó la UART por interrupción para asegurar no perder mensajes entrantes, además de informar los flancos del pulsador, en caso de que sea descendente, se informa tambían el período con el que el led parpadea.
El puerto utilizado es el UART5, pines PC12 (UART5_TX) y PD2 (UART5_RX) pines 3 y 4 del conector CN7 de la placa NUCLEO-F446RE.