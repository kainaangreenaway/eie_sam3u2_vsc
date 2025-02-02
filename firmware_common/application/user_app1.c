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
  static u8 buttons_pressed_to_begin = 0;
  static u8 buttons_to_lights = 0;
  if(IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) || IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3)){
    buttons_pressed_to_begin++;
  }
  if (buttons_pressed_to_begin == 1) {
    LcdCommand(LCD_CLEAR_CMD);
  }
  else if (buttons_pressed_to_begin ==2 ){
    buttons_pressed_to_begin = 2;
    LcdMessage(LINE1_START_ADDR, "hold each button");
    LcdMessage(LINE2_START_ADDR, "see what it do ");
  if (IsButtonHeld(BUTTON0, 500)){
    LedOn(WHITE);
    PWMAudioSetFrequency(BUZZER1,500);
    PWMAudioOn(BUZZER1);
  }
  if (IsButtonHeld(BUTTON1,500)){
    LedOn(BLUE);
    PWMAudioSetFrequency(BUZZER1,392);
    PWMAudioOn(BUZZER1);
  }
  if (IsButtonHeld(BUTTON2,500)){
    LedOn(YELLOW);
    PWMAudioSetFrequency(BUZZER1,330);
    PWMAudioOn(BUZZER1);
  }
  if (IsButtonHeld(BUTTON3,500)){
    LedOn(RED);
    PWMAudioSetFrequency(BUZZER1,294);
    PWMAudioOn(BUZZER1);
  }
  if (!(IsButtonHeld(BUTTON0, 500) || IsButtonHeld(BUTTON1, 500) ||
      IsButtonHeld(BUTTON2, 500) || IsButtonHeld(BUTTON3, 500))) {
      ButtonAcknowledge(BUTTON0);
      ButtonAcknowledge(BUTTON1);
      ButtonAcknowledge(BUTTON2);
      ButtonAcknowledge(BUTTON3);
      LedOff(WHITE);
      LedOff(BLUE);
      LedOff(YELLOW);
      LedOff(RED);
      PWMAudioOff(BUZZER1);
}
}  
} /* end UserApp1SM_Idle() */
     

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