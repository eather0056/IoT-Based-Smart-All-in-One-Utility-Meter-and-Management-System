#include<math.h>
#include <Adafruit_GFX.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <SPI.h> 
#include <LiquidCrystal_I2C.h>
#define SAMPLES 300   
#define ACS_Pin A1    

SimpleTimer timer;
LiquidCrystal_I2C lcd(0x27,16,2); 

/////////////Water Billing///////////////
#define FLOWSENSORPIN 2
volatile uint16_t pulses = 0;
volatile uint8_t lastflowpinstate;
volatile uint32_t lastflowratetimer = 0;
volatile float flowrate;
float WUP = 14.46;
float TWC = 0;
float WK = 0;

SIGNAL(TIMER0_COMPA_vect) {
uint8_t x = digitalRead(FLOWSENSORPIN);
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; 
  }
  if (x == HIGH) {
    
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer; 
  lastflowratetimer = 0;
}

void useInterrupt(boolean v) {
  if (v) {
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    TIMSK0 &= ~_BV(OCIE0A);
  }
}

/////////////////////////Electric Bill////////////////////////

//****************Voltage*********************************

        int decimalPrecision = 2;                   // decimal places for all values shown in LED Display & Serial Monitor
        int VoltageAnalogInputPin = A2;            
        float voltageSampleRead  = 0;               /* to read the value of a sample in analog including voltageOffset1 */
        float voltageLastSample  = 0;               /* to count time for each sample. Technically 1 milli second 1 sample is taken */
        float voltageSampleSum   = 0;               /* accumulation of sample readings */
        float voltageSampleCount = 0;               /* to count number of sample. */
        float voltageMean ;                         /* to calculate the average value from all samples, in analog values*/ 
        float RMSVoltageMean ;                      /* square roof of voltageMean without offset value, in analog value*/
        float adjustRMSVoltageMean;
        float FinalRMSVoltage;                      /* final voltage value with offset value*/
   
        float voltageOffset1 =0.00 ;         
        float voltageOffset2 = 0.00;  

//-----------------------------Current-------------------------------------
        float High_peak,Low_peak;        
        float Amps_Peak_Peak, Amps_RMS;
        
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&Bill Parameter&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
 
        float poweru = 0;                           /* Power used by the consuer*/
        float KWH    = 0;                           /* Total ussed unit*/
        float EUP    = 8;                           /* Per unit price of electricity */
        float EB     = 0;                           /* Total Electrical Bill*/


            /* 1.1 - Phase Angle Offset*/

            float currentOffsetRead =0;
            float currentOffsetLastSample =0;
            float currentOffsetSampleCount=0;
            float voltageOffsetRead =0;
            float voltageOffsetLastSample =0;
            float voltageOffsetSampleCount=0;
            
///////////////////////////////////Gas////////////////////////////////
           int Gin = A0;
           float gasfv =0;
           float gv =0;
           float GU =0;
           float GC =0;
           float GUP = 115;

///////////////////////////////////////////////////////////////////////
          float TotalBi =0;
          float NetBa =0;
          float Cbalance = 5000; 
          int lamp = 6;
          int buzzer = 7;          

void setup()
{
   Wire.begin();
   Serial.begin(9600);
   Serial.print("Smart Billing!");
   lcd.init();                 
   lcd.backlight();

   pinMode(FLOWSENSORPIN, INPUT);
   digitalWrite(FLOWSENSORPIN, HIGH);
   lastflowpinstate = digitalRead(FLOWSENSORPIN);
   useInterrupt(true);

   pinMode(lamp, OUTPUT);
   pinMode(buzzer, OUTPUT);
   pinMode(Gin, INPUT);


   pinMode(ACS_Pin,INPUT);

   
}

void loop()
{

////////////////Water Billing///////////////////
 Serial.println("/////////////Water Bill//////////////////");
  Serial.print("Pulses: ");
  Serial.println(pulses, DEC);
  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;
  WK = liters /10;
  TWC = WK * WUP;
  
  Serial.print("Water Used: ");
  Serial.print(liters);
  Serial.println(" L");
  
  lcd.setCursor(0, 1);
  lcd.print("Water used: ");
  lcd.print(liters);

  Serial.print("Water Bill: ");
  Serial.print(TWC);
  Serial.println(" Taka");

  delay(100);
  
///////////////////////////////////Electric Billing System ///////////////////////////

//***************************Voltage************************************

 // if(micros() >= voltageLastSample + 1000 )                                                                      /* every 0.2 milli second taking 1 reading */
     //     {
            voltageSampleRead = (analogRead(VoltageAnalogInputPin)- 512)+ voltageOffset1;                             /* read the sample value including offset value*/
            voltageSampleSum = voltageSampleSum + sq(voltageSampleRead) ;                                             /* accumulate total analog values for each sample readings*/
            
            voltageSampleCount = voltageSampleCount + 1;                                                              /* to move on to the next following count */
       //     voltageLastSample = micros() ;                                                                            /* to reset the time again so that next cycle can start again*/ 
       //   }
        
      //  if(voltageSampleCount == 1000)                                                                                /* after 4000 count or 800 milli seconds (0.8 second), do the calculation and display value*/
        //  {
            voltageMean = voltageSampleSum/voltageSampleCount;                                                        /* calculate average value of all sample readings taken*/
            RMSVoltageMean = (sqrt(voltageMean))*1.5;                                                                 // The value X 1.5 means the ratio towards the module amplification.      
            adjustRMSVoltageMean = RMSVoltageMean + voltageOffset2;                                                   /* square root of the average value including offset value */                                                                                                                                                       /* square root of the average value*/                                                                                                             
            FinalRMSVoltage = RMSVoltageMean + voltageOffset2;                                                        /* this is the final RMS voltage*/
            if(FinalRMSVoltage <= 10)                                                                                /* to eliminate any possible ghost value*/
            {FinalRMSVoltage = 0;}
//            lcd.setCursor ( 0, 0 ); 
//            lcd.print("RMS");
//            lcd.setCursor ( 0, 1); 
//            lcd.print("Voltages = ");
//            lcd.print(FinalRMSVoltage,decimalPrecision);
            Serial.println(FinalRMSVoltage,decimalPrecision);
            voltageSampleSum =0;                                                                                      /* to reset accumulate sample values for the next cycle */
            voltageSampleCount=0;                                                                                     /* to reset number of sample for the next cycle */
         // }

//----------------------------Current---------------------------------------------
  read_Amps();                              
  Amps_RMS = Amps_Peak_Peak*0.3536*0.06;     //Now we have the peak to peak value normally the formula requires only multiplying times 0.3536                    
                                           
      if(Amps_RMS < 0.2)
         {Amps_RMS =0;}                                       
  Serial.println(Amps_RMS);                   
  //Serial.print("\t");
  //Serial.println(Amps_Peak_Peak);
  delay(200);


    poweru = Amps_RMS * FinalRMSVoltage* 0.8;
    KWH = KWH + (poweru * (2.05/60/60/1000))*1000;

    EB = KWH*EUP;
    
          Serial.println("/////////////Electrical Bill//////////////////");
          Serial.print("Power Used =");
          Serial.print(KWH);                                  /* display current value in LCD in first row  */
          Serial.println(" U    ");
          Serial.print("Total Electric Bill =");
          Serial.print(EB);                                  /* display current value in LCD in first row  */
          Serial.println(" Taka");

              
/////////////////////////////////////////////////////////Gas Bill ///////////////////////////////////////////////////////////////////////

           gasfv = analogRead(Gin)*5.0/1023;  //domestic LPG cylinders pressure avagrage 800 KPa and flow rate around 2 Kg/H
           gv = gasfv/100;  // The amount of gas pass in that moment
           GU = GU + gv;           // the total amount of gas 
           GC = GU * GUP;         // the total gas price

           Serial.println("/////////////Gass Bill//////////////////");
           Serial.print("The amount of gas used: ");
           Serial.print(GU);
           Serial.println(" Kg");
           Serial.print("The total gas bill: ");
           Serial.print(GC);
           Serial.println(" Taka");
           




///////////////////////////////////Total Utiity Bill ///////////////////////////////////////

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("S. Utility Bill");
          delay(2000);

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Electricity bill");
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("KWH : ");
          lcd.setCursor(7,0);
          lcd.print(KWH);
          lcd.setCursor(12,0);
          lcd.print("U");
          lcd.setCursor(0,1);
          lcd.print("Bill: ");
          lcd.setCursor(7,1);
          lcd.print(EB);
          lcd.setCursor(12,1);
          lcd.print("Tk");
          delay(3000);


          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Gas bill");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Gas U.: ");
          lcd.setCursor(9,0);
          lcd.print(GU);
          lcd.setCursor(14,0);
          lcd.print("Kg");
          lcd.setCursor(0,1);
          lcd.print("Bill  : ");
          lcd.setCursor(9,1);
          lcd.print(GC);
          lcd.setCursor(14,1);
          lcd.print("Tk");
          delay(3000);

          
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Water bill");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Water U.: ");
          lcd.setCursor(10,0);
          lcd.print(liters);
          lcd.setCursor(15,0);
          lcd.print("L");
          lcd.setCursor(0,1);
          lcd.print("Bill    : ");
          lcd.setCursor(10,1);
          lcd.print(TWC);
          lcd.setCursor(15,1);
          lcd.print("Tk");
          delay(3000);
          
          Serial.println("S. Utility Bill");
          Serial.println("Electricity bill");
          Serial.print("KWH: ");
          Serial.print(KWH);
          Serial.println("U");
          Serial.print("Bill: ");
          Serial.print(EB);
          Serial.println("Tk");
          Serial.println("Gas bill");
          Serial.print("Gas U.: ");
          Serial.print(GU);
          Serial.println("Kg");
          Serial.print("Bill    : ");
          Serial.print(GC);
          Serial.println("Tk");
          Serial.println("Water bill");
          Serial.print("Water U.: ");
          Serial.print(liters);
          Serial.println("L");
          Serial.print("Bill: ");
          Serial.print(TWC);
          Serial.println("Tk");

          TotalBi = TWC + GC + EB;

          NetBa = Cbalance - TotalBi;

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("T. Bil:");
          lcd.setCursor(8,0);
          lcd.print(TotalBi);
          lcd.setCursor(15,0);
          lcd.print("Tk");
          
          lcd.setCursor(0,1);
          lcd.print("C. Bal:");
          lcd.setCursor(8,1);
          lcd.print(NetBa);
          lcd.setCursor(15,1);
          lcd.print("Tk");
          delay(4000);
          Serial.println(" ");
          Serial.print("T. Bill:");
          Serial.print(TotalBi);
          Serial.println("Tk");
          
          Serial.println(" ");
          Serial.print("C. Bill:");
          Serial.print(NetBa);
          Serial.println("Tk");





          if(NetBa >=5000)
          {Serial.println("Stable Balance");
          lcd.clear();
          lcd.print("Stable Balance");
          delay(1000);
          digitalWrite(lamp,LOW);
           digitalWrite(buzzer,LOW);
           } 
           
          else if(NetBa <=5000 && NetBa >=2000)
          {Serial.println("Moderat Balance");
          lcd.clear();
          lcd.print("Moderat Balance");
          digitalWrite(lamp,LOW);
           digitalWrite(buzzer,LOW);
          delay(1000);
          
           }
           
           else if(NetBa <=2000 && NetBa >=500)
          {Serial.println("Low Balance");
          lcd.clear();
          lcd.print("Low Balance");
          digitalWrite(lamp,HIGH);
           digitalWrite(buzzer,LOW);
          delay(1000);

           }

          else if(NetBa <=500)
          {Serial.println("Very Low Balance");
          lcd.clear();
          lcd.print("Very L. Balance");
          
          digitalWrite(lamp,HIGH);
          digitalWrite(buzzer,HIGH);
          delay(1000);
           }

}

void read_Amps()           
{                           
  int cnt;            
  High_peak = 0;      
  Low_peak = 1024;
  
      for(cnt=0 ; cnt<SAMPLES ; cnt++)       
      {
        float ACS_Value = analogRead(ACS_Pin); 

        
        if(ACS_Value > High_peak)              
            {
              High_peak = ACS_Value;           
            }
        
        if(ACS_Value < Low_peak)               
            {
              Low_peak = ACS_Value;            
            }
      }                                       
      
  Amps_Peak_Peak = High_peak - Low_peak;      
}
