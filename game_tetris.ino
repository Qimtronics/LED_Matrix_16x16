/**
 * 
 * @brief tetris game functions
 * 
 * @credit : 
 * 
 * 
*/


#if (TESTRIS_GAME_EN == 1)
    
    // **************** GAMES SETTINGS ****************
    #define FAST_SPEED 75      // falling speed when holding down (smaller is faster)
    #define STEER_SPEED 100    // the speed of moving to the side while holding the button (smaller is faster).
    
    // **************** GAMES SETTINGS ****************
    #define ADD_COLOR 0x010101
    
    int8_t fig              = 0;
    int8_t ang              = 0;
    int8_t pos              = WIDTH_ / 2;
    int8_t height           = HEIGHT_ - 1;
    int8_t prev_ang;
    int8_t prev_pos;
    int8_t prev_height;
    
    uint32_t colors[7]      = {0x0000EE, 0xEE0000, 0x00EE00, 0x00EEEE, 0xEE00EE, 0xEEEE00, 0xEEEEEE};
    uint32_t color          = 0x000088;
    byte color_index;
    byte linesToClear;
    boolean down_flag = true;
    byte lineCleanCounter;
    uint8_t buff_data_4[2]            = {0,0};
    
    // the most important part of the program! Shape pixel coordinates
    //  0 - stick
    //  1 - cube
    //  2 - Г
    //  3 - Г reverse
    //  4 - Z
    //  5 - Z reverse
    //  6 - Т
    
    const int8_t figures[7][12][2] PROGMEM = {
      {
        { -1, 0}, {1, 0}, {2, 0},
        {0, 1}, {0, 2}, {0, 3},
        { -1, 0}, {1, 0}, {2, 0},
        {0, 1}, {0, 2}, {0, 3},
      },
      {
        {0, 1}, {1, 0}, {1, 1},
        {0, 1}, {1, 0}, {1, 1},
        {0, 1}, {1, 0}, {1, 1},
        {0, 1}, {1, 0}, {1, 1},
      },
      {
        { -1, 0}, { -1, 1}, {1, 0},
        {0, 1}, {0, 2}, {1, 2},
        { -2, 1}, { -1, 1}, {0, 1},
        { -1, 0}, {0, 1}, {0, 2},
      },
      {
        { -1, 0}, {1, 0}, {1, 1},
        {0, 1}, {0, 2}, {1, 0},
        {0, 1}, {1, 1}, {2, 1},
        {0, 1}, {0, 2}, { -1, 2},
      },
      {
        { -1, 0}, {0, 1}, {1, 1},
        {0, 1}, { -1, 1}, { -1, 2},
        { -1, 0}, {0, 1}, {1, 1},
        {0, 1}, { -1, 1}, { -1, 2},
      },
      {
        { -1, 1}, {0, 1}, {1, 0},
        {0, 1}, {1, 1}, {1, 2},
        { -1, 1}, {0, 1}, {1, 0},
        {0, 1}, {1, 1}, {1, 2},
      },
      {
        { -1, 0}, {0, 1}, {1, 0},
        {0, 1}, {0, 2}, {1, 1},
        { -1, 1}, {0, 1}, {1, 1},
        { -1, 1}, {0, 1}, {0, 2},
      }
    };
    
    void tetrisRoutine() 
    {
      if (loadingFlag_b) 
      {
        FastLED.clear();
        loadingFlag_b = false;
        newGameTetris();
      }
    
      if (got_new_command_g) 
      {
        switch(control_games_g)
        {
          case games_control_left_d:
            control_games_g     = games_control_off_d;
            stepLeft();
            got_new_command_g   = false;
          break;
          case games_control_rigth_d:
            control_games_g     = games_control_off_d;
            stepRight();
            got_new_command_g   = false;
          break;
          case games_control_up_d:
            control_games_g     = games_control_off_d;
            if (checkArea(3)) {       // urning ability check
              prev_ang = ang;         // remember the old corner
              ang++;
              ang = ang % 4;        // modifiable ang from 0 to 3 (yes, clever)
              redrawFigure(prev_ang, pos, height);    // redraw the figure
            }
            got_new_command_g   = false;
          break;
          case games_control_down_d:
            control_games_g     = games_control_off_d;
            gameTimer.setInterval(FAST_SPEED);  // increase a speed
            got_new_command_g   = false;
          break;
          default:
          break;
          
        }
      }
    
      if (gameTimer.isReady()) 
      {       
        prev_height = height;
    
        if (!checkArea(0)) 
        {            
          if (height >= HEIGHT_ - 2) 
          {   
            gameOverTetris();                 
            newGameTetris();                 
          } 
          else 
          {                     
            fixFigure();                
            checkAndClear();           
            newGameTetris();                 
          }
        } 
        else if (height == 0) 
        {       
          fixFigure();                  
          checkAndClear();              
          newGameTetris();                  
        } 
        else 
        {                      
          height--;                             
          redrawFigure(ang, pos, prev_height);  
        }
      }
    }
    
    // searching and clearing filled levels
    void checkAndClear() 
    {
      linesToClear = 1;                 
      boolean full_flag = true;         
      while (linesToClear != 0) 
      {     
        linesToClear = 0;
        byte lineNum = 255;       
        for (byte Y = 0; Y < HEIGHT_; Y++) 
        {   
          full_flag = true;                  
          for (byte X = 0; X < WIDTH_; X++) 
          { 
            if ((long)getPixColorXY(X, Y) == (long)0x000000) {  
              full_flag = false;                                 
            }
          }
          if (full_flag) 
          {       
            linesToClear++;       
            if (lineNum == 255)   
              lineNum = Y;        
          } 
          else 
          {               
            if (lineNum != 255)  
              break;             
          }
        }
        if (linesToClear > 0) 
        {             
          lineCleanCounter += linesToClear;  
    
          
          for (byte X = 0; X < WIDTH_; X++) 
          {
            for (byte i = 0; i < linesToClear; i++) 
            {
              leds[getPixelNumber(X, lineNum + i)] = CHSV(0, 0, 255);        
            }
            FastLED.show();
            delay(5);     // delay between pixels from left to right
          }
          delay(10);
    
          // now smoothly reduce the brightness of the entire white block to zero
          for (byte val = 0; val <= 30; val++) 
          {
            for (byte X = 0; X < WIDTH_; X++) 
            {
              for (byte i = 0; i < linesToClear; i++) 
              {
                leds[getPixelNumber(X, lineNum + i)] = CHSV(0, 0, 240 - 8 * val);  
              }
            }
            FastLED.show();
            delay(5);       // delay between color change
          }
          delay(10);
    
          // and now we shift down all pixels above the level with the first found line
          for (byte i = 0; i < linesToClear; i++) 
          {
            for (byte Y = lineNum; Y < HEIGHT_ - 1; Y++) 
            {
              for (byte X = 0; X < WIDTH_; X++) 
              {
                drawPixelXY(X, Y, getPixColorXY(X, Y + 1));     
              }
              FastLED.show();
            }
            delay(100);       // the delay between the "shifts" of all pixels by one level
          }
          tone_game_control();
        }
      }
      gameTimer.reset();
    }
    
    
    // figure fixation function
    void fixFigure() 
    {
      color += ADD_COLOR;                   // slightly repaint
      redrawFigure(ang, pos, prev_height);  // redraw
      tone_game_control();
    }
    
    // losing
    void gameOverTetris() 
    {
      FastLED.clear();
      FastLED.show();
    
      // тут можно вывести счёт lineCleanCounter
      //if (!gameDemo) displayScore(lineCleanCounter);
      displayScore(lineCleanCounter);

      //save data scroe to eeprom
      if(lineCleanCounter > high_score_tetris)
      {
        high_score_tetris = lineCleanCounter;
  
        buff_data[0] = high_score_tetris >> 8 & 0xFF;
        buff_data[1] = high_score_tetris & 0xFF;
        
        EEPROM.write(buff_data[0], score_game_tetris_addr);
        EEPROM.write(buff_data[1], score_game_tetris_addr+1);
        EEPROM.commit();  
        
        //show_info_high_score();
        tone_high_score();
      }
      else
      {
        tone_game_over();
      }  

      
      tone_game_over();
      
      delay(1000);
      lineCleanCounter = 0; 
      FastLED.clear();
      FastLED.show();
      delay(20);
    }
    
    // new round
    void newGameTetris() 
    {
//      SerialBT.println("lolkek");   // без этого работает некорректно! магия ебаная
//      Serial.println("[INFO] NEW GAME TETRIS - lolkek"); 
//      delay(10);
      control_games_g     = games_control_off_d;
      height = HEIGHT_;    
      pos = WIDTH_ / 2;    
      fig = random(7);    
      ang = random(4);   
      //color = colors[random(6)];     

      /* color_index++;
      color_index = color_index % 6; 
      color = colors[color_index]; */
	  color = colors[fig];				  
    
      
      gameTimer.setInterval(GAME_SPEED * 4);
      down_flag = false; 
      delay(10);
    }
    
     // control shapes left and right
    void stepRight() 
    {
      if (checkArea(1)) 
      {
        prev_pos = pos;
        if (++pos > WIDTH_) pos = WIDTH_;
        redrawFigure(ang, prev_pos, height);
      }
    }
    void stepLeft() 
    {
      if (checkArea(2)) 
      {
        prev_pos = pos;
        if (--pos < 0) pos = 0;
        redrawFigure(ang, prev_pos, height);
      }
    }
    
    // collision check
    boolean checkArea(int8_t check_type) 
    {
      // check type:
      // 0 - checking lying figures and floors
      // 1 - checking the wall on the right and the figures
      // 2 - checking the wall on the left and the figures
      // 3 - checking both walls and floor
    
      boolean flag = true;
      int8_t X, Y;
      boolean offset = 1;
      int8_t this_ang = ang;
    
      if (check_type == 3) 
      {
        this_ang++;
        this_ang = this_ang % 4;
        offset = 0; 
      }
    
      for (byte i = 0; i < 4; i++) 
      {
        if (i == 0) 
        { 
          X = pos;
          Y = height;
        } 
        else 
        {        
          X = pos + (int8_t)pgm_read_byte(&figures[fig][this_ang * 3 + i - 1][0]);
          Y = height + (int8_t)pgm_read_byte(&figures[fig][this_ang * 3 + i - 1][1]);
        }
    
        if (check_type == 1 || check_type == 3) 
        {
          if (X + 1 > WIDTH_ - 1) flag = false;    
          uint32_t getColor;
          if (Y < HEIGHT_)
            getColor = getPixColorXY(X + offset, Y);
          if (getColor != color && getColor != 0x000000) 
          {
            flag = false;   
          }
        }
    
        if (check_type == 2 || check_type == 3) 
        {
          if (X - 1 < 0) flag = false;   
          uint32_t getColor;
          if (Y < HEIGHT_)
            getColor = getPixColorXY(X - offset, Y);
          if (getColor != color && getColor != 0x000000) 
          {
            flag = false;        
          }
        }
    
        if (check_type == 0 || check_type == 3) 
        {
          uint32_t getColor;
          if (Y < HEIGHT_) {
            getColor = getPixColorXY(X, Y - 1);
            if (getColor != color && getColor != 0x000000) 
            {
              flag = false;     
            }
          }
        }
      }
      return flag;    // we return a global flag that indicates whether we will collide with something or not
    }
    
    // a function that removes the current shape (paints it black)
    // and then redraws it in a new position
    void redrawFigure(int8_t clr_ang, int8_t clr_pos, int8_t clr_height) 
    {
      drawFigure(fig, clr_ang, clr_pos, clr_height, 0x000000);     
      drawFigure(fig, ang, pos, height, color);                          
      FastLED.show();
    }
    
    // a function that draws a shape with a specified color and at the desired angle
    void drawFigure(byte figure, byte angle, byte x, byte y, uint32_t color) 
    {
      drawPixelXY(x, y, color);    
      int8_t X, Y;                     
      for (byte i = 0; i < 3; i++) 
      {    
        X = x + (int8_t)pgm_read_byte(&figures[figure][angle * 3 + i][0]);
        Y = y + (int8_t)pgm_read_byte(&figures[figure][angle * 3 + i][1]);
        if (Y > HEIGHT_ - 1) continue;  
        drawPixelXY(X, Y, color);
      }
    }
    
#endif

//OK
