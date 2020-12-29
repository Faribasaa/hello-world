/*
 * stopwatch.c
 *
 * Created: 6/13/2020 8:22:48 PM
 * Author : Fariba
 */ 
 

 
#define F_CPU 16000000 UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>

#define true 1
#define false 0

#define SEVEN_SEGMENT_PORT PORTD

#define SEVEN_SEGMENT_DDR DDRD

volatile uint8_t digits[4];
 
volatile unsigned int   clock_millisecond=0;
volatile unsigned char  clock_second=0;

volatile unsigned char  clock_minute=0;
volatile unsigned char  clock_hour=0;

 
uint8_t hide_display = false;

 

uint8_t blink_display = true;



void SevenSegment(uint8_t n,uint8_t dp)
{
 
   if(n<11)
   {
      switch (n)
      {
         case 0:
         SEVEN_SEGMENT_PORT=0b00000011;
         break;

         case 1:
         SEVEN_SEGMENT_PORT=0b10011111;
         break;

         case 2:
         SEVEN_SEGMENT_PORT=0b00100101;
         break;

         case 3:
         SEVEN_SEGMENT_PORT=0b00001101;
         break;

         case 4:
         SEVEN_SEGMENT_PORT=0b10011001;
         break;

         case 5:
         SEVEN_SEGMENT_PORT=0b01001001;
         break;

         case 6:
         SEVEN_SEGMENT_PORT=0b01000001;
         break;

         case 7:
         SEVEN_SEGMENT_PORT=0b00011111;
         break;

         case 8:
         SEVEN_SEGMENT_PORT=0b00000001;
         break;

         case 9:
         SEVEN_SEGMENT_PORT=0b00001001;
         break;

         case 10:
         //A BLANK DISPLAY

         SEVEN_SEGMENT_PORT=0b11111111;
         break;

      }
      if(dp)
      {
         
         SEVEN_SEGMENT_PORT&=0b11111110;
      }
   }
   else

   { 
      SEVEN_SEGMENT_PORT=0b11111101;
   }
}

void Wait()
{
   uint8_t i;
   for(i=0;i<10;i++)
   {
      _delay_loop_2(0);
   }
}


void Print(uint16_t num)
{
   

   uint8_t i=0;
   uint8_t j;
   if(num>9999) return;


   while(num)
   {
      digits[i]=num%10;
      i++;

      num=num/10;
   }
 

   for(j=i;j<4;j++) digits[j]=0;
}




int main()
{
    

   // Prescaler = FCPU/1024
   TCCR0|=(1<<CS02);

   //Enable Overflow Interrupt Enable

   TIMSK|=(1<<TOIE0);

   //Initialize Counter
   TCNT0=0;

   //Port c[3,2,1,0] as out put
   DDRC|=0b00001111;

   PORTC=0b00001110;

   //Port D

   SEVEN_SEGMENT_DDR=0XFF;

   //Turn off all segments
   SEVEN_SEGMENT_PORT=0XFF;

   //Set up the timer1 as described in the
      //tutorial
      //TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10);

   //initailly stop the timer by setting clock source =000
   TCCR1B&=(~((1<<CS12)|(1<<CS11)|(1<<CS10)));
      OCR1A=250;

      //Enable the Output Compare A interrupt
      TIMSK|=(1<<OCIE1A);

      //Enable interrupts globally

      sei();
 
      while(1)
      {
         int disp;
      disp=(clock_hour*100)+clock_minute;
      

      Print(disp);

      if(!(PINB & (1<<PB2)))
      {
          
         clock_millisecond=0;
         clock_second=0;
         clock_minute=0;
         clock_hour=0;
      }

      if(!(PINB & (1<<PB1)))
      {
          
 
         TCCR1B&=(~((1<<CS12)|(1<<CS11)|(1<<CS10)));

          
         blink_display=true;
      }

      if(!(PINB & (1<<PB0)))
      {
          
            TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10);

         

         blink_display=false;

          
         hide_display=false;
      }

         _delay_loop_2(0);
      _delay_loop_2(0);

      }


}

ISR(TIMER0_OVF_vect)
{


    
   static uint8_t i=0;

   if(i==3)
   {
      
      i=0;
   }
   else
   {
      
      i++;
   }

    

   PORTC=~(1<<i);

   if(hide_display)
   {
       
      SevenSegment(10,0);

   }
   else
   {
     

      SevenSegment(digits[i],0);
   }

   

   if(!blink_display) return;

   static uint8_t blink_counter=0;

   blink_counter++;
   if(blink_counter == 16)
   {
      blink_counter =0;

      hide_display=!hide_display;
   }

}


 
ISR(TIMER1_COMPA_vect)
{
      clock_millisecond++;
      if(clock_millisecond==1000)
      {
      clock_second++;
         clock_millisecond=0;
         if(clock_second==60)
         {
            clock_minute++;
            clock_second=0;
         }
      if(clock_minute==60)
      {
         clock_hour++;
         clock_minute=0;
      }
      }
}