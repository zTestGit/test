#define zEMtimer_C

//test git in c
//made again some change in testMerge, in c file
//more changes in c, to test rebasing

#include "zEMmain.h"

void mwTimerProcess()    //called in main with 10ms period, only when MW is on
{
  if(u16adcIgnoreCount != 0){    
    u16adcIgnoreCount--;
    //INTERCON1_TOGGLE;
  }
  //this only works when in normal operation
  if(u16MWwakeupWindow != 0){    //in normal operation when BothPIRtrigged that wakes up MW
    u16MWwakeupWindow--;
    if(u16MWwakeupWindow == 0){  //time out, suggesting BothPIRtrigged without MW trigger
      mwOff();
    }
  }
}

void rtcTimerProcess()  //called in 2sec period, when bRtcTimerReceived
{
  uint8_t i;
  static  uint8_t count = 0;
    
  if(bWalkingTest == true){     //this only relevent when during walking test 
    if(u16porTimer != 0){
      u16porTimer--;
      if(u16porTimer == 0){
        bWalkingTest = false;   //end of walk test
        u16warmupTimer = 0;     //relevent timer no longer needed
        u8porCycleTimer = 0;
        pirOff();       //this can be polled later
        mwOff();
      }
    }
    if(u16warmupTimer != 0){      //this is for initial power up, for analog circuit warm up
      u16warmupTimer--;
      if(u16warmupTimer == 0){
        u8porCycleTimer = 1;  //will do the first MWrange immediately, then back MWsignal
        mwOn();
        pirOn();
        RED_LED_OFF;
        GREEN_LED_OFF;
        ORANGE_LED_OFF;
      }
      else{
        RED_LED_TOGGLE;
        GREEN_LED_TOGGLE;
        ORANGE_LED_TOGGLE;
      }
    }
    
    if(u8porCycleTimer != 0){
      u8porCycleTimer--;
      if( u8porCycleTimer == 0){
        MWRANGE_POWER_ON;             
        u32adChannel = mwAdcInitSingle(AMW_RANGE_ADC_CH); 
        u16adcIgnoreCount = MW_RANGEON_IGNORE_TIMEOUT;     //this counts when adc buffer done (10ms)
        if(bWalkingTest == true){                 //in walk test, re-start MWRANGE  
          u8porCycleTimer = POR_CYCLE_TIMEOUT;    //after 10sec, do another MWrange, then back MWsignal
        }
      }
    }
  }
  
  if(bWalkingTest == false){    //this only relevent during normal operation, poll set/unset
    if(PIC_ARM_ISHIGH == true){ //INTERCON2-PE11 from PIC PA2, pin high 
      if(bPIRisOn == false){    //request set
        pirOn();
      }
    }
    else{                        //set/unset pin low, request unset
      if(bPIRisOn == true){
        pirOff();
      }
    }
  }

  //------------------------------------------- 
  for (i=0;i<PIR_MAX;i++){    //2 PIR
    if(u8PC2windowTimer[i] != 0){
      u8PC2windowTimer[i]--;
      if(u8PC2windowTimer[i] == 0)
      {
        u8LastPirCrossing[i] = PIR_CROSSING_NONE;
      }
    }
    if(u8SppTimer[i] != 0){
      u8SppTimer[i]--;
      if(u8SppTimer[i] == 0){
        u8LastPirCrossing[i] = PIR_CROSSING_NONE;
      }
    }
    if(u8PIRtriggedWindow[i] != 0){
      u8PIRtriggedWindow[i]--;
    }
  }  
  
  if(u8BothPIRtriggedWindow !=0){ 
    u8BothPIRtriggedWindow--;    
  }
  if(u8MWtriggedWindow != 0){   
    u8MWtriggedWindow--;        
  }

  
  if(PIR1_SENSITIVITY_ISHIGH){    //PC11: pin high, dip off
    if(bHighSensitivity[PIR_UPPERONE] == false){
      bHighSensitivity[PIR_UPPERONE] = true;    //pin high, high sensitivity
    }
  }
  else{
    if(bHighSensitivity[PIR_UPPERONE] == true){
      bHighSensitivity[PIR_UPPERONE] = false;    
    }
  }
  if(PIR2_SENSITIVITY_ISHIGH){    //PC10: pin high, dip off
    if(bHighSensitivity[PIR_LOWERONE] == false){
      bHighSensitivity[PIR_LOWERONE] = true;
    }
  }
  else{
    if(bHighSensitivity[PIR_LOWERONE] == true){
      bHighSensitivity[PIR_LOWERONE] = false;    
    }
  }
  
    
  if(count != 0){
    count--;
  }
  else{
    count=5;            //10sec
  //  alarmTriggerProcess();    //debug
    
#ifdef zDEBUG_SINE    
    u16sineFreq += 5;
    if (u16sineFreq >150) 
      u16sineFreq = 10;
    
    SineFrequencySetTimerTop();           //is there change in frequency, or amplitude, or offset?
    FillAndModifySignalBuffer(true);
    FillAndModifySignalBuffer(false);    
#endif    
  }
   //pulsedOn();       //debug 
}

void leTimerProcess(bool bDebounceIsDone)   //this is called in main when LETimer int at 10ms
{
  uint8_t i;
  bool bAllLedIsOff = true;
  bool bAllHoldOffTimerOff = true;
  
  for (i=0;i<LED_MAX;i++){
    if( u8pLedOnTimer[i] != 0){
      u8pLedOnTimer[i]--;
      if (u8pLedOnTimer[i] == 0){
        ledOff(i);
      }
      bAllLedIsOff = false;
    }
  }
  for (i=0;i<PIR_MAX;i++){    //2 PIR
    if(u16PirHoldoffTimer[i] != 0){
      u16PirHoldoffTimer[i]--;
      bAllHoldOffTimerOff = false;
    }
    if(u16DualPirWindow[i] != 0){
      u16DualPirWindow[i]--;
      bAllHoldOffTimerOff = false;
    }    
  }  
  
  if(u16MWholdoffTimer != 0){    //this prevent MW led flash too often
    u16MWholdoffTimer--;
    bAllHoldOffTimerOff = false;
  }
  
  if(u16alarmHoldoffTimer != 0){     //if there is an alarm, do not alarm again in 4 sec   
    u16alarmHoldoffTimer--;
    bAllHoldOffTimerOff = false;
    if (u16alarmHoldoffTimer<ALARMPULSE_WIDTH){ //give a 0.5 sec neg pulse to PIC
      if( !ALARM_ISOFF){ 
        ALARM_OFF;
        TIMER_Enable(BUZZ_TIMER, false);     //turn off sound  
       }
    }    
  }
  
  if( (bDebounceIsDone == true)         //all u8pirDebounce[index] == 0, debounce finished
     && (bAllLedIsOff == true)          //all led off         
       && (bAllHoldOffTimerOff == true) //all shortTimer is done
         ){      
           LETIMER_Enable(SHORT_TIMER, false);  //so no longer need to go through isrLEtimer again     
         }  
}
 

void ledOn(uint8_t led)
{
  switch(led)
  {
  case LED_RED: RED_LED_ON;
  break;
  case LED_GREEN: GREEN_LED_ON;
  break;
  case LED_ORANGE: ORANGE_LED_ON;
  break;        
  }
  u8pLedOnTimer[led] = LED_ON_TIMEOUT;            //500ms with LETimer int at 10ms
  //LETIMER_Enable(SHORT_TIMER, false);
  LETIMER_IntClear(SHORT_TIMER, LETIMER_IF_UF);
  LETIMER_Enable(SHORT_TIMER, true);                    //start letimer
}

void ledOff(uint8_t led)
{
  switch(led)
  {
  case LED_RED: RED_LED_OFF;
  break;
  case LED_GREEN: GREEN_LED_OFF;
  break;
  case LED_ORANGE: ORANGE_LED_OFF;
  break;        
  }
}

/*
void pulsedOn(uint8_t u8Component, uint32_t u32OnTime)
{
  uint32_t u32Counter;
  
  u8gPulsedOnComponent = u8Component; 
  switch(u8gPulsedOnComponent)
  {
  case ALARM:ALARM_ON;   
  break;
  case BUZZ: BUZZ_ON;
  break;
  default:
    break;
  }
  u32Counter = RTC_CounterGet();
  u32Counter += u32OnTime;
  if(u32Counter>= LF_COUNT_2SEC){
    u32Counter -= LF_COUNT_2SEC;
  }
  RTC_CompareSet(1,                             //compare reg, 0 or 1 
                 u32Counter);               //COMP1 top value, 24bit, upcount
  RTC_IntClear(RTC_IF_COMP1);
  RTC_IntEnable(RTC_IF_COMP1);  //can be OF, COMP0, COMP1
}

void pulsedOff()
{
  switch(u8gPulsedOnComponent)
  {
  case ALARM:ALARM_OFF;   
  break;
  case BUZZ: BUZZ_OFF;
  break;
  default:
    break;
  }
  u8gPulsedOnComponent = 0;

}
*/
