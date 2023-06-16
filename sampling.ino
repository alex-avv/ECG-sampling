// ------ Global Variables ------ //
int ADCValue;       /*ADC value will be assigned to this varibale*/
int serialContinue; /*Flag indicating that a fresh value of the ADC is available for transmission to the serial port*/


// -------- Functions --------- //
static inline void initTimer(void)
{
  // Code to initialize Timer 1
  TCCR1B = 0b00000100; /*Prescaler: 256*/
  /* Timing in ATMega328 is at 16MHz, by using the prescaler it reduces the timing to 62500Hz*/
  /* For the ADC, we will use the appropiate sampling rate for Heart Rate Variability analysis, which is 250 Hz(1). If we need a higher sampling rate to give more precise results we can always change it later.
   * - A sampling frequency of 250Hz means the triggering should happen every 1/250=4ms
   * - 62500Hz*0.004s=250, so 250 timer steps are needed to make 4ms
   * - Timer 1 has 16-bits: 65535 steps
   * - Our clock should start counting from 65535-250+1=65286 to overflow (the +1 is to include the count from overflow)*/
  TCNT1 = 65286;
  TIMSK1 |= (1 << TOIE1); /*Allowing Timer 1 Overflow Interrupts*/
}

static inline void initADC(void)
{
  // Code to initialize the ADC
  ADMUX |= (1 << REFS0);                 /*Selecting the Reference Voltage (5V)*/
  ADCSRA |= (1 << ADPS1) | (1 << ADPS0); /*Setting the ADC clock prescaler to 8*/
  ADMUX &= ~(1 << ADLAR);                /*The result of the ADC Data Register is right adjusted*/
  ADCSRB |= (1 << ADTS2) | (1 << ADTS1); /*Setting Timer1 as the Auto-Trigger Source*/
  ADCSRA |= (1 << ADATE);                /*ADC Auto-Trigger Enable*/
  ADCSRA |= (1 << ADIE);                 /*ADC Interrumpt Enable*/
  ADCSRA |= (1 << ADEN);                 /*Analog to Digital Conversion Enabled*/
}


// ------ Interrupt Service Routine ------ //
ISR(TIMER1_OVF_vect)
{
  // Interrupt service routine for Timer 1 Overflow
  TCNT1 = 65286; /*Set T-Count 1 register back to its initial value*/
}

ISR(ADC_vect)
{
  // Interrupt service routine for the ADC; executed when the ADC completes a conversion
  ADCValue = ADC;     /*Storing the ADC value in the variable ADCValue*/
  serialContinue = 1; /*Flag is set to 1 to show a fresh ADC value is available for transmission*/
}


// ------ Main Code ------ //
int main(void)
{
  serialContinue = 0; /*Clearing the flag*/

  Serial.begin(250000); /*Initializing the serial port to transmit at 250000 bps*/
  noInterrupts();       /*Disabling global interrupts*/
  initTimer();          /*Initializing Timer 1 and Timer 0*/
  initADC();            /*Initializing ADC*/
  interrupts();         /*Enabling global interrupts*/

  while (1)
  {
    // Code to send the ADC value through the serial port
    Serial.flush();
    if (serialContinue == 1)
    {
      Serial.println(ADCValue); /*If flag indicates a fresh ADC value is available, ADCValue is transmitted to the serial port*/
      serialContinue = 0;       /*Clearing the flag*/
    }
  }
  return 0; /* This line is never reached */
}


/* References:
 * 1. Kwon O, Jeong J, Kim H bin, Kwon IH, Park SY, Kim JE, et al. Electrocardiogram sampling frequency range acceptable for heart rate variability analysis. Healthcare Informatics Research [Internet]. 2018 Jul 1 [cited 2020 Nov 9];24(3):198–206. Available from: /pmc/articles/PMC6085204/?report=abstract
 */
