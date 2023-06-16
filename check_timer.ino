
int counter;              // Variable that will be incremented each time Timer0 overflows

void initialiseTimer0(void) {

  TCCR0A = 0;
  TCCR0B |= 0b00000101;    // Set the prescaler for a division by 1024

  TIMSK0 |= (1 << TOIE0);  // Enable Timer0 overflow interrupts

}


ISR(TIMER0_OVF_vect) {    // This code is executed each time Timer 0 overflows.

  counter++;              // Variable counter is increased by one.
  if (counter >= 61) {
    counter = 0;          // Reset the Timer0 o/f counter variable.

    println(numberOfSamples);
    if (numberOfSamples == 10) {
      print("Correct");
    }
  }
  
}


int main(void) {
    noInterrupts();       // Disable global interrupts
    counter = 0;          // Reset the Timer0 o/f counter variable.
    initialiseTimer0();   // Initialise Timer0
    interrupts();         // Enable global interrupts
    Serial.begin(9600);   // Initialise the serial port to transmitt 9600 bps.
  
}
