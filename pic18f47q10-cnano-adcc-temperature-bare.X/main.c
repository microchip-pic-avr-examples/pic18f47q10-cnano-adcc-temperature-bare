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

#pragma config WDTE = OFF     /*disable Watchdog*/
#pragma config LVP = ON  /* Low voltage programming enabled, RE3 pin is MCLR */

#include <xc.h>
#include <stdint.h>

/* channel that connects the ADCC to VSS */
#define DISCHARGE_SAMPLE_CAP                0x3C  
#define TEMPERATURE_CHANNEL                 0x3D
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
    OSCCON1 = _OSCCON1_NOSC1_MASK | _OSCCON1_NOSC2_MASK;
    /* set HFFRQ to 1 MHz */
    OSCFRQ = ~_OSCFREQ_HFFRQ_MASK;
}


static void FVR_init(void)
{
    FVRCON |= _FVRCON_TSEN_MASK   /*enable temperature sensor*/
            | _FVRCON_FVREN_MASK; /*enable FVR*/
}

static void ADCC_init(void)
{
    ADCON0 = _ADCON0_ADON_MASK     /*Enable ADCC module*/
           | _ADCON0_ADCS_MASK     /*Select FRC clock*/
           | _ADCON0_ADFM_MASK;    /*result right justified*/
}

static void ADCC_dischargeSampleCap(void)
{
    ADPCH = DISCHARGE_SAMPLE_CAP;
}

static uint16_t ADCC_readValue(uint8_t channel)
{    
    ADPCH = channel;
    
    ADCON0 |= _ADCON0_ADGO_MASK; /*start conversion*/

    while (ADCON0 & _ADCON0_ADGO_MASK)
    {
        ;
    }
    
    return ((uint16_t)((ADRESH << 8) + ADRESL));
}

void main(void) {
  
    CLK_init();
    
    FVR_init();
    
    ADCC_init();
    
    ADCC_dischargeSampleCap();
    
    adcVal = ADCC_readValue(TEMPERATURE_CHANNEL);
    
    celsiusValue = ADC_TO_CELSIUS(adcVal); 
    fahrenheitValue = ADC_TO_FAHRENHEIT(adcVal);
    
    while(1)
    {
          ;
    }
}
