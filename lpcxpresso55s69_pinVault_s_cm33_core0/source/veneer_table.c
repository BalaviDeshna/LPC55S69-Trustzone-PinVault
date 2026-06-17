/*
 * Copyright 2018 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif

#include "stdint.h"
#include "arm_cmse.h"
#include "tzm_api.h"
#include "veneer_table.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include <string.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_STRING_LENGTH 0x400
#define MAX_LOG_ENTRIES 20

typedef struct {
	int event; // 1, 0, -1
	int attempt_number;
}Audit_Entry;

static Audit_Entry audit_log[MAX_LOG_ENTRIES];
static int log_count = 0;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/* strnlen function implementation for arm compiler */
#if defined(__arm__)
size_t strnlen(const char *s, size_t maxLength)
{
    size_t length = 0;
    while ((length <= maxLength) && (*s))
    {
        s++;
        length++;
    }
    return length;
}
#endif

TZM_IS_NOSECURE_ENTRY void DbgConsole_Printf_NSE(char const *s)
{
    size_t string_length;
    /* Access to non-secure memory from secure world has to be properly validated */
    /* Check whether string is properly terminated */
    string_length = strnlen(s, MAX_STRING_LENGTH);
    if ((string_length == MAX_STRING_LENGTH) && (s[string_length] != '\0'))
    {
        PRINTF("String too long or invalid string termination!\r\n");
        while (1)
            ;
    }

    /* Check whether string is located in non-secure memory */
    /* Due to the bug in GCC 10 cmse_check_pointed_object() always fail, do not call it, see GCC Bugzilla - Bug 99157.
       Solved in GCC 10.3 version */
#if !((__GNUC__ == 10) && (__GNUC_MINOR__ < 3))
    if (cmse_check_address_range((void *)s, string_length, CMSE_NONSECURE | CMSE_MPU_READ) == NULL)
    {
        PRINTF("String is not located in normal world!\r\n");
        while (1)
            ;
    }
#endif
    PRINTF(s);
}

TZM_IS_NOSECURE_ENTRY char DbgConsole_Getchar_NSE(void){
    return DbgConsole_Getchar();
}


TZM_IS_NOSECURE_ENTRY void DbgConsole_Putchar_NSE(char ch){
    DbgConsole_Putchar(ch);
}

TZM_IS_NOSECURE_ENTRY int verify_pin(const char* entered_pin){
	static const char secure_pin[] = "0987";
	static int attempt_counter = 0;
	int result;

	// locks out on 3 failures
	if (attempt_counter >= 3){
		result = -1;
	}

	// success on pin match
	if (strcmp(entered_pin, secure_pin) == 0){
		attempt_counter = 0;
		result = 1;
	}

	// failure on all other cases
	else{
		attempt_counter++;
		result = 0;
	}

	// logging the attempt
	audit_log[log_count % MAX_LOG_ENTRIES].event = result;
	audit_log[log_count % MAX_LOG_ENTRIES].attempt_number = attempt_counter;
	log_count++;
	return result;
}

TZM_IS_NOSECURE_ENTRY void GPIO_PortClear_NSE(uint8_t pin_number){
	GPIO_PortClear(GPIO, 1u, 1u << pin_number);
}

TZM_IS_NOSECURE_ENTRY void GPIO_PortSet_NSE(uint8_t pin_number){
	GPIO_PortSet(GPIO, 1u, 1u << pin_number);
}
