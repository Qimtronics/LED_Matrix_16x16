/**
 * Copyright (c) 2020 - 2023, LED matrix 16x16
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Qimtronics
 *    integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Qimtronics nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Qimtronics integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY QIMTRONICS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL QIMTRONICS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @file Program LED MATRIX 16x16 pixels
 *
 *
 */

//config hardware
#define BRANCH_CD                       "default"
#define UNIQUE_CD                       "b9d767b4609e_v2_4_0"

//hardware ble enabling by using NimBle library -----------------------------------------
#define BLE_LIB_IS_NIMBLE               1
#define EXCHANGE_DATA_ON_INT_NOT_STR    1
#define RECV_SPECS_CMD                  0

//debuging options is enabled/disabled here ---------------------------------------------
//debugging definition
#define DEBUG_DATA_RAW                  0
#define DISABLE_PROCESSING_RECV_DATA    0
#define DEBUG_WIDGET_RESULT             0
#define DEBUG_SERIAL_ALL_RECEIVED_DATA  0
#define SPECIAL_DEBUG_FOR_ANIMATION     0
#define SPECIAL_DEBUG_FOR_GIF           0
#define CHK_TASK_DURATION_EACH_CORE     1 
#define CHK_AND_DEBUG_PLAYLIST          0

#define CHK_DATA_SAVED_PLAYLIST         0

#define DEBUG_AT_STARTUP
//#define DEBUG_AT_RECEIVE_DATA
//#define DEBUG_AT_GIF    
//TEST_2 
//TEST_3

/*
 * definisi untuk tujuan debugging
 */
//#define DEBUG_PLAYLIST_VIEW 
//#define DEBUG_PLAYLIST_EEPROM
//#define DEBUG_MODE_PLAYLIST
//#define DEBUG_PLAYLIST
//#define DEBUG_PLAYLIST_CHECK_VAL
//#define DEBUG_PLAYLIST_VIEW
//#define DEBUG_PLAYLIST_VIEW
//#define DEBUG_PLAYLIST_VIEW
//#define DEBUG_STOP_SIGN_DATA_MPU_FEATURE
//#define DEBUG_DATA_DUMMY_MPU_FOR_STOP_SIGN 
//#define DEBUG_SEND_DATA_MPU_VIA_BLE_FOR_STOP_SIGN
#define DEBUG_PLAYLIST_VIEW_PLAY

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#if(BLE_LIB_IS_NIMBLE == 1)
  #include <NimBLEDevice.h>
#else
  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
#endif
#include <EEPROM.h>

#include <AnimatedGIF.h> // library decoder GIF

#include <FastLED.h>
#include <String.h>
//#include <SerialFlash.h>
#include <SPI.h>
#include <SPIMemory.h>
#include <Tone32.h>

#include "ESP32TimerInterrupt.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

//#include <esp_task_wdt.h>
#include <EEPROM.h> //untuk simpan ID device (sekaligus ID BLE)

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
//#include <Fonts/FreeSans9pt7b.h>
//#include <Fonts/FreeSerifPlan7pt.h>
#include <Fonts/FreeDialogBold13pt.h>
#include "src/timer_for_games.h"
#include "src/uart_0_drv.h"
/*
 * animation asset ciqa2 header file
 */
#include <ble_disable.h>
#include <ble_enable.h>

#include <ciqa2_aset_bulb_lamp.h>
#include <ciqa2_aset_hamtaro.h>
#include <ciqa2_aset_pacman.h>
#include <ciqa2_aset_rabbit.h>

#ifdef DEBUG_DATA_DUMMY_MPU_FOR_STOP_SIGN
  #include <data_dummy2.h>
#endif


//all defines is here
/*
 * Definisi --------------------------------------------------------------------------------------
 */
#define LED16x16                      1
#define LED8x8                        0
#define Measure_Batt                  1 // 0: disable 1: enable

#define EEPROM_SIZE                   50 
#define DATA_OTA_ON_EEPROM            47

#define version_firm_break            2
#define version_firm_feature          2
#define version_firm_fix              0
#define addres_status_update          45
#define start_data_eeprom_ota         22

//#define posisi_login_status         28 
#define brigthness_addr               15
#define alert_config_addr             16
#define auto_smart_sign_addr          17
#define auto_power_off_addr           18
#define auto_power_time_addr          19
#define kemalingan_status_addr        20
#define interval_time_sign_addr       21
#define score_game_arkanoid_addr      22
#define score_game_snake_addr         24
#define score_game_tetris_addr        26
#define score_game_flappy_bird_addr   28
#define size_data_config              15 //sebelumnya 12

#define SERVICE_SERVICE_UUID               "75604670-1494-4d87-861c-cf183804655a" // UART service UUID
#define CHAR_PAIRING_UUID                  "cb908b68-f6a5-489c-a36c-55ccc847c422" 
#define CHAR_WIDGET_UUID                   "328f77f5-2024-4402-ad55-a365ba2525a1" 
#define CHAR_REMOTE_UUID                   "000799f4-b000-43a7-a06f-b48841cfc792"
#define CHAR_GAMES_UUID                    "c39df3cd-b994-423f-80b0-972d7ce84a93"
#define CHAR_TEXT_UUID                     "7be5c71e-a2b4-4634-88b9-d7e83075e151"
#define CHAR_SETTING_UUID                  "e9609a23-2a36-46d4-8663-9141e846d67f"
#define CHAR_DESIGN_UUID                   "bd04bb51-ec8f-4ac1-b609-e4803e7b058b"
#define CHAR_IMAGE_UUID                    "756a5d94-5339-4a3b-92fe-403b7fff9461"
#define CHAR_ANIMATION_UUID                "3bb93e48-56f9-41f6-8535-f86810529e2c"
#define CHAR_PLAYLIST_UUID                 "f86fd11c-6799-448e-93d3-97f2066c325a"
#define CHAR_UPLOADGIF_UUID                "0c79197a-1711-4811-b4c2-cd0ebaada642"

#define false_val                     0
#define truee_val                     126

#define SEGMENTS                      1               // diodes in one "pixel" (to create a matrix from pieces of tape)
#define COLOR_ORDER                   GRB             // the order of the colors on the ribbon. If the color is displayed incorrectly, change it. You can start with RGB

//config untuk LED matrix saat ini
#define MATRIX_TYPE                   0               // matrix type: 0 - zigzag, 1 - parallel
#define CONNECTION_ANGLE              1       //0        // connection angle: 0 - lower left, 1 - upper left, 2 - upper right, 3 - lower right
#define STRIP_DIRECTION               3       //1        // direction of the tape from the corner: 0 - right, 1 - up, 2 - left, 3 - down

#define vertical_d                    0
#define horizontal_d                  1

#define WIDTH_                        16              // the width of the matrix
#define HEIGHT_                       16              // the height of the matrix
#define NUM_LEDS                      WIDTH_ * HEIGHT_
#define LED_PIN                       13              //13: board yang dicetak, 14: board lubang

#define DEFAULT_BRIGHTNESS           (uint8_t)12   // 25  // untuk testing - kalo untuk produk ready 60 

#define INTERRUPT_PIN                 33
#define delay_sampling_g              10

#define BUZZER_PIN                    14  // buzzer pin untuk module solderan ada di pin 14 sedangkan di pcb
#define BUZZER_PIN_2                  14
#define BUZZER_CHANNEL                0

#define INDEX_GF_ARR                  1
#define LPF_CONSTANTA                 (float) 0.1
#define LPF_OUTPUT_GF                 (float) 0.05
#define SCALE_FOR_ACCEL               16384 //customizable according to accel data when it idle and free no vibration found (FS:2 => 16384)

#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
  #define data_pairing_length         13    
  #define pos_info_len_data           18        
#endif

#if(CHK_TASK_DURATION_EACH_CORE == 1)
  #define CORE_TASK_0_PIN             25
  #define CORE_TASK_1_PIN             26
#endif

#define THREHOLD_VALUE                (float)-0.125

#ifdef JENIS_TAS_SLING_BAG
  #define THREHOLD_VALUE                (float)-0.400
#endif

#define DELAY_BACA_MPU                100
#define WAKTU_BERHENTI                100
#define MIN_VALUE_HAPPEN              WAKTU_BERHENTI/DELAY_BACA_MPU

#define TOTAL_UPDATED_DATA            50
#define TH_VAL_A                      15000
#define TH_VAL_B                      10000
#define TH_VAL_C                      5000
#define ALL_SPIKE_VAL                 2000

#define g_th_value                    1750     
#define g_coor_x                      0
#define g_coor_y                      1
#define g_coor_z                      2
#define g_max_counter                 5
#define g_jmlh_counter_maling         7
#define thr_value                     15

#define ERROR_CODE_CALIB_MPU          (int16_t) 32768  

#define NO_ACTION                     0
#define FORWARD                       1
#define RIGHT                         2
#define LEFT                          3
#define UN_KNOWN                      4
#define REM_STOP                      5

#define MAX_REPEAT_FORWARD_ANIMATION  20
#define MAX_REPEAT_STOP_ANIMATION     6

#define DIAM_TEGAK                    6
#define BERGERAK                      7
#define DIAM_LAYDOWN                  8

#define ON                            1
#define OFF                           0
#define THRESHOLD_DEG                 5
#define STEADY_VAL_DEFAULT            180
#define g_delay_animation             20

#define MAX_val(X36, X18)             (X36 > abs(X18)) ? X36 : X18;           

#define DEFAULT_DURATION_SIGN         20  //dalam satuan waktu detik

#define SPI_SCK                       18              // Serial Clock (Master Output)
#define SPI_MOSI                      23              // Master Output,Slave Input
#define SPI_MISO                      19              // Master Input, Slave Output
#define SPI_CS                        5               // Chip Select, Slave Transmit Enable (active low, Master Output)
#define WP_PIN                        17
#define HOLD_PIN                      16

#define MAX_ADDRESS_MEMORY            0x3FFF00        //look at datasheet -> max 0x3F FF FF -> but give a space FF byte

#define timer0_interval_ms_d           100  // milli seconds
#define timer1_interval_ms_d           10
#define timer2_interval_ms_d           250
#define timer3_interval_ms_d           1000 
#define g_max_timer_counter            5 * (1000/timer2_interval_ms_d)
#define time_batt_notif_status_volt_d  35 //20 detik

#define widget_stop_d                  0
#define widget_clock_d                 1 
#define widget_stopwatch_d             2
#define widget_timer_d                 3
#define widget_scoreboard_d            4

#define remote_off_d                   0 
#define remote_up_d                    1
#define remote_right_d                 2
#define remote_stop_d                  3 
#define remote_left_d                  4

#define remote_no_mode_d               0
#define remote_bike_mode_d             1
#define remote_run_mode_d              2

#define SCORE_SIZE                     0          // размер символов счёта в игре. 0 - маленький для 8х8 (шрифт 3х5), 1 - большой (шрифт 5х7)
#define GLOBAL_COLOR_1                 CRGB::Green   
#define GLOBAL_COLOR_2                 CRGB::Orange   

#define ARKANOID_GAME_EN               1
#define SNAKE_GAME_EN                  1
#define TESTRIS_GAME_EN                1
#define FLAPPY_BIRD_GAME_EN            1

#define games_stop_d                   0
#define games_tetris_d                 1
#define games_snake_d                  2
#define games_flappy_bird_d            3
#define games_arkanoid_d               4

#define games_control_off_d            0
#define games_control_up_d             1 
#define games_control_rigth_d          2
#define games_control_down_d           3
#define games_control_left_d           4 

#define default_speed_animasi_d        2 // (1 - 10)

#define duration_show_img_default      10 //sebelumnya 2500
#define duration_show_animation        20
#define size_buffer_animasi_d           (uint16_t)15360 
#define size_buffer_gif_d               (uint16_t)15360 
#define size_buffer_gambar_d            (uint16_t)768
#define size_buffer_text_d              (uint8_t)25

#define length_data_per_packet_d      175

#define R_color                        0
#define G_color                        1
#define B_color                        2
#define data_text_d                    1
#define data_image_d                   2
#define data_animasi_d                 3
#define data_gif_d                     4
#define data_animasi_default           5
#define addrs_sect_start               (uint32_t)0x000000
#define max_number_playlist_d          5

#define page_memory_size_d              256
#define addres_header_size_eeprom_d     6
#define max_name_playlist_d             10
#define max_frame_animasi               20

#define MAX_GIF_SIZE                    (float)15104.00  

#define off_time_never_d                1
#define off_time_5_mnt_d                5
#define off_time_20_mnt_d               20
#define off_time_35_mnt_d               35
#define off_time_50_mnt_d               50
#define off_time_65_mnt_d               65
#define off_time_80_mnt_d               80
#define off_time_95_mnt_d               95
#define off_time_110_mnt_d              110
#define off_time_125_mnt_d              125
#define off_time_140_mnt_d              140

#define konstanta_konv_ke_detik_d       10
#define time_towakeup_board_d           1 * konstanta_konv_ke_detik_d    // dalam detik
#define time_toenter_sleep_d            2 * konstanta_konv_ke_detik_d
#define time_tocalibrate_mpu_d          4 * konstanta_konv_ke_detik_d
#define time_enter_reset_patter_d       5 * konstanta_konv_ke_detik_d

#define Voltage_5V                      0
#define Voltage_3V3                     1
#define batt_volt_pin_d                34
#define en_boost_5v_pin_d              26 
#define en_supply_3v3_pin_d            27 

const String branch_id    = BRANCH_CD;
const String unique_id    = UNIQUE_CD;
/**
 * Instance --------------------------------------------------------------------------------------
 * 
 */
int8_t SPIPins[4] = {SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS};

MPU6050 mpu;
ESP32Timer h_timer0(0);  
ESP32Timer h_timer1(1);
ESP32Timer h_timer2(2);
ESP32Timer h_timer3(3);
TaskHandle_t Task0;
TaskHandle_t Task1;

AnimatedGIF gif_h;      
//SPIFlash flash_mem;

SPIFlash flash_mem;

/*
 * Defintion of variables ------------------------------------------------------------------
 * 
 */
#ifdef DATA_DUMMY_MPU
  uint16_t idx_data              = 0;
  float *data_dummy              = NULL;
#endif

#if(CHK_TASK_DURATION_EACH_CORE == 1)
  bool  status_core_0            = false;
  bool  status_core_1            = false;
#endif

/**
 * @brief State Machine declaration and definition
 * 
 * 
 * */
typedef enum
{
  ST_BOOT_UP = 0,
  ST_OTA,
  ST_ANIMATION,
  ST_WIDGET,
  ST_GAME,
  ST_REMOTE,
  ST_THIEF_ALARM,
} state_ciqa2;

typedef enum
{
  ST_IDLE = 0,
  ST_OTA_CORE0,
  ST_MPU_CALC,
} state_mpu_ciqa2;

int current_state               = ST_BOOT_UP;
int prev_state                  = ST_BOOT_UP;
int current_state_core0         = ST_IDLE;
int prev_state_core0            = ST_IDLE;

CRGB leds[NUM_LEDS];
//khusus neomatrix
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, WIDTH_, HEIGHT_, 1, 1, 
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
    NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG );

/*
 * definisi dan variable EEPROM
 * -baca ID BLE
 * -menyimpan hasil config sebelumnya
 */			   
const uint8_t start_addres_ID   = 0;
std::string ID_device_g         = "";
     
uint8_t brightness_level        = 0;
uint8_t settings_config[size_data_config];    

#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
  uint8_t pairing_buffer[data_pairing_length]     = {};		  
#else
  String pairing_buffer           = "";           
#endif

uint8_t buff_universal_recv[15368];

/*
 * -----------------------------definisi dan variable BLE ---------------------------------//
 */
#if(BLE_LIB_IS_NIMBLE == 1)
  NimBLEServer         *pServer                     = NULL;
  NimBLECharacteristic *p_pairing_characteristic    = NULL;
  NimBLECharacteristic *p_widget_characteristic     = NULL;
  NimBLECharacteristic *p_remote_characteristic     = NULL;
  NimBLECharacteristic *p_games_characteristic      = NULL;
  NimBLECharacteristic *p_text_characteristic       = NULL;
  NimBLECharacteristic *p_setting_characteristic    = NULL;
  NimBLECharacteristic *p_design_characteristic     = NULL;
  NimBLECharacteristic *p_image_characteristic      = NULL;
  NimBLECharacteristic *p_animation_characteristic  = NULL;
  NimBLECharacteristic *p_playlist_characteristic   = NULL;
  NimBLECharacteristic *p_uploadGif_characteristic  = NULL;
#else
  BLEServer         *pServer                     = NULL;
  BLECharacteristic *p_pairing_characteristic    = NULL;
  BLECharacteristic *p_widget_characteristic     = NULL;
  BLECharacteristic *p_remote_characteristic     = NULL;
  BLECharacteristic *p_games_characteristic      = NULL;
  BLECharacteristic *p_text_characteristic       = NULL;
  BLECharacteristic *p_setting_characteristic    = NULL;
  BLECharacteristic *p_design_characteristic     = NULL;
  BLECharacteristic *p_image_characteristic      = NULL;
  BLECharacteristic *p_animation_characteristic  = NULL;
  BLECharacteristic *p_playlist_characteristic   = NULL;
  BLECharacteristic *p_uploadGif_characteristic  = NULL;
#endif
/*
 * -----------------------------------definisi utility -----------------------------------//
 */

volatile bool mode_remote_enabled    = false;

uint8_t mode_display_led_g           = ST_ANIMATION;
uint8_t prev_mode_display_g          = ST_ANIMATION;
uint8_t brightness_level_g           = DEFAULT_BRIGHTNESS;
    
//const uint16_t text_colors[] = {matrix->Color(255, 255, 0)};
const uint8_t color_table_global_g[8][3] = {{0x00,0x00,0x00}, //hitam
                                            {0x00,0xFF,0x00}, //hijau
                                            {0x00,0x00,0xFF}, //biru
                                            {0xFF,0x00,0x00}, //merah
                                            {0xFF,0xFF,0x00}, //kuning
                                            {0xFF,0x00,0xFF}, //ungu
                                            {0xCC,0xCC,0xCC}, //array ke-7 untuk custom
                                            {0xCC,0xCC,0xCC}};//putih

#if (LED16x16 == 1)
const uint8_t ArrowRight[256] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t ArrowLeft[256] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t ArrowUp[256] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00,
  0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00,
  0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t alert_stop_maling[256] = 
{
  0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00,
  0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00,
  0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03,
  0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03,
  0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03,
  0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00,
  0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00,
  0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00
};
#else
const uint8_t ArrowRight[64]={
0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00,  
0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};

const uint8_t ArrowLeft[64]{
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00
};

const uint8_t ArrowUp[64]={
0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00
};

const uint8_t alert_stop_maling[64]={
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};
#endif

                                            
/*
 * -------------------------------definisi dan variable MPU--------------------------------//
 */
float f_ax                            = 0.0;
float f_ay                            = 0.0;
float f_az                            = 0.0;
float only_z_axis                     = 0.0;
float resultan_gf                     = 0.0;
float result_gforce[INDEX_GF_ARR]     = {0.0};
float result_gf_average               = 0.0;
float result_total_gf                 = 0.0;
float prev_result_total_gf            = 0.0;

float threshold_value_var             = THREHOLD_VALUE;

uint8_t min_val_happen_var            = MIN_VALUE_HAPPEN;
bool sudden_spike                     = false;
float offset_calibration              = 0.00;
uint16_t count_val_above_gf            = 0;
uint16_t count_val_below_gf            = 0;
float gfx, gfy, gfz;

Quaternion qrtn;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

uint8_t fifoBuffer[64];

int16_t offset_accel[3];
int16_t offset_gyro[3];

int16_t ax, ay, az;
int16_t gx, gy, gz;

bool save_calibration_data_b          = false;
volatile bool setting_receive_b       = false;
volatile bool mode_kemalingan_b       = false;
volatile bool automatic_sign_b        = false;
volatile bool mode_safety_enabled_b   = false;
volatile bool mpuInterrupt_g          = false;     // indicates whether MPU interrupt pin has gone high
bool dmpReady_b                       = false;
bool decision_diproses_b              = false;
//uint8_t b_timer2_active               = false;
volatile bool b_timer2_active         = false;
bool timer2_enabled_for_safety        = false;
bool prev_sign_stop                   = false;

const uint8_t TOTAL_SEGMENT           = 2;
const uint8_t TOTAL_TH                = 4;

bool sampling_data_mpu_b              = false;
int16_t g_sudut_kemiringan            = 180;
uint16_t *tot_spike;
float spike_X_percentA                = 0.0;
float spike_Y_percentA                = 0.0;
float spike_Z_percentA                = 0.0;
float *spike_percent;



const int16_t threshold_value[TOTAL_TH]     = {ALL_SPIKE_VAL, TH_VAL_C, TH_VAL_B, TH_VAL_A};

//----------------------------------------------------------------------------------------------------------------

int16_t g_mean_val_x  [g_max_counter];
int16_t g_mean_val_y  [g_max_counter];
int16_t g_mean_val_z  [g_max_counter];

int8_t  g_counter                       = 0;
uint8_t g_jmlh_spike_x                  = 0;
uint8_t g_jmlh_spike_y                  = 0;
uint8_t g_jmlh_spike_z                  = 0;
uint8_t g_cek_maling_counter            = 0;

uint16_t g_jmlh_spike                   = 0;

int16_t g_value_th_top[3]               = {0,0,0};
int16_t g_value_th_btm[3]               = {0,0,0};
int16_t g_mean_value  [3]               = {0,0,0};
bool    b_determine_stable_value        = true;
bool    b_deteksi_dipindahkan           = false;
uint16_t buf_idx_mpu                    = 0;
												   
volatile bool mpu_calibrated_b          = true; //set to false 
volatile bool command_mpu_calib_b       = false;

const uint8_t g_data_persegment = (TOTAL_UPDATED_DATA/TOTAL_SEGMENT);

int16_t data_accx_th[TOTAL_UPDATED_DATA];
int16_t data_accy_th[TOTAL_UPDATED_DATA];
int16_t data_accz_th[TOTAL_UPDATED_DATA];

//uint8_t spike_totalx[TOTAL_SEGMENT][TOTAL_TH+1];
//uint8_t spike_totaly[TOTAL_SEGMENT][TOTAL_TH+1];
//uint8_t spike_totalz[TOTAL_SEGMENT][TOTAL_TH+1];
//
//uint16_t All_spike_X[TOTAL_TH+1];
//uint16_t All_spike_Y[TOTAL_TH+1];
//uint16_t All_spike_Z[TOTAL_TH+1];

int16_t accx_seg[TOTAL_SEGMENT][TOTAL_UPDATED_DATA/TOTAL_SEGMENT];
int16_t accy_seg[TOTAL_SEGMENT][TOTAL_UPDATED_DATA/TOTAL_SEGMENT];
int16_t accz_seg[TOTAL_SEGMENT][TOTAL_UPDATED_DATA/TOTAL_SEGMENT];
float   submitted_ypr[TOTAL_UPDATED_DATA];

//determinasi MPU arah jalan

uint16_t STEADY_VAL_MIN       = STEADY_VAL_DEFAULT;

//uint8_t orientation, animation;

uint8_t DIRECTION             = NO_ACTION;
uint8_t SIGN_ANIMATION        = ON;
uint8_t durasi_sign_g         = DEFAULT_DURATION_SIGN;
uint16_t interval_ticks_sign  = (uint16_t)(durasi_sign_g * 0.75);

uint8_t buf_led[HEIGHT_ * WIDTH_];

int8_t  g_frame               = 0;
bool b_blank_array            = false;
uint8_t g_repeat_animation    = 0;
uint8_t g_next_part_animation = 0;
uint8_t g_xpos                = 0;
uint8_t g_ypos                = 0;
uint8_t prev_direction        = NO_ACTION;
/*
 * ----------------------------definisi dan variables SPI Memori -------------------------//
 */

const uint8_t   used_eeprom_id_g[3]       = {0xEF, 0x40, 0x16};
uint16_t total_used_sector_g         = 0;
 /*
 * --------------------------definisi dan variable timer---------------------------------//
 */
portMUX_TYPE timerMux_mpu = portMUX_INITIALIZER_UNLOCKED;

bool timer3_aktif_b                   = false;
volatile uint16_t timer0_tick_g                = 0;
volatile uint16_t timer0_counter_vsupply_status_g = 0;
volatile uint32_t timer0_counter_power_off_g   = 0;
volatile uint16_t timer0_notify_hw_status_g    = 0;
volatile uint16_t timer2_tick_g                = 0;
volatile uint16_t timer3_tick_g                = 0;
volatile uint16_t timer3_counter_power_off_g   = 0;

/*
 * -----------------------------definisi dan variable widget ----------------------------//
 */ 
uint8_t tipe_widget_g                = widget_stop_d;
uint8_t prev_tipe_widget_g           = widget_stop_d;
int8_t time_seconds_g                = 0;
int8_t time_minutes_g                = 0;
int8_t time_hours_g                  = 0;
int8_t time_m_timer_g                = 0;
int8_t time_s_timer_g                = 0;

uint8_t score_tim_a_g                = 0;
uint8_t score_tim_b_g                = 0;
uint8_t prev_score_a_g               = 0;
uint8_t prev_score_b_g               = 0;
bool ganti_tampilan_waktu_b          = false;
bool new_entry_b                     = false;
/*
 * -----------------------------definisi dan variable remote ----------------------------//
 */
uint8_t remote_sign_direction_g       = remote_off_d;
uint8_t remote_mode_g                 = remote_no_mode_d;
uint8_t prev_sign_stop_or_fwd         = remote_no_mode_d;

volatile uint8_t notif_kemalingan     = 0;
bool alert_alarm_sign_b               = false;
bool auto_stop_sign_b                 = false;
bool play_timer_b                     = false;

/*
 * -----------------------------definisi dan variable games ----------------------------//
 */ 
uint8_t games_mode_g                  = games_stop_d;
uint8_t control_games_g               = games_control_off_d;
uint8_t prev_games_mode_g             = games_control_off_d;
bool got_new_command_g                = false;
bool loadingFlag_b                    = false;

uint16_t high_score_arkanoid          = 0;
uint16_t high_score_snake             = 0;
uint16_t high_score_tetris            = 0;
uint16_t high_score_flappy_bird       = 0;
/*
 * ----------------------definisi dan variable animasi text ----------------------------//
 */ 
char text_animasi_g[20];
char text_info_animasi_g[20];
uint8_t speed_animasi_text_g         = default_speed_animasi_d;
uint8_t text_color_rgb_g[4]          = {0x00, 0xFF, 0x00, 0x00};
bool show_text_animation_b           = false;
uint8_t total_pixel_led_used_g       = 0;
int16_t cursor_x_text_g              = WIDTH_;
int16_t cursor_y_text_g              = 12;


/*
 * ------------------definisi dan variable animasi, gambar, design----------------------//
 */ 
char buf_receive_ble[length_data_per_packet_d];
bool flag_parse_and_show_b           = true;

// read data from eeprom
volatile uint8_t frame_seq_receive_g          = 0;
volatile uint8_t length_data_packet_g         = 0;
volatile uint8_t sequence_kirim_data_g      = 0;
volatile uint8_t total_frame_animasi_g        = 0;
uint8_t total_frame_dr_eeprom_g      = 0;
volatile uint8_t receive_complete_g        = 0;
volatile bool receive_failed_b             = false;
uint8_t fps_animasi_received_data_g  = 0;
uint8_t fps_animasi_eeprom_g         = 0;
uint8_t frame_num_playlist_g         = 0;
uint8_t repeat_frame_animasi         = 0;
//variable for streaming design
bool design_stream_b                 = false;
uint16_t led_array_idx_g             = 0;
uint8_t deisgn_color_rgb_g[4]        = {0x00, 0x00, 0x00, 0x00};
uint16_t idx_buf                     = 0;

bool failed_flag_b                   = false;
volatile bool notify_progress_b      = false;
volatile uint8_t progres_status      = 0;
/*
 * ----------------------definisi dan variable general playlist--------------------------//
 */
//------General Utilities-------------------------------------------------------------------//
uint16_t total_max_data_est           = 0;
uint8_t len_last_gif_msg              = 0;
uint8_t image_repeated_show_d         = 0;
bool play_animasi_playlist_g          = true;
uint8_t playlist_seq                  = 0;
bool still_send_notify_seq_b          = false;
volatile bool receive_design_not_image_b       = false;
volatile bool gif_show_enable         = false;
bool design_not_img_eeprom_b          = false;
uint8_t type_data_to_save_g           = data_text_d;

uint16_t xcount_mem                   = 0;
uint16_t ycount_mem                   = 0;
uint8_t tipe_data_received_g          = data_text_d;
uint8_t total_playlist_g              = 0;
uint8_t urutan_playlist_g             = 0;
uint8_t prev_playlist_g               = 0;     
//uint8_t frame_num_playlist_g        = 0;  
uint8_t repeat_animasi_image_g        = 0; 
volatile bool got_sequence_data_playlist_b     = false;
volatile bool flash_on_read_process   = false;
volatile bool save_to_playlist_b   = false;
bool clear_eeprom_b                   = false;
uint8_t *collected_received_data_g;

bool first_time_receive_design_b      = true;
bool next_playlist_animation_b        = true; 

volatile uint16_t data_size_animasi_g  = 0 ;
uint16_t data_size_animasi_eeprom      = 0 ;

// uint8_t *buff_img; //untuk terima data dari handphone
// uint8_t *buff_animasi;
// uint8_t *buff_text;

// volatile uint8_t buff_img[768];                            //untuk terima data dari handphone
// volatile uint8_t buff_animasi[15360];
// volatile uint8_t buff_text[25];

//uint8_t buff_universal_recv[15368];

// uint8_t *buff_imgEEPROM;                      //untuk terima data eeoprom
// uint8_t *buff_animasiEEPROM;                // ada tambahan utk fps dan jumlah frame
// uint8_t *buff_textEEPROM;

uint8_t buff_imgEEPROM[768];                      //untuk terima data eeoprom
uint8_t buff_animasiEEPROM[15368];                // ada tambahan utk fps dan jumlah frame
uint8_t buff_textEEPROM[25];

//BUG animasi put on first declaration
// uint8_t buff_img[768];                            //untuk terima data dari handphone
// uint8_t buff_animasi[15360];
// uint8_t buff_text[25];

const uint8_t header_file_eeprom_g[3]   = {'Q','I','M'};
uint8_t bufs_for_check_and_save_g[256];
uint8_t amount_of_playlist_show_g       = 0;
uint32_t mem_addre_current_g            = 0x00000000;
uint32_t mem_addre_playlist_g           = 0x00000000;
volatile uint8_t type_data_to_show_g    = data_animasi_default;
uint8_t prev_type_data_to_show_g        = data_animasi_default;
bool packet_parse_b                     = false;
bool packet_show_b                      = false;
uint8_t next_animation_g                = 0;

volatile bool first_playlist_b          = false;

/*
 * 
 * ------------------------definisi dan variable upload data GIF--------------------------//
 */ 
uint8_t times_sent                    = 0;  
bool block_rest_of_gif_file_b         = false;
bool this_is_gif_header_b             = false;
uint16_t size_gif_file_received       = 0;
uint16_t size_animasi_gif_eeprom      = 0;

bool file_gif_opened              = false;          
//uint8_t buffer_GIF_EEPROM[15368]  = {0};
char last_byte_for_next_g 			  = 0;
uint16_t decript_pos_g    			  = 0;
uint16_t frame_pos_g      			  = 0;
bool gif_file_is_end_b    			  = false;
uint8_t len_stream_data_g 			  = 0;
uint8_t animation_gif_repeat 		  = 0;


/*
 * -----------------------definisi dan variable setup led -----------------------------//
 */ 
bool auto_power_off_led_b             = false;
uint8_t auto_power_off_time_g         = off_time_35_mnt_d;
bool erase_eeprom_b                   = false;
bool device_connected_b               = false;
bool apps_connected_b                 = false;
bool show_status_connetion            = false;											  
uint8_t count_conn_device             = 0;

uint8_t len_special_cmd_via_serial    = 0;

/*
 * 
 * ------------------------definisi dan variable general setup--------------------------//
 */
esp_reset_reason_t reset_reason_g;

 /*
 * ---------------------definisi dan variable batt & pin touch pin------------------------//
 */

volatile bool device_shutdown_b       = false;
bool sleep_status_b                   = false;
bool please_reset_me_b                = false;                   

float batt_voltage_g                  = 0.0;

#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
  uint8_t buf_send_notify_g[3];
#else
  char buf_send_notify_g[5];
#endif

volatile uint8_t count_failed_to_receive_g     = 0;
uint8_t receive_data_b                = truee_val;
bool    notify_received_data_b        = false;
volatile bool    frist_get_b          = false;

extern "C" int rom_phy_get_vdd33();

/**
 * @brief Declaration and definition all functions
 * 
 * 
 * */
int boot_up_and_setup(int curr_state);
int animation_show(int curr_state);
int widget_show(int curr_state);
int game_show(int curr_state);
int remote_show(int curr_state);
int thief_alarm(int curr_state);
int(*func_state_ptr[7])(int) = { boot_up_and_setup, \
                            animation_show, \
                            widget_show, \
                            game_show, \
                            remote_show, \
                            thief_alarm \
                          };

int idle_core_0(int curr_state);
int mpu_calc(int curr_state);
int(*func_state_ptr_core0[2])(int)  = { idle_core_0, \
                                   mpu_calc \
                                 };


bool check_header_gif(uint8_t *bytes);
void close_gif(uint8_t mode_display, uint8_t type_data_recv);
void show_animation_GIF();

void IRAM_ATTR timer0_handler(void);
void IRAM_ATTR timer1_handler(void);
void IRAM_ATTR timer2_handler(void);
void IRAM_ATTR timer3_handler(void);

void save_eeprom_ota_status(bool succes);
void tone_timer_end(uint8_t repeat);

void receive_text();
void receive_image();
void receive_animasi();
void receive_GIF();
bool check_incoming_gif_header(char* buf_src, uint8_t start_idx);

void collect_data(uint8_t seq, char *buf_src, uint8_t *buf_dst, uint8_t len, uint8_t tipe_data);
void get_accel_gyro_buff(uint8_t idx);
void split_into_segment(int16_t *data_full, int16_t *segmented_data);
void predict_action();
void clear_LED(uint8_t width_led, uint8_t height_led);
void ESP32_wake_up();

uint8_t rotate_image(uint8_t val, int8_t rotation);
uint8_t flip_image(uint8_t val, uint8_t orientation);

void calibrate_mpu();

void serial_config();
void  ble_config();
void  flash_memory_config();
void  led_matrix_config();
void  mpu_config();
void  timer_config();
void  gpio_config();
void eeprom_config();

void enable_supply(uint8_t supply_type, bool yes);

uint16_t getPixelNumber(int8_t x, int8_t y);
uint16_t getPixelNumber_GIFframe(int8_t x, int8_t y); 
uint16_t getPixelNumber_IMGframe(int8_t x, int8_t y);

void tone_alarm_maling_blink_on(bool led_displayed_b); 
void ESP32_start_sleep();

//void memset_pointer(volatile uint8_t* p, uint8_t desire_res_val, uint16_t size);
uint16_t size_(uint8_t tipe_data);
void protect_eeprom_memory(bool b_yes_or_no);
uint32_t check_available_addr_memory(uint32_t mem_addr);
void read_playlist();
void delete_files_from_playlist();
void show_animation(uint8_t jenis_data, uint8_t *buf_to_show);
uint16_t size_sectore_used_d(uint8_t tipe_data_);
void add_file_to_playlist(uint32_t *addrs, volatile uint8_t *data_buf);
void get_buffer_data(uint32_t  *address_mem, uint8_t *buffered_data, uint8_t size_sector_toread);

bool check_available_sector_size(uint32_t *current_address, uint8_t type_data);
void loadImage(const uint16_t (*frame));

void decode_animasi(uint8_t *buf);
void decode_text(uint8_t *buf);

void action_base_on_prediction();

void widget_clock();
void widget_stopwatch();
void widget_timer();
void widget_score_board();

void snakeRoutine();
void tetrisRoutine();
void flappyRoutine();
void arkanoidRoutine();

String sendback_notify_value(uint8_t *yang_sedang_tampil);
void set_sign_animation(uint8_t *prev_dir, uint8_t *new_dir);

void touch_sensor_option();
void tone_alarm_maling_blink_on();
void tone_warning();
void tone_mode_safety();
void tone_mode_setting();
void tone_game_control();
void tone_game_over();

//additional
bool forward_has_over_b     = false;

std::string rxValue_g;
bool get_init_pairing_b  = false;
uint8_t pairing_timeout  = 0;

/**
 * @brief void setup functions
 * 
 */
void setup() 
{
  gpio_config();
  eeprom_config();
  serial_config(); 
  flash_memory_config();
  led_matrix_config();
  mpu_config();
  get_device_config(); 

  current_state = (*func_state_ptr[ST_BOOT_UP])(ST_BOOT_UP);

  timer_config();    
  
  enable_supply(Voltage_3V3, true);
  
  //delete_files_from_playlist();
  //deteksi penyebab reset hardware jika karena backtrace akan hapus semua playlist
  reset_reason_g = esp_reset_reason();

  //if exception handler on esp32 occur then we erase all data in memory back to defualt animation
  if(reset_reason_g == ESP_RST_PANIC)
  {
      protect_eeprom_memory(false);
      

      Serial.println("[INFO] EEPROM ERASE PROCESS...");

      unsigned long dotMillis = millis();
      if (flash_mem.eraseChip()) 
      {
        Serial.println("[INFO] EEPROM ERASED :");
        Serial.print("[INFO] time needed (ms): ");
        Serial.println(millis() - dotMillis);
      }
      else
      {
        Serial.println("[INFO] EEPROM CANT BE ERASED");
      }
      protect_eeprom_memory(true);
  }

  //playlist setup untuk mendapatkan ketersediaan playlist dan alamat memori terakhir untuk simpan data  
  Serial.print("[INFO] CURR MEM: ");
  Serial.println(mem_addre_current_g);
  protect_eeprom_memory(false);
  mem_addre_current_g = check_available_addr_memory(addrs_sect_start);
  protect_eeprom_memory(true);

  if(mem_addre_current_g != 0)
  {
    play_animasi_playlist_g = true;
  }
  else
  {
    play_animasi_playlist_g = false;
  }
  
  Serial.print("[INFO] CURR MEM: ");
  Serial.println(mem_addre_current_g);
  
  h_timer1.restartTimer();      //timer sampling MPU
  //h_timer2.restartTimer();
  //Serial.println("[INFO] Timer1 started . . . ");
  timer0_tick_g = 0;
  
  Serial.print("[INFO] HMSpace: ");
  Serial.println(ESP.getFreeHeap());

  #if(CHK_TASK_DURATION_EACH_CORE == 1)
    pinMode(CORE_TASK_0_PIN, OUTPUT);
    pinMode(CORE_TASK_1_PIN, OUTPUT);
    status_core_0  = false;
    status_core_1  = false;
    digitalWrite(CORE_TASK_0_PIN, status_core_0);
    digitalWrite(CORE_TASK_1_PIN, status_core_1);
  #endif

  Serial.println("-------------------------------------------------");
  Serial.print("[INFO]ID : ");
  Serial.println(ID_device_g.c_str());
  Serial.print("[INFO] HM: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("[INFO] FB: ");
  Serial.println(heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));  

  Serial.println("-------------------------------------------------");
  vTaskDelay(50);

  if(mode_kemalingan_b)
  {
    current_state = ST_THIEF_ALARM;
  }

  //task untuk core 0 proses data MPU  //task dengan nilai tertinggi memiliki prioritas yang tinggi dalam pengerjaan
  xTaskCreatePinnedToCore(
                  Task1code,   /* Task function. */
                  "LED dan BLE",     /* name of task. */
                  6400,       /* Stack size of task */
                  NULL,        /* parameter of the task */
                  10,           /* priority of the task */
                  &Task1,      /* Task handle to keep track of created task */
                  1);          /* pin task to core 0 */   
  
  xTaskCreatePinnedToCore(
                    Task0code_pairing,   /* Task function. */
                    "dev pairing",     /* name of task. */
                    6400,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    5,           /* priority of the task */
                    &Task0,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */ 
                    
  vTaskDelay(500);
}

/**
 * @brief task 0 code
 * 
 * this code is specialize on processing mpu data
 * 
 */
void Task0code_pairing( void * pvParameters )
{
  for(;;)
  {
    if(!device_connected_b && apps_connected_b)
    {   
        vTaskDelay(300);
        apps_connected_b                    = device_connected_b;
        pServer->startAdvertising();

        if(mode_kemalingan_b)
        {
          mode_display_led_g                  = ST_THIEF_ALARM;
        }
        else
        {
          mode_display_led_g                  = ST_ANIMATION;
        }

        show_status_connetion               = true;
        clear_LED(WIDTH_, HEIGHT_);
        
        vTaskDelay(100);
        
        loadImage(ble_disable_array[0]);
        FastLED.show();
        vTaskDelay(1000);
        clear_LED(WIDTH_, HEIGHT_);
        show_status_connetion               = false;
    }
    
    //pairing process with apps
    if(count_conn_device < 1)
    {
      if(device_connected_b && !apps_connected_b)
      {

          vTaskDelay(300);
          apps_connected_b                    = device_connected_b;
          
        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
          p_pairing_characteristic->setValue(pairing_buffer, data_pairing_length); 
        #else
          p_pairing_characteristic->setValue(std::string(pairing_buffer.c_str()));
        #endif
      
        #if(BLE_LIB_IS_NIMBLE == 1)
            p_widget_characteristic->notify();
        #endif 

          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
            for(uint8_t x = 0; x < data_pairing_length; x++)
            {
              Serial.print(pairing_buffer[x]);
              Serial.print(" ");
            }            
            Serial.println("");
          #else
            Serial.println(pairing_buffer.c_str());
          #endif
          timer0_counter_power_off_g          = 0;
          
          show_status_connetion               = true;
          clear_LED(WIDTH_, HEIGHT_);
          vTaskDelay(100);
          loadImage(ble_enable_array[0]);
          FastLED.show();
          vTaskDelay(1000);
          clear_LED(WIDTH_, HEIGHT_);
          show_status_connetion               = false; 
      }
    }
    else
    {
      if(device_connected_b && apps_connected_b)
      {
  //        Serial.print("pairing2 pServer->getConnectedCount: ");
  //        Serial.println(pServer->getConnectedCount());
        
        p_pairing_characteristic->setValue("UNKNOWN");
        #if(BLE_LIB_IS_NIMBLE == 1)
            p_widget_characteristic->notify();
        #endif 
        Serial.println("UNKNOWN");
        vTaskDelay(50);
        //apps_connected_b                    = device_connected_b;

      }
    }
  
    vTaskDelay(1000);
  }  
}

//void Task0code( void * pvParameters )
//{
//  //esp_task_wdt_init(10, false);
//  
//  for(;;)
//  {
//    /*
//    //unpair with apps
//    if(setting_receive_b)        
//    {
//      tone_mode_setting();
//      setting_receive_b       = false;
//    }
//
//    if(notify_progress_b)
//    {
//      if(device_connected_b && apps_connected_b)     
//      {
//          memset(buf_send_notify_g, 0, sizeof(buf_send_notify_g));
//          
//          if(progres_status >= 100)
//          { 
//            progres_status = 99;
//          }
//
//          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
//            buf_send_notify_g[0] = '%';
//            buf_send_notify_g[1] = progres_status;
//
//            Serial.print(buf_send_notify_g[0]);
//            Serial.print("-");
//            Serial.println(buf_send_notify_g[1]);
//          #endif
//          
//          //Serial.println(buf_send_notify_g);        
//          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)     
//            p_widget_characteristic->setValue(&buf_send_notify_g[0], 2);
//          #else
//            p_widget_characteristic->setValue(buf_send_notify_g);
//          #endif
//
//          #if(BLE_LIB_IS_NIMBLE == 1)
//              p_widget_characteristic->notify(true);
//          #else
//              p_widget_characteristic->notify();
//          #endif
//          
//          notify_progress_b       = false;
//      }
//    }
//    
//    if(!device_connected_b && apps_connected_b)
//    {   
//        vTaskDelay(300);
//        apps_connected_b                    = device_connected_b;
//        pServer->startAdvertising();
//
//        if(mode_kemalingan_b)
//        {
//          mode_display_led_g                  = ST_THIEF_ALARM;
//        }
//        else
//        {
//          mode_display_led_g                  = ST_ANIMATION;
//        }
//
//        show_status_connetion               = true;
//        clear_LED(WIDTH_, HEIGHT_);
//        
//        vTaskDelay(100);
//        
//        loadImage(ble_disable_array[0]);
//        FastLED.show();
//        vTaskDelay(1000);
//        clear_LED(WIDTH_, HEIGHT_);
//        show_status_connetion               = false;
//    }
//    
//    //pairing process with apps
//    if(count_conn_device < 1)
//    {
//      if(device_connected_b && !apps_connected_b)
//      {
//
//          vTaskDelay(300);
//          apps_connected_b                    = device_connected_b;
//          
//        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
//          p_pairing_characteristic->setValue(pairing_buffer, data_pairing_length); 
//        #else
//          p_pairing_characteristic->setValue(std::string(pairing_buffer.c_str()));
//        #endif
//      
//        #if(BLE_LIB_IS_NIMBLE == 1)
//            p_widget_characteristic->notify();
//        #endif 
//
//          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
//            Serial.println("pair code: ");
//            for(uint8_t x = 0; x < data_pairing_length; x++)
//            {
//              Serial.print(pairing_buffer[x]);
//              Serial.print(" ");
//            }            
//            Serial.println("");
//          #else
//            Serial.println(pairing_buffer.c_str());
//          #endif
//          timer0_counter_power_off_g          = 0;
//          
//          show_status_connetion               = true;
//          clear_LED(WIDTH_, HEIGHT_);
//          vTaskDelay(100);
//          loadImage(ble_enable_array[0]);
//          FastLED.show();
//          vTaskDelay(1000);
//          clear_LED(WIDTH_, HEIGHT_);
//          show_status_connetion               = false; 
//      }
//    }
//    else
//    {
//      if(device_connected_b && apps_connected_b)
//      {
//  //        Serial.print("pairing2 pServer->getConnectedCount: ");
//  //        Serial.println(pServer->getConnectedCount());
//        
//        p_pairing_characteristic->setValue("UNKNOWN");
//        #if(BLE_LIB_IS_NIMBLE == 1)
//            p_widget_characteristic->notify();
//        #endif 
//        Serial.println("UNKNOWN");
//        vTaskDelay(50);
//        //apps_connected_b                    = device_connected_b;
//
//      }
//    }
//    */
//    
//    current_state_core0 = (*func_state_ptr_core0[current_state_core0])(current_state_core0);
//
//    prev_state_core0 = current_state_core0;
//    
//    #if(CHK_TASK_DURATION_EACH_CORE == 1)
//      status_core_0 = !status_core_0;
//      digitalWrite(CORE_TASK_0_PIN, status_core_0);
//    #endif
//  }
//}


/**
 * @brief task 1 code
 * 
 * this code is specialize on showing animation on LED matrix
 * 
 */
void Task1code( void * pvParameters )  
{
  for(;;)
  {    
    FastLED.setBrightness(brightness_level_g);  

    reading_MPU__();

    if(notify_progress_b)
    {
      if(device_connected_b && apps_connected_b)     
      {
          memset(buf_send_notify_g, 0, sizeof(buf_send_notify_g));
          
          if(progres_status >= 100)
          { 
            progres_status = 99;
          }

          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
            buf_send_notify_g[0] = '%';
            buf_send_notify_g[1] = progres_status;

            Serial.print(buf_send_notify_g[0]);
            Serial.print("-");
            Serial.println(buf_send_notify_g[1]);
          #endif
          
          //Serial.println(buf_send_notify_g);        
          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)     
            p_widget_characteristic->setValue(&buf_send_notify_g[0], 2);
          #else
            p_widget_characteristic->setValue(buf_send_notify_g);
          #endif

          #if(BLE_LIB_IS_NIMBLE == 1)
              p_widget_characteristic->notify(true);
          #else
              p_widget_characteristic->notify();
          #endif
          
          notify_progress_b       = false;
      }
    }

    //unpair with apps
    if(setting_receive_b)        
    {
      tone_mode_setting();
      setting_receive_b       = false;
    }    
    if(!show_status_connetion)
    {
      current_state = (*func_state_ptr[current_state])(current_state);

      //Serial.printf("%d - %d - %d - %d \r\n", current_state, prev_state, packet_show_b, type_data_to_show_g);
    }

    if(current_state != ST_THIEF_ALARM)
    {
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(BUZZER_PIN_2, LOW);

      //send notifikasi berhasil terima data
      if(receive_complete_g > 0)
      {
        if(device_connected_b && apps_connected_b && (!notify_progress_b) && (progres_status >= 99))
        {   
            if(progres_status >= 99)
            {
              progres_status = 0;
            }

            #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
              buf_send_notify_g[0]  = 20;   
              p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
            #else
              p_widget_characteristic->setValue("21");
            #endif

            #if(BLE_LIB_IS_NIMBLE == 1)
                p_widget_characteristic->notify(true);
            #else
                p_widget_characteristic->notify();
            #endif

            #if(DEBUG_SERIAL_ALL_RECEIVED_DATA == 1)
              Serial.println("[INFO] Data complete received");
            #endif

            receive_complete_g--;
            receive_data_b                  = false_val;
            

            if(device_shutdown_b)
            {
              enable_supply(Voltage_3V3, true);
              mpu_config();
              device_shutdown_b               = false;
            }
            else
            {
              device_shutdown_b               = device_shutdown_b;
            }

            if(receive_complete_g  == 0)
            {
              receive_complete_g              = 0;
              notify_received_data_b          = false;
              frame_seq_receive_g             = 0;
            }
        }
      }

      if((!device_shutdown_b) && (!show_status_connetion))
      {
          //baca nilai touch sensor
            
          //hitung voltage tegangan baterai
          if(((uint16_t)timer0_counter_vsupply_status_g/10) >= time_batt_notif_status_volt_d)
          {
            //batt_voltage_g       = ((float)analogRead(batt_volt_pin_d)/4096.0) * 3.3 * 2.0;
            batt_voltage_g       = ((float)rom_phy_get_vdd33()) / 1000.0;
            uint8_t batt_percent = ((4.20 - batt_voltage_g) / 4.20) * 100;
            if(batt_percent > 100)
            {
              batt_percent = 100;
            }
            
            if(device_connected_b && apps_connected_b && (!notify_received_data_b))
            {
                  //if(p_pairing_characteristic->isReadyForData())
                  //{
                    memset(buf_send_notify_g, 0, sizeof(buf_send_notify_g));
                    
                    #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
                      buf_send_notify_g[0] = 'B';
                      buf_send_notify_g[1] = batt_percent;
                      p_widget_characteristic->setValue(&buf_send_notify_g[0], 2);
                    #else 
                      if(batt_percent < 10)
                      {
                        sprintf(buf_send_notify_g, "B0%d", batt_percent);//, batt_percent); 
                      }buf_send_notify_g
                      else
                      {
                        sprintf(buf_send_notify_g, "B%d", batt_percent);//, batt_percent); 
                      }
                      p_widget_characteristic->setValue(buf_send_notify_g);
                    #endif

                    #if(BLE_LIB_IS_NIMBLE == 1)
                        p_widget_characteristic->notify(true);
                    #else
                        p_widget_characteristic->notify();
                    #endif

                    #if(DEBUG_SERIAL_ALL_RECEIVED_DATA == 1)
                      Serial.print("[INFO]->");
                      Serial.print(alert_alarm_sign_b);
                      Serial.print(" - ");
                      Serial.print(automatic_sign_b);
                      Serial.print(" - ");                   
                      Serial.print(auto_power_off_led_b);
                      Serial.print(" - ");                    
                      Serial.print((float)timer0_counter_power_off_g/600);
                      Serial.print(" - ");
                      Serial.print(sendback_notify_value(&mode_display_led_g));
                      Serial.print(" - ");
                      #if(EXCHANGE_DATA_ON_INT_NOT_STR == 0)
                      Serial.print(String(buf_send_notify_g));
                      #endif
                      Serial.print(" - ");
                      Serial.print(mode_remote_enabled);
                      Serial.print(" - ");   
                      Serial.print(got_sequence_data_playlist_b);
                      Serial.print(" - "); 
                      Serial.print(mpu_calibrated_b);
                      Serial.print(" - ");                                                      
                      Serial.print(b_timer2_active);
                      Serial.print(" - ");
                      Serial.print(b_timer2_active);
                      Serial.print(" - ");                      
                      Serial.print(receive_failed_b);
                      Serial.print(" - ");
                      Serial.print(mode_safety_enabled_b);
                      Serial.print(" - ");                      
                      Serial.println(ESP.getFreeHeap());
                    #endif

                    count_failed_to_receive_g++;                  
            }
            timer0_counter_vsupply_status_g = 0;
          }

          //hitung waktu durasi untuk power off
          if(auto_power_off_led_b || sleep_status_b)
          {
            
            if((((uint16_t)timer0_counter_power_off_g/600) >= auto_power_off_time_g) || sleep_status_b)
            {
              #if(DEBUG_SERIAL_ALL_RECEIVED_DATA == 1)
                Serial.println("[INFO] I AM GOING TO SLEEP");
              #endif
              
              clear_LED(WIDTH_, HEIGHT_);
              
              enable_supply(Voltage_3V3, false);
              // protect_eeprom_memory(false);
              // flash.powerDown();
              // protect_eeprom_memory(true);
              mpu.setSleepEnabled(true);
              device_shutdown_b  = true;
              //ESP32_start_sleep();
            }
          }

          //apakah ada perintah hapus eeprom
          if(erase_eeprom_b)
          {
            clear_LED(WIDTH_, HEIGHT_);					  
            type_data_to_show_g    = data_animasi_default;
            delete_files_from_playlist();

            #if(DEBUG_SERIAL_ALL_RECEIVED_DATA == 1)
              Serial.println("[INFO] MEMORY CLEAR & READY TO USE");
            #endif

            erase_eeprom_b               = false;
            next_animation_g             = 0;
            clear_LED(WIDTH_, HEIGHT_);
          }

          //reset factory board -- aslinya restart esp dan delete semua playlist
          if(please_reset_me_b)
          {
            clear_LED(WIDTH_, HEIGHT_);

            type_data_to_show_g          = data_animasi_default; 

            delete_files_from_playlist();
            //ESP.restart();
            please_reset_me_b            = false;
            mode_display_led_g           = ST_ANIMATION;
            frame_num_playlist_g         = true;
            repeat_animasi_image_g       = 0;
            show_text_animation_b        = false;
 
          }

          prev_mode_display_g = mode_display_led_g;
      }
      else
      {   
        if(device_shutdown_b)
        {
          current_state_core0 = ST_IDLE;      //determine state of core 0
        }

        if(!show_status_connetion)
        { 
          clear_LED(WIDTH_, HEIGHT_); 
        }
      }

      //failed receive data
      if(receive_data_b >= truee_val)
      {
        if((count_failed_to_receive_g > 5) && (urutan_playlist_g < total_playlist_g))
        {
          receive_failed_b = true;
          receive_data_b   = false;
        }
    
        if(!got_sequence_data_playlist_b)
        {
          if((count_failed_to_receive_g > 5) && (frame_seq_receive_g < total_frame_animasi_g))
          {
            receive_failed_b = true;
            receive_data_b   = false;
          }
        }
      }

    }
    
    if(receive_failed_b)
    {
        if(device_connected_b && apps_connected_b && (!notify_progress_b))
        {      
            #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
            buf_send_notify_g[0]  = 21;
            p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
            #else
            p_widget_characteristic->setValue("20");
            #endif

            #if(BLE_LIB_IS_NIMBLE == 1)
                //p_widget_characteristic->notify(true);
            #else
                p_widget_characteristic->notify();
            #endif

            #if(DEBUG_SERIAL_ALL_RECEIVED_DATA == 1)
            Serial.println("[INFO] Data not kemalingan failed");
            #endif

            frame_seq_receive_g             = 0;
        }
        receive_failed_b = false;
      }

    prev_state = current_state;

    #if(CHK_TASK_DURATION_EACH_CORE == 1)
//      status_core_1 = !status_core_1;
//      digitalWrite(CORE_TASK_1_PIN, status_core_1);
    #endif

    receive_payload_uart0(&len_special_cmd_via_serial);
    check_special_command(payload_uart);
    
    vTaskDelay(7/portTICK_PERIOD_MS);                                                  //give mcu to rest from displaying img to led
  }
}

void loop()
{
    /*
    if(!device_connected_b && apps_connected_b)
    {   
        vTaskDelay(300);
        apps_connected_b                    = device_connected_b;
        pServer->startAdvertising();

        if(mode_kemalingan_b)
        {
          mode_display_led_g                  = ST_THIEF_ALARM;
        }
        else
        {
          mode_display_led_g                  = ST_ANIMATION;
        }

        show_status_connetion               = true;
        clear_LED(WIDTH_, HEIGHT_);
        
        vTaskDelay(100);
        
        loadImage(ble_disable_array[0]);
        FastLED.show();
        vTaskDelay(1000);
        clear_LED(WIDTH_, HEIGHT_);
        show_status_connetion               = false;
    }
    
    //pairing process with apps
    if(count_conn_device < 1)
    {
      if(device_connected_b && !apps_connected_b)
      {

          vTaskDelay(300);
          apps_connected_b                    = device_connected_b;
          
        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
          p_pairing_characteristic->setValue(pairing_buffer, data_pairing_length); 
        #else
          p_pairing_characteristic->setValue(std::string(pairing_buffer.c_str()));
        #endif
      
        #if(BLE_LIB_IS_NIMBLE == 1)
            p_widget_characteristic->notify();
        #endif 

          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
            Serial.println("pair code: ");
            for(uint8_t x = 0; x < data_pairing_length; x++)
            {
              Serial.print(pairing_buffer[x]);
              Serial.print(" ");
            }            
            Serial.println("");
          #else
            Serial.println(pairing_buffer.c_str());
          #endif
          timer0_counter_power_off_g          = 0;
          
          show_status_connetion               = true;
          clear_LED(WIDTH_, HEIGHT_);
          vTaskDelay(100);
          loadImage(ble_enable_array[0]);
          FastLED.show();
          vTaskDelay(1000);
          clear_LED(WIDTH_, HEIGHT_);
          show_status_connetion               = false; 
      }
    }
    else
    {
      if(device_connected_b && apps_connected_b)
      {
  //        Serial.print("pairing2 pServer->getConnectedCount: ");
  //        Serial.println(pServer->getConnectedCount());
        
        p_pairing_characteristic->setValue("UNKNOWN");
        #if(BLE_LIB_IS_NIMBLE == 1)
            p_widget_characteristic->notify();
        #endif 
        Serial.println("UNKNOWN");
        vTaskDelay(50);
        //apps_connected_b                    = device_connected_b;

      }
    }
    */
    
    vTaskDelay(1000/portTICK_PERIOD_MS);                                                 //give delay here, based on experiment it will reduce power consumption
}
