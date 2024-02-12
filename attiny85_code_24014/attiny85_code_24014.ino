
/*
  Fading effect using an EL Wire and a microcontroller

  Operation: This code is designed to control an EL wire to display complex flashing patterns that go well beyond the standard ones built into the inverters.
  We use ATtiny85 microcontroller to perform the timing and the frequency of the ON/OFF pulse patterns generated for achieving the various flashing effects. Timings have been carefully measured via bench tests
  and has been optimized to provide the best visual experience.

  In this code, we provide three compelling flashing patterns which can be enabled by commenting in ONE of the #define MENU_ITEM statements in the code. Following is a brief description of what each pattern does.

  1 - Psychedelic Bounce (enable #define MENU_ITEM 0 statement, compile and flash code)
  This cycles through 7 events that initially start fast flashes moving towards slower flashes over time. After the slower flash, the pattern reverses towards faster flashes.
  This creates a ping pong flash motion between slow and fast flash eventsand is mesmerizing. This cycle continues indefinitely.

  2 - True Chaos (enable #define MENU_ITEM 1 statement, compile and flash code)
  This flash pattern was designed to give the impression that the EL wire is intermittently operational. It creates random patterns of ON and OFF cycles and long OFF periods.
  The interesting part is the patterns are not repeatable between powercycles as the last used random seed is stored on the nonvolatile memory of the microcontroller and advanced between power cycles!
  This little detail creates a true sense of randomness and chaos in the way this flashing pattern operates.

  3 - One Hertz Beat (enable #define MENU_ITEM 2 statement, compile and flash code)
  As the name suggests this pattern generates one EL wire flash per second. The EL wire is kept on for 100 milliseconds during each flash. This flash pattern continues indefinitely.

  4 - Count to Three (enable #define MENU_ITEM 3 statement, compile and flash code)
  This pattern starts with a single flash (i.e., count 1) and then gradually goes up to three. Each count has faster flashes than the previous one.

  Of course, there are countless flashing patterns that can be created using this code and we presented only the above three as examples. You can use this code as a template and add your own to expand the functionality.

  P.S. We have also included a serial monitoring functionality which you can use to debug your code during development. Simply comment in #define SERIAL_DEBUG_ON statement and call serialObj.print("<add your debug string>")
  statement wherever you need to write to serial port.

  by circuitapps
  February 2024
*/

// MENU OF AVAILABLE FLASHING PATTERNS. CHOOSE THE ONE YOU WANT TO SEE, COMPILE THE CODE AND FLASH TO THE ATTINY85 TO SEE THE RESULTS!
//#define MENU_ITEM 0  // Activates Psychedelic Bounce flashing pattern
//#define MENU_ITEM 1  // Activates True Chaos flashing pattern (i.e., pure random flashing behavior)
//#define MENU_ITEM 2  // Activates One Hertz Beat flashing pattern (i.e., one flash every second)
#define MENU_ITEM 3  // Activates Count to Three flashing pattern (i.e., flash period gets smaller with increasing count)

// Why not try and add your own unique flashing patterns ?


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// USERS DO NOT NEED TO MODIFY ANYTHING BELOW THIS LINE. CURIOUS USERS ARE WELCOME TO EXPLORE & EXPERIMENT OF COURSE :) /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <avr/io.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

//#define SERIAL_DEBUG_ON  // if defined, serial port output will be enabled for troubleshooting

#define RANDOM_SEED_EEPROM_ADDRESS 0  // This is where the random number generator seed will be stored on EEPROM

// PORT B DEFINITIONS
#define PB5_PIN1 PB5
#define PB4_PIN3 PB4
#define PB3_PIN2 PB3
#define PB2_PIN7 PB2
#define PB1_PIN6 PB1
#define PB0_PIN5 PB0

#ifdef SERIAL_DEBUG_ON
  // SERIAL PORT PIN ASSIGNMENTS (IF USED)
  #define SERIAL_RX PB3_PIN2  // AtTiny85 Serial Rx pin 
  #define SERIAL_TX PB4_PIN3  // AtTiny85 Serial Tx pin 
#endif

// Application pin definitions
#define ELW_OUT_PIN PB2_PIN7

#ifdef SERIAL_DEBUG_ON
  SoftwareSerial serialObj(SERIAL_RX, SERIAL_TX);
#endif

void turn_on(unsigned int pin_name)
{
  digitalWrite(pin_name, HIGH);
}

void turn_off(unsigned int pin_name)
{
  digitalWrite(pin_name, LOW);
}

void wait_ms(unsigned int delay_ms)
{
  delay(delay_ms);
}

// EL WIRE ILLUMINATION PATTERN
void (*pattern_1_funcs[])(unsigned int) = {&turn_on,  &wait_ms,   &turn_off,   &wait_ms};


///////// VARIOUS FLASHING FUNCTIONS DEFINED BELOW ///////////////////////////////////////

#if MENU_ITEM == 0  // Psychedelic Bounce pattern

  unsigned int *pattern_1_func_args[] = {
                                            (unsigned int[]){ELW_OUT_PIN,       20,         ELW_OUT_PIN,  20},
                                            (unsigned int[]){ELW_OUT_PIN,       30,         ELW_OUT_PIN,  30},
                                            (unsigned int[]){ELW_OUT_PIN,       35,         ELW_OUT_PIN,  35},
                                            (unsigned int[]){ELW_OUT_PIN,       40,         ELW_OUT_PIN,  40},
                                            (unsigned int[]){ELW_OUT_PIN,       45,         ELW_OUT_PIN,  45},
                                            (unsigned int[]){ELW_OUT_PIN,       50,         ELW_OUT_PIN,  50},
                                            (unsigned int[]){ELW_OUT_PIN,       70,         ELW_OUT_PIN,  70}
                                          };

    int number_of_loops[] = {25, 17, 15, 13, 11, 10, 7};  // NEEDS TO HAVE THE SAME NUMBER OF ELEMENTS AS THE NUMBER OF ROWS IN *pattern_1_func_args[]

  void psychedelic_bounce()
  {
    int i, loop_idx, arg_row;
    int num_pattern_1_elements = sizeof(pattern_1_funcs) / sizeof(pattern_1_funcs[0]);
    int num_arg_rows = sizeof(pattern_1_func_args) / sizeof(pattern_1_func_args[0]);

    // SCAN FORWARD
    for( arg_row = 0; arg_row < num_arg_rows; ++arg_row )
    {
      for(loop_idx = 0 ; loop_idx < number_of_loops[arg_row]; ++loop_idx)
      {
        for(i = 0 ; i < num_pattern_1_elements; ++i)
        {// Cycling through each pattern defined by the user.
          pattern_1_funcs[i](pattern_1_func_args[arg_row][i]);  // calling the function via function pointers and the specified function argument.
        }

      }// end loop_idx
    }// end arg_row

    // SCAN BACKWARD
    for( arg_row = num_arg_rows - 1; arg_row >=0 ; --arg_row )
    {
      for(loop_idx = 0 ; loop_idx < number_of_loops[arg_row]; ++loop_idx)
      {
        for(i = 0 ; i < num_pattern_1_elements; ++i)
        {// Cycling through each pattern defined by the user.
          pattern_1_funcs[i](pattern_1_func_args[arg_row][i]);  // calling the function via function pointers and the specified function argument.
        }

      }// end loop_idx
    }// end arg_row

  }

#endif  // Psychedelic Bounce pattern

#if MENU_ITEM == 1  // True Chaos pattern

  void random_seed_manager()
  {
    // This function is meant to keep seeds different between powercycles/resets of the MCU
    int seed = EEPROM.read(RANDOM_SEED_EEPROM_ADDRESS);
    randomSeed(seed);
    seed = (seed + 1) & 0xFF;  // seed is a single byte
    EEPROM.update(RANDOM_SEED_EEPROM_ADDRESS, seed);  // Update random seed for next power cycle
    delay(100);  // wait for write to complete
  }

  void true_chaos()
  {
    unsigned int pattern_1_func_args[] = {ELW_OUT_PIN,       0,         ELW_OUT_PIN,  0};
    unsigned int on_time_ms, off_time_ms, number_of_loops, ms_delay_between_bursts;

    int num_pattern_1_elements = sizeof(pattern_1_funcs) / sizeof(pattern_1_funcs[0]);
    
    while(1)
    {
      on_time_ms = random(10, 50);  // Random EL wire ON duration picked between 10 ms and 50 ms
      off_time_ms = random(10, 50);  // Random EL wire OFF duration picked between 10 ms and 50 ms
      number_of_loops = random(2, 6);  // Random cycle time of EL wire ON and OFF period between 2 and 6 repetitions
      ms_delay_between_bursts = random(300, 1500);  // Interruption between bursts of lighting events between 300 and 1500 ms

      pattern_1_func_args[1] = on_time_ms;
      pattern_1_func_args[3] = off_time_ms;

      for(int j=0 ; j < number_of_loops; ++j)
      {
        for(int i=0 ; i < num_pattern_1_elements; ++i)
          pattern_1_funcs[i](pattern_1_func_args[i]);  // calling the function via function pointers and the specified function argument.
      }

      delay(ms_delay_between_bursts);

    }

  }
#endif  // MENU_ITEM == 1


#if MENU_ITEM == 2  // One Hertz Beat pattern
  void one_hertz_beat()
  {
    unsigned int pattern_1_func_args[] = {ELW_OUT_PIN,       100,         ELW_OUT_PIN,  900};
    int num_pattern_1_elements = sizeof(pattern_1_funcs) / sizeof(pattern_1_funcs[0]);

    while(1)
    {
        for(int i=0 ; i < num_pattern_1_elements; ++i)
          pattern_1_funcs[i](pattern_1_func_args[i]);  // calling the function via function pointers and the specified function argument.
    }

  }
#endif   // MENU_ITEM == 2


#if MENU_ITEM == 3  // Count to Three pattern
    unsigned int *pattern_1_func_args[] = {
                                              (unsigned int[]){ELW_OUT_PIN,       100,         ELW_OUT_PIN,  900},
                                              (unsigned int[]){ELW_OUT_PIN,       100,         ELW_OUT_PIN,  450},
                                              (unsigned int[]){ELW_OUT_PIN,       100,         ELW_OUT_PIN,  450},
                                              (unsigned int[]){ELW_OUT_PIN,       100,         ELW_OUT_PIN,  300},
                                              (unsigned int[]){ELW_OUT_PIN,       100,         ELW_OUT_PIN,  300},
                                              (unsigned int[]){ELW_OUT_PIN,       100,         ELW_OUT_PIN,  300}
                                            };

  void count_to_three()
  {
    int num_pattern_1_elements = sizeof(pattern_1_funcs) / sizeof(pattern_1_funcs[0]);
    int num_pattern_rows = sizeof(pattern_1_func_args) / sizeof(pattern_1_func_args[0]);

    while(1)
    {
        for(int j=0; j<num_pattern_rows ; ++j)
        {
          for(int i=0 ; i < num_pattern_1_elements; ++i)
            pattern_1_funcs[i](pattern_1_func_args[j][i]);  // calling the function via function pointers and the specified function argument.
        }
    }

  }
#endif   // MENU_ITEM == 2

/////////////////////////////////////////////////////////////////////////////////

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(ELW_OUT_PIN, OUTPUT);
  digitalWrite(ELW_OUT_PIN, LOW);

  #if MENU_ITEM == 1  // True Chaos pattern
    random_seed_manager();  // update random seed
  #endif

  #ifdef SERIAL_DEBUG_ON
    serialObj.begin(9600); // send serial data at 9600 bits/sec
    delay(200);
    serialObj.print("Serial debugging enabled");
  #endif
}


void loop() {

  #if MENU_ITEM == 0
    psychedelic_bounce();
  #elif MENU_ITEM == 1
    true_chaos();
  #elif MENU_ITEM == 2
    one_hertz_beat();
  #elif MENU_ITEM == 3
    count_to_three();
  #else
    // Do nothing
  #endif

}
