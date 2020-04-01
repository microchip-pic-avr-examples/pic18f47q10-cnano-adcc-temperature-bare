/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/*disable Watchdog*/
#pragma config WDTE = OFF
/* Low voltage programming enabled, RE3 pin is MCLR */
#pragma config LVP = ON  

#include <xc.h>
#include <stdint.h>

#define VDD                                 3.3
#define ADC_TO_CELSIUS(adcVal)              (int16_t) ((1241.4967 - VDD * (1024 - (adcVal))) / 2.70336)
#define ADC_TO_FAHRENHEIT(adcVal)           (int16_t) ((((1241.4967 - VDD * (1024 - (adcVal))) / 2.70336) * 1.8) + 32)

static void CLK_init(void);
static void FVR_init(void);
static void ADCC_init(void);
static void ADCC_dischargeSampleCap(void);
static uint16_t ADCC_readValue(uint8_t channel);

uint16_t volatile adcVal;
int16_t volatile celsiusValue;
int16_t volatile fahrenheitValue; 

static void CLK_init(void)
{
    /* set HFINTOSC Oscillator */
    OSCCON1bits.NOSC = 6;
    /* set HFFRQ to 1 MHz */
    OSCFRQbits.HFFRQ = 0;
}

static void FVR_init(void)
{
    /*Enable temperature sensor*/
    FVRCONbits.TSEN = 1;
    /*Enable FVR*/
    FVRCONbits.FVREN = 1;
}

static void ADCC_init(void)
{
    /* Enable the ADCC module */
    ADCON0bits.ADON = 1; 
    /* Select FRC clock */
    ADCON0bits.ADCS = 1;
    /* result right justified */
    ADCON0bits.ADFM = 1;
}

static void ADCC_dischargeSampleCap(void)
{
    /*channel number that connects to VSS*/
    ADPCH = 0x3C;
}

static uint16_t ADCC_readValue(uint8_t channel)
{   
    ADPCH = channel;
    /*start conversion*/
    ADCON0bits.ADGO = 1;
    while (ADCON0bits.ADGO)
    {
        ;
    }
        
    return ((uint16_t)((ADRESH << 8) + ADRESL));
}

void main(void)
{
    CLK_init();   
    FVR_init();
    ADCC_init();
    ADCC_dischargeSampleCap();
    
    /*channel number that connects to the temperature sensor*/
    adcVal = ADCC_readValue(0x3D);
    celsiusValue = ADC_TO_CELSIUS(adcVal); 
    fahrenheitValue = ADC_TO_FAHRENHEIT(adcVal);    
    while(1)
    {
          ;
    }
}
