/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include "bsp.h"

#if !(defined SILABS_PLATFORM_WMB912)
#define ERROR_HOOK while (TRUE) \
    { \
  for (rollingCounter = 0u; \
  rollingCounter < 0xFFFF; \
  rollingCounter++)  ; \
}
#else
#define ERROR_HOOK while (TRUE) \
    { \
  for (rollingCounter = 0u; \
  rollingCounter < 0xFFFF; \
  rollingCounter++)  ; \
}
#endif
/*****************************************************************************
 *  Local Macros & Definitions
 *****************************************************************************/
SEGMENT_VARIABLE(bPositionInPayload, U8, SEG_XDATA) = 0u;

SEGMENT_VARIABLE(bNumOfRestBytes, U8, SEG_XDATA) = 0u;

const  U8* pPositionInPayload_t;

SEGMENT_VARIABLE(rollingCounter, U16, SEG_XDATA);

SEGMENT_VARIABLE(bNumOfFreeBytes, U16, SEG_XDATA) = RADIO_MAX_LONG_PACKET_LENGTH;

U8* pPositionInPayload_r =  &fixRadioPacket[0u];

/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
const SEGMENT_VARIABLE(Radio_Configuration_Data_Array[], U8, SEG_CODE) = \
              RADIO_CONFIGURATION_DATA_ARRAY;

const SEGMENT_VARIABLE(Radio_Configuration_Data_Custom_Long_Payload_Array[], U8, SEG_CODE) = \
              RADIO_CONFIGURATION_DATA_CUSTOM_LONG_PAYLOAD;

const SEGMENT_VARIABLE(RadioConfiguration, tRadioConfiguration, SEG_CODE) = \
                        RADIO_CONFIGURATION_DATA;

const SEGMENT_VARIABLE_SEGMENT_POINTER(pRadioConfiguration, tRadioConfiguration, SEG_CODE, SEG_CODE) = \
                        &RadioConfiguration;

SEGMENT_VARIABLE(fixRadioPacket[RADIO_MAX_LONG_PACKET_LENGTH], U8, SEG_XDATA);

/*****************************************************************************
 *  Local Function Declarations
 *****************************************************************************/
void vRadio_PowerUp(void);

/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{
  SEGMENT_VARIABLE(wDelay,  U16, SEG_XDATA) = 0u;

  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
  U16 wDelay;

  /* Power Up the radio chip */
  vRadio_PowerUp();

  /* Load radio configuration */
  while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
  {

    for (wDelay = 0x7FFF; wDelay--; ) ;

    /* Power Up the radio chip */
    vRadio_PowerUp();
  }

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);
}

/*!
 *  Check if Packet sent IT flag is pending.
 *
 *  @return   TRUE / FALSE
 *
 *  @note
 *
 */
BIT gRadio_CheckTransmitted(void)
{
  if (RF_NIRQ == FALSE)
  {
    /* Read ITs, clear pending ones */
    si446x_get_int_status(0u, 0u, 0u);

    /* check the reason for the IT */
    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
    {
      /* Nothing is sent to TX FIFO */
      bPositionInPayload = 0u;

      /* Position to the very beginning of the custom long payload */
     // pPositionInPayload_t = (U8*) &pRadioConfiguration->Radio_Custom_Long_Payload;

      return TRUE;
    }

    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_TX_FIFO_ALMOST_EMPTY_PEND_BIT)
    {
        /* Calculate the number of remaining bytes has to be sent to TX FIFO */
        bNumOfRestBytes = RadioConfiguration.Radio_PacketLength - bPositionInPayload;

        if(bNumOfRestBytes > RADIO_TX_ALMOST_EMPTY_THRESHOLD)
        { // remaining byte more than threshold

          /* Fill TX FIFO with the number of THRESHOLD bytes */
          si446x_write_tx_fifo(RADIO_TX_ALMOST_EMPTY_THRESHOLD, pPositionInPayload_t);

          /* Calculate how many bytes are sent to TX FIFO */
          bPositionInPayload += RADIO_TX_ALMOST_EMPTY_THRESHOLD;

          /* Position to the next first byte that can be sent to TX FIFO in next round */
          pPositionInPayload_t += RADIO_TX_ALMOST_EMPTY_THRESHOLD;

        }
        else if(bNumOfRestBytes != 0)
        { // remaining byte less or equal than threshold

         /* Fill TX FIFO with the number of rest bytes */
          si446x_write_tx_fifo(bNumOfRestBytes, pPositionInPayload_t);

          /* Calculate how many bytes are sent to TX FIFO */
          bPositionInPayload += bNumOfRestBytes;

          /* Position to the next first byte that can be sent to TX FIFO in next round */
          pPositionInPayload_t += bNumOfRestBytes;
        }
    }
  }

  return FALSE;
}

/*BIT handle_CheckReceive()
{
	switch(gRadio_CheckReceived())
	{
		case SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT:
			return TRUE;

		case SI446X_CMD_GET_INT_STATUS_REP_RX_FIFO_ALMOST_FULL_BIT:

		break;
	}
}*/
/*!
 *  Check if Packet received IT flag is pending.
 *
 *  @return   TRUE - Packet successfully received / FALSE - No packet pending.
 *
 *  @note
 *
 */
BIT gRadio_CheckReceived(void)
{
  if (RF_NIRQ == FALSE)
  {
    /* Read ITs, clear pending ones */
    si446x_get_int_status(0u, 0u, 0u);

    /* check the reason for the IT */
    // if (Si446xCmd.GET_INT_STATUS.MODEM_PEND & SI446X_CMD_GET_INT_STATUS_REP_SYNC_DETECT_BIT)
     //{
       /* Blink once LED2 to show Sync Word detected */
//       vHmi_ChangeLedState(eHmi_Led2_c, eHmi_LedBlinkOnce_c);
     //}

    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
    {
      /* Blink once LED3 as CRC OK or not enabled */
//      vHmi_ChangeLedState(eHmi_Led3_c, eHmi_LedBlinkOnce_c);

      /* Calculate the number of free bytes in the array */
      bNumOfFreeBytes = RADIO_MAX_LONG_PACKET_LENGTH - bPositionInPayload;

      if (bNumOfFreeBytes >= RADIO_MAX_PACKET_LENGTH)
      {// free space in buffer more than RX FIFO size

        /* Read the RX FIFO with the number of RX FIFO size */
        si446x_read_rx_fifo(RADIO_MAX_PACKET_LENGTH, pPositionInPayload_r);

        /* Calculate how many bytes are already stored in the array */
        bPositionInPayload += RADIO_MAX_PACKET_LENGTH;

        /* Position to the next free byte that can be written in the next RX FIFO reading */
        pPositionInPayload_r += RADIO_MAX_PACKET_LENGTH;
      }
      else
      {
        /* Read the RX FIFO with the number of free bytes */
        si446x_read_rx_fifo(bNumOfFreeBytes, pPositionInPayload_r);

        /* Calculate how many bytes are already stored in the array */
        bPositionInPayload += bNumOfFreeBytes;

        /* Position to the next free byte that can be written in the next RX FIFO reading */
        pPositionInPayload_r += bNumOfFreeBytes;

      }

      /* Calculate how many bytes are already stored in the array */
       bPositionInPayload = 0u;

       /* Set writing pointer to the beginning of the array */
       pPositionInPayload_r = &fixRadioPacket[0u];

       /* free space */
       bNumOfFreeBytes = RADIO_MAX_LONG_PACKET_LENGTH;

       /* Start the radio */
       vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber);

#ifdef UART_LOGGING_SUPPORT
      {
          U8 lCnt;
          
          /* Send it to UART */
          for (lCnt = 0u; lCnt < RadioConfiguration.Radio_PacketLength; lCnt++)
          {
            Comm_IF_SendUART(*((U8 *) &fixRadioPacket[0u] + lCnt));
          }
          Comm_IF_SendUART('\n');
      }
#endif

      return TRUE;
    }

    if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_RX_FIFO_ALMOST_FULL_BIT)
    {
      /* Calculate the number of free bytes in the array */
      bNumOfFreeBytes = RADIO_MAX_LONG_PACKET_LENGTH - bPositionInPayload;

      if (bNumOfFreeBytes >= RADIO_RX_ALMOST_FULL_THRESHOLD)
      { // free space in the array is more than the threshold

        /* Read the RX FIFO with the number of THRESHOLD bytes */
        si446x_read_rx_fifo(RADIO_RX_ALMOST_FULL_THRESHOLD, pPositionInPayload_r);

        /* Calculate how many bytes are already stored in the array */
        bPositionInPayload += RADIO_RX_ALMOST_FULL_THRESHOLD;

        /* Position to the next free byte that can be written in the next RX FIFO reading */
        pPositionInPayload_r += RADIO_RX_ALMOST_FULL_THRESHOLD;

		//return SI446X_CMD_GET_INT_STATUS_REP_RX_FIFO_ALMOST_FULL_BIT;
      }
      //else
      //{
        /* Not enough free space reserved in the program */
        //ERROR_HOOK;
		//return FALSE;
//      }
    }


    if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
    {
      /* Reset FIFO */
      si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
	  return FALSE;
    }
  }

  return FALSE;
}

/*!
 *  Set Radio to RX mode, fixed packet length.
 *
 *  @param channel Freq. Channel
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 channel)
{
  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  /* Reset FIFO */
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

  /* Start Receiving packet, channel 0, START immediately, Packet length according to PH */
  si446x_start_rx(channel, 0u, RadioConfiguration.Radio_PacketLength,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_READY,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX );

  /* Switch on LED1 to show RX state */
//  vHmi_ChangeLedState(eHmi_Led1_c, eHmi_LedOn_c);
}


/*!
 *  Set Radio to TX mode, fixed packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent
 *
 *  @note
 *
 */
void  vRadio_StartTx(U8 channel, U8 *pioFixRadioPacket)
{
  /* Leave RX state */
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

  /* Reset TX FIFO */
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT);

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  /* Position to the very beginning of the custom long payload */
  pPositionInPayload_t = pioFixRadioPacket;

  /* Fill the TX fifo with datas */
  if( RADIO_MAX_PACKET_LENGTH < RadioConfiguration.Radio_PacketLength)
  {
    /* Data to be sent is more than the size of TX FIFO */
    si446x_write_tx_fifo(RADIO_MAX_PACKET_LENGTH, pPositionInPayload_t);

    /* Calculate how many bytes are sent to TX FIFO */
    bPositionInPayload += RADIO_MAX_PACKET_LENGTH;

    /* Position to the next first byte that can be sent to TX FIFO in next round */
    pPositionInPayload_t += RADIO_MAX_PACKET_LENGTH;
  }
  else
  {
    /* Calculate how many bytes are sent to TX FIFO */
    bPositionInPayload += RadioConfiguration.Radio_PacketLength;

    // Data to be sent is less or equal than the size of TX FIFO
    si446x_write_tx_fifo(RadioConfiguration.Radio_PacketLength, pioFixRadioPacket);
  }


  /* Start sending packet, channel 0, START immediately, Packet length according to PH, go READY when done */
  si446x_start_tx(channel, 0x80,  0);
}
