# TrabajoFinal

	Trabajo final para la materia programación de microcontroladores de la carrera especialización 
		en sistemas embebidos.

Objetivos
	
	Implementar drivers portables para los dispositivos (Display, transceptor MRF24J40) conectados 
		a la placa de desarrollo NUCLEO-F446RE.
	Implementar una máquina de estados finitos.
	Comunicar dos placas por medio de transceptores inhalámbricos, con comunicación bidireccional.
	
Estructura del proyecto

	TrabajoFinalPCSE
		├── /API
		│	├── /Inc
	    │	│	├── API_debounce.h
	    │	│	├── API_delay.h
	    │	│	├── API_LCD_port.h
	    │	│	├── API_LCD.h
	    │	│	├── API_MRF24J40_port.h
	    │	│	├── API_MRF24J40.h
	    │	│	├── API_UART.h
		│	│	└── compatibility.h
		│	└── /Src
		│		├── API_debounce.c
		│		├── API_delay.c
		│		├── API_LCD_port.c
		│		├── API_LCD.c
		│		├── API_MRF24J40_port.c
		│		├── API_MRF24J40.c
		│		└── API_UART.c
		├── /Core
		│	├── /Inc    
	    │	│   └── main.h    
		│	└── /Src
		│	    └── main.c
		└── /Drivers


Debounce API

	Implementación de antirrebote para pulsador con delay no bloqueante.
	
	Funciones públicas:

		/**
		  * @brief  Inicializa las variables de la máuina de estado para la función anti rebote.
		  * @param  Puntero a la estructura que contiene las variabes.
		  * @retval None
		  */
		void DebounceFSMInit(debounceData_t * antirrebote_boton_n);
		
		/**
		  * @brief  Utilizando una máquina de estados compruebo si hubo un cambio con el estado anterior.
		  * @param  Puntero a la estructura que contiene las variabes.
		  * @param  Estado actual - booleano.
		  * @retval Devuelvo la información si el botón fue presionado, soltado, sin cambios o si hubo un error.
		  * @note	En el enunciado la función no posee parámetros, pero de esta forma independizo totalmente
		  * 		el driver del hardware en donde lo implemento.
		  */
		estadoPulsador_t DebounceFSMUpdate(debounceData_t * antirrebote_boton_n, bool_t estado_pin);

Delay API

	Implementación de delay no bloqueante.
    
	Funciones privadas:
    	
    	/**
		  * @brief  Consulto el estado del delay.
		  * @param  Puntero a la estructura con las variables.
		  * @retval Estado del delay (contando / tiempo cumplido).
		  * @note	En el trabajo práctico la define como función pública, pero para mejorar
		  * 		el encapsulamiento la defino como privada ya que solo se utiliza en el
		  * 		ámbito de este archivo.
		  */
    	static bool_t DelayIsRunning(delayNoBloqueanteData * delay);
    	
	Funciones públicas:
	
		/**
		  * @brief  Inicializa el delay no bloqueante.
		  * @param  Puntero a la estructura con las variables.
		  * @param  Duración del delay.
		  * @retval None
		  */
		void DelayInit(delayNoBloqueanteData * delay, tick_t duration);
		
		/**
		  * @brief  Obtengo el estado del delay.
		  * @param  Puntero a la estructura con las variables.
		  * @retval Estado del delay (contando / tiempo cumplido).
		  */
		bool_t DelayRead(delayNoBloqueanteData * delay);
		
		/**
		  * @brief  Redefino el tiempo de delay.
		  * @param  Puntero a la estructura con las variables.
		  * @param  Duración del delay.
		  * @retval None
		  */
		void DelayWrite(delayNoBloqueanteData * delay, tick_t duration);
		
		/**
		  * @brief  Reseteo la cuenta del tiempo.
		  * @param  Puntero a la estructura con las variables.
		  * @retval None
		  */
		void DelayReset( delayNoBloqueanteData * delay);

LCD API

	Implementación driver display.
		
	Funciones privadas:
	
		/**
		  * @brief	Escribo 8 bits por el puerto destinado al display de 4 bits por vez.
		  * @param  Dato que escribo, 1 byte.
		  * @param  Indicador si es un comando lo que se escribe o información, 1 byte.
		  * @retval None.
		  */
		static void LCDWrite4Bits(char dato, uint8_t rs);

		/**
		  * @brief	Escribo 8 bits por el puerto destinado al display.
		  * @param  Dato que escribo, 1 byte.
		  * @param  Indicador si es un comando lo que se escribe o información, 1 byte.
		  * @retval None.
		  */
		static void LCDWrite8Bits(char dato, uint8_t rs);

	Funciones públicas:

		/**
		  * @brief  Inicialización del display
		  * @retval None
		  */
		void LCDInint(void);
		
		/**
		  * @brief  Escribo un caracter en el display.
		  * @param  Caracter que voy a escribir.
		  * @retval None.
		  */
		void LCDWriteCaracter(char caracter);
		
		/**
		  * @brief	Escribo una cadena en el display.
		  * @param  Puntero a la cadenas que voy a escribir.
		  * @retval None.
		  */
		void LCDWriteString(char * cadena);
		
		/**
		  * @brief  Limpio la pantalla del display.
		  * @retval None.
		  */
		void LCDClear(void);
		
		/**
		  * @brief	Dejo el cursor en la posición indicada.
		  * @param  Cantidad de caracteres desplazados desde el inicio de la primer linea.
		  * @retval None.
		  */
		void LCDGoto(uint8_t pos);
		
		/**
		  * @brief  Escribo dos cadenas de caracteres, una en cada linea.
		  * @param  Puntero a la primer cadena que voy a escribir.
		  * @param  Puntero a la segunda cadena que voy a escribir.
		  * @retval None.
		  */
		void LCDWrite2String(char *linea1, char *llinea2);
		
		/**
		  * @brief  Borro el contenido de una linea del display.
		  * @param  Cantidad de caracteres desplazados desde el inicio de la primer linea.
		  * @retval None.
		  */
		void LCDClearLinea(uint8_t pos);

UART API

	Implementación driver puerto UART con interrupciones para la lectura

	Funciones públicas:

		/**
		  * @brief  Inicialización del puerto serie.
		  * @param  Puntero a la estructura que contiene las variabes del puerto seleccionado.
		  * @retval Devuelvo verdadero si la inicialización se realizo sin problemas.
		  */
		bool_t UARTtInit(puerto_UART * data_port, UART_HandleTypeDef * huart);
		
		/**
		  * @brief  Envío por el puerto una cadena de caracteres.
		  * @param  Puntero a la estructura que contiene las variabes del puerto.
		  * @param  Puntero a la cadena que se desea enviar.
		  * @retval Devuelvo la información del resultado de la transmisión
		  */
		estado_TX_RX_t UARTSendString(puerto_UART * data_port, const uint8_t * pstring);
		
		/**
		  * @brief  Envío por el puerto una cadena de caracteres.
		  * @param  Puntero a la estructura que contiene las variabes del puerto.
		  * @param  Puntero a la cadena que se desea enviar.
		  * @param  Tamaño de la cadena a enviar.
		  * @retval Devuelvo la información del resultado de la transmisión.
		  */
		estado_TX_RX_t UARTSendStringSize(puerto_UART * data_port, const uint8_t * pstring, uint16_t size);
		
		/**
		  * @brief  Recibo un mensaje entrante.
		  * @param  Puntero a la estructura que contiene las variabes del puerto.
		  * @param  Tamaño de la cadena que se espera entre.
		  * @retval None.
		  */
		void UARTReceiveStringSize(puerto_UART * data_port, uint16_t size);
		
		/**
		  * @brief  Consulto si ha llegado un mensaje por el puerto.
		  * @param  Puntero a la estructura que contiene las variabes del puerto.
		  * @retval Booleano indicando la llegada.
		  */
		bool_t UARTIsNewMessage(puerto_UART * data_port);
		
		/**
		  * @brief  Envio por el puerto comandos ASCII para limpiar la pantalla e imprimir información de la conección.
		  * @param  Puntero a la estructura que contiene las variabes del puerto.
		  * @retval Devuelvo la información del resultado de la transmisión.
		  */
		bool_t PutConfiguration(puerto_UART * data_port);
		
		/**
		  * @brief	Inicializa las variables de la máuina de estado para la función anti rebote.
		  * @param  Puntero a la estructura que contiene las variabes.
		  * @param  Puntero a la estructura que contiene las variabes del puerto.
		  * @param  Puntero al buffer de almacenamiento del mensaje entrante.
		  * @retval
		  */
		void UARTFSMInit(datosFSMUART_t * datosFSM, puerto_UART * data_port, char * bufferEntrada, uint8_t bufferSize);
		
		/**
		  * @brief	Utilizando una MEF compruebo el mensaje entrante, esperando un enter como
		  * 		marca para el final de la cadena.
		  * @param  Puntero a la estructura que contiene las variabes de la máquina de estados.
		  * @retval	Devuelvo la información del llenado del buffer.
		  */
		estadoComunicacionUART_t CheckMsgUART(datosFSMUART_t * datosFSM);

MRF24J40 API

	Implementación driver módulo MRF24J40
	Driver independiente de la plataforma de uso y del compilardor.

	Funciones privadas:
	
		/**
		 * @brief  Inicialización de variables de configuración por defecto.
		 * @param  None
		 * @retval None
		 */
		 static void InicializoVariables(void);
 
		 /**
		 * @brief  Inicialización del módulo MRF24J40MA
		 * @param  None
		 * @retval Estado de la operación (TIME_OUT_OCURRIDO, INICIALIZACION_OK)
		 */
		static MRF24_State_t InicializoMRF24(void);

		/**
		 * @brief  Escribo en registro de 1 byte un dato de 1 byte
		 * @param  Dirección del registro - 1 byte
		 * @param  Dato - 1 byte
		 * @retval None
		 */
		static void SetShortAddr(uint8_t reg_address, uint8_t valor);

		/**
		 * @brief  Escribo de en registro de 2 bytes un dato de 1 byte
		 * @param  Dirección del registro - 2 bytes
		 * @param  Dato - 1 byte
		 * @retval None
		 */
		static void SetLongAddr(uint16_t reg_address, uint8_t valor);

		/**
		 * @brief  Leo en registro de 1 byte un dato de 1 byte
		 * @param  Dirección del registro - 1 byte
		 * @param  Dato - 1 byte
		 * @retval Valor devuelto por el módulo - 1 byte
		 */
		static uint8_t GetShortAddr(uint8_t reg_address);

		/**
		 * @brief  Leo en registro de 2 bytes un dato de 1 byte
		 * @param  Dirección del registro - 2 bytes
		 * @param  Dato - 1 byte
		 * @retval Valor devuelto por el módulo - 1 byte
		 */
		static uint8_t GetLongAddr(uint16_t reg_address);

		/**
		 * @brief  Seteo en el módulo la dirección corta guardada en mrf24_data_config
		 * @param  None
		 * @retval None
		 */
		static void SetDeviceAddress(void);

		/**
		 * @brief  Seteo en el módulo en canal guardado en mrf24_data_config
		 * @param  None
		 * @retval None
		 */
		static void SetChannel(void);

		/**
		 * @brief  Seteo en el módulo la dirección mac guardada en mrf24_data_config
		 * @param  None
		 * @retval None
		 */
		static void SetDeviceMACAddress(void);

	Funciones públicas:

		 * @brief  Inicialización del módulo MRF24J40MA.
		 * @param  None.
		 * @retval Estado de la operación (TIME_OUT_OCURRIDO, INICIALIZACION_OK).
		 */
		MRF24_State_t MRF24J40Init(void)

		/**
		 * @brief   Paso por referencia la dirección del mensaje a enviar.
		 * @param   Puntero al mensaje.
		 * @retval  Estado de la operación (ERROR_INESPERADO, OPERACION_REALIZADA).
		 */
		 void MRF24SetMensajeSalida(const char * mensaje);

		/**
		 * @brief   Configuro la dirección corta del dispositivo con el que me comunicaré.
		 * @param   Dirección corta del dispositivo - 2 bytes.
		 * @retval  Estado de la operación (ERROR_INESPERADO, OPERACION_REALIZADA).
		 */
		void MRF24SetDireccionDestino(uint16_t direccion);

		/**
		 * @brief   Configuro la PANID del dispositivo con el que me comunicaré.
		 * @param   Dirección PANID de dos bytes.
		 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, OPERACION_REALIZADA).
		 */
		void MRF24SetPANIDDestino(uint16_t panid);

		/**
		 * @brief   Envío la información almacenada en la estructura de salida
		 * @param   None
		 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, TRANSMISION_REALIZADA).
		 */
		void MRF24TransmitirDato(void);

		/**
		 * @brief   Consulto si se levantó la bandera indicando la llegada de un mensaje.
		 * @param   None
		 * @retval  Estado de la operación (ERROR_INESPERADO, MSG_PRESENTE, MSG_NO_PRESENTE).
		 */
		bool_t MRF24IsNewMsg(void);

		/**
		 * @brief   Recibir un paquete y dejarlo en el bufer de entrada de mrf24_data_config
		 * @param   None
		 * @retval  Estado de la operación (OPERACION_NO_REALIZADA, MSG_LEIDO).
		 */
		void MRF24ReciboPaquete(void);

		/**
		 * @brief   Devuelvo un puntero al mensaje recibido por RF.
		 * @param   None.
		 * @retval  Puntero a la cadena recibida.
		 */
		uint8_t * MRF24GetMensajeEntrada(void);

		/**
		 * @brief   Obtengo el PANID en el que estoy.
		 * @param   None.
		 * @retval  La dirección de 2 bytes de mi PANID.
		 */
		uint16_t MRF24GetMiPANID(void);


