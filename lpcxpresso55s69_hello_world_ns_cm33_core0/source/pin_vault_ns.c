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

		//

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

		// lockout
		if (count == 3) {
			PRINTF_NSE("TOO MANY INCORRECT ATTEMPTS. LOCKED OUT.\r\n");

			while (1) {
				PRINTF_NSE("Enter admin code to reset lockout");
				char admin_input[16];
				int k = 0;
				int c;

				while (k < 15){
					c = DbgConsole_Getchar_NSE();
					if (c == '\r' || c == '\n') break;
					admin_input[k] = (char) c;
					DbgConsole_Putchar_NSE('o');

					k++;
				}
				admin_input[k] = '\0';
				DbgConsole_Putchar_NSE('\r');
				DbgConsole_Putchar_NSE('\n');

				if (admin_reset_NSE(admin_input)){
					PRINTF_NSE("Lockout cleared.\r\n");
					PRINTF_NSE("Returning to PIN entry . . . . . . \r\n");
					all_leds_off();
					count = 0;
					break;
				}
				else {
					PRINTF_NSE("INCORRECT ADMIN PIN.\r\n");
					PRINTF_NSE("try again \r\n");

					LED_ON(RED_PIN);
					volatile int d = 2000000;
					while (d--)
						;
					all_leds_off();
				}
			}
			continue;
		}

	}

	{
		uint8_t valid_image[] = {0x01, 0x02, 0x03, 0x04, 0x05,
									0xAE, 0xD2, 0xA6, 0xAB, 0xF7,
									0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
		uint8_t tampered_image[] = {0xff, 0x02, 0x03, 0x04, 0x05,
									0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		uint8_t valid_signature[] = {0x2A, 0x7C, 0x16, 0x12, 0x2D,
									0x00, 0x00, 0x00, 0x00, 0x00,
									0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		uint8_t wrong_signature[16] = {0x00};

		firmware_buffer valid_image_info;
		firmware_buffer tampered_image_info;
		firmware_buffer valid_signature_info;
		firmware_buffer wrong_signature_info;

		valid_image_info.array = valid_image;
		tampered_image_info.array = tampered_image;
		valid_signature_info.array = valid_signature;
		wrong_signature_info.array = wrong_signature;

		valid_image_info.array_length = sizeof(valid_image);
		tampered_image_info.array_length = sizeof(tampered_image);
		valid_signature_info.array_length = sizeof(valid_signature);
		wrong_signature_info.array_length = sizeof(wrong_signature);

		int fw;

		PRINTF_NSE("\r\n--------------------------"
				   "\r\nFIRMWARE VERIFICATION DEMO"
				   "\r\n--------------------------\r\n");

		// test1: verify that the valid image matches with the signature
		fw = verify_firmware_NSE(&valid_image_info, &valid_signature_info, 2);

		// 0x2A, 0x7C, 0x16, 0x12, 0x2D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		//compute_signature_NSE(&valid_image_info);

		// 0x01, 0x02, 0x03, 0x04, 0x05, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
		//compute_signature_NSE(&valid_signature_info);
		PRINTF_NSE(fw == 1? "test1 passed, valid fw accepted\r\n": "test1 failed\r\n");
		PRINTF_NSE("\r\n");

		// test2: verify that the tampered image doesn't match with the signature
		fw = verify_firmware_NSE(&tampered_image_info, &valid_signature_info, 3);
		PRINTF_NSE(fw == -3? "test2 passed, tampered fw rejected\r\n": "test2 failed\r\n");
		PRINTF_NSE("\r\n");

		// test3: verify that downgrade version doesn't work
		fw = verify_firmware_NSE(&valid_image_info, &valid_signature_info, 1);
		PRINTF_NSE(fw == -2? "test3 passed, downgrade rejected\r\n": "test3 failed\r\n");
		PRINTF_NSE("\r\n");

		// test4: verify that wrong signature is rejected
		fw = verify_firmware_NSE(&valid_image_info, &wrong_signature_info, 4);
		PRINTF_NSE(fw == -3? "test4 passed, wrong signature rejected\r\n": "test4 failed\r\n");
		PRINTF_NSE("\r\n");

	}

	/*
			 * audit log information
			 */
			{
				int j;
				int total = get_log_count_NSE();
				PRINTF_NSE("\r\n--------------------------"
						   "\r\n     AUDIT LOG START"
						   "\r\n--------------------------\r\n");
				for(j = 0; j<total && j<20; j++){
					int ev = get_log_event_NSE(j);
					int at = get_log_attempt_NSE(j);
					if (ev == 1) PRINTF_NSE("ACCESSS GRANTED\r\n");
					else if(ev == 0) PRINTF_NSE("ACCESS DENIED\r\n");
					else if(ev == -1) PRINTF_NSE("LOCKDOWN\r\n");
					else if(ev == 2) PRINTF_NSE("FIRMWARE ACCEPTED\r\n");
					else if(ev == -2) PRINTF_NSE("FIRMWARE REJECTED\r\n");
					else if(ev == 3) PRINTF_NSE("ADMIN RESET SUCCESS\r\n");
					else if(ev == -3) PRINTF_NSE("ADMIN RESET FAILED\r\n");
					else PRINTF_NSE("UNKNOWN");
				}
				PRINTF_NSE("--------------------------\r\n");
			}

}

