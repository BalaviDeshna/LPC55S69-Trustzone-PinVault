/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _VENEER_TABLE_H_
#define _VENEER_TABLE_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Entry function for debug PRINTF (DbgConsole_Printf)
 *
 * This function provides interface between secure and normal worlds
 * This function is called from normal world only
 *
 * @param s     String to be printed
 *
 */
void DbgConsole_Printf_NSE(char const *s);

/*
 * interface between secure and normal world for functions that print in debug console
 * called from normal world only
 */
char DbgConsole_Getchar_NSE(void);
void DbgConsole_Putchar_NSE(char ch);
void print_int_NSE(int value);

/*
 * function to verify the pin input from the NS world
 * input: (string) entered pin
 * output: 1 on success, 0 on failure, -1 on lockout(in case of 3 consecutive failures)
 */
int verify_pin(const char* entered_pin);

/*
 * functions to show LED output from the NS world
 * input: pin number
 * output: GPIO port gets cleared/set
 */
void GPIO_PortClear_NSE(uint8_t pin_number);
void GPIO_PortSet_NSE(uint8_t pin_number);

/*
 * read only functions to get information from the audit log
 */
int get_log_count_NSE(void);
int get_log_event_NSE(int index);
int get_log_attempt_NSE(int index);

/*
 * very basic verification
 */
int verify_firmware_NSE(const uint8_t *image, uint32_t image_length, const uint8_t *signature, uint32_t signature_length, uint32_t version);

#endif /* _VENEER_TABLE_H_ */
