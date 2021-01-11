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
#define APP1_ADDRESS 0xA0000000
//#define APP2_ADDRESS 0xA0010000
//#define APP3_ADDRESS 0xA0020000
#define REG1_ADDRESS 0x00000000
#define REG2_ADDRESS 0x00000004
#define REG3_ADDRESS 0x00000008
#define REG4_ADDRESS 0x0000000C
#define REG5_ADDRESS 0x00000010
#define REG6_ADDRESS 0x00000014
#define REG7_ADDRESS 0x00000018
#define REG8_ADDRESS 0x0000001C
#define REG9_ADDRESS 0x00000020
#define REG10_ADDRESS 0x00000024
#define REG11_ADDRESS 0x00000028
#define REG12_ADDRESS 0x0000002C
#define REG13_ADDRESS 0x00000030
#define REG14_ADDRESS 0x00000034
#define REG15_ADDRESS 0x00000038

/*
 * The following constant controls the length of the buffers to be sent
 * and received with the device. This constant must be 32 bytes or less so the
 * entire buffer will fit into the TX and RX FIFOs of the device.
 */
#define TEST_BUFFER_SIZE    16
#define RECEIVE_BUFFER_SIZE    32
#define TRANSMIT_BUFFER_SIZE    32

/************************** Function Prototypes ****************************/

int TwoUartsPsEcho(u32 UartBaseAddress_0, u32 UartBaseAddress_1);
void TransferAccessToICAP();
void SetOutputPins();
void InitSystem();
void ReleaseReset();
void ReleaseLoadSeed();
void DisableLfsr();
void EnableLfsr();
void ReadRegisters();
void PrintRegisters();
void SendTxtMessage(char *msg, u32 UartBaseAddress, u32 CntrlRegister);
void CompareRegisters (char *msg);
char *CompareThreeRegisters (u32 r1, u32 r2, u32 r3);
void checkErrorRegisters (char *msg);
void resetRegisters ();
void CompareRegisters (char *msg);
void DisableAppClock();
void EnableAppClock();
/************************** Variable Definitions ***************************/

/*
 * The following buffers are used in this example to send and receive data
 * with the UART.
 */
char ReceiveBuffer0[RECEIVE_BUFFER_SIZE],  ReceiveBuffer1[RECEIVE_BUFFER_SIZE];

u32 error_cpy1=0, error_cpy2=0, error_cpy3=0, error_lfsr1=0, error_lfsr2=0, error_lfsr3=0, error_all_cpy=0, error_all_lfsr=0, cycle_count=0;
u32 out_cpy1=0, out_cpy2=0, out_cpy3=0, out_lfsr1=0, out_lfsr2=0, out_lfsr3=0;
/***************************************************************************/
/**
* Main function
****************************************************************************/
int main(void) {
    int Status;

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

    // Runs main loop forever, receiving and answering commands frrom user or forwarding them to SEM
    Status = TwoUartsPsEcho(UART_BASEADDR_0, UART_BASEADDR_1);
    if (Status != XST_SUCCESS) {
        xil_printf("Application Failed\r\n");
        return XST_FAILURE;
    }

    xil_printf("Successfully ran Application\r\n");
    return XST_SUCCESS;

}

void SetOutputPins() {
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A02C4); // read DIRM3
    register_value = register_value | 0x0000001F; // set DIRM3, bit 4,3,2,1,0 [OUTPUT]
    Xil_Out32(0xFF0A02C4, register_value); // write DIRM3

    register_value = Xil_In32(0xFF0A02C8); // read OEN_3
    register_value = register_value | 0x0000001F; // set OEN_3, bit 4,3,2,1,0 [ENABLE]
    Xil_Out32(0xFF0A02C8, register_value); // write OEN_3
}

void InitSystem(){
    /////////////////////////////////////////////////////
    //////
    /////  BIT 0 -> enable clock to the SEM
    /////  BIT 1 -> Reset (active high) the application and LFSR
    /////  BIT 2 -> Load seed (active high) to the LFSR
    /////  BIT 3 -> Enable (active high) LFSR
    /////  BIT 4 -> App Clk Enable (active high)
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C); // read DATA_3
    register_value = register_value | 0x0000001F; // set DATA_3, bit 4,3,2,1,0 [HIGH]
    Xil_Out32(0xFF0A004C, register_value); // write DATA_3
}

void ResetAplication(){
    /////////////////////////////////////////////////////
    //////
    /////  BIT 0 -> enable clock to the SEM
    /////  BIT 1 -> Reset (active high) the application and LFSR
    /////  BIT 2 -> Load seed (active high) to the LFSR
    /////  BIT 3 -> Enable (active high) LFSR
	/////  BIT 4 -> App Clk (active high) [Not enabled here]
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C); // read DATA_3
    register_value = register_value | 0x0000000F; // set DATA_3, bit 3,2,1,0 [HIGH]
    Xil_Out32(0xFF0A004C, register_value); // write DATA_3
}

void ReleaseReset(){
    /////////////////////////////////////////////////////
    ////// Release Reset
    /////  BIT 1 -> Reset (active high) the application and LFSR
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C);
    register_value = register_value & ~(0x00000002);
    Xil_Out32(0xFF0A004C, register_value);
}

void ReleaseLoadSeed() {
    /////////////////////////////////////////////////////
    ////// Stop "loading seed"
    /////  BIT 2 -> Load seed (active high) to the LFSR
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C);
    register_value = register_value & ~(0x00000004);
    Xil_Out32(0xFF0A004C, register_value);
}

void DisableLfsr() {
    /////////////////////////////////////////////////////
    ////// Stop LSFR Generation
    /////  BIT 3 -> Enable (active high) LFSR
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C);
    register_value = register_value & ~(0x00000008);
    Xil_Out32(0xFF0A004C, register_value);
}

void ReadRegisters() {
    /////////////////////////////////////////////////////
    ////// Read Registers
    // copy1
	xil_printf("Starting Reading Register1!\n\r");
    error_cpy1 = Xil_In32(APP1_ADDRESS+REG1_ADDRESS);
	xil_printf("Starting Reading Register2!\n\r");
	error_cpy2 = Xil_In32(APP1_ADDRESS+REG2_ADDRESS);
	xil_printf("Starting Reading Register3!\n\r");
    error_cpy3 = Xil_In32(APP1_ADDRESS+REG3_ADDRESS);
	xil_printf("Starting Reading Register4!\n\r");
    error_lfsr1 = Xil_In32(APP1_ADDRESS+REG4_ADDRESS);
	xil_printf("Starting Reading Register5!\n\r");
    error_lfsr2 = Xil_In32(APP1_ADDRESS+REG5_ADDRESS);
	xil_printf("Starting Reading Register6!\n\r");
    error_lfsr3 = Xil_In32(APP1_ADDRESS+REG6_ADDRESS);
	xil_printf("Starting Reading Register7!\n\r");
    error_all_cpy = Xil_In32(APP1_ADDRESS+REG7_ADDRESS);
	xil_printf("Starting Reading Register8!\n\r");
    error_all_lfsr = Xil_In32(APP1_ADDRESS+REG8_ADDRESS);
	xil_printf("Starting Reading Register9!\n\r");
    cycle_count = Xil_In32(APP1_ADDRESS+REG9_ADDRESS);
	xil_printf("Starting Reading Register10!\n\r");
	out_cpy1 = Xil_In32(APP1_ADDRESS+REG10_ADDRESS);
	xil_printf("Starting Reading Register11!\n\r");
	out_cpy2 = Xil_In32(APP1_ADDRESS+REG11_ADDRESS);
	xil_printf("Starting Reading Register12!\n\r");
	out_cpy3 = Xil_In32(APP1_ADDRESS+REG12_ADDRESS);
	xil_printf("Starting Reading Register13!\n\r");
	out_lfsr1 = Xil_In32(APP1_ADDRESS+REG13_ADDRESS);
	xil_printf("Starting Reading Register14!\n\r");
	out_lfsr2 = Xil_In32(APP1_ADDRESS+REG14_ADDRESS);
	xil_printf("Starting Reading Register15!\n\r");
	out_lfsr3 = Xil_In32(APP1_ADDRESS+REG15_ADDRESS);
}

void PrintRegisters() {
    xil_printf("error_cpy1 = 0x%02X\n\r", error_cpy1);
    xil_printf("error_cpy2 = 0x%02X\n\r", error_cpy2);
    xil_printf("error_cpy3 = 0x%02X\n\r", error_cpy3);
    xil_printf("error_lfsr1 = 0x%02X\n\r", error_lfsr1);
    xil_printf("error_lfsr2 = 0x%02X\n\r", error_lfsr2);
    xil_printf("error_lfsr3 = 0x%02X\n\r", error_lfsr3);
    xil_printf("error_all_cpy = 0x%02X\n\r", error_all_cpy);
    xil_printf("error_all_lfsr = 0x%02X\n\r", error_all_lfsr);
    xil_printf("cycle_count = 0x%02X\n\r", cycle_count);
    xil_printf("out_cpy1 = 0x%02X\n\r", out_cpy1);
    xil_printf("out_cpy2 = 0x%02X\n\r", out_cpy2);
    xil_printf("out_cpy3 = 0x%02X\n\r", out_cpy3);
    xil_printf("out_lfsr1 = 0x%02X\n\r", out_lfsr1);
    xil_printf("out_lfsr2 = 0x%02X\n\r", out_lfsr2);
    xil_printf("out_lfsr3 = 0x%02X\n\r", out_lfsr3);
}

void EnableLfsr(){
    /////////////////////////////////////////////////////
    ////// Set LSFR Generation
    /////  BIT 3 -> Enable (active high) LFSR
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C);
    register_value = register_value | 0x00000008;
    Xil_Out32(0xFF0A004C, register_value);
}

void TransferAccessToICAP(){
	u32 register_value =0;
	register_value = Xil_In32(0xFFCA3008); // read PCAP_CTRL
	register_value = register_value & 0xFFFFFFFE; // clear PCAP_CTRL, bit 0 [ICAP]
	Xil_Out32(0xFFCA3008, register_value); // write PCAP_CTRL
}

int TwoUartsPsEcho(u32 UartBaseAddress_0, u32 UartBaseAddress_1) {
	char my_msg[64]="";
    char SendBuffer[]="INIT_DONE\n\r";

    u32 Running;
    u8 RecvChar0, RecvChar1;
    u32 CntrlRegister_0, CntrlRegister_1;

    CntrlRegister_0 = XUartPs_ReadReg(UartBaseAddress_0, XUARTPS_CR_OFFSET);
    CntrlRegister_1 = XUartPs_ReadReg(UartBaseAddress_1, XUARTPS_CR_OFFSET);

    /* Enable TX and RX for the XUartPs_0 */
    XUartPs_WriteReg(UartBaseAddress_0, XUARTPS_CR_OFFSET,
              ((CntrlRegister_0 & ~XUARTPS_CR_EN_DIS_MASK) |
               XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
    /* Enable TX and RX for the XUartPs_1 */
    XUartPs_WriteReg(UartBaseAddress_1, XUARTPS_CR_OFFSET,
              ((CntrlRegister_1 & ~XUARTPS_CR_EN_DIS_MASK) |
               XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));

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
				ResetAplication();
				usleep(1); // wait for 1us to stabilize registers (just in case)
				memset(my_msg, 0, 64);
				ReleaseReset(); // release reset
				ReleaseLoadSeed(); // Stop "loading seed"
				EnableAppClock();
				usleep(1000); // run app for 2us
				DisableAppClock();
				DisableLfsr(); // Stop LSFR Generation
				usleep(1); // wait for 1us to stabilize registers (just in case)
				checkErrorRegisters(my_msg);
				PrintRegisters();
				xil_printf("My Msg: %s %d\n\r",my_msg,strlen(my_msg));
				SendTxtMessage(my_msg, UartBaseAddress_0, CntrlRegister_0);
				usleep(1); // wait for 1us to stabilize registers (just in case)

//				ReadRegisters();
            }
        }
    }
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

char *CompareThreeRegisters (u32 r1, u32 r2, u32 r3) {
	if (r1 != r2){
		if (r2 == r3){
			return("REG1_DIFF ");
		} else {
			if (r1 == r3){
				return("REG2_DIFF ");
			} else {
				return("ALL_REGS_DIFF "); // ALL 3 ARE DIFFERENT
			}
		}
	} else { // mul1 == mul2
		if (r1 != r3){
			return("REG3_DIFF ");
		} else { // mul1 = mul2 = mul3
			return("REGS_EQUAL ");
		}
	}
}

void resetRegisters () {
	error_cpy1=0;
	error_cpy2=0;
	error_cpy3=0;
	error_lfsr1=0;
	error_lfsr2=0;
	error_lfsr3=0;
	error_all_cpy=0;
	error_all_lfsr=0;
	cycle_count = 0;
	out_cpy1 = 0;
	out_cpy2 = 0;
	out_cpy3 = 0;
	out_lfsr1 = 0;
	out_lfsr2 = 0;
	out_lfsr3 = 0;
}

void checkErrorRegisters (char *msg) {
	resetRegisters();
	ReadRegisters();
	char msg_tmp[64]="";
	if(error_cpy1>0) {
		printf("ERROR_CPY1, %d, cycle, %d",error_cpy1, cycle_count);
		sprintf(msg_tmp, "ERROR_CPY1, %d, cycle, %d, END",error_cpy1, cycle_count);
		strcat(msg,msg_tmp);
	}
	if(error_cpy2>0) {
		printf("ERROR_CPY2, %d, cycle, %d",error_cpy2, cycle_count);
		sprintf(msg_tmp, "ERROR_CPY2, %d, cycle, %d, END",error_cpy2, cycle_count);
		strcat(msg,msg_tmp);
	}
	if(error_cpy3>0) {
		printf("ERROR_CPY3, %d, cycle, %d",error_cpy3, cycle_count);
		sprintf(msg_tmp, "ERROR_CPY3, %d, cycle, %d, END",error_cpy3, cycle_count);
		strcat(msg,msg_tmp);
	}
	if(error_lfsr1>0) {
		printf("ERROR_LFSR1, %d, cycle, %d",error_lfsr1, cycle_count);
		sprintf(msg_tmp, "ERROR_LFSR1, %d, cycle, %d, END",error_lfsr1, cycle_count);
		strcat(msg,msg_tmp);
	}
	if(error_lfsr2>0) {
		printf("ERROR_LFSR2, %d, cycle, %d",error_lfsr2, cycle_count);
		sprintf(msg_tmp, "ERROR_LFSR2, %d, cycle, %d, END",error_lfsr2, cycle_count);
		strcat(msg,msg_tmp);
	}
	if(error_lfsr3>0) {
		printf("ERROR_LFSR3, %d, cycle, %d",error_lfsr3, cycle_count);
		sprintf(msg_tmp, "ERROR_LFSR3, %d, cycle, %d, END",error_lfsr3, cycle_count);
		strcat(msg,msg_tmp);
	}
	if(error_all_cpy>0) {
		printf("ERROR_ALL_CPY, %d, cycle, %d",error_all_cpy, cycle_count);
		sprintf(msg_tmp, "ERROR_ALL_CPY, %d, cycle, %d, END",error_all_cpy, cycle_count);
		strcat(msg,msg_tmp);
	}
	if(error_all_lfsr>0) {
		printf("ERROR_ALL_LSFR, %d, cycle, %d",error_all_lfsr, cycle_count);
		sprintf(msg_tmp, "ERROR_ALL_LSFR, %d, cycle, %d, END",error_all_lfsr, cycle_count);
		strcat(msg,msg_tmp);
	}
	if(cycle_count>0) {
		printf("Finished with cycle_count, %d", cycle_count);
		sprintf(msg_tmp, "FINISHED! CYCLE COUNT, %d, END", cycle_count);
		strcat(msg,msg_tmp);
	}

	if (error_cpy1 <= 0 &&  error_cpy2 <= 0 &&  error_cpy3 <= 0 &&
			error_lfsr1 <= 0 && error_lfsr2 <= 0 && error_lfsr3 <= 0 &&
			error_all_cpy <= 0 && error_all_lfsr <= 0 && cycle_count <= 0) {

			sprintf(msg_tmp, "Unknown condition!, cycle, %d, END", cycle_count);
			strcat(msg,msg_tmp);
	}
}

void DisableAppClock(){
    /////////////////////////////////////////////////////
    ////// Release Reset
    /////  BIT 4 -> App Clock (active high)
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C);
    register_value = register_value & ~(0x00000010); // reset DATA_3, bit 4 [LOW]
    Xil_Out32(0xFF0A004C, register_value);
}

void EnableAppClock(){
    /////////////////////////////////////////////////////
    ////// Release Reset
    /////  BIT 4 -> App Clock (active high)
	u32 register_value =0;
    register_value = Xil_In32(0xFF0A004C);
    register_value = register_value | 0x00000010; // set DATA_3, bit 4 [HIGH]
    Xil_Out32(0xFF0A004C, register_value);
}
