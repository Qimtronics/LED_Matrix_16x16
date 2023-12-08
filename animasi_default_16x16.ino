uint8_t frameNum   = 0; 

/**
 * 
 * @brief load image which stored inside Eksternal EEPROM memory
 * 
*/
void loadImage(const uint16_t (*frame)) {
  int8_t offset_x = (WIDTH_ - WIDTH_) / 2;
  int8_t offset_y = (HEIGHT_ - HEIGHT_) / 2;
  for (byte i = 0; i < WIDTH_; i++) {
    if (offset_x + i < 0 || offset_x + i > WIDTH_ - 1) continue;
    for (byte j = 0; j < HEIGHT_; j++) {
      if (offset_y + j < 0  || offset_y + j > HEIGHT_ - 1) continue;
      drawPixelXY(offset_x + i, offset_y + j, gammaCorrection(expandColor((pgm_read_word(&(frame[(HEIGHT_ - j - 1) * WIDTH_ + i]))))));
    }
  }
}


/**
 * 
 * @brief show default animation when first time the firmware is flashed into a board
 * 
*/
void animation_default(uint8_t* animation)
{
  static bool first_come_b  = true;
  static int8_t animation_repeat = 0;
  static int8_t repeat_frame = 0;
  switch(*animation)
  {
    case 0:
        if(first_come_b)
        {
          sprintf(text_animasi_g, "Light u'r life");
          total_pixel_led_used_g          = (String(text_animasi_g).length() * 7) + 32;
          first_come_b      = false;
        }
        
        show_text_animation_b           = true;
        if(show_text_animation_b)
        {
          matrix->fillScreen(0);
          matrix->setCursor(cursor_x_text_g, cursor_y_text_g);
          matrix->print(text_animasi_g);
          matrix->setFont(&Dialog_bold_13);
          matrix->setTextColor(matrix->Color(color_table_global_g[4][0], color_table_global_g[4][1], color_table_global_g[4][2]));
          if(--cursor_x_text_g < -total_pixel_led_used_g) 
          {
            cursor_x_text_g = matrix->width();
            show_text_animation_b = false;
            *animation = *animation + 1; 
            repeat_frame = 2; 
          }
          matrix->show();
        }
    break;
    case 1:
        loadImage(bulb_lamp_animation_array[frameNum]);
        
        repeat_frame--;
        if(repeat_frame <= 0)
        {
          repeat_frame = 4;
          if (++frameNum >= (sizeof(bulb_lamp_animation_array) / sizeof(bulb_lamp_animation_array[0]))) 
          {
            frameNum = 0;
            *animation = *animation + 1;
            animation_repeat = 3;
          }   
        }
    break;
    case 2:
        loadImage(pacman_animation_array[frameNum]);

        repeat_frame--;
        if(repeat_frame <= 0)
        {
          repeat_frame = 6;
          if (++frameNum >= (sizeof(pacman_animation_array) / sizeof(pacman_animation_array[0]))) 
          {
            frameNum = 0;

            animation_repeat--;
            if(animation_repeat <= 0)
            {
              *animation = *animation + 1;
              animation_repeat = 2;
            }
          } 
        }
    break;
    case 3:
        loadImage(hamtaro_animation_array[frameNum]);
        
        repeat_frame--;
        if(repeat_frame <= 0)
        {
          repeat_frame = 6;
          if (++frameNum >= (sizeof(hamtaro_animation_array) / sizeof(hamtaro_animation_array[0]))) 
          {
            frameNum = 0;
            
            animation_repeat--;
            if(animation_repeat <= 0)
            {
              *animation = *animation + 1;
              animation_repeat = 2;
            }
          } 
        }
    break;
    case 4:
        loadImage(rabbit_animation_array[frameNum]);

        repeat_frame--;
        if(repeat_frame <= 0)
        {
          repeat_frame = 10;        
          if (++frameNum >= (sizeof(rabbit_animation_array) / sizeof(rabbit_animation_array[0]))) 
          {
            frameNum = 0;
            first_come_b      = true;
            *animation        = 0;
          } 
        }
    break;    
    default:
      Serial.println("[INFO] animation default out of range");
    break;
  }

  FastLED.show();
  if(*animation == 0)
  {
    vTaskDelay(20/portTICK_PERIOD_MS); 
  }
  else
  {
    vTaskDelay(duration_show_animation/portTICK_PERIOD_MS); 
  }
}


//ok
