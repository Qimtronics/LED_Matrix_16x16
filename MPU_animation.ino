/**
 * 
 * @brief function to show led animation on led matrix
 * 
 */
void viewLED(uint8_t *buf)
{ 
  for(uint8_t x = 0; x < WIDTH_; x++)
  {
    for(uint8_t y = 0; y < HEIGHT_; y++)
    {
      leds[getPixelNumber(x, 15 - y)] =  CRGB(color_table_global_g[buf[getPixelNumber(x, y)]][0],
                                         color_table_global_g[buf[getPixelNumber(x, y)]][1],
                                         color_table_global_g[buf[getPixelNumber(x, y)]][2]);
    }
  }
  FastLED.show(); 
}

/**
 * 
 * @brief function to show "stop" animation
 * 
 */
void stop_animation()
{
    //memcpy(buf_led, alert_stop_maling, sizeof(buf_led));
    g_ypos = 0;
    g_xpos = 0;
    for(g_ypos = 0 ; g_ypos < 16; g_ypos++)
    {
       for(g_xpos = 0; g_xpos < 16; g_xpos++)
      {
        buf_led[getPixelNumber(g_xpos, g_ypos)] = alert_stop_maling[getPixelNumber_GIFframe(g_xpos, g_ypos)];
      }
    }
   viewLED(buf_led);
   vTaskDelay(500);
   clear_LED(WIDTH_, HEIGHT_);
   vTaskDelay(250);
}

/**
 * 
 * @brief function to show "go forward" animation
 * 
 */
void forward_animation(int8_t *frm)
{ 
    for(uint8_t x_pos = 0; x_pos < HEIGHT_; x_pos++)
    {
      if(!b_blank_array)
      {
        buf_led[getPixelNumber(x_pos, (HEIGHT_  - 1))] = ArrowUp[getPixelNumber_GIFframe(x_pos, *frm)];
      }
      else
      {
        buf_led[getPixelNumber(x_pos, (HEIGHT_  - 1))] = 0x00;
      }
    }

    viewLED(buf_led);

    g_ypos = 0;
    for(g_ypos = 0 ; g_ypos < (HEIGHT_-1); g_ypos++)
    {
       for(g_xpos = 0; g_xpos < HEIGHT_; g_xpos++)
      {
        buf_led[getPixelNumber(g_xpos, g_ypos)] = buf_led[getPixelNumber(g_xpos, g_ypos + 1)];
      }
    }
}

/**
 * 
 * @brief function to show "turn left" animation
 * 
 */
void left_animation(int8_t *frm)
{ 
    for(uint8_t y_pos = 0; y_pos < WIDTH_; y_pos++)
    {
      if(!b_blank_array)
      {
        buf_led[getPixelNumber((WIDTH_  - 1), y_pos)] = ArrowLeft[getPixelNumber_GIFframe(*frm, y_pos)];
      }
      else
      {
        buf_led[getPixelNumber((WIDTH_  - 1), y_pos)] = 0x00;
      }
    }

    viewLED(buf_led);

    g_xpos = 0;
    for(g_xpos = 0 ; g_xpos < (WIDTH_-1); g_xpos++)
    {
       for(g_ypos = 0; g_ypos < WIDTH_; g_ypos++)
      {
        buf_led[getPixelNumber(g_xpos, g_ypos)] = buf_led[getPixelNumber(g_xpos + 1, g_ypos)];
      }
    }
}

/**
 * 
 * @brief function to show "turn right" animation
 * 
 */
void right_animation(int8_t *frm)
{
  for(uint8_t y_pos = 0; y_pos < WIDTH_; y_pos++)
  {
    if(!b_blank_array)
    {
      buf_led[getPixelNumber(0, y_pos)] = ArrowRight[getPixelNumber_GIFframe(*frm, y_pos)];
    }
    else
    {
      buf_led[getPixelNumber(0, y_pos)] = 0x00;
    }
  }
  
  viewLED(buf_led);

  g_xpos = (WIDTH_-1);
  for(g_xpos = (WIDTH_-1); g_xpos > 0; g_xpos--)
  {
      for(g_ypos = 0; g_ypos < WIDTH_; g_ypos++)
      {
          buf_led[getPixelNumber(g_xpos, g_ypos)] = buf_led[getPixelNumber(g_xpos - 1, g_ypos)];
      }
  }
}

/**
 * 
 * @brief function to show control sign left and right animation
 * 
 */
void play_animation()
{ 
   uint8_t max_counter      = 0;

   if(DIRECTION != REM_STOP)
   {
      max_counter = 8;
   }
   else
   {
      max_counter = 1;
   }
    
   if(SIGN_ANIMATION == ON)
   {
     for(uint8_t mount_leds_to_move = 0; mount_leds_to_move < max_counter; mount_leds_to_move++)
     {
        switch(DIRECTION)
        {
           case REM_STOP : 
              stop_animation();     
           break;
           case RIGHT :   
              right_animation(&g_frame);   
           break;
           case LEFT :  
              left_animation(&g_frame);    
           break;
           case FORWARD :   
              forward_animation(&g_frame);   
           break;
        }

        
        if((DIRECTION != RIGHT) && (DIRECTION != REM_STOP))
        {
           g_frame++;
           if((g_frame >= WIDTH_) || (g_frame >= HEIGHT_))
           {
             g_frame  = 0;
             b_blank_array     =! b_blank_array;
           }
        }
        else
        {
            g_frame--;
            if(g_frame < 0)
            {
              g_frame           = WIDTH_ - 1;
              b_blank_array     =! b_blank_array;
            }
        } 
        
        vTaskDelay(g_delay_animation);
     }

     g_next_part_animation++;
     //b_animasi_diproses = false;
     if(g_next_part_animation >= 2)
     { 
       g_next_part_animation = 0;
       g_repeat_animation++;
     }

   }
   else
   {
      if(!forward_has_over_b)
      {
        //Serial.println("[INFO 342] no animation displayed");
        memset(buf_led, 0, sizeof(buf_led)); 
        //viewLED(buf_led);
        clear_LED(WIDTH_, HEIGHT_);
      }
   }
}

//OK--