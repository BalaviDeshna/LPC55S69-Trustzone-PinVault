/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "veneer_table.h"
#include "app.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define PRINTF_NSE DbgConsole_Printf_NSE
#define MAX_PIN_LENGTH 4

#define RED_PIN 6u
#define GREEN_PIN 7u
#define BLUE_PIN 4u

#define LED_ON(pin)  GPIO_PortClear_NSE(pin)
#define LED_OFF(pin) GPIO_PortSet_NSE(pin)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Code
 ******************************************************************************/

void all_leds_off(void) {
	LED_OFF(RED_PIN);
	LED_OFF(GREEN_PIN);
	LED_OFF(BLUE_PIN);
}

/*!
 * @brief Main function
 */
int main(void) {
	int result;
	int i;
	int ch;
	char entered_pin[MAX_PIN_LENGTH + 1];
	int count = 0;

	BOARD_InitHardware();

	all_leds_off();

	PRINTF_NSE("Welcome to Pin_Vault_NS\r\n");

	while (1) {
		PRINTF_NSE("ENTER THE PIN: ");
		i = 0;

		/*
		 * accepting input
		 */
		while (i < MAX_PIN_LENGTH) {
			ch = DbgConsole_Getchar_NSE();
			if (ch == '\r' || ch == '\n') break;
			entered_pin[i] = (char) ch;
			DbgConsole_Putchar_NSE(ch);

			i++;
		}
		DbgConsole_Putchar_NSE('\r');
		DbgConsole_Putchar_NSE('\n');
		entered_pin[i] = '\0';

		all_leds_off();

		/*
		 * result handling block
		 */
		result = verify_pin(entered_pin); // 1: success, 0: failure, -1: lockout
		// correct pin
		if (result == 1) {
			PRINTF_NSE("access granted\r\n");
			LED_ON(GREEN_PIN);
			volatile int delay = 5500000;
			while (delay--)
				;
			all_leds_off();
			count = 0;
			break;
		}
		// wrong pin
		else if (result == 0) {
			PRINTF_NSE("WRONG PIN.\r\n");
			LED_ON(RED_PIN);
			volatile int delay = 5500000;
			while (delay--)
				;
			all_leds_off();
			count++;

		}

		/*
		 * audit log information
		 */

		// lockout
		if (count == 3) {
			PRINTF_NSE("TOO MANY INCORRECT ATTEMPTS. LOCKED OUT.\r\n");
			while (1) {
				LED_ON(RED_PIN);
				LED_OFF(BLUE_PIN);
				volatile int d = 5500000;
				while (d--)
					;
				LED_OFF(RED_PIN);
				LED_ON(BLUE_PIN);
				d = 5500000;
				while (d--)
					;
			}
		}
	}
}

