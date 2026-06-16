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
typedef int (*callbackptr)(char const *s1, char const *s2);

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
 * interface between secure and normal world for GETCHAR and PUTCHAR functions
 * called from normal world only
 */
char DbgConsole_Getchar_NSE(void);
void DbgConsole_Putchar_NSE(char ch);

/*
 * function to verify the pin input from the NS world
 * input: (string) entered pin
 * output: 1 on success, 0 on failure, -1 on lockout(in case of 3 consecutive failures)
 */
int verify_pin(const char* entered_pin);
void GPIO_PortClear_NSE(uint8_t pin_number);
void GPIO_PortSet_NSE(uint8_t pin_number);
#endif /* _VENEER_TABLE_H_ */
