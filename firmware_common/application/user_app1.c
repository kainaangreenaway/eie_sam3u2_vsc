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

static u8 UserApp_au8GameName[] = "A Memory Game    TM";
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
void gameseq_0();
void gameseq_1();
void gameseq_2();
void gameseq_3();
void level_1();
void level_2();
void level_3();
void level_4();
void level_5();
void level_6();
void level_7();
void level_8();
void level_9();
void level_10();
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
  LcdMessage(LINE2_START_ADDR, "Press to Begin");
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
  // game state variables
  static u8 game_state = 0;  // initial = 0, tutorial = 1, levels 1-5 = 2-6, game complete = 7
  
  // button track
  static u8 button0_pressed = 0;
  static u8 button1_pressed = 0;
  static u8 button2_pressed = 0;
  static u8 button3_pressed = 0;
  
  // sequence track
  static u8 sequence_position = 0;
  
  // game flow control
  static u8 substate = 0;  // sub-state within each game state (tutorial has 2 (LCD message and buttons), each level has 5(lcd, delay, sequence, user input, error), celebration has 2 (LCD message and jingle))
  static u32 last_time = 0;  // for timing non-blocking operations
  static u8 flash_count = 0;  // counter for light/sound pattern
  static u8 celebration_count = 0;  // celebration
  static u8 all_buttons_delay_started = 0;  // for tutorial delay
  static u8 level_complete_delay_started = 0;  // for level completion delay
  
  // Patterns
  static const u8 level1_pattern[] = {0, 2, 1, 3};  // WHITE, YELLOW, BLUE, RED
  static const u8 level2_pattern[] = {1, 3, 2, 0, 1};  // BLUE, RED, YELLOW, WHITE, BLUE
  static const u8 level3_pattern[] = {0, 3, 1, 2, 0, 3};  // WHITE, RED, BLUE, YELLOW, WHITE, RED
  static const u8 level4_pattern[] = {2, 0, 2, 1, 3, 0, 1};  // YELLOW, WHITE, YELLOW, BLUE, RED, WHITE, BLUE
  static const u8 level5_pattern[] = {3, 1, 0, 2, 3, 2, 1, 0};  // RED, BLUE, WHITE, YELLOW, RED, YELLOW, BLUE, WHITE
  
  static u8 pattern_length = 0;  // length of current level pattern
  static const u8* current_pattern = NULL;  // pointer to current level pattern
  
  WATCHDOG_BONE();
  
  //wait for any button press to start
  if (game_state == 0) {
    if (IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) || 
        IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3)) {
      LcdCommand(LCD_CLEAR_CMD);
      game_state = 1;  // move to tutorial
      substate = 0;
      button0_pressed = 0;
      button1_pressed = 0;
      button2_pressed = 0;
      button3_pressed = 0;
    }
  }
  
  // TUTORIAL - Show instructions and let user try each button
  else if (game_state == 1) {
    if (substate == 0) {
      LcdMessage(LINE1_START_ADDR, "Tutorial:");
      LcdMessage(LINE2_START_ADDR, "Press The Buttons");
      substate = 1;
    }
    
    // track which buttons in tutorial
    static u8 tutorial_button0_pressed = 0;
    static u8 tutorial_button1_pressed = 0;
    static u8 tutorial_button2_pressed = 0;
    static u8 tutorial_button3_pressed = 0;
    
    // handle button presses and lights/sounds
    if (IsButtonPressed(BUTTON0) && !button0_pressed) {
      ButtonAcknowledge(BUTTON0);
      LedPWM(WHITE, LED_PWM_10);
      PWMAudioSetFrequency(BUZZER1, 500);
      PWMAudioOn(BUZZER1);
      button0_pressed = 1;
      tutorial_button0_pressed = 1; // track for tutorial completion
    }
    if (IsButtonPressed(BUTTON1) && !button1_pressed) {
      ButtonAcknowledge(BUTTON1);
      LedPWM(BLUE, LED_PWM_25);
      PWMAudioSetFrequency(BUZZER1, 392);
      PWMAudioOn(BUZZER1);
      button1_pressed = 1;
      tutorial_button1_pressed = 1; // track for tutorial completion
    }
    if (IsButtonPressed(BUTTON2) && !button2_pressed) {
      ButtonAcknowledge(BUTTON2);
      LedOn(YELLOW);
      PWMAudioSetFrequency(BUZZER1, 330);
      PWMAudioOn(BUZZER1);
      button2_pressed = 1;
      tutorial_button2_pressed = 1; // track for tutorial completion
    }
    if (IsButtonPressed(BUTTON3) && !button3_pressed) {
      ButtonAcknowledge(BUTTON3);
      LedOn(RED);
      PWMAudioSetFrequency(BUZZER1, 294);
      PWMAudioOn(BUZZER1);
      button3_pressed = 1;
      tutorial_button3_pressed = 1; // track for tutorial completion
    }
    
    // reset buttons when released
    if (!IsButtonPressed(BUTTON0) && button0_pressed) {
      LedOff(WHITE);
      if (!IsButtonPressed(BUTTON1) && !IsButtonPressed(BUTTON2) && !IsButtonPressed(BUTTON3)) {
        PWMAudioOff(BUZZER1);
      }
      button0_pressed = 0;
    }
    
    if (!IsButtonPressed(BUTTON1) && button1_pressed) {
      LedOff(BLUE);
      if (!IsButtonPressed(BUTTON0) && !IsButtonPressed(BUTTON2) && !IsButtonPressed(BUTTON3)) {
        PWMAudioOff(BUZZER1);
      }
      button1_pressed = 0;
    }
    
    if (!IsButtonPressed(BUTTON2) && button2_pressed) {
      LedOff(YELLOW);
      if (!IsButtonPressed(BUTTON0) && !IsButtonPressed(BUTTON1) && !IsButtonPressed(BUTTON3)) {
        PWMAudioOff(BUZZER1);
      }
      button2_pressed = 0;
    }
    
    if (!IsButtonPressed(BUTTON3) && button3_pressed) {
      LedOff(RED);
      if (!IsButtonPressed(BUTTON0) && !IsButtonPressed(BUTTON1) && !IsButtonPressed(BUTTON2)) {
        PWMAudioOff(BUZZER1);
      }
      button3_pressed = 0;
    }
    
    // check if all buttons have been pressed (tutorial complete)
    if (tutorial_button0_pressed && tutorial_button1_pressed && tutorial_button2_pressed && tutorial_button3_pressed) {
      // start delay timer when all buttons are first detected as pressed
      if (!all_buttons_delay_started) {
        all_buttons_delay_started = 1;
        last_time = G_u32SystemTime1ms;
      }
      
      // proceed after delay
      if (G_u32SystemTime1ms - last_time >= 500) {  // 500ms delay
        // reset for level 1
        LcdCommand(LCD_CLEAR_CMD);
        
        // everything off
        PWMAudioOff(BUZZER1);
        LedOff(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);
        
        // reset for level 1
        button0_pressed = 0;
        button1_pressed = 0;
        button2_pressed = 0;
        button3_pressed = 0;
        
        // reset tutorial tracking
        tutorial_button0_pressed = 0;
        tutorial_button1_pressed = 0;
        tutorial_button2_pressed = 0;
        tutorial_button3_pressed = 0;
        substate = 0;
        all_buttons_delay_started = 0; // reset for potential game restart
        
        // set up first level
        current_pattern = level1_pattern;
        pattern_length = sizeof(level1_pattern) / sizeof(level1_pattern[0]);
        
        // move to level 1
        game_state = 2;
        last_time = G_u32SystemTime1ms;  // reset timer
      }
    }
  }
  
  // GAMEPLAY LEVELS (2-6)
  else if (game_state >= 2 && game_state <= 6) {
    // set the correct pattern for each level
    if (current_pattern == NULL) {
      switch (game_state) {
        case 2:  // Level 1
          current_pattern = level1_pattern;
          pattern_length = 4;  // level length
          break;
        case 3:  // Level 2
          current_pattern = level2_pattern;
          pattern_length = 5;  // level length
          break;
        case 4:  // Level 3
          current_pattern = level3_pattern;
          pattern_length = 6;  // level length
          break;
        case 5:  // Level 4
          current_pattern = level4_pattern;
          pattern_length = 7;  // level length
          break;
        case 6:  // Level 5
          current_pattern = level5_pattern;
          pattern_length = 8;  // level length
          break;
      }
    }
    
    // SUB-STATE MACHINE FOR EACH LEVEL
    
    // SUBSTATE 0: Display level message
    if (substate == 0) {
      LcdCommand(LCD_CLEAR_CMD);
      
      // LCD level message
      if (game_state == 2) {
        LcdMessage(LINE1_START_ADDR, "round 1");
      } else if (game_state == 3) {
        LcdMessage(LINE1_START_ADDR, "round 2");
      } else if (game_state == 4) {
        LcdMessage(LINE1_START_ADDR, "round 3");
      } else if (game_state == 5) {
        LcdMessage(LINE1_START_ADDR, "round 4");
      } else if (game_state == 6) {
        LcdMessage(LINE1_START_ADDR, "round 5");
        LcdMessage(LINE2_START_ADDR, "FINAL BOSS!");
      }
      
      last_time = G_u32SystemTime1ms;
      substate = 1;  // wait state
    }
    
    // SUBSTATE 1: Wait for message to be displayed (non-blocking)
    else if (substate == 1) {
      if (G_u32SystemTime1ms - last_time >= 2000) {  // 2 second delay
        substate = 2;  // move to pattern display
        flash_count = 0;
        last_time = G_u32SystemTime1ms;
      }
    }
    
    // SUBSTATE 2: Display pattern (non-blocking)
    else if (substate == 2) {
      // We need to show the pattern alternating light on/off
      // Each pattern element needs 2 states: ON and OFF
      u8 pattern_step = flash_count / 2;  // Which step in the pattern
      u8 is_on = flash_count % 2 == 0;    // Is the light on or off
      
      // if we've completed the pattern
      if (pattern_step >= pattern_length) {
        // pattern display complete, move to user input
        substate = 3;
        sequence_position = 0;
        button0_pressed = 0;
        button1_pressed = 0;
        button2_pressed = 0;
        button3_pressed = 0;
        
        // turn off any lights/sounds
        PWMAudioOff(BUZZER1);
        LedOff(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);
        
        last_time = G_u32SystemTime1ms;
      } 
      // still displaying the pattern
      else {
        // handle timing (1 second per state)
        if (G_u32SystemTime1ms - last_time >= 1000) {
          last_time = G_u32SystemTime1ms;
          
          // ON state
          if (is_on) {
            // turn on appropriate light/sound based on pattern
            switch(current_pattern[pattern_step]) {
              case 0:  // WHITE
                LedPWM(WHITE, LED_PWM_10);
                PWMAudioSetFrequency(BUZZER1, 500);
                PWMAudioOn(BUZZER1);
                break;
              case 1:  // BLUE
                LedPWM(BLUE, LED_PWM_25);
                PWMAudioSetFrequency(BUZZER1, 392);
                PWMAudioOn(BUZZER1);
                break;
              case 2:  // YELLOW
                LedOn(YELLOW);
                PWMAudioSetFrequency(BUZZER1, 330);
                PWMAudioOn(BUZZER1);
                break;
              case 3:  // RED
                LedOn(RED);
                PWMAudioSetFrequency(BUZZER1, 294);
                PWMAudioOn(BUZZER1);
                break;
            }
          } 
          // OFF state
          else {
            // Turn off all lights/sounds
            PWMAudioOff(BUZZER1);
            LedOff(WHITE);
            LedOff(BLUE);
            LedOff(YELLOW);
            LedOff(RED);
          }
          
          flash_count++;  // Move to next state
        }
      }
    }
    
    // SUBSTATE 3: User input to match the pattern
    else if (substate == 3) {
      // if user has completed the pattern
      if (sequence_position >= pattern_length) {
        // start the delay when the pattern is first completed
        if (!level_complete_delay_started) {
          level_complete_delay_started = 1;
          last_time = G_u32SystemTime1ms;
        }
        
        // proceed after the delay
        if (G_u32SystemTime1ms - last_time >= 500) {  // 500ms delay
          // move to the next level
          game_state++;
          substate = 0;
          current_pattern = NULL;
          pattern_length = 0;  // reset pattern length for next level
          level_complete_delay_started = 0;  // reset for next level
          
          // turn off all lights/sounds
          PWMAudioOff(BUZZER1);
          LedOff(WHITE);
          LedOff(BLUE);
          LedOff(YELLOW);
          LedOff(RED);
          
          // reset sequence position and button tracking for next level
          sequence_position = 0;
          button0_pressed = 0;
          button1_pressed = 0;
          button2_pressed = 0;
          button3_pressed = 0;
          
          // if that was the final level, move to completion
          if (game_state > 6) {
            game_state = 7;  // game complete state
            celebration_count = 0;
            last_time = G_u32SystemTime1ms;
          }
        }
      } 
      // still collecting user input
      else {
        u8 expected_button = current_pattern[sequence_position];
        u8 button_pressed = 0;
        
        // check for button presses and validate against expected sequence
        if (IsButtonPressed(BUTTON0) && !button0_pressed) {
          ButtonAcknowledge(BUTTON0);
          LedPWM(WHITE, LED_PWM_10);
          PWMAudioSetFrequency(BUZZER1, 500);
          PWMAudioOn(BUZZER1);
          button0_pressed = 1;
          button_pressed = 1;
          
          // check if correct button was pressed
          if (expected_button == 0) {
            sequence_position++;
            last_time = G_u32SystemTime1ms;  // reset timer for the success delay
          } else {
            // wrong button - flash error and restart level
            substate = 4;  // error state
            last_time = G_u32SystemTime1ms;
          }
        }
        else if (IsButtonPressed(BUTTON1) && !button1_pressed) {
          ButtonAcknowledge(BUTTON1);
          LedPWM(BLUE, LED_PWM_25);
          PWMAudioSetFrequency(BUZZER1, 392);
          PWMAudioOn(BUZZER1);
          button1_pressed = 1;
          button_pressed = 1;
          
          // check if correct button was pressed
          if (expected_button == 1) {
            sequence_position++;
            last_time = G_u32SystemTime1ms;  // reset timer for the success delay
          } else {
            // wrong button - flash error and restart level
            substate = 4;  // error state
            last_time = G_u32SystemTime1ms;
          }
        }
        else if (IsButtonPressed(BUTTON2) && !button2_pressed) {
          ButtonAcknowledge(BUTTON2);
          LedOn(YELLOW);
          PWMAudioSetFrequency(BUZZER1, 330);
          PWMAudioOn(BUZZER1);
          button2_pressed = 1;
          button_pressed = 1;
          
          // check if correct button was pressed
          if (expected_button == 2) {
            sequence_position++;
            last_time = G_u32SystemTime1ms;  // reset timer for the success delay
          } else {
            // wrong button - flash error and restart level
            substate = 4;  // error state
            last_time = G_u32SystemTime1ms;
          }
        }
        else if (IsButtonPressed(BUTTON3) && !button3_pressed) {
          ButtonAcknowledge(BUTTON3);
          LedOn(RED);
          PWMAudioSetFrequency(BUZZER1, 294);
          PWMAudioOn(BUZZER1);
          button3_pressed = 1;
          button_pressed = 1;
          
          // check if correct button was pressed
          if (expected_button == 3) {
            sequence_position++;
            last_time = G_u32SystemTime1ms;  // reset timer for the success delay
          } else {
            // wrong button - flash error and restart level
            substate = 4;  // error state
            last_time = G_u32SystemTime1ms;
          }
        }
        
        // reset individual buttons when they are released (FIX 2)
        if (!IsButtonPressed(BUTTON0) && button0_pressed) {
          LedOff(WHITE);
          if (!IsButtonPressed(BUTTON1) && !IsButtonPressed(BUTTON2) && !IsButtonPressed(BUTTON3)) {
            PWMAudioOff(BUZZER1);
          }
          button0_pressed = 0;
        }
        
        if (!IsButtonPressed(BUTTON1) && button1_pressed) {
          LedOff(BLUE);
          if (!IsButtonPressed(BUTTON0) && !IsButtonPressed(BUTTON2) && !IsButtonPressed(BUTTON3)) {
            PWMAudioOff(BUZZER1);
          }
          button1_pressed = 0;
        }
        
        if (!IsButtonPressed(BUTTON2) && button2_pressed) {
          LedOff(YELLOW);
          if (!IsButtonPressed(BUTTON0) && !IsButtonPressed(BUTTON1) && !IsButtonPressed(BUTTON3)) {
            PWMAudioOff(BUZZER1);
          }
          button2_pressed = 0;
        }
        
        if (!IsButtonPressed(BUTTON3) && button3_pressed) {
          LedOff(RED);
          if (!IsButtonPressed(BUTTON0) && !IsButtonPressed(BUTTON1) && !IsButtonPressed(BUTTON2)) {
            PWMAudioOff(BUZZER1);
          }
          button3_pressed = 0;
        }
      }
    }
    
    // SUBSTATE 4: Error state
    else if (substate == 4) {
      // flash all LEDs to indicate error (non-blocking)
      uint32_t elapsed = G_u32SystemTime1ms - last_time;
      
      if (elapsed < 500) {
        LedOn(WHITE);
        LedOn(BLUE);
        LedOn(YELLOW);
        LedOn(RED);
        PWMAudioSetFrequency(BUZZER1, 150);
        PWMAudioOn(BUZZER1);
      }
      else if (elapsed < 1000) {
        LedOff(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);
        PWMAudioOff(BUZZER1);
      }
      else if (elapsed < 1500) {
        LedOn(WHITE);
        LedOn(BLUE);
        LedOn(YELLOW);
        LedOn(RED);
        PWMAudioSetFrequency(BUZZER1, 150);
        PWMAudioOn(BUZZER1);
      }
      else if (elapsed < 2000) {
        LedOff(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);
        PWMAudioOff(BUZZER1);
      }
      else {
        // error indication complete, restart level
        substate = 0;
        sequence_position = 0;
        button0_pressed = 0;
        button1_pressed = 0;
        button2_pressed = 0;
        button3_pressed = 0;
      }
    }
  }
  
  // GAME COMPLETE - celebration!!
  else if (game_state == 7) {
    // first display completion message
    if (substate == 0) {
      LcdCommand(LCD_CLEAR_CMD);
      LcdMessage(LINE1_START_ADDR, "GAME COMPLETE!");
      LcdMessage(LINE2_START_ADDR, "(MARIO KINDA)");
      substate = 1;
      last_time = G_u32SystemTime1ms;
    }
    // then do the celebration
    else if (substate == 1) {
  static u32 jingle_step = 0;
  static u32 jingle_last_time = 0;
  
  // first time in this state, initialize the jingle
  if (jingle_step == 0) {
    jingle_last_time = G_u32SystemTime1ms;
    jingle_step = 1;
    
    // turn on all LEDs
    LedPWM(WHITE, LED_PWM_10);
    LedPWM(BLUE, LED_PWM_25);
    LedPWM(CYAN, LED_PWM_30);
    LedPWM(GREEN, LED_PWM_30);
    LedOn(ORANGE);
    LedPWM(PURPLE, LED_PWM_25);
    LedOn(YELLOW);
    LedOn(RED);
  }
  
  // play Mario level clear jingle
  uint32_t current_time = G_u32SystemTime1ms;
  
  // jingle sequence with timings
  switch (jingle_step) {
    case 1: // G4 sixteenth note
      if (current_time - jingle_last_time >= 0) {
        PWMAudioSetFrequency(BUZZER1, G4);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 2: // Ready for C5
      if (current_time - jingle_last_time >= (SN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 3: // Small gap
      if (current_time - jingle_last_time >= RT) {
        PWMAudioSetFrequency(BUZZER1, C5);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 4: // Ready for E5
      if (current_time - jingle_last_time >= (SN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 5: // Small gap
      if (current_time - jingle_last_time >= RT) {
        PWMAudioSetFrequency(BUZZER1, E5);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 6: // Ready for G5
      if (current_time - jingle_last_time >= (SN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 7: // Small gap
      if (current_time - jingle_last_time >= RT) {
        PWMAudioSetFrequency(BUZZER1, G5);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 8: // Ready for C6
      if (current_time - jingle_last_time >= (SN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 9: // Small gap
      if (current_time - jingle_last_time >= RT) {
        PWMAudioSetFrequency(BUZZER1, C6);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 10: // Ready for E6
      if (current_time - jingle_last_time >= (SN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 11: // Small gap
      if (current_time - jingle_last_time >= RT) {
        PWMAudioSetFrequency(BUZZER1, E6);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 12: // Ready for G6
      if (current_time - jingle_last_time >= (SN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 13: // Small gap
      if (current_time - jingle_last_time >= RT) {
        PWMAudioSetFrequency(BUZZER1, G6);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 14: // G6 quarter note (hold longer)
      if (current_time - jingle_last_time >= (QN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 15: // Small gap
      if (current_time - jingle_last_time >= RT) {
        // Short pause (rest)
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 16: // Pause for SN
      if (current_time - jingle_last_time >= SN) {
        PWMAudioSetFrequency(BUZZER1, E6);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 17: // E6 sixteenth note
      if (current_time - jingle_last_time >= (SN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 18: // Small gap
      if (current_time - jingle_last_time >= RT) {
        PWMAudioSetFrequency(BUZZER1, C6);
        PWMAudioOn(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 19: // C6 half note (triumphant finish)
      if (current_time - jingle_last_time >= (HN - RT)) {
        PWMAudioOff(BUZZER1);
        jingle_last_time = current_time;
        jingle_step++;
      }
      break;
      
    case 20: // End of jingle, reset everything
      if (current_time - jingle_last_time >= RT) {
        // Turn off everything
        PWMAudioOff(BUZZER1);
        LedOff(WHITE);
        LedOff(BLUE);
        LedOff(CYAN);
        LedOff(GREEN);
        LedOff(PURPLE);
        LedOff(ORANGE);
        LedOff(YELLOW);
        LedOff(RED);
        // Reset the game
        LcdCommand(LCD_CLEAR_CMD);
        jingle_step = 0;
        button0_pressed = 0;
        button1_pressed = 0;
        button2_pressed = 0;
        button3_pressed = 0;
        substate = 0;
        current_pattern = NULL;
        
        // Return to initial state
        game_state = 0;
      }
      break;
  }
  
  // Reset if any button is pressed (skip the jingle)
  if (IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) || 
      IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3)) {
    // Turn off everything
    PWMAudioOff(BUZZER1);
    LedOff(WHITE);
    LedOff(BLUE);
    LedOff(CYAN);
    LedOff(GREEN);
    LedOff(PURPLE);
    LedOff(ORANGE);
    LedOff(YELLOW);
    LedOff(RED);
    
    // Reset the game
    LcdCommand(LCD_CLEAR_CMD);
    jingle_step = 0;
    button0_pressed = 0;
    button1_pressed = 0;
    button2_pressed = 0;
    button3_pressed = 0;
    substate = 0;
    current_pattern = NULL;
    
    // Return to initial state
    game_state = 0;
  }
}

WATCHDOG_BONE();  // Feed the watchdog once more before exiting
}
}
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
void delay_1s(void)
{
    uint32_t start_time = G_u32SystemTime1ms;
    while ( (G_u32SystemTime1ms - start_time) < 1000 );  // Wait for 1000ms
    WATCHDOG_BONE();  // Feed the watchdog during delay
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/

//to do: turn down led brightness

//biggest issues i ran into:
//-system crash when doing a level sequence:
//fixed by the feeding the watchdog
//-buggy from too many if/else statements
//fixed by using case switchs (easier to go through)
//-lcd wouldnt display level before the level 
// fixed by adding delay in the right spot
//






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
  //   transition_to_lvl = 1;
  // }
      // good interview question: what do you enjoy most about working for garmin/ in this field



  // kainaans code for game march 6
  // after user_app_idle
//   static u8 buttons_pressed_to_begin = 0;
//   static u8 buttons_for_level_message = 0;
//   static u8 message_shown = 0;
//   static u8 button0_pressed = 0;
//   static u8 button1_pressed = 0;
//   static u8 button2_pressed = 0;
//   static u8 button3_pressed = 0;
//   static u8 all_buttons_pressed = 0;
//   static u8 transition_to_lvl = 0;
//   static u8 to_level_1 = 0;
//   static u8 level_1_executed = 0;
//   static u8 level_1_complete = 0;
//   static u8 to_lvl_2 = 0;
//   static u8 level_2_executed = 0;
//   static u8 level_2_complete = 0;
  
//   if (buttons_pressed_to_begin == 0) {
//     if (IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) || IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3)) {
//       LcdCommand(LCD_CLEAR_CMD); // Clear LCD when buttons are pressed
//       //delay_1s();
//       buttons_pressed_to_begin = 1;
//     }
//   }
  
//   if (buttons_pressed_to_begin == 1 && message_shown == 0) { 
//     LcdMessage(LINE1_START_ADDR, "press each button");
//     LcdMessage(LINE2_START_ADDR, "see what it do");
//     message_shown = 1;  // Now it gets set once and persists!
//   }
//     // Handle button presses and LEDs/audio
//     if (WasButtonPressed(BUTTON0)) {
//       ButtonAcknowledge(BUTTON0);
//       LedOn(WHITE);
//       PWMAudioSetFrequency(BUZZER1, 500);
//       button0_pressed = 1;
//     }
//     if (WasButtonPressed(BUTTON1)) {
//       ButtonAcknowledge(BUTTON1);
//       LedOn(BLUE);
//       PWMAudioSetFrequency(BUZZER1, 392);
//       button1_pressed = 1;
//     }
//     if (WasButtonPressed(BUTTON2)) {
//       ButtonAcknowledge(BUTTON2);
//       LedOn(YELLOW);
//       PWMAudioSetFrequency(BUZZER1, 330);
//       button2_pressed = 1;
//     }
//     if (WasButtonPressed(BUTTON3)) {
//       ButtonAcknowledge(BUTTON3);
//       LedOn(RED);
//       PWMAudioSetFrequency(BUZZER1, 294);
//       button3_pressed = 1;
//     }
//     // Update buzzer and LED based on button presses
//     if (IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) || IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3)) {
//       PWMAudioOn(BUZZER1);
//     } 
//     else {
//       PWMAudioOff(BUZZER1);
//       LedOff(WHITE);
//       LedOff(BLUE);
//       LedOff(YELLOW);
//       LedOff(RED);
//     }
  
  
//   // All buttons pressed, set flags for transition
//   if (button0_pressed && button1_pressed && button2_pressed && button3_pressed && !all_buttons_pressed) {
//     all_buttons_pressed = 1;
//     LcdCommand(LCD_CLEAR_CMD);
//     buttons_for_level_message = 2;
//     delay_1s();
//     PWMAudioOff(BUZZER1);
//     LedOff(WHITE);
//     LedOff(BLUE);
//     LedOff(YELLOW);
//     LedOff(RED);
//     //transition_to_lvl = 1; // Set transition flag
//   }

//   if (buttons_for_level_message == 2 && message_shown == 1){
//       LcdMessage(LINE1_START_ADDR, "level 1");
//       message_shown = 2;
//       transition_to_lvl = 1; 
//   }
  
//   // Handle transition to level 1
//    if (transition_to_lvl == 1) {
//   //   LcdMessage(LINE1_START_ADDR, "Level 1");
//   //   delay_1s();
//   to_level_1 = 1;
//   }
  
  
//   // Execute level 1 when the flag is set
//   if (to_level_1 == 1 && level_1_executed == 0) {
//     delay_1s();
//     level_1();  // Execute the sequence
//     level_1_executed = 1; // Set flag to indicate level 1 is done
//   }
  
//   // Handle completion of level 1 and transition to level 2
//   if (level_1_executed) {
//     button0_pressed = 0;
//     button2_pressed = 0;
//     button1_pressed = 0;
//     button3_pressed = 0;
    
//     if (IsButtonPressed(BUTTON0) && !button0_pressed) {
//       ButtonAcknowledge(BUTTON0);
//       button0_pressed = 1;
//     }
    
//     if (IsButtonPressed(BUTTON2) && button0_pressed && !button2_pressed) {
//       ButtonAcknowledge(BUTTON2);
//       button2_pressed = 1;
//     }
    
//     if (IsButtonPressed(BUTTON1) && button2_pressed && !button1_pressed) {
//       ButtonAcknowledge(BUTTON1);
//       button1_pressed = 1;
//     }
    
//     if (IsButtonPressed(BUTTON3) && button1_pressed && !button3_pressed) {
//       ButtonAcknowledge(BUTTON3);
//       button3_pressed = 1;
//     }
    
//     // Check if all buttons in the sequence are pressed
//     if (button0_pressed && button2_pressed && button1_pressed && button3_pressed) {
//       level_1_complete = 1;
//     }
//   }
  
//   // Transition to level 2 after completion of level 1
//   if (level_1_complete) {
//     delay_1s();
//     buttons_for_level_message = 3;
//     PWMAudioOff(BUZZER1);
//     LedOff(WHITE);
//     LedOff(BLUE);
//     LedOff(YELLOW);
//     LedOff(RED);

//   if (buttons_for_level_message == 3 && message_shown == 2){
//       LcdMessage(LINE1_START_ADDR, "level 2");
//       message_shown = 3;
//       transition_to_lvl = 2;
//     }


//     level_1_complete = 0;  
//     button0_pressed = 0;
//     button1_pressed = 0;
//     button2_pressed = 0;
//     button3_pressed = 0;

//     to_lvl_2 = 1;
//     level_2_executed = 0;
//   }

//   static u8 level_2_started = 0;
//   if (to_lvl_2 == 1 && level_2_executed == 0 && !level_2_started){
//       level_2_started = 1;  // Set flag to prevent repeated execution
//       level_2();
//       delay_1s();
//       level_2_executed = 1; 
//   }

//   if (level_2_executed) {
//     button1_pressed = 0;
//     button3_pressed = 0;
//     button2_pressed = 0;
//     button0_pressed = 0;
//     static u8 button1_second_press = 0;

//     if (IsButtonPressed(BUTTON1) && !button1_pressed) {
//         ButtonAcknowledge(BUTTON1);
//         button1_pressed = 1;
//     }

//     if (IsButtonPressed(BUTTON3) && button1_pressed && !button3_pressed) {
//         ButtonAcknowledge(BUTTON3);
//         button3_pressed = 1;
//     }

//     if (IsButtonPressed(BUTTON2) && button3_pressed && !button2_pressed) {
//         ButtonAcknowledge(BUTTON2);
//         button2_pressed = 1;
//     }

//     if (IsButtonPressed(BUTTON0) && button2_pressed && !button0_pressed) {
//         ButtonAcknowledge(BUTTON0);
//         button0_pressed = 1;
//     }

//     if (IsButtonPressed(BUTTON1) && button0_pressed && !button1_second_press) {
//         ButtonAcknowledge(BUTTON1);
//         button1_second_press = 1;
//     }

//     if (button1_pressed && button3_pressed && button2_pressed && button0_pressed && button1_second_press) {
//         level_2_complete = 1;
//     }
// }

// // Display Level 3 Message After Completing Level 2
// if (level_2_complete) {
//   // Turn off all LEDs and buzzer
//   PWMAudioOff(BUZZER1);
//   LedOff(WHITE);
//   LedOff(BLUE);
//   LedOff(YELLOW);
//   LedOff(RED);
  
//   LcdCommand(LCD_CLEAR_CMD);
//   delay_1s();
//   LcdMessage(LINE1_START_ADDR, "level 3");
  
//   // Reset flags for level 3
//   level_2_complete = 0;
//   level_2_started = 0;
// }
// } /* end UserApp1SM_Idle() */
//  void whitelightandbuzz(){
//     LedOn(WHITE);
//     PWMAudioSetFrequency(BUZZER1,500);
//     PWMAudioOn(BUZZER1);
//  }
//   void bluelightandbuzz(){
//     LedOn(BLUE);
//     PWMAudioSetFrequency(BUZZER1,392);
//     PWMAudioOn(BUZZER1);
//  }
//   void yellowlightandbuzz(){
//     LedOn(YELLOW);
//     PWMAudioSetFrequency(BUZZER1,330);
//     PWMAudioOn(BUZZER1);
//  }
//   void redlightandbuzz(){
//     LedOn(RED);
//     PWMAudioSetFrequency(BUZZER1,294);
//     PWMAudioOn(BUZZER1);
//  }
//  void deacwhitelightandbuzz(){
//   LedOff(WHITE);
//   PWMAudioOff(BUZZER1);
//  }
//   void deacbluelightandbuzz(){
//   LedOff(BLUE);
//   PWMAudioOff(BUZZER1);
//  }
//   void deacyellowlightandbuzz(){
//   LedOff(YELLOW);
//   PWMAudioOff(BUZZER1);
//  }
//   void deacredlightandbuzz(){
//   LedOff(RED);
//   PWMAudioOff(BUZZER1);
//  }
//  void delay_non_blocking() {
//   static uint32_t last_time = 0;
//   if (G_u32SystemTime1ms - last_time >= 1000) {
//     last_time = G_u32SystemTime1ms;
//     // Do something after 1s
//   }
// }
// void delay_1s(void)
// {
//     uint32_t start_time = G_u32SystemTime1ms;
//     while ( (G_u32SystemTime1ms - start_time) < 1000 );  // Wait for 1000ms
// }
//  void gameseq_0(){
//   delay_1s();
//   whitelightandbuzz();
//   delay_1s();
//   deacwhitelightandbuzz();
//  }
//  void gameseq_1(){
//   delay_1s();
//   bluelightandbuzz();
//   delay_1s();
//   deacbluelightandbuzz();
//  }
//  void gameseq_2(){
//   delay_1s();
//   yellowlightandbuzz();
//   delay_1s();
//   deacyellowlightandbuzz();
//  }
//  void gameseq_3(){
//   delay_1s();
//   redlightandbuzz();
//   delay_1s();
//   deacredlightandbuzz();
//  }
//  void level_1_message(){
//   LcdCommand(LCD_CLEAR_CMD);
//   delay_1s();
//   LcdMessage(LINE1_START_ADDR, "Level 1");
//  }

// void level_1(){
//   LcdCommand(LCD_CLEAR_CMD);
//   delay_1s();
//   LcdMessage(LINE1_START_ADDR, "Level 1");
//   delay_1s();
//   WATCHDOG_BONE();
//   gameseq_0();
//   WATCHDOG_BONE();
//   gameseq_2();
//   WATCHDOG_BONE();
//   gameseq_1();
//   WATCHDOG_BONE();
//   gameseq_3();
//   WATCHDOG_BONE();
// }
// void level_2(){
//   WATCHDOG_BONE();
//   gameseq_1();
//   WATCHDOG_BONE();
//   gameseq_3();
//   WATCHDOG_BONE();
//   gameseq_2();
//   WATCHDOG_BONE();
//   gameseq_0();
//   WATCHDOG_BONE();
//   gameseq_1();
// }













// working celebration level
 // Then do the celebration
//  else if (substate == 1) {
//   // Flash to celebrate 
//   uint32_t current_time = G_u32SystemTime1ms;
//   if (current_time - last_time >= 500) {  // Toggle every 500ms
//     last_time = current_time;
    
//     if (celebration_count % 2 == 0) {
//       LedPWM(WHITE, LED_PWM_10);
//       LedPWM(BLUE, LED_PWM_25);
//       LedOn(YELLOW);
//       LedOn(RED);
//       PWMAudioSetFrequency(BUZZER1, 800);
//       PWMAudioOn(BUZZER1);
//     } else {
//       LedOff(WHITE);
//       LedOff(BLUE);
//       LedOff(YELLOW);
//       LedOff(RED);
//       PWMAudioOff(BUZZER1);
//     }
    
//     celebration_count++;
    
//     // After 10, reset game
//     if (celebration_count >= 10) {
//       PWMAudioOff(BUZZER1);
//       LedOff(WHITE);
//       LedOff(BLUE);
//       LedOff(YELLOW);
//       LedOff(RED);
      
//       LcdCommand(LCD_CLEAR_CMD);
//       celebration_count = 0;
//       button0_pressed = 0;
//       button1_pressed = 0;
//       button2_pressed = 0;
//       button3_pressed = 0;
//       substate = 0;
//       current_pattern = NULL;
      
//       // Return to initial state
//       game_state = 0;
//     }
//   }
  
//   // Reset if any button is pressed
//   if (IsButtonPressed(BUTTON0) || IsButtonPressed(BUTTON1) || 
//       IsButtonPressed(BUTTON2) || IsButtonPressed(BUTTON3)) {
//     // Turn off everything
//     PWMAudioOff(BUZZER1);
//     LedOff(WHITE);
//     LedOff(BLUE);
//     LedOff(YELLOW);
//     LedOff(RED);
    
//     // Reset the game
//     LcdCommand(LCD_CLEAR_CMD);
//     celebration_count = 0;
//     button0_pressed = 0;
//     button1_pressed = 0;
//     button2_pressed = 0;
//     button3_pressed = 0;
//     substate = 0;
//     current_pattern = NULL;
    
//     // Return to initial state
//     game_state = 0;
//   }
// }
// }

// WATCHDOG_BONE();  // Feed the watchdog once more before exiting
// }