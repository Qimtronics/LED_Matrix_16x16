#ifndef UART_0_DRIVER_h
#define UART_0_DRIVER_h

#include "driver/uart.h"
#include "Arduino.h"

#define SIZE_PAYLOAD_RECV_MSG			30

uint8_t payload_uart[SIZE_PAYLOAD_RECV_MSG];

typedef enum
{
	CHANGE_ID_BLE = 0x40,
	RESET_SCORE = 0x41,
	RESET_FACTORY = 0x42,
};

void send_payload_uart_0_on_byte(uint8_t *buf, uint8_t size_)
{
	for(uint8_t x = 0; x < size_; x++)
	{
		Serial.print(buf[x], HEX);
		Serial.print(" ");
	}
	Serial.println();
}

void send_payload_uart_0_on_str(String msg)
{
	Serial.println(msg);
}

void receive_payload_uart0(uint8_t *len_data)
{
	uint8_t UART0_data_length = 0;
	bool two_byte_before_eof_founded = false;
	bool collect_data = false;
	bool check_dest = false;
	
	*len_data = 0;
	
	memset(payload_uart, 0, sizeof(payload_uart));
	
	while(Serial.available())
	{
		uint8_t data = Serial.read();		
		
		if(two_byte_before_eof_founded)
		{
			if((data == 0x0A) && (UART0_data_length > 4))
			{
				payload_uart[UART0_data_length] = data;
				UART0_data_length++;				
				collect_data = false;	
				*len_data = UART0_data_length;
				break;					
			}
			else
			{
				two_byte_before_eof_founded = false;
			}
		}
		
		if(check_dest)
		{
			if((payload_uart[UART0_data_length - 1] == 'C') || (data == 'Q'))
			{
				collect_data = true;
			}
			else
			{
				collect_data = false;
			}
			check_dest = false;
		}

		if(data == 0x0D)
		{
			two_byte_before_eof_founded = true;
		}
		else
		{
			if(!collect_data)
			{
				if(data == 'C') 
				{
					two_byte_before_eof_founded = false;
					collect_data = true;
					UART0_data_length = 0;
					check_dest = true;
					memset(&payload_uart[0], 0, (uint8_t)SIZE_PAYLOAD_RECV_MSG);
				}
			}
		}	
		
		if(collect_data)
		{			
			payload_uart[UART0_data_length] = data;
			UART0_data_length++;	
		}
	}
}

#endif /* UART_0_DRIVER_h */

