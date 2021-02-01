/***************************** Include Files *******************************/
#include "xparameters.h"
#include "xstatus.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xuartps_hw.h"
#include "xil_printf.h"
#include <sleep.h>
#include <string.h>
#include <stdio.h>
/************************** Constant Definitions ***************************/
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define UART_BASEADDR_0     XPAR_XUARTPS_0_BASEADDR
#define UART_CLOCK_HZ_0     XPAR_XUARTPS_0_CLOCK_HZ
#define UART_BASEADDR_1     XPAR_XUARTPS_1_BASEADDR
#define UART_CLOCK_HZ_1     XPAR_XUARTPS_1_CLOCK_HZ
#define CLK_EN_MASK 0x00000001
#define WR_EN_MASK 0x00000002
#define RST_EN_MASK 0x00000004
#define RD_TRIG_MASK 0x00000008
#define GPIO_INPUT_MASK 0x0000FF00
#define DATA_3_RO 0xFF0A006C
#define DATA_3 0xFF0A004C

/*
 * The following constant controls the length of the buffers to be sent
 * and received with the device. This constant must be 32 bytes or less so the
 * entire buffer will fit into the TX and RX FIFOs of the device.
 */
#define TEST_BUFFER_SIZE    16
#define RECEIVE_BUFFER_SIZE    32
#define TRANSMIT_BUFFER_SIZE    32

/************************** Function Prototypes ****************************/

void TransferAccessToICAP();
void SetOutputPins();
void InitSystem();
void SendTxtMessage(char *msg, u32 UartBaseAddress, u32 CntrlRegister);
u32 readGPIO();
int initSerials(u32 BaseAddr_0, u32 BaseAddr_1, u32 *CtrlReg_0, u32 *CtrlReg_1);
void enableUART_TX_FIFO();
void disableUART_TX_FIFO();
void resetTX_FIFO();
void enClock();
void readFIFO();
void triggerFIFOReadEn();
int TwoUartsPsEcho(u32 UartBaseAddress_0, u32 UartBaseAddress_1, u32 CntrlRegister_0, u32 CntrlRegister_1);
/************************** Variable Definitions ***************************/

/*
 * The following buffers are used in this example to send and receive data
 * with the UART.
 */
char ReceiveBuffer0[RECEIVE_BUFFER_SIZE],  ReceiveBuffer1[RECEIVE_BUFFER_SIZE];

/***************************************************************************/
/**
* Main function
****************************************************************************/
int main(void) {
    int Status;
    u32 *CtrlReg_0=NULL, *CtrlReg_1=NULL;
    // First, transfer control of the PL configuratiom logic from PCAP to ICAP,
    // as ICAP is used by SEM IP.
    //  In order for the controller to function,
    //  configuration logic access must be transferred to the ICAP. This is accomplished by clearing
    //  PCAP_CTRL (address  0xFFCA3008) bit 0 - PCAP_PR.
    TransferAccessToICAP();

    // Second, enable the SEM IP clock in the PL. To do this, set pin zero of GPIO into the
    // PL, to logic one.

    SetOutputPins();
    InitSystem(); // enable clock to SEM, reset app, load LFSR Seed, enable LFSR

    usleep(1000000); // This delay assures time for the SEM to initialize.

    // Initiate serials
    Status = initSerials(UART_BASEADDR_0, UART_BASEADDR_1, CtrlReg_0, CtrlReg_1);
    if (Status != XST_SUCCESS) {
        xil_printf("Application Failed\r\n");
        return XST_FAILURE;
    }

    Status = TwoUartsPsEcho(UART_BASEADDR_0, UART_BASEADDR_1, *CtrlReg_0, *CtrlReg_1);

	if (Status != XST_SUCCESS) {
		xil_printf("Application Failed\r\n");
		return XST_FAILURE;
	}

//    for(long int k=0;k<1000;k++){
//        for(long int j=0;j<10000;j++){
//			while (XUartPs_IsTransmitFull(*CtrlReg_1));
//
//			enableUART_TX_FIFO(); // Enable UART_TX fifo
////			usleep(1);
//			/* Write the byte into the TX FIFO */
//			XUartPs_WriteReg(UART_BASEADDR_1, XUARTPS_FIFO_OFFSET,
//				  'Y');
//			disableUART_TX_FIFO(); // Disable UART_TX fifo
//
////			xil_printf("j=%d k=%d\r\n",j,k);
//
////			readFIFO();
//        }
//    }

    xil_printf("Successfully ran Application\r\n");
    return XST_SUCCESS;
}

void readFIFO(){
	u32 rd_data=99;
	xil_printf("Data read=\t");
	for (int i=0; i<512; i++){
		triggerFIFOReadEn();
		rd_data=readGPIO();
		xil_printf("%d",rd_data);
		if (i%64==63)
			xil_printf("\n\r\t\t");
	}
	xil_printf("\n\r");
}

void triggerFIFOReadEn(){
	u32 register_value =0;
	register_value = Xil_In32(DATA_3_RO); // read DATA_3_RO
	register_value = register_value | RD_TRIG_MASK; // Set Reset FIFO
	Xil_Out32(DATA_3, register_value); // write DATA_3
	register_value = register_value & ~RD_TRIG_MASK; // Reset Reset FIFO
	Xil_Out32(DATA_3, register_value); // write DATA_3
}

void SetOutputPins() {
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A02C4); // read DIRM3
    register_value = register_value | CLK_EN_MASK
    								| WR_EN_MASK
									| RST_EN_MASK
									| RD_TRIG_MASK; // set DIRM3, bit 3,2,1,0 [OUTPUT]
    Xil_Out32(0xFF0A02C4, register_value); // write DIRM3

    register_value = Xil_In32(0xFF0A02C8); // read OEN_3
    register_value = register_value | CLK_EN_MASK
									| WR_EN_MASK
									| RST_EN_MASK
									| RD_TRIG_MASK; // set OEN_3, bit 3,2,1,0 [ENABLE]
    Xil_Out32(0xFF0A02C8, register_value); // write OEN_3
}

void InitSystem(){
    /////////////////////////////////////////////////////
    //////
	resetTX_FIFO();
	enClock();
}

void enClock(){
    /////////////////////////////////////////////////////
    //////
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C); // read DATA_3
    register_value = register_value | CLK_EN_MASK; // set DATA_3, bit 0 [HIGH]
    Xil_Out32(0xFF0A004C, register_value); // write DATA_3
}

void TransferAccessToICAP(){
	u32 register_value =0;
	register_value = Xil_In32(0xFFCA3008); // read PCAP_CTRL
	register_value = register_value & 0xFFFFFFFE; // clear PCAP_CTRL, bit 0 [ICAP]
	Xil_Out32(0xFFCA3008, register_value); // write PCAP_CTRL
}

void enableUART_TX_FIFO(){
	u32 register_value =0;
	register_value = Xil_In32(DATA_3_RO); // read DATA_3_RO
	register_value = register_value | WR_EN_MASK; // enable WR en at the FIFO
	Xil_Out32(DATA_3, register_value); // write DATA_3_RO
//	xil_printf("enableUART_TX_FIFO = %d\n\r",register_value);
}

void disableUART_TX_FIFO(){
	u32 register_value =0;
	register_value = Xil_In32(DATA_3_RO); // read DATA_3_RO
	register_value = register_value & ~WR_EN_MASK; // Disable WR en at the FIFO
	Xil_Out32(DATA_3, register_value); // write DATA_3_RO
//	xil_printf("disableUART_TX_FIFO = %d\n\r",register_value);
}

void resetTX_FIFO() {
	u32 register_value =0;
	register_value = Xil_In32(DATA_3_RO); // read DATA_3_RO
	register_value = register_value | RST_EN_MASK; // Set Reset FIFO
	Xil_Out32(DATA_3, register_value); // write DATA_3_RO
	register_value = register_value & ~RST_EN_MASK; // Reset Reset FIFO
	Xil_Out32(DATA_3, register_value); // write DATA_3_RO
}

int initSerials(u32 BaseAddr_0, u32 BaseAddr_1, u32 *CtrlReg_0, u32 *CtrlReg_1){
    char SendBuffer[]="INIT_DONE\n\r";

    *CtrlReg_0 = XUartPs_ReadReg(BaseAddr_0, XUARTPS_CR_OFFSET);
    *CtrlReg_1 = XUartPs_ReadReg(BaseAddr_1, XUARTPS_CR_OFFSET);

    /* Enable TX and RX for the XUartPs_0 */
    XUartPs_WriteReg(BaseAddr_0, XUARTPS_CR_OFFSET,
              ((*CtrlReg_0 & ~XUARTPS_CR_EN_DIS_MASK) |
               XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
    xil_printf("Enable TX and RX for the XUartPs_0: %d\n\r",((*CtrlReg_0 & ~XUARTPS_CR_EN_DIS_MASK) |
                   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));

    /* Enable TX and RX for the XUartPs_1 */
    XUartPs_WriteReg(BaseAddr_1, XUARTPS_CR_OFFSET,
              ((*CtrlReg_1 & ~XUARTPS_CR_EN_DIS_MASK) |
               XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
    xil_printf("Enable TX and RX for the XUartPs_1: %d\n\r",((*CtrlReg_1 & ~XUARTPS_CR_EN_DIS_MASK) |
            XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));

    usleep(10);

    SendTxtMessage(SendBuffer, BaseAddr_0, *CtrlReg_0);

    return XST_SUCCESS;
}

void SendTxtMessage(char *msg, u32 UartBaseAddress, u32 CntrlRegister){
	xil_printf("Sending: %s %d\n\r",msg,strlen(msg));
    /* Send the entire transmit buffer. */
  for (int Index = 0; Index < strlen(msg); Index++) {
      /* Wait until there is space in TX FIFO */
       while (XUartPs_IsTransmitFull(CntrlRegister));

      /* Write the byte into the TX FIFO */
      XUartPs_WriteReg(UartBaseAddress, XUARTPS_FIFO_OFFSET,
    		  msg[Index]);
  }
}

u32 readGPIO(){
	u32 register_value =0;
	register_value = Xil_In32(0xFF0A006C); // read DATA_3_RO
	return (register_value >> 8) & 0xFF;  // Bits 15 downto 8 are input and 7 downto 0 are output
}

int TwoUartsPsEcho(u32 UartBaseAddress_0, u32 UartBaseAddress_1, u32 CntrlRegister_0, u32 CntrlRegister_1) {
	char my_msg[64]="";
    char SendBuffer[]="INIT_DONE\n\r";

    u32 Running;
    u8 RecvChar0, RecvChar1;

    usleep(10);

    xil_printf("Sending: %s %d\r\n", SendBuffer, sizeof(SendBuffer));

    usleep(1000);
    SendTxtMessage(SendBuffer, UartBaseAddress_0, CntrlRegister_0);
    Running = TRUE;
    while (Running) {

        /************************************************************
        * Forward character received in RX_UART1 (SEM)  to  (PC) TX_UART0 */
        if (XUartPs_IsReceiveData(UartBaseAddress_1)) {
            RecvChar1 = XUartPs_ReadReg(UartBaseAddress_1, XUARTPS_FIFO_OFFSET);
            XUartPs_WriteReg(UartBaseAddress_0,  XUARTPS_FIFO_OFFSET, RecvChar1);
        }

         /************************************************************
         * Forward character received in RX_UART0 (PC) to (SEM) TX_UART1 */
        if (XUartPs_IsReceiveData(UartBaseAddress_0)) {
            RecvChar0 = XUartPs_ReadReg(UartBaseAddress_0, XUARTPS_FIFO_OFFSET);
            XUartPs_WriteReg(UartBaseAddress_1, XUARTPS_FIFO_OFFSET, RecvChar0);
//            Check received register comparison command

            if (RecvChar0=='Z') {
            	memset(my_msg, 0, 64);
				usleep(1000); // run app for 2us
				usleep(1); // wait for 1us to stabilize registers (just in case)
				strcat(my_msg," END");
				xil_printf("My Msg: %s %d\n\r",my_msg,strlen(my_msg));
				SendTxtMessage(my_msg, UartBaseAddress_0, CntrlRegister_0);

            }
        }
    }
    return XST_SUCCESS;
}


