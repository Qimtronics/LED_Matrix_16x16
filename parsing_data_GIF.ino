#define gif_header_size        6

const char header_gif89[6]   = {'G','I','F','8','9','a'};
const char header_gif87[6]   = {'G','I','F','8','7','a'};

bool check_header_gif(uint8_t *bytes)
{
  char buffers[6];

  memcpy(buffers, bytes, sizeof(buffers));

   if ((strncmp(buffers, header_gif89, gif_header_size) == 0) || (strncmp(buffers, header_gif87, gif_header_size) == 0))
   {
      return true;
   }
   else
   {
      return false;
   }
}

bool check_incoming_gif_header(char* buf_src, uint8_t start_idx)
{
   char buf_compares[8] = {'4','7','4','9','4','6','3','8'};
   uint8_t valid_val    = 0;

   for(uint8_t idx = start_idx; idx < (start_idx + 8); idx++)
   {
      if(buf_src[idx] == buf_compares[idx - start_idx])
      {
         valid_val++;
      }
      else
      {
         valid_val--;
      }
   }

   if(valid_val >= 8)
   {
      return true;
   }
   else
   {
      return false;
   }
}

void show_animation_GIF()
{
  if(!file_gif_opened)
  {
    if(gif_h.open(buff_animasiEEPROM, data_size_animasi_eeprom, GIFDraw))
    {
      //#ifdef DEBUG_AT_GIF
      //Serial.printf("[INFO] YOUR GIF FILE SIZE PIXEL = %d x %d\n", gif_h.getCanvasWidth(), gif_h.getCanvasHeight());
      //#endif
      file_gif_opened         = true;
      mode_display_led_g      = ST_ANIMATION;
    }
    else
    {
      //#ifdef DEBUG_AT_GIF
      //Serial.printf("[INFO] YOUR GIF FAIL to OPEN \r\n");
      //#endif      
    }
  }
  else
  {
      //Serial.printf("[INFO] %d \r\n", mode_display_led_g);

      if((mode_display_led_g == ST_ANIMATION) && (!show_status_connetion))
      {
        switch (gif_h.playFrame(true, NULL))
        {
          case 1:
            //Serial.println("read frame...");
            //mode_display_led_g                = ST_ANIMATION;
          break;
          
          case 0 :
            //Serial.println("GIF played all framed...");
            repeat_animasi_image_g--;
            gif_h.close();
            file_gif_opened = false;
            
            if(gif_show_enable)
            {
              mode_display_led_g             = ST_ANIMATION; 
              type_data_to_show_g            = data_gif_d;

                //#ifdef DEBUG_AT_GIF
                //Serial.print("[INFO]GIF: MHSpace: ");
                //Serial.println(ESP.getFreeHeap());
                //#endif

              notify_progress_b = 0;
            }
            else
            {
              clear_LED(WIDTH_, HEIGHT_);
//              mode_display_led_g             = ST_ANIMATION;
//              type_data_to_show_g            = ST_ANIMATION;
            }
          break;
          
          case -1:
            Serial.println("GIF not decoded...");
            file_gif_opened = false;
            gif_h.close();
          break;

          default:
            Serial.println("GIF unknwon issue...");
          break;
        }
      }
  }
}

void close_gif(uint8_t mode_display, uint8_t type_data_recv)
{
  uint8_t temp  = type_data_recv;
  if(file_gif_opened)
  {            
    #ifdef DEBUG_AT_GIF 
    Serial.println("GIF NOT OPENED");
    #endif

    memset(buff_universal_recv, 0, sizeof(buff_universal_recv));
    gif_h.close();
    file_gif_opened                 = false;  
    gif_show_enable                 = false;
    mode_display_led_g              = ST_ANIMATION;
    
    if(temp != data_animasi_default)
    { 
        type_data_to_show_g             = data_animasi_default;
        tipe_data_received_g            = type_data_recv;
    }
  }
  else
  {
    mode_display_led_g              = mode_display;

    if(temp != data_animasi_default)
    { 
        type_data_to_show_g             = data_animasi_default;
        tipe_data_received_g            = type_data_recv;
    }
  }
}

//OK--