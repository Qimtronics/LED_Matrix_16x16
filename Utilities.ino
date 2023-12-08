#if (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 0)
#define _WIDTH_ WIDTH_
#define THIS_X x
#define THIS_Y y

#elif (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 1)
#define _WIDTH_ HEIGHT_
#define THIS_X y
#define THIS_Y x

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 0)
#define _WIDTH_ WIDTH_
#define THIS_X x
#define THIS_Y (HEIGHT_ - y - 1)

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 3)
#define _WIDTH_ HEIGHT_
#define THIS_X (HEIGHT_ - y - 1)
#define THIS_Y x

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 2)
#define _WIDTH_ WIDTH_
#define THIS_X (WIDTH_ - x - 1)
#define THIS_Y (HEIGHT_ - y - 1)

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 3)
#define _WIDTH_ HEIGHT_
#define THIS_X (HEIGHT_ - y - 1)
#define THIS_Y (WIDTH_ - x - 1)

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 2)
#define _WIDTH_ WIDTH_
#define THIS_X (WIDTH_ - x - 1)
#define THIS_Y y

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 1)
#define _WIDTH_ HEIGHT_
#define THIS_X y
#define THIS_Y (WIDTH_ - x - 1)

#else
#define _WIDTH_ WIDTH_
#define THIS_X x
#define THIS_Y y
#pragma message "Wrong matrix parameters! Set to default"

#endif

const uint8_t flip_horizontal [256] = 
{
  15,16,47,48,79,80,111,112,143,144,175,176,207,208,239,240,
  14,17,46,49,78,81,110,113,142,145,174,177,206,209,238,241,
  13,18,45,50,77,82,109,114,141,146,173,178,205,210,237,242,
  12,19,44,51,76,83,108,115,140,147,172,179,204,211,236,243,
  11,20,43,52,75,84,107,116,139,148,171,180,203,212,235,244,
  10,21,42,53,74,85,106,117,138,149,170,181,202,213,234,245,
  9,22,41,54,73,86,105,118,137,150,169,182,201,214,233,246,
  8,23,40,55,72,87,104,119,136,151,168,183,200,215,232,247,
  7,24,39,56,71,88,103,120,135,152,167,184,199,216,231,248,
  6,25,38,57,70,89,102,121,134,153,166,185,198,217,230,249,
  5,26,37,58,69,90,101,122,133,154,165,186,197,218,229,250,
  4,27,36,59,68,91,100,123,132,155,164,187,196,219,228,251,
  3,28,35,60,67,92,99,124,131,156,163,188,195,220,227,252,
  2,29,34,61,66,93,98,125,130,157,162,189,194,221,226,253,
  1,30,33,62,65,94,97,126,129,158,161,190,193,222,225,254,
  0,31,32,63,64,95,96,127,128,159,160,191,192,223,224,255
};

const uint8_t flip_vertical [256] = 
{
255,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,
239,238,237,236,235,234,233,232,231,230,229,228,227,226,225,224,
223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208,
207,206,205,204,203,202,201,200,199,198,197,196,195,194,193,192,
191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176,
175,174,173,172,171,170,169,168,167,166,165,164,163,162,161,160,
159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144,
143,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128,
127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,
111,110,109,108,107,106,105,104,103,102,101,100,99,98,97,96,
95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,
79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,
63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,
47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,
31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,
15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
};

const uint8_t rotate_min90 [256] = 
{
  255,224,223,192,191,160,159,128,127,96 ,95 ,64 ,63 ,32 ,31 ,0,
  1  ,30 ,33 ,62 ,65 ,94 ,97 ,126,129,158,161,190,193,222,225,254,
  253,226,221,194,189,162,157,130,125,98 ,93 ,66 ,61 ,34 ,29 ,2,
  3  ,28 ,35 ,60 ,67 ,92 ,99 ,124,131,156,163,188,195,220,227,252,
  251,228,219,196,187,164,155,132,123,100,91 ,68 ,59 ,36 ,27 ,4, 
  5  ,26 ,37 ,58 ,69 ,90 ,101,122,133,154,165,186,197,218,229,250,
  249,230,217,198,185,166,153,134,121,102,89 ,70 ,57 ,38 ,25 ,6,
  7  ,24 ,39 ,56 ,71 ,88 ,103,120,135,152,167,184,199,216,231,248,
  247,232,215,200,183,168,151,136,119,104,87 ,72 ,55 ,40 ,23 ,8,
  9  ,22 ,41 ,54 ,73 ,86 ,105,118,137,150,169,182,201,214,233,246,
  245,234,213,202,181,170,149,138,117,106,85 ,74 ,53 ,42 ,21 ,10,
  11 ,20 ,43 ,52 ,75 ,84 ,107,116,139,148,171,180,203,212,235,244,
  243,236,211,204,179,172,147,140,115,108,83 ,76 ,51 ,44 ,19 ,12,
  13 ,18 ,45 ,50 ,77 ,82 ,109,114,141,146,173,178,205,210,237,242,
  241,238,209,206,177,174,145,142,113,110,81 ,78 ,49 ,46 ,17 ,14,
  15 ,16 ,47 ,48 ,79 ,80 ,111,112,143,144,175,176,207,208,239,240
};

const uint8_t rotate_90 [256] = 
{
  15 ,16 ,47 ,48 ,79 ,80 ,111,112,143,144,175,176,207,208,239,240,
  241,238,209,206,177,174,145,142,113,110,81, 78, 49, 46, 17, 14,
  13 ,18 ,45 ,50 ,77 ,82 ,109,114,141,146,173,178,205,210,237,242,
  243,236,211,204,179,172,147,140,115,108,83, 76, 51, 44, 19, 12,
  11 ,20 ,43 ,52 ,75 ,84 ,107,116,139,148,171,180,203,212,235,244,
  245,234,213,202,181,170,149,138,117,106,85, 74, 53, 42, 21, 10,
  9  ,22 ,41 ,54 ,73 ,86 ,105,118,137,150,169,182,201,214,233,246,
  247,232,215,200,183,168,151,136,119,104,87, 72, 55, 40, 23, 8,
  7  ,24 ,39 ,56 ,71 ,88 ,103,120,135,152,167,184,199,216,231,248,
  249,230,217,198,185,166,153,134,121,102,89, 70, 57, 38, 25, 6,
  5  ,26 ,37 ,58 ,69 ,90 ,101,122,133,154,165,186,197,218,229,250,
  251,228,219,196,187,164,155,132,123,100,91, 68, 59, 36, 27, 4,
  3  ,28 ,35 ,60 ,67 ,92 ,99 ,124,131,156,163,188,195,220,227,252,
  253,226,221,194,189,162,157,130,125,98, 93, 66, 61, 34, 29, 2,
  1  ,30 ,33 ,62 ,65 ,94 ,97 ,126,129,158,161,190,193,222,225,254,
  255,224,223,192,191,160,159,128,127,96, 95, 64, 63, 32, 31, 0
};

// Gamma коррекция (Defalt Gamma = 2.8)
const uint8_t PROGMEM gammaR[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,
  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,
  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,
  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14,
  15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22,
  23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33,
  33, 34, 35, 36, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 46,
  46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
  62, 63, 65, 66, 67, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 80,
  81, 83, 84, 85, 87, 88, 89, 91, 92, 94, 95, 97, 98, 99, 101, 102,
  104, 105, 107, 109, 110, 112, 113, 115, 116, 118, 120, 121, 123, 125, 127, 128,
  130, 132, 134, 135, 137, 139, 141, 143, 145, 146, 148, 150, 152, 154, 156, 158,
  160, 162, 164, 166, 168, 170, 172, 174, 177, 179, 181, 183, 185, 187, 190, 192,
  194, 196, 199, 201, 203, 206, 208, 210, 213, 215, 218, 220, 223, 225, 227, 230
};

const uint8_t PROGMEM gammaG[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

const uint8_t PROGMEM gammaB[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,
  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,
  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  8,
  8,  8,  8,  9,  9,  9, 10, 10, 10, 10, 11, 11, 12, 12, 12, 13,
  13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 19,
  20, 20, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28,
  29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
  40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53,
  54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 69, 70,
  71, 72, 73, 74, 75, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
  90, 92, 93, 94, 96, 97, 98, 100, 101, 103, 104, 106, 107, 109, 110, 112,
  113, 115, 116, 118, 119, 121, 122, 124, 126, 127, 129, 131, 132, 134, 136, 137,
  139, 141, 143, 144, 146, 148, 150, 152, 153, 155, 157, 159, 161, 163, 165, 167,
  169, 171, 173, 175, 177, 179, 181, 183, 185, 187, 189, 191, 193, 196, 198, 200
};

// gamma correction for expandColor
static const uint8_t PROGMEM
gamma5[] = {
  0x00, 0x01, 0x02, 0x03, 0x05, 0x07, 0x09, 0x0b,
  0x0e, 0x11, 0x14, 0x18, 0x1d, 0x22, 0x28, 0x2e,
  0x36, 0x3d, 0x46, 0x4f, 0x59, 0x64, 0x6f, 0x7c,
  0x89, 0x97, 0xa6, 0xb6, 0xc7, 0xd9, 0xeb, 0xff
},
gamma6[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08,
  0x09, 0x0a, 0x0b, 0x0d, 0x0e, 0x10, 0x12, 0x13,
  0x15, 0x17, 0x19, 0x1b, 0x1d, 0x20, 0x22, 0x25,
  0x27, 0x2a, 0x2d, 0x30, 0x33, 0x37, 0x3a, 0x3e,
  0x41, 0x45, 0x49, 0x4d, 0x52, 0x56, 0x5b, 0x5f,
  0x64, 0x69, 0x6e, 0x74, 0x79, 0x7f, 0x85, 0x8b,
  0x91, 0x97, 0x9d, 0xa4, 0xab, 0xb2, 0xb9, 0xc0,
  0xc7, 0xcf, 0xd6, 0xde, 0xe6, 0xee, 0xf7, 0xff
};

// шрифт 5х7
const uint8_t font5x7[][5] PROGMEM = {
  {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0 0x30 48
  {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1 0x31 49
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2 0x32 50
  {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3 0x33 51
  {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4 0x34 52
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5 0x35 53
  {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6 0x36 54
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7 0x37 55
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8 0x38 56
  {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9 0x39 57
};

// шрифт 3х5 квадратный
const uint8_t font3x5[][3] PROGMEM = {
  {0b11111, 0b10001, 0b11111},    // 0
  {0b00000, 0b00000, 0b11111},    // 1
  {0b10111, 0b10101, 0b11101},    // 2
  {0b10101, 0b10101, 0b11111},    // 3
  {0b11100, 0b00100, 0b11111},    // 4
  {0b11101, 0b10101, 0b10111},    // 5
  {0b11111, 0b10101, 0b10111},    // 6
  {0b10000, 0b10000, 0b11111},    // 7
  {0b11111, 0b10101, 0b11111},    // 8
  {0b11101, 0b10101, 0b11111},    // 9
};

// шрифт 3х5 скруглённый
const uint8_t font3x5_s[][3] PROGMEM = {
  {0b01111, 0b10001, 0b11110},    // 0
  {0b00000, 0b01000, 0b11111},    // 1
  {0b10011, 0b10101, 0b01001},    // 2
  {0b10001, 0b10101, 0b11111},    // 3
  {0b11100, 0b00100, 0b11111},    // 4
  {0b11101, 0b10101, 0b10111},    // 5
  {0b01111, 0b10101, 0b10111},    // 6
  {0b10000, 0b10011, 0b11100},    // 7
  {0b11111, 0b10101, 0b11111},    // 8
  {0b11101, 0b10101, 0b11110},    // 9
};

void ESP32_start_sleep()
{
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start();
}

void enable_supply(uint8_t supply_type, bool yes)
{
  if(supply_type == Voltage_5V)
  {
      if(yes)
      {
         digitalWrite(en_boost_5v_pin_d, HIGH);
      }
      else
      {
         digitalWrite(en_boost_5v_pin_d, LOW);
      }
  }
  else
  {
      if(yes)
      {
         digitalWrite(en_supply_3v3_pin_d, HIGH);
      }
      else
      {
         digitalWrite(en_supply_3v3_pin_d, LOW);
      }
  }
}

//OTA utilities
void save_eeprom_ota_status(bool succes)
{
  uint8_t adrs_status = addres_status_update + 1;

  EEPROM.write(start_data_eeprom_ota, 0);
  if(succes)
  {
    EEPROM.write(addres_status_update, 1);
    EEPROM.write(adrs_status, 1);
  }
  else
  {
    EEPROM.write(addres_status_update, 1);
    EEPROM.write(adrs_status, 0);
  }
  EEPROM.commit();
}

void read_available_RAM()
{
  Serial.print("ESP Heap Memory available:");
  Serial.println(ESP.getFreeHeap());
}

char hex_to_val(char val)
{
  if((val >= 0x30) && (val <= 0x39))
  {
    return val - 0x30;
  }
  else if ((val >= 0x41) && (val <= 0x46))
  {
    return val - 0x41 + 10;
  }
  else if ((val >= 0x61) && (val <= 0x66))
  {
    return val - 0x61 + 10;
  }
  else
  {
    return 0x00;
  }
}

String sendback_notify_value(uint8_t *yang_sedang_tampil)
{
  switch(*yang_sedang_tampil)
  {
    case ST_WIDGET:
        return "widget";
    break;
    case ST_GAME:
        return "games";
    break;
    case ST_REMOTE:
        return "remote";
    break;
    case ST_ANIMATION:
        return "animasi";
    break;
    default:
        return "???";
    break;
  }
}

//------Game Utilities---------------------------------------------------------------------//
void displayScore(byte score) {
  if (SCORE_SIZE == 0) {
    if (score > 9) drawDigit3x5(score / 10, WIDTH_ / 2 - 4, HEIGHT_ / 2 - 3, GLOBAL_COLOR_1);
    drawDigit3x5(score % 10, WIDTH_ / 2, HEIGHT_ / 2 - 3, GLOBAL_COLOR_1);
  } else {
    if (score > 9) drawDigit5x7(score / 10, WIDTH_ / 2 - 6, HEIGHT_ / 2 - 4, GLOBAL_COLOR_1);
    drawDigit5x7(score % 10, WIDTH_ / 2, HEIGHT_ / 2 - 4, GLOBAL_COLOR_1);
  }
  FastLED.show();
}

void drawDigit3x5(byte digit, byte X, byte Y, CRGB color) {
  if (digit > 9) return;
  for (byte x = 0; x < 3; x++) {
    byte thisByte = pgm_read_byte(&(font3x5[digit][x]));
    for (byte y = 0; y < 5; y++) {
      if (x + X > WIDTH_ || y + Y > HEIGHT_) continue;
      if (thisByte & (1 << y)) drawPixelXY(x + X, y + Y, color);
    }
  }
}

void drawDigit5x7(byte digit, byte X, byte Y, CRGB color) {
  if (digit > 9) return;
  for (byte x = 0; x < 5; x++) {
    byte thisByte = pgm_read_byte(&(font5x7[digit][x]));
    for (byte y = 0; y < 7; y++) {
      if (x + X > WIDTH_ || y + Y > HEIGHT_) continue;
      if (thisByte & (1 << (6 - y))) {drawPixelXY(x + X, y + Y, color);}
    }
  }
}

//------LED Utilities-----------------------------------------------------------------------//
void fillAll(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
}

// function for drawing a point by coordinates X Y
void drawPixelXY(int8_t x, int8_t y, CRGB color) {
  //Serial.println(color);
  if (x < 0 || x > WIDTH_ - 1 || y < 0 || y > HEIGHT_ - 1) return;
  int thisPixel = getPixelNumber(x, y) * SEGMENTS;
  for (byte i = 0; i < SEGMENTS; i++) {
    leds[thisPixel + i] = color;
  }
}

uint32_t getPixColor(int thisSegm) {
  int thisPixel = thisSegm * SEGMENTS;
  if (thisPixel < 0 || thisPixel > NUM_LEDS - 1) return 0;
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

uint32_t getPixColorXY(int8_t x, int8_t y) {
  return getPixColor(getPixelNumber(x, y));
}

// get the pixel number in the type by coordinates
uint16_t getPixelNumber(int8_t x, int8_t y) {
  if ((THIS_Y % 2 == 0) || MATRIX_TYPE) {               
    return (THIS_Y * _WIDTH_ + THIS_X);
  } else {                                              
    return (THIS_Y * _WIDTH_ + _WIDTH_ - THIS_X - 1);
  }
}

// get the pixel number in the type by coordinates
uint16_t getPixelNumber_GIFframe(int8_t x, int8_t y) {
  if ((y % 2 == 0) || 1) {               // если чётная строка
    return (y * WIDTH_ + x);
  } else {                                           
    return (y * WIDTH_ + WIDTH_ - x - 1);
  }
}

// get the pixel number of receive image
uint16_t getPixelNumber_IMGframe(int8_t x, int8_t y) {
  uint8_t Y_val = 15 - THIS_Y;
  //uint8_t X_val = 15 - THIS_X;
  if ((Y_val % 2 == 0) || 1) {               
    return (Y_val * _WIDTH_ + THIS_X);
  } else {                                         
    return (Y_val * _WIDTH_ + _WIDTH_ - THIS_X - 1);
  }
}

uint8_t flip_image(uint8_t val, uint8_t orientation)
{
  switch(orientation)
  {
     case vertical_d:
      return flip_vertical[val];
     break;
     case horizontal_d:
      return flip_horizontal[val];
     break;
     default:
      return val;
     break;
  }
}

uint8_t rotate_image(uint8_t val, int8_t rotation)
{
  switch(rotation)
  {
    case (0-90):
        return rotate_min90[val];
    break;
    case 90    :
        return rotate_90[val];
    break;
    default:
        return val;
    break;
  }
}

//uint16_t getPixelNumber_IMGframe(int8_t x, int8_t y) {
//  if ((y % 2 == 0) || 1) {               // если чётная строка
//    return (y * WIDTH_ + x);
//  } else {                                              // если нечётная строка
//    return (y * WIDTH_ + WIDTH_ - x - 1);
//  }
//}

uint8_t gamma_correction_ciqa2(uint8_t color, uint8_t type_color)
{
  switch(type_color)
  {
    case R_color:
      return pgm_read_byte(&gammaR[color & 0xFF]) & 0xFF;
    break;

    case G_color:
      return pgm_read_byte(&gammaG[color & 0xFF]) & 0xFF;
    break;

    case B_color:
      return pgm_read_byte(&gammaB[color & 0xFF]) & 0xFF;
    break;
  }
}


// gamma correction (more natural colors)
uint32_t gammaCorrection(uint32_t color) {
  byte r = (color >> 16) & 0xFF;  // Extract the RR byte
  byte g = (color >> 8) & 0xFF;   // Extract the GG byte
  byte b = color & 0xFF;        // Extract the BB byte

  r = pgm_read_byte(&gammaR[r]);
  g = pgm_read_byte(&gammaG[g]);
  b = pgm_read_byte(&gammaB[b]);

  uint32_t newColor = ((long)(r & 0xff) << 16) + ((long)(g & 0xff) << 8) + ((long)b & 0xff);
  return newColor;
}

// convert color from 16 bit to 24 bit
static uint32_t expandColor(uint16_t color) {
  return ((uint32_t)pgm_read_byte(&gamma5[ color >> 11       ]) << 16) |
         ((uint32_t)pgm_read_byte(&gamma6[(color >> 5) & 0x3F]) <<  8) |
                    pgm_read_byte(&gamma5[ color       & 0x1F]);
}

//OK--