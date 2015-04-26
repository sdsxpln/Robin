/*! @file radio.h
 * @brief This file is contains the public radio interface functions.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_H_
#define RADIO_H_

/*****************************************************************************
 *  Global Macros & Definitions
 *****************************************************************************/
/*! Maximal packet length definition (FIFO size) */
#define RADIO_MAX_PACKET_LENGTH     64u
/*! Maximal long packet length */
#define RADIO_MAX_LONG_PACKET_LENGTH (3u * RADIO_MAX_PACKET_LENGTH)

/*! Threshold for TX FIFO */
#define RADIO_RX_ALMOST_FULL_THRESHOLD 30u
        
/*! Threshold for TX FIFO */
#define RADIO_TX_ALMOST_EMPTY_THRESHOLD 30u

/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
typedef struct
{
    U8   *Radio_ConfigurationArray;

    U8   Radio_ChannelNumber;
    U8   Radio_PacketLength;
    U8   Radio_State_After_Power_Up;

    U16  Radio_Delay_Cnt_After_Reset;

    U8   *Radio_Custom_Long_Payload;
} tRadioConfiguration;


/*****************************************************************************
 *  Global Variable Declarations
 *****************************************************************************/
extern const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE);
extern SEGMENT_VARIABLE(fixRadioPacket[RADIO_MAX_LONG_PACKET_LENGTH], U8, SEG_XDATA);

/*! Si446x configuration array */
extern const SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE);

/*****************************************************************************
 *  Global Function Declarations
 *****************************************************************************/
void  vRadio_Init(void);
BIT gRadio_GetBuffer(U8 * byte);
BIT   gRadio_CheckReceived(void);
BIT   gRadio_CheckTransmitted(void);
void  vRadio_StartRX(U8);
void  vRadio_StartTx(U8, U8 *);
U8    bRadio_Check_Ezconfig(U16);
BIT gRadio_CheckTransmitted_t(void);

#endif /* RADIO_H_ */
