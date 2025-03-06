/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"
// #include "unistd.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */

static u8 UserApp_au8GameName[] = "The Memory Game";
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/
void delay_1s();
void delay_non_blocking();
void whitelightandbuzz();
void bluelightandbuzz();
void yellowlightandbuzz();
void redlightandbuzz();
void deacwhitelightandbuzz();
void deacbluelightandbuzz();
void deacyellowlightandbuzz();
void deacredlightandbuzz();
void whitegameseq();
void level_1();
/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  LedOff(WHITE);          //LEDS initialized
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
  
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }
  LcdClearChars(LINE1_START_ADDR + 13,15);
  LcdCommand(LCD_CLEAR_CMD); 
  LcdMessage(LINE1_START_ADDR, UserApp_au8GameName);
  LcdMessage(LINE2_START_ADDR, "Press Any Button to Begin");
  //LcdCommand(LCD_SHIFT_DISPLAY | 0x08);
} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
  // void delay_1s();
  // void whitelightandbuzz();
  // void bluelightandbuzz();
  // void yellowlightandbuzz();
  // void redlightandbuzz();
  // void deacwhitelightandbuzz();
  // void deacbluelightandbuzz();
  // void deacyellowlightandbuzz();
  // void deacredlightandbuzz();
  // void whitegameseq();
  // void level_1();
  
  static u8 buttons_pressed_to_begin = 0;
  static u8 button0_pressed = 0;
  static u8 button1_pressed = 0;
  static u8 button2_pressed = 0;
  static u8 button3_pressed = 0;
  static u8 all_buttons_pressed = 0;
  static u8 transition_to_lvl_1 = 0;
  static u8 to_level_1 = 0;
  static u8 level_1_executed = 0;
  static u8 level_1_complete = 0;
  
  if (buttons_pressed_to_begin == 0) {
    if (IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) || IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3)) {
      LcdCommand(LCD_CLEAR_CMD); // Clear LCD when buttons are pressed
      buttons_pressed_to_begin = 1;
    }
  }
  
  if (buttons_pressed_to_begin == 1) {
    static u8 message_shown = 0;
    if (!message_shown) {
      LcdMessage(LINE1_START_ADDR, "press each button");
      LcdMessage(LINE2_START_ADDR, "see what it do");
      message_shown = 1;
    }
    // Handle button presses and LEDs/audio
    if (WasButtonPressed(BUTTON0)) {
      ButtonAcknowledge(BUTTON0);
      LedOn(WHITE);
      PWMAudioSetFrequency(BUZZER1, 500);
      button0_pressed = 1;
    }
    if (WasButtonPressed(BUTTON1)) {
      ButtonAcknowledge(BUTTON1);
      LedOn(BLUE);
      PWMAudioSetFrequency(BUZZER1, 392);
      button1_pressed = 1;
    }
    if (WasButtonPressed(BUTTON2)) {
      ButtonAcknowledge(BUTTON2);
      LedOn(YELLOW);
      PWMAudioSetFrequency(BUZZER1, 330);
      button2_pressed = 1;
    }
    if (WasButtonPressed(BUTTON3)) {
      ButtonAcknowledge(BUTTON3);
      LedOn(RED);
      PWMAudioSetFrequency(BUZZER1, 294);
      button3_pressed = 1;
    }
    // Update buzzer and LED based on button presses
    if (IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) || IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3)) {
      PWMAudioOn(BUZZER1);
    } else {
      PWMAudioOff(BUZZER1);
      LedOff(WHITE);
      LedOff(BLUE);
      LedOff(YELLOW);
      LedOff(RED);
    }
  }
  
  // All buttons pressed, set flags for transition
  if (button0_pressed && button1_pressed && button2_pressed && button3_pressed && !all_buttons_pressed) {
    all_buttons_pressed = 1;
    delay_1s();
    PWMAudioOff(BUZZER1);
    LedOff(WHITE);
    LedOff(BLUE);
    LedOff(YELLOW);
    LedOff(RED);
    transition_to_lvl_1 = 1; // Set transition flag
  }
  
  // Handle transition to level 1
  if (transition_to_lvl_1 == 1) {
    static u8 message_shown_level_1 = 0;
    if (!message_shown_level_1) {
      LcdCommand(LCD_CLEAR_CMD); // Clear LCD before showing the next message
      delay_1s();  // Delay to allow clearing
      LcdMessage(LINE1_START_ADDR, "now copy the lights");
      LcdMessage(LINE2_START_ADDR, "and sounds");
      delay_1s();  // Wait for a bit before transitioning
      message_shown_level_1 = 1;
      to_level_1 = 1; // Set flag for level 1 transition
    }
  }
  
  // Execute level 1 when the flag is set
  if (to_level_1 == 1 && level_1_executed == 0) {
    level_1();  // Execute the sequence
    level_1_executed = 1; // Set flag to indicate level 1 is done
  }
  
  // Handle completion of level 1 and transition to level 2
  if (level_1_executed) {
    static u8 button0_pressed = 0;
    static u8 button2_pressed = 0;
    static u8 button1_pressed = 0;
    static u8 button3_pressed = 0;
    
    if (IsButtonPressed(BUTTON0) && !button0_pressed) {
      ButtonAcknowledge(BUTTON0);
      button0_pressed = 1;
    }
    
    if (IsButtonPressed(BUTTON2) && button0_pressed && !button2_pressed) {
      ButtonAcknowledge(BUTTON2);
      button2_pressed = 1;
    }
    
    if (IsButtonPressed(BUTTON1) && button2_pressed && !button1_pressed) {
      ButtonAcknowledge(BUTTON1);
      button1_pressed = 1;
    }
    
    if (IsButtonPressed(BUTTON3) && button1_pressed && !button3_pressed) {
      ButtonAcknowledge(BUTTON3);
      button3_pressed = 1;
    }
    
    // Check if all buttons in the sequence are pressed
    if (button0_pressed && button2_pressed && button1_pressed && button3_pressed) {
      level_1_complete = 1;
    }
  }
  
  // Transition to level 2 after completion of level 1
  if (level_1_complete) {
    LcdCommand(LCD_CLEAR_CMD);  // Clear screen
    delay_1s();  // Wait for 1 second
    LcdMessage(LINE1_START_ADDR, "level 2");  // Show level 2 message
    level_1_complete = 0;  // Reset for next use
  }

} /* end UserApp1SM_Idle() */
 void whitelightandbuzz(){
    LedOn(WHITE);
    PWMAudioSetFrequency(BUZZER1,500);
    PWMAudioOn(BUZZER1);
 }
  void bluelightandbuzz(){
    LedOn(BLUE);
    PWMAudioSetFrequency(BUZZER1,392);
    PWMAudioOn(BUZZER1);
 }
  void yellowlightandbuzz(){
    LedOn(YELLOW);
    PWMAudioSetFrequency(BUZZER1,330);
    PWMAudioOn(BUZZER1);
 }
  void redlightandbuzz(){
    LedOn(RED);
    PWMAudioSetFrequency(BUZZER1,294);
    PWMAudioOn(BUZZER1);
 }
 void deacwhitelightandbuzz(){
  LedOff(WHITE);
  PWMAudioOff(BUZZER1);
 }
  void deacbluelightandbuzz(){
  LedOff(BLUE);
  PWMAudioOff(BUZZER1);
 }
  void deacyellowlightandbuzz(){
  LedOff(YELLOW);
  PWMAudioOff(BUZZER1);
 }
  void deacredlightandbuzz(){
  LedOff(RED);
  PWMAudioOff(BUZZER1);
 }
 void delay_non_blocking() {
  static uint32_t last_time = 0;
  if (G_u32SystemTime1ms - last_time >= 1000) {
    last_time = G_u32SystemTime1ms;
    // Do something after 1s
  }
}
void delay_1s(void)
{
    uint32_t start_time = G_u32SystemTime1ms;
    while ( (G_u32SystemTime1ms - start_time) < 1000 );  // Wait for 1000ms
}
 void whitegameseq(){
  delay_1s();
  whitelightandbuzz();
  delay_1s();
  deacwhitelightandbuzz();
 }
 void bluegameseq(){
  delay_1s();
  bluelightandbuzz();
  delay_1s();
  deacbluelightandbuzz();
 }
 void yellowgameseq(){
  delay_1s();
  yellowlightandbuzz();
  delay_1s();
  deacyellowlightandbuzz();
 }
 void redgameseq(){
  delay_1s();
  redlightandbuzz();
  delay_1s();
  deacredlightandbuzz();
 }
void level_1(){
  WATCHDOG_BONE();
  whitegameseq();
  WATCHDOG_BONE();
  yellowgameseq();
  WATCHDOG_BONE();
  bluegameseq();
  WATCHDOG_BONE();
  redgameseq();
  WATCHDOG_BONE();
  // need to add a watchdog timer
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

// Kainaans main project to do list:
//figure out how to scroll the lcd messages to begin
//clear start screen and print next message 
// flash each buttons led and buzzer

//up to this point i got first screen to work
// then second screen
//then the buttons and associated leds and buzzers to go but not shut off properly 
//after the buttons were released 

//spare code
    // u16blinkcounter++;
    // if (u16blinkcounter==250){
    //   u16blinkcounter=0;
    //   u8counter++;
      // if(u8counter==16){
      //   u8counter=0;
      // }
      // if(u8counter &0x01){
      //   redlightandbuzz();
      // }
      // if(u8counter & 0x02){
      //   whitelightandbuzz;
      // }
      // if(u8counter& 0x04){
      // bluelightandbuzz;
      // }
      // if(u8counter & 0x08){
      //   yellowlightandbuzz;
      // }

      // was at line 178   // if (buttons_pressed_to_begin == 1) {
  //   LcdCommand(LCD_CLEAR_CMD);
  //   delay_1s();
  // //   buttons_pressed_to_begin++;
  // // }
  // // if (buttons_pressed_to_begin == 2 ){
  // //   buttons_pressed_to_begin = 2;
  //   LcdMessage(LINE1_START_ADDR, "hold each button");
  //   LcdMessage(LINE2_START_ADDR, "see what it do ");
  // }

    // if (IsButtonHeld(BUTTON0, 200)){
  //   buttons_to_lights++;
  //   whitelightandbuzz();
  // }
  // if (IsButtonHeld(BUTTON1,200)){
  //   buttons_to_lights++;
  //   bluelightandbuzz();
  // }
  // if (IsButtonHeld(BUTTON2,200)){
  //   buttons_to_lights++;
  //   yellowlightandbuzz();
  // }
  // if (IsButtonHeld(BUTTON3,200)){
  //   redlightandbuzz();
  //   delay_1s();
  //   delay_1s();
  //   buttons_to_lights=40;
  // }
//   if (!(IsButtonHeld(BUTTON0, 200) || IsButtonHeld(BUTTON1, 200) ||
//       IsButtonHeld(BUTTON2, 200) || IsButtonHeld(BUTTON3, 200))) {
//       ButtonAcknowledge(BUTTON0);
//       ButtonAcknowledge(BUTTON1);
//       ButtonAcknowledge(BUTTON2);
//       ButtonAcknowledge(BUTTON3);
//       LedOff(WHITE);
//       LedOff(BLUE);
//       LedOff(YELLOW);
//       LedOff(RED);
//       PWMAudioOff(BUZZER1);
// }

  // if(buttons_to_lights >= 40){
  //   LcdCommand(LCD_CLEAR_CMD);
  //   LcdMessage(LINE1_START_ADDR, "now copy the lights");
  //   LcdMessage(LINE2_START_ADDR, "and or sounds :)");
  //   delay_1s();
  //   transition_to_lvl_1 = 1;
  // }
      // good interview question: what do you enjoy most about working for garmin/ in this field