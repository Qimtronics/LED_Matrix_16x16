/**
 * 
 * @brief flappy game functions
 * 
 * @credit : 
 * 
 * 
*/

#if (FLAPPY_BIRD_GAME_EN == 1)

  #define POP_SPEED_F 200     // hopping speed (in mm / s)
  #define POP_POS_F 20        // hopping speed (in mm / s)
  #define GRAVITY_F 500       // falling acceleration (in mm / s2)
  #define DT_F 10             // integration period (ms)
  #define OBST_SPEED_F 175    // obstacle speed /prev value 150
  #define MIN_GAP_F 12        // minimum distance between obstacles /prev value: 9
  #define OBST_HEIGHT_F 9     // Max. obstacle height (slot size)   /prev value: 6
  #define DEMO_JUMP_F 6       // how far to jump in demo mode
  
  timerMinim flappyTimer(DT_F);
  timerMinim obstfTimer(OBST_SPEED_F);
  
  int posFlap = 0;
  int velFlap = 0;  // in mm and mm / s
  byte prevposFlap;
  byte obstCounterFlap;
  int flappyScore;
  int prev_flappyScore               = 0;
  uint8_t buff_data_2[2]            = {0,0};
  
  void flappyRoutine() 
  {
    if (loadingFlag_b)  
    {
  //    Serial.print("[INFO] FLAPPY BIRDS MODE GAME: ");
  //    Serial.println(gamemodeFlag);
      FastLED.clear();
      loadingFlag_b = false;
    }
  
//    if (gameDemo) {
//      for (byte i = 1; i < DEMO_JUMP_F; i++)
//        if (getPixColorXY(i, posFlap) == GLOBAL_COLOR_1) buttons = 0;   // автопрыжок
//      if (getPixColorXY(1, posFlap - 1) == GLOBAL_COLOR_1) buttons = 0; // автопрыжок
//    }
  
    if (got_new_command_g) 
    {
      if (control_games_g == games_control_up_d) 
      {   // кнопка нажата
        velFlap = POP_SPEED_F;
        posFlap += POP_POS_F;
        tone_game_control();
        
        control_games_g     = games_control_off_d; 
        got_new_command_g   = false;
      }
    }
  
    if (flappyTimer.isReady()) 
    {
      if (obstfTimer.isReady()) 
      {
        for (byte i = 0; i < WIDTH_ - 1; i++) 
        {
          for (byte j = 0; j < HEIGHT_; j++) 
          {
            leds[getPixelNumber(i, j)] = getPixColorXY(i + 1, j);
          }
        }
  
        obstCounterFlap++;
        if (obstCounterFlap >= MIN_GAP_F) 
        {
          obstCounterFlap = 0;
          flappyScore++;
  
          for (byte i = 0; i < HEIGHT_; i++) 
          {
            drawPixelXY(WIDTH_ - 1, i, GLOBAL_COLOR_1);
            drawPixelXY(WIDTH_ - 2, i, GLOBAL_COLOR_1);
          }
          byte thisHeight = random(1, HEIGHT_ - OBST_HEIGHT_F);
          for (byte i = thisHeight; i < thisHeight + OBST_HEIGHT_F; i++) {
            drawPixelXY(WIDTH_ - 1, i, 0);
            drawPixelXY(WIDTH_ - 2, i, 0);
          }
        } else {
          for (byte i = 0; i < HEIGHT_; i++)
            drawPixelXY(WIDTH_ - 1, i, 0);
        }
      }
  
      velFlap -= (float)GRAVITY_F * DT_F / 1000;
      posFlap += (float)velFlap * DT_F / 1000;
      if (posFlap < 0) {
        posFlap = 0;
        velFlap = 0;
      } else if (posFlap > (HEIGHT_ - 1) * 10) {
        posFlap = (HEIGHT_ - 1) * 10;
        velFlap = 0;
      }
      if (getPixColorXY(0, posFlap / 10) == GLOBAL_COLOR_1 ||
          getPixColorXY(1, posFlap / 10) == GLOBAL_COLOR_1) {

          displayScore(flappyScore);
    
          //save data scroe to eeprom
          if(flappyScore > high_score_flappy_bird)
          {
            high_score_flappy_bird = flappyScore;
    
            buff_data[0] = high_score_flappy_bird >> 8 & 0xFF;
            buff_data[1] = high_score_flappy_bird & 0xFF;
            
            EEPROM.write(buff_data[0], score_game_flappy_bird_addr);
            EEPROM.write(buff_data[1], score_game_flappy_bird_addr + 1);
            EEPROM.commit();  
            
            //show_info_high_score();
            tone_high_score();
          }
          else
          {
            tone_game_over();
          }  
          
        flappyScore = 0;
        loadingFlag_b = true;
        delay(800);
        return;
      }
      drawPixelXY(1, prevposFlap / 10, 0x000000);
      drawPixelXY(1, posFlap / 10, GLOBAL_COLOR_2);
      drawPixelXY(0, prevposFlap / 10, 0x000000);
      drawPixelXY(0, posFlap / 10, GLOBAL_COLOR_2);
      prevposFlap = posFlap;
      FastLED.show();
    }
  }

#endif

//OK
