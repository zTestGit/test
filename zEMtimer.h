#ifndef zEMtimer_H
#define zEMtimer_H

//test git

#define LED_RED         0
#define LED_GREEN       1
#define LED_ORANGE      2
#define LED_MAX         3
#define ALARM           4
#define BUZZ            5


//#define PIR_MAX         2
//#define DUALPIR_WINDOW_UPPERONE 0
//#define DUALPIR_WINDOW_LOWERONE 1
//#define LETIMER_MAX 3

//these running on RTC 2s
  //the following are u16
#define POR_TIMEOUT             300    //10min = 2secRtc * 300
#define WARMUP_TIMEOUT          15      //30sec
#define WARMUP_TIMEOUT_DIP      7
  //the following are u8
#define POR_CYCLE_TIMEOUT       10       //2secRtc * 10 = 20sec

#define PC2WINDOW_TIMEOUT       3               //6sec
#define SPP_TIMEOUT             10               //10*2sec=20sec

#define PIR_TRIGGED_WINDOW              5       //for each PIR, not used for now  
#define BOTHPIR_TRIGGED_WINDOW          3       //during walking test, if within 6 sec window,
#define MW_TRIGGED_WINDOW               3       //BOTHPIR_TRIGGER and MW_TRIGGED, then alarm         

//these running on LETimer 10ms
  //the following are u8
#define LED_ON_TIMEOUT          (uint8_t)50    //0.5sec   
#define PIR_DEBOUNCE_DEFAULT    (uint8_t)5     //50ms
  //the following are u16
#define DUALPIRWINDOW_TIMEOUT   300             //if 2 pir trig within 3sec, then BOTHPIR_TRIGGER
#define PIRHOLDOFF_TIMEOUT      200             //if 1 pir trigs, don't trig again within 2sec
#define MWHOLDOFF_TIMEOUT       200             //if mw trigs, don't trig again within 2sec
#define ALARMHOLDOFF_TIMEOUT    400             //if alarm trigs, don't go through alarm process within 4 sec
#define ALARMPULSE_WIDTH        (ALARMHOLDOFF_TIMEOUT - 50)   //start from ALARMHOLDOFF, ALARM_ON for 50*10ms =0.5sec
#define ALARMHOLDOFF_DELAYMW    300             //this must <ALARMPULSE_WIDTH to avoid ALARM_ON pulse

//this runs on adcBuffer, only when MW is on, 10ms
#define MW_RANGEON_IGNORE_TIMEOUT        50      //10ms * 50 = 500ms     
#define MW_RANGEOFF_IGNORE_TIMEOUT       10      //100ms
#define MW_WAKEUP_IGNORE_TIMEOUT        150      //1.5s
#define MW_RF_IGNORE_TIMEOUT            50      //500ms ignore when there is a TX

#define MW_WAKEUP_WINDOW                600       //in normal operation, BOTHPIR_TRIGGER will wakeup MW for 6 sec

//---------------------------------------------------------------------
#ifdef zEMtimer_C
//these running on RTC 2s
uint16_t u16porTimer;           //2secRtc * 300 = 10min
uint16_t u16warmupTimer;        //2secRtc * 30 = 1min
uint8_t u8porCycleTimer;        //2secRtc * 5 = 10sec

uint8_t u8PC2windowTimer[PIR_MAX];            //3*2sec = 6sec
uint8_t u8SppTimer[PIR_MAX];                  //10*2sec = 20sec

uint8_t u8PIRtriggedWindow[PIR_MAX];  //2secRtc * 5 = 10sec       //for each PIR, not used for now 
uint8_t u8BothPIRtriggedWindow;//3*2sec = 6sec  //during walking test, BOTHPIR_TRIGGER and MW_TRIGGED within 6sec, then alarm
uint8_t u8MWtriggedWindow;     //3*2sec = 6sec
//these running on LETimer 10ms
uint8_t u8pLedOnTimer[LED_MAX]; //50*10ms=500ms

uint16_t u16DualPirWindow[PIR_MAX];      //300*10ms //if 2 pir trig within 3sec, then BOTHPIR_TRIGGER
uint16_t u16PirHoldoffTimer[PIR_MAX];         //300*10ms //if 1 pir trigs, don't trig again within 3sec
uint16_t u16MWholdoffTimer;           //300*10ms //if mw trigs, don't trig again within 3sec
uint16_t u16alarmHoldoffTimer;        //500*10ms //if alarm trigs, don't go through alarm process within 5 sec
//this runs on adcBuffer, only when MW is on, 10ms
uint16_t u16adcIgnoreCount;
uint16_t u16MWwakeupWindow;   //MW_WAKEUP_WINDOW  600

//uint8_t u8gPulsedOnComponent;

#else
//these running on RTC 2s
extern uint16_t u16porTimer;
extern uint16_t u16warmupTimer;
extern uint8_t u8porCycleTimer;

extern uint8_t u8PC2windowTimer[PIR_MAX];
extern uint8_t u8SppTimer[PIR_MAX];

extern uint8_t u8PIRtriggedWindow[PIR_MAX];
extern uint8_t u8BothPIRtriggedWindow;
extern  uint8_t u8MWtriggedWindow;
//these running on LETimer 10ms
extern uint8_t u8pLedOnTimer[LED_MAX];
 
extern uint16_t u16DualPirWindow[PIR_MAX];
extern uint16_t u16PirHoldoffTimer[PIR_MAX];
extern uint16_t u16MWholdoffTimer;
extern uint16_t u16alarmHoldoffTimer;
//this runs on adcBuffer, only when MW is on, 10ms
extern uint16_t u16adcIgnoreCount;
extern  uint16_t u16MWwakeupWindow;

//extern uint8_t u8gPulsedOnComponent;
#endif

void mwTimerProcess(void);
void rtcTimerProcess(void);
void leTimerProcess(bool);
void ledOn(uint8_t led);
void ledOff(uint8_t led);
 
//void pulsedOn(uint8_t u8Component, uint32_t u32OnTime);
//void pulsedOff(void);

#endif  //zEMtimer_H