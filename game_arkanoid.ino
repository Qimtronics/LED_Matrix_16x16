/**
 * 
 * @brief arkanoid game functions
 * 
 * @credit : 
 * 
 * 
*/

#if (ARKANOID_GAME_EN == 1)

    // **************** GAME SETTING ****************
    #define SHELF_LENGTH            5        // shelf length
    #define VELOCITY                5        // ball speed
    #define BALL_SPEED              70       // integration period
    
    #define BLOCKS_H                4        // block pile height
    #define LINE_NUM                8        // number of "lines" with blocks of other levels
    #define LINE_MAX                4        // Max. line length
    
    //color block
    #define BLOCK_COLOR_1           CRGB::Aqua
    #define BLOCK_COLOR_2           CRGB::Amethyst
    #define BLOCK_COLOR_3           CRGB::Blue
    // **************** GAME SETTING ****************


    int posX_ark                    = 0;
    int posY_ark                    = 0;
    int8_t velX_ark                 = 3;
    int8_t velY_ark                 = (long)sqrt(sq(VELOCITY) - sq(velX_ark));
    int8_t shelf_x                  = WIDTH_ / 2 - SHELF_LENGTH / 2;
    byte shelfMAX                   = WIDTH_ - SHELF_LENGTH;
    int arkScore                    = 0;
    int prev_arkScore               = 0;
    uint8_t buff_data[2]            = {0,0};

    timerMinim ballTimer(BALL_SPEED);
    timerMinim popTimeout(500);
    
    void newGameArkan() 
    {
      arkScore                      = 0;
      generateBlocks();
      shelf_x                       = WIDTH_ / 2 - SHELF_LENGTH / 2;
      posX_ark                      = WIDTH_ / 2 * 10;
      posY_ark                      = 15;
      velX_ark                      = random(1, 4);
      velY_ark                      = (long)sqrt(sq(VELOCITY) - sq(velX_ark));
    }
    
    void arkanoidRoutine() 
    {
      if (loadingFlag_b) 
      {
        FastLED.clear();
        loadingFlag_b                 = false;
        newGameArkan();
        FastLED.show();
      }
    
    //  if (gameDemo) {
    //    if (shelfTimer.isReady()) {
    //      if (floor(posX_ark / 10) > shelf_x - SHELF_LENGTH / 2 - 1) buttons = 1;
    //      if (floor(posX_ark / 10) < shelf_x + SHELF_LENGTH / 2 + 1) buttons = 3;
    //    }
    //  }
    
    if (got_new_command_g) 
    {
      switch(control_games_g)
      {
        case games_control_left_d:
            shelfLeft();
            got_new_command_g  = false;
        break;
        case games_control_rigth_d:
            shelfRight();
            got_new_command_g  = false;
        break;
        default:
            control_games_g = games_control_off_d;
        break;
      }
      
      control_games_g    = games_control_off_d;     
    }
    
    if (ballTimer.isReady()) 
    {        // main game timer
      drawPixelXY(posX_ark / 10, posY_ark / 10, CRGB::Black);
      posX_ark = posX_ark + velX_ark;
      posY_ark = posY_ark + velY_ark;
      int8_t posX_arkm = posX_ark / 10;
      int8_t posY_arkm = posY_ark / 10;
      if (abs(velY_ark) <= 2) 
      {
        velX_ark = 3;
        velY_ark = (long)sqrt(sq(VELOCITY) - sq(velX_ark));
      }
    
      // bounce left edge
      if (posX_arkm < 0) 
      {
        posX_ark = 0;
        velX_ark = -velX_ark;
      }
    
      // отскок правый край
      if (posX_arkm > WIDTH_ - 1) 
      {
        posX_ark = (WIDTH_ - 1) * 10;
        velX_ark = -velX_ark;
      }
    
      // bottom break check
      if (posY_ark < 9) 
      {
        gameOverArkan();

        //save data scroe to eeprom
        if(arkScore > high_score_arkanoid)
        {
          high_score_arkanoid = arkScore;

          buff_data[0] = high_score_arkanoid >> 8 & 0xFF;
          buff_data[1] = high_score_arkanoid & 0xFF;
          
          EEPROM.write(buff_data[0], score_game_arkanoid_addr);
          EEPROM.write(buff_data[1], score_game_arkanoid_addr  +1);
          EEPROM.commit();  
          
          //show_info_high_score();
          tone_high_score();
        }
        else
        {
          tone_game_over();
        }


        delay(800);
        FastLED.clear();
        newGameArkan();
        //posY_ark = 0;
        //velY_ark = -velY_ark;
      }
    
      // stress check with pad
      if (velY_ark < 0 && posY_ark > 10 && posY_ark <= 20 && posX_arkm >= shelf_x && posX_arkm < (shelf_x + SHELF_LENGTH)) 
      {
        // timeout to exclude jerking off the shelf
        if (popTimeout.isReady()) 
        {
    
          // here it is shorter if the length of the shelf is more than two, then its edges are "twisted"
          // ball, i.e. when rebounding, change the speed along the X axis
          if (SHELF_LENGTH > 2) 
          {
            tone_game_control();
            
            if (posX_arkm == shelf_x) 
            {
              velX_ark -= 2;  // we decrease the speed along X
              // calculation of the speed according to Y, taking into account the general specified speed vector
              velY_ark = (long)sqrt(sq(VELOCITY) - sq(velX_ark));
            } 
            else if (posX_arkm == (shelf_x + SHELF_LENGTH - 1)) 
            {
              velX_ark += 2;  // increase the speed in X
              velY_ark = (long)sqrt(sq(VELOCITY) - sq(velX_ark));
            } 
            else 
            {
              velY_ark = -velY_ark;
            }
          } 
          else 
          {
            velY_ark = -velY_ark;
          }
        }
      }
    
      // breaking through the top
      if (posY_arkm > HEIGHT_ - 1) 
      {
        posY_ark = (HEIGHT_ - 1) * 10;
        velY_ark = -velY_ark;
      }
      byte ballX = floor(posX_ark / 10);
      byte ballY = floor(posY_ark / 10);
    
      if (ballY > 2) 
      {
        if (ballX < WIDTH_ - 1 && velX_ark > 0 && getPixColorXY(ballX + 1, ballY) != 0) 
        {
          redrawBlock(ballX + 1, ballY);
          velX_ark = -velX_ark;
        }
        if (ballX > 1 && velX_ark < 0 && getPixColorXY(ballX - 1, ballY) != 0) 
        {
          redrawBlock(ballX - 1, ballY);
          velX_ark = -velX_ark;
        }
        if (ballY < HEIGHT_ - 1 && velY_ark > 0 && getPixColorXY(ballX, ballY + 1) != 0)
        {
          redrawBlock(ballX, ballY + 1);
          velY_ark = -velY_ark;
        }
        if (velY_ark < 0 && getPixColorXY(ballX, ballY - 1) != 0) 
        {
          redrawBlock(ballX, ballY - 1);
          velY_ark = -velY_ark;
        }
      }
    
      if (checkBlocks()) gameOverArkan();    
    
      drawPixelXY(ballX, ballY, GLOBAL_COLOR_1);
      for (byte i = shelf_x; i < shelf_x + SHELF_LENGTH; i++) 
      {
        drawPixelXY(i, 0, GLOBAL_COLOR_2);
      }
      FastLED.show();
    }
   }
    
    boolean checkBlocks() 
    {   // return YES if there are no blocks
      for (byte j = HEIGHT_ - 1; j > HEIGHT_ - 1 - BLOCKS_H; j--) {
        for (byte i = 0; i < WIDTH_; i++) {
          if (getPixColorXY(i, j) != BLOCK_COLOR_1) return false;
          else if (getPixColorXY(i, j) != BLOCK_COLOR_2) return false;
          else if (getPixColorXY(i, j) != BLOCK_COLOR_3) return false;
        }
      }
      return true;
    }
    
    void redrawBlock(byte blockX, byte blockY) 
    {
      arkScore++;
      if (getPixColorXY(blockX, blockY) == BLOCK_COLOR_1) drawPixelXY(blockX, blockY, 0);
      else if (getPixColorXY(blockX, blockY) == BLOCK_COLOR_2) drawPixelXY(blockX, blockY, BLOCK_COLOR_1);
      else if (getPixColorXY(blockX, blockY) == BLOCK_COLOR_3) drawPixelXY(blockX, blockY, BLOCK_COLOR_2);
    }
    
    void generateBlocks() 
    {
      for (byte j = HEIGHT_ - 1; j > HEIGHT_ - 1 - BLOCKS_H; j--) {
        for (byte i = 0; i < WIDTH_; i++) {
          drawPixelXY(i, j, BLOCK_COLOR_1);
        }
      }
      for (byte k = 0; k < LINE_NUM; k++) {
        byte newPosX = random(0, WIDTH_ - 1 - LINE_MAX);
        byte newPosY = random(HEIGHT_ - BLOCKS_H, HEIGHT_);
        byte newColor = random(0, 3);
        for (byte i = newPosX; i < newPosX + LINE_MAX; i++) {
          switch (newColor) {
            case 0: drawPixelXY(i, newPosY, 0);
              break;
            case 1: drawPixelXY(i, newPosY, BLOCK_COLOR_2);
              break;
            case 2: drawPixelXY(i, newPosY, BLOCK_COLOR_3);
              break;
          }
        }
      }
    }
    
    void gameOverArkan() 
    {
      displayScore(arkScore);
      delay(1200);
      FastLED.clear();
      //newGameArkan();
    }
    
    void shelfRight() 
    {
      shelf_x++;            // add shelf coordinate
      if (shelf_x > shelfMAX) 
      { // if the shelf has broken through the right edge
        shelf_x = shelfMAX;
      } 
      else 
      {
        drawPixelXY(shelf_x - 1, 0, CRGB::Black);   // erase last point
        drawPixelXY(shelf_x + SHELF_LENGTH - 1, 0, GLOBAL_COLOR_2);  // draw the first one
        FastLED.show();
      }
    }
    
    void shelfLeft() 
    {
      shelf_x--;
      if (shelf_x < 0) 
      { // if the shelf has broken through the left edge
        shelf_x = 0;
      } 
      else 
      {
        drawPixelXY(shelf_x, 0, GLOBAL_COLOR_2);   // erase last point
        drawPixelXY(shelf_x + SHELF_LENGTH, 0, CRGB::Black);  // draw the first one
        FastLED.show();
      }
    }

#endif


//OK