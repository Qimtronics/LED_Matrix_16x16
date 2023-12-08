/**
 * 
 * @brief snack game functions
 * 
 * @credit : 
 * 
 * 
*/

#define GAME_SPEED                80

#if (SNAKE_GAME_EN == 1)

  // **************** GAMES SETTINGS ****************
  #define START_LENGTH            4       // initial snake length
  #define MAX_LENGTH              100      // maximum snake length
  
  // **************** GAMES SETTINGS ****************

  timerMinim gameTimer(GAME_SPEED * 4);
  
  int8_t vectorX, vectorY;
  int8_t headX, headY, buttX, buttY;
  int8_t appleX, appleY;
  boolean apple_flag, missDelete = false;
  
  int8_t buttVector[MAX_LENGTH];
  int snakeLength;
  int prev_snakeLength               = 0;
  boolean butt_flag, pizdetc;
  uint8_t buff_data_3[2]            = {0,0};
  
  void snakeRoutine() 
  {
    if (loadingFlag_b) 
    {
      
      FastLED.clear();
      loadingFlag_b = false;
      newGameSnake();
    }  
    
    buttonsTickSnake();
  
    if (gameTimer.isReady()) 
    {
      // APPLE GENERATION BLOCK
      while (!apple_flag) 
      {                                                  // wait until the apple is created
        appleX = random(1, (WIDTH_-2));                  // take random coordinates to place a created apple
        appleY = random(1, (HEIGHT_-2));
  
        // check if the coordinate is the same as the snake's body
        if ((long)getPixColorXY(appleX, appleY) == 0) 
        {
          apple_flag = true;                                   // if it does not match, we consider that the apple has been created
          drawPixelXY(appleX, appleY, GLOBAL_COLOR_2);         // then draw it on led matrix screen
          FastLED.show();
        }
      }
  
      // remember where we turned our heads
      // 0 - right, 1 - left, 2 - up, 3 - down
      if (vectorX > 0) buttVector[snakeLength] = 0;
      else if (vectorX < 0) buttVector[snakeLength] = 1;
      if (vectorY > 0) buttVector[snakeLength] = 2;
      else if (vectorY < 0) buttVector[snakeLength] = 3;
  
      // snake head displacement
      headX += vectorX;
      headY += vectorY;
  
      if (headX < 1 || headX > WIDTH_ - 2 || headY < 1 || headY > HEIGHT_ - 2) // if you go out of bounds
      { 
        pizdetc = true;
      }
  
      if (!pizdetc) 
      {
        // check for pizdetc
        if ((long)(getPixColorXY(headX, headY) != 0 && (long)getPixColorXY(headX, headY) != GLOBAL_COLOR_2)) 
        {   // if a snake crashed into something, but not an apple
          pizdetc = true;                           // flag for working off
        }
  
        // APPLE EATING UNIT
        if (!pizdetc && (long)getPixColorXY(headX, headY) == (long)GLOBAL_COLOR_2) 
        { // if you hit an apple with your head
          apple_flag = false;                       // flag that the apple is gone
          snakeLength++;                            // increase the length of the snake
          tone_game_control();
          buttVector[snakeLength] = 4;              // remember that it will be necessary not to wash the tail
        }
  
        // calculate the tail coordinate (to erase) from the vector array
        switch (buttVector[0]) 
        {
          case 0: buttX += 1;
            break;
          case 1: buttX -= 1;
            break;
          case 2: buttY += 1;
            break;
          case 3: buttY -= 1;
            break;
          case 4: missDelete = true;  // 4 значит не стирать!
            break;
        }
  
        // shift the entire array of tail vectors to the LEFT
        for (byte i = 0; i < snakeLength; i++) 
        {
          buttVector[i] = buttVector[i + 1];
        }
  
        // if the snake is not in the process of growing, paint the former tail black
        if (!missDelete) 
        {
          drawPixelXY(buttX, buttY, 0x000000);
        }
        else missDelete = false;
  
        // draw the snake's head in a new position
        drawPixelXY(headX, headY, GLOBAL_COLOR_1);
        FastLED.show();
      }
    }
  
    // if it comes
    if (pizdetc) 
    {
      pizdetc = false;
  
      // Well, in general, blink smoothly, like the snake “hurts”
      for (byte bright = 0; bright < 15; bright++) 
      {
        FastLED.setBrightness(bright);
        for (int i = 0; i < NUM_LEDS; i++) 
        {
          leds[i] = CRGB::Red;
        }
        FastLED.show();
        delay(10);
      }
      delay(100);
      FastLED.clear();
      FastLED.show();   

      uint8_t temp_score = snakeLength - START_LENGTH;
      displayScore(temp_score);

      //save data scroe to eeprom
      if(temp_score > high_score_snake)
      {
        high_score_snake = temp_score;

        buff_data[0] = high_score_snake >> 8 & 0xFF;
        buff_data[1] = high_score_snake & 0xFF;
        
        EEPROM.write(buff_data[0], score_game_snake_addr);
        EEPROM.write(buff_data[1], score_game_snake_addr + 1);
        EEPROM.commit();  
        
        //show_info_high_score();
        tone_high_score();
      }
      else
      {
        tone_game_over();
      }  

      delay(1000);
      FastLED.clear();
      FastLED.show();
      //FastLED.setBrightness(DEFAULT_BRIGHTNESS);
      newGameSnake();                          // that's okay, let's do it again
    }
  }
  
  void buttonsTickSnake() 
  {
    if (got_new_command_g) 
    {
      switch(control_games_g)
      {
        case games_control_left_d:
          vectorX = -1;
          vectorY = 0;
          control_games_g     = games_control_off_d;
          got_new_command_g   = false;
        break;
        case games_control_rigth_d:
          vectorX = 1;
          vectorY = 0;
          control_games_g     = games_control_off_d;
          got_new_command_g   = false;
        break;
        case games_control_up_d:
          vectorY = 1;
          vectorX = 0;
          control_games_g     = games_control_off_d;
          got_new_command_g   = false;
        break;
        case games_control_down_d:
          vectorY = -1;
          vectorX = 0;
          control_games_g     = games_control_off_d;
          got_new_command_g   = false;
        break;
        default:
        break;
      }
    }
  }
  
  void newGameSnake() 
  {
    FastLED.clear();
    // fresh grain for a random number generator
    randomSeed(millis());
    tampilan_border(2);
    // length from settings, start in the middle of the screen
    snakeLength = START_LENGTH;
    headX = WIDTH_ / 2;
    headY = HEIGHT_ / 2;
    buttY = headY;
  
    vectorX = 1;  // the initial motion vector is set here
    vectorY = 0;
    control_games_g     = games_control_off_d;
  
    // initial drawing of the snake and filling in an array of vectors for the tail
    for (byte i = 0; i < snakeLength; i++) 
    {
      drawPixelXY(headX - i, headY, GLOBAL_COLOR_1);
      buttVector[i] = 0;
    }
    FastLED.show();
    buttX = headX - snakeLength;   // the coordinate of the tail as the head is the length
    missDelete = false;
    apple_flag = false;
  }
    
#endif


//OK