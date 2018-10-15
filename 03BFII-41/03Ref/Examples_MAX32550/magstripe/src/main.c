/*
 * @file magstripe/src/main.c
 *
 ******************************************************************************
 * Copyright (C) 2016-2018 Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 ******************************************************************************
 */

/**
 * @defgroup EX_MAGSTRIPE Magstripe Example
 * @{
 *
 *
 * @ingroup MML_Examples
 * @brief This example show you how to configure MSR to write data decode by MSR tracks on a terminal via UART.
 * @note Set of maximum frequency is already did in function SystemCoreClockUpdate() in system/src/cortexm/_initialize_hardware.c
 *
 * @}
 */

/* Global includes */
#include <MAX325xx.h>
#include <stdio.h>
#include <string.h>
#include <errors.h>


#define EX_MAGSTRIPE_VERS_MAJOR	<VERSMAJ>
#define EX_MAGSTRIPE_VERS_MINOR	<VERSMIN>
#define EX_MAGSTRIPE_VERS_PATCH	<VERSPAT>
#define EX_MAGSTRIPE_VERSION_STRING	"v" xstr(EX_MAGSTRIPE_VERS_MAJOR) "." xstr(EX_MAGSTRIPE_VERS_MINOR) "." xstr(EX_MAGSTRIPE_VERS_PATCH)


/* Function Prototypes */
/*
 * This function is used to prepare magstripe reader for a card to be swiped.
 */
void PrepareForSwipe(void);

/*
 *  This function is called at completion of swipe data collection.
 */
void MsrCallback(void);

/*
 * This function is called at completion of swipe data collection.
 */
void ProcessSwipe(void);

/* Global Data */
volatile unsigned int SwipeComplete;
mml_msr_decoded_track_t decoded_track[MSR_NUM_TRACKS];
int Processed = 0;
int Discarded = 0;

int main(void) {
	int result = COMMON_ERR_UNKNOWN;
	mml_uart_config_t uart_conf;

	/* Initialize UATR0 port with default configurations */
	uart_conf.baudrate = 115200;
	uart_conf.data_bits = UARTn_CTRL_SIZE_bits8;
	uart_conf.flwctrl = UARTn_CTRL_RTSCTSF_disable;
	uart_conf.parity = MML_UART_PARITY_NONE;
	uart_conf.parity_mode = MML_UART_PARITY_MODE_ONES;
	uart_conf.rts_ctl = UARTn_PIN_RTS_lo;
	uart_conf.stop_bits = UARTn_CTRL_STOP_stop1;
	result = mml_uart_init(MML_UART_DEV0, uart_conf);
	if (result) {
		return result;
	}
	/* Welcome message */
	printf("Welcome to Magnetic Stripe Reader Application !\n");
	/* Initialize the magstripe reader and wait before enabling */
	mml_msr_init();
	/* Register the swipe complete callback */
	mml_msr_set_complete_callback(MsrCallback);
	printf("Waiting for swipe ...\n");
	PrepareForSwipe();
	/* Wait for swipes */
	while (1) {
		if (SwipeComplete) {
			ProcessSwipe();
		}
	}
	return result;
}

void PrepareForSwipe(void) {
	mml_msr_restart();
	SwipeComplete = 0;
	mml_msr_enable();
}

void MsrCallback(void) {
	SwipeComplete = 1;
}

void ProcessSwipe(void) {
	unsigned int dirsum, speedsum, speed;
	unsigned int tidx;

	/* Decode each track */
	for (tidx = 0; tidx < MSR_NUM_TRACKS; tidx++) {
		// Try to decode the track
		mml_msr_track_decode(tidx + 1, &decoded_track[tidx]);
		/* Capture Statistics */
		Processed++;
		if ((decoded_track[tidx].len == 0) || (decoded_track[tidx].lrc != 0))
			Discarded++;
	}
	/* Determine swipe speed and direction */
	dirsum = decoded_track[0].direction + decoded_track[1].direction
			+ decoded_track[2].direction;
	speedsum = decoded_track[0].speed + decoded_track[1].speed
			+ decoded_track[2].speed;
	if (dirsum < 2)
		printf("\nForward, ");
	else
		printf("\nReverse, ");
	if (speedsum > 0) {
		speed = speedsum / 3;
		printf("Rate %u.%u in/sec\n", speed / 10, speed % 10);
	} else
		printf("Rate invalid\n");
	/* Display the decoding results for each track */
	for (tidx = 0; tidx < MSR_NUM_TRACKS; tidx++) {
		printf("\n--=  Track %u  =--\n", tidx + 1);
		if (decoded_track[tidx].error_code != MML_MSR_ERRORS_MSR_ERR_OK) {
			printf("Error: 0x%02x", decoded_track[tidx].error_code);
			if (decoded_track[tidx].error_code
					& MML_MSR_ERRORS_MSR_ERR_BAD_LEN)
				printf(" BAD_LEN");
			if (decoded_track[tidx].error_code
					& MML_MSR_ERRORS_MSR_ERR_START_SEN)
				printf(" START_SEN");
			if (decoded_track[tidx].error_code
					& MML_MSR_ERRORS_MSR_ERR_END_SEN)
				printf(" END_SEN");
			if (decoded_track[tidx].error_code
					& MML_MSR_ERRORS_MSR_ERR_OUTLIER)
				printf(" OUTLIER");
			if (decoded_track[tidx].error_code
					& MML_MSR_ERRORS_MSR_ERR_PARITY)
				printf(" PARITY");
			if (decoded_track[tidx].error_code & MML_MSR_ERRORS_MSR_ERR_LRC)
				printf(" LRC");
			printf("\n");
		}
		if (decoded_track[tidx].lrc == 0)
			printf("LRC check passed\n");
		else
			printf("LRC check failed\n");
		printf("Decoded %d chars ", decoded_track[tidx].len);
		if (decoded_track[tidx].direction == MSR_FORWARD)
			printf("Forward\n");
		else
			printf("Reverse\n");
		printf("%s\n", decoded_track[tidx].data);
	}
	printf("\nDecode Stats:\n");
	printf("  Tracks %d, Discarded %d\n", Processed, Discarded);
	printf("\nWaiting for swipe ...\n");
	/* Get ready for next swipe */
	PrepareForSwipe();
}

#ifdef __SCPA_FWK__
#error "This example does not handle SCPA Target"
#endif //__SCPA_FWK__

/* EOF */
