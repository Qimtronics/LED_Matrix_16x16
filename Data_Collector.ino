
volatile bool got_hdr_describe_gif            = false;
volatile uint8_t counter_to_data_frame        = 0;
volatile uint8_t counter_receive_data_frame   = 0;
volatile bool receiveing_data_frame           = false;
volatile uint8_t len_data_frame               = 0;
volatile bool real_ending_gif_code            = false;

/**
 * 
 * @brief function to collect image, existing animation, and running text data received from apps
 * 
*/
#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
void collect_data(uint8_t seq, char *buf_src, uint8_t *buf_dst, uint8_t len, uint8_t tipe_data)
{
  switch(tipe_data)
   {
     case data_text_d:
        for(uint8_t idx = 0; idx <= len ; idx++)
        {
          buf_dst[idx] = buf_src[idx];
        }     
     break;

     case data_image_d:
        idx_buf = seq * len;
        // Serial.println(idx_buf);
        // uint8_t *ptr;
        for(uint8_t idx = 0; idx < len; idx++)
        {
          *(buf_dst + (idx_buf + idx)) = buf_src[idx+1];
          //ptr = &buff_universal_recv[idx_buf + idx];
          //Serial.printf("src_addr: %p - %x --->", ptr, buff_universal_recv[idx_buf + idx]);
          //Serial.printf("add: %p - %x \r\n", (buf_dst + (idx_buf + idx)), *(buf_dst + (idx_buf + idx)));
        }
        //Serial.println();
        
        // for (uint16_t i = 0; i < 1024; i++) 
        // {
        //     Serial.print(buff_universal_recv[i], HEX);
        //     Serial.print("-");
        //     if(((i+1) % 48) == 0)
        //     {
        //     Serial.println();
        //     }
        // }
        // Serial.println(); 
     break;

     case data_animasi_d:
         idx_buf = (frame_seq_receive_g * 768) + seq * len;
        
         #if(SPECIAL_DEBUG_FOR_ANIMATION == 1)
          Serial.print("collect:");
          Serial.println(idx_buf);
          Serial.println("========");
         #endif


         for(uint8_t idx = 0; idx <= len; idx++)
         {
            buf_dst[idx_buf + idx] = buf_src[idx];

            #if(SPECIAL_DEBUG_FOR_ANIMATION == 1)
              if(frame_seq_receive_g == 0)
              {
                  Serial.print(buf_dst[idx_buf + idx], HEX);
                  Serial.print("=");
              }
              else
              {
                  Serial.print(buf_dst[(seq * len) + idx], HEX);
                  Serial.print("=");
              }
            #endif
         }
     break;

     case data_gif_d:
      idx_buf = seq * 96;

      for(uint8_t idx = 0; idx <= len; idx++)
      {
          buf_dst[idx_buf + idx] = buf_src[idx];

          #if(SPECIAL_DEBUG_FOR_GIF == 1)
            Serial.printf("%d _ %d _ %d _ %d\r\n", idx, idx_buf, buf_dst[idx_buf + idx], buf_dst[idx_buf + (idx-1)]);
          #endif

          if((idx_buf != 0) && (buf_dst[idx_buf + idx] == 0x3B) && ((buf_dst[idx_buf + (idx-1)] == 0x00)))
          {
            if(idx >= (len-1))
            {
              gif_file_is_end_b    = true;
            }
          }

          // if(times_sent == 1)
          // {
          //   if((idx_buf + idx + len_last_gif_msg) >= total_max_data_est)
          //   {
          //     gif_file_is_end_b = true;
          //   }
          // }
      }     
     break;

     default:
     break;
   }
}
#else
void collect_data(uint8_t seq, char *buf_src, uint8_t *buf_dst, uint8_t len, uint8_t tipe_data)
{
   char temp_val_uint_high;
   char temp_val_uint_low;

   switch(tipe_data)
   {
     case data_image_d:
         for(uint8_t idx = 0; idx < len; idx++)
         {
            if((idx % 2) == 0)
            {
               temp_val_uint_high = hex_to_val(buf_src[idx]);
               temp_val_uint_low  = hex_to_val(buf_src[idx + 1]); 
               
               
               buf_dst[idx_buf] = (temp_val_uint_high << 4 | temp_val_uint_low) & 0xFF;      
               //Serial.print(buf_dst[idx_buf], HEX);
               //Serial.print("-");         
               idx_buf++;  
            }

              //buf_dst[idx_buf] = buf_src[idx];
              //Serial.print((char)buf_dst[idx_buf]);              
              //Serial.print("-");   
              //idx_buf++;
         }       
     break;  
     case data_animasi_d:
         for(uint8_t idx = 0; idx < len; idx++)
         {
            if((idx % 2) == 0)
            {
               temp_val_uint_high = hex_to_val(buf_src[idx]);
               temp_val_uint_low  = hex_to_val(buf_src[idx + 1]); 
               
               buf_dst[idx_buf] = (temp_val_uint_high << 4 | temp_val_uint_low) & 0xFF; 
//               Serial.print(buf_dst[idx_buf], HEX);
//               Serial.print("-");   
               idx_buf++;
            }
         }
     break;
     case data_text_d:
        for(uint8_t idx = 0; idx < len ; idx++)
        {
            if((idx >= 10) && (idx <= len)) //ambil tulisannya
            {  
               buf_dst[idx_buf] = check_got_char(buf_src[idx]);

               #ifdef DEBUG_AT_RECEIVE_DATA
                 Serial.print((char)buf_dst[idx_buf]);
                 Serial.print(" + ");
               #endif

               idx_buf++;
            }
            else
            {
              if((idx % 2) == 0) //ambil warna dan speednya
              {
                 temp_val_uint_high = hex_to_val(buf_src[idx]);
                 temp_val_uint_low  = hex_to_val(buf_src[idx + 1]); 

                 if(idx >= 0)
                 {
                    buf_dst[idx_buf] = (temp_val_uint_high << 4 | temp_val_uint_low) & 0xFF; 
                 }
                 else
                 {
                    buf_dst[idx_buf] = (uint8_t)temp_val_uint_high * 10 + temp_val_uint_low;
                 }

                 #ifdef DEBUG_AT_RECEIVE_DATA
                   Serial.print(buf_dst[idx_buf], HEX);
                   Serial.print(" | ");  
                 #endif

                 idx_buf++;
              }
            }
        }
     break;
     default:
         #ifdef DEBUG_AT_RECEIVE_DATA
          Serial.println("[INFO] data not categorized");
         #endif
     break;
   }
}
#endif

/**
 * 
 * @brief function to collect gif animation
 * 
*/
#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
void collect_gif(uint8_t seq, char *buf_src, uint8_t *buf_dst, uint8_t len)
{
  idx_buf = seq * len;
  for(uint8_t idx = 0; idx <= len; idx++)
  {
    buf_dst[idx_buf + idx] = buf_src[idx];

    if((idx_buf != 0) && (buf_dst[idx_buf + idx] == 0x3B) && ((buf_dst[idx_buf + (idx-1)] == 0x00)))
    {
      if(idx >= len)
      {
        gif_file_is_end_b    = true;
      }
    }
  }
}
#else
void collect_gif(uint8_t seq, char *buf_src, uint8_t *buf_dst, uint8_t len)
{
   char temp_val_uint_high;
   char temp_val_uint_low;
   
   if((seq % 2) != 0)
   {
     last_byte_for_next_g = buf_src[len-1];
     len = len - 1;
     
//     Serial.print("last_byte_for_next_g: ");
//     Serial.println(last_byte_for_next_g);
     
     for(uint8_t idx = 0; idx < len; idx++)
     {
        if((idx % 2) == 0)
        { 
             temp_val_uint_high = hex_to_val(buf_src[idx]);
             temp_val_uint_low  = hex_to_val(buf_src[idx + 1]); 
             
             buf_dst[idx_buf_gif_g] = ((temp_val_uint_high << 4) | temp_val_uint_low) & 0xFF;  
//             Serial.print(buf_dst[idx_buf_gif_g], HEX);
//             Serial.print("-");   
           if(receiveing_data_frame)
           {
            counter_receive_data_frame++;
            if(counter_receive_data_frame == len_data_frame)
            {
              #ifdef DEBUG_AT_RECEIVE_DATA
                Serial.print("[INFO-1] data frame:");
                Serial.print(counter_receive_data_frame);
                Serial.print(" len_data:");
                Serial.print(len_data_frame);
                Serial.print(" bool:");
                Serial.println(real_ending_gif_code);
              #endif
              counter_receive_data_frame  = 0;
              receiveing_data_frame       = false;
              real_ending_gif_code        = true;
            }
           }
           
           if(got_hdr_describe_gif)
           {
             counter_to_data_frame++;
             if(counter_to_data_frame == pos_info_len_data)
             {
               len_data_frame             = buf_dst[idx_buf_gif_g];
               counter_to_data_frame      = 0;
               got_hdr_describe_gif       = false;
               receiveing_data_frame      = true;
             }
           }          
           
           if((buf_dst[idx_buf_gif_g] == 0xf9) && (buf_dst[idx_buf_gif_g - 1] == 0x21))
           {
              got_hdr_describe_gif  = true;
              counter_to_data_frame = 0;
              real_ending_gif_code  = false;
           }
             
           if((idx_buf_gif_g != 0) && (buf_dst[idx_buf_gif_g] == 0x3B) && ((buf_dst[idx_buf_gif_g - 1] == 0x00)) && (real_ending_gif_code))
           {
             gif_file_is_end_b    = true;
             real_ending_gif_code = false;
             break;
           }
           else
           {     
             idx_buf_gif_g++;  
           }
        }
        else
        {
          continue;
        }
     }    
   }
   else
   {
     len = len + 1;

     for(uint8_t idx = 0; idx < len; idx++)
     {
        if((idx % 2) == 0)
        {
           if(idx == 0)
           {
               temp_val_uint_high = hex_to_val(last_byte_for_next_g);
               temp_val_uint_low  = hex_to_val(buf_src[idx]); 
           }
           else
           {
               temp_val_uint_high = hex_to_val(buf_src[idx - 1]);
               temp_val_uint_low  = hex_to_val(buf_src[idx]); 
           }

           buf_dst[idx_buf_gif_g] = ((temp_val_uint_high << 4) | temp_val_uint_low) & 0xFF;  
//           Serial.print(buf_dst[idx_buf_gif_g], HEX);
//           Serial.print("-");       
           if(receiveing_data_frame)
           {
            counter_receive_data_frame++;
            if(counter_receive_data_frame == len_data_frame)
            {
              #ifdef DEBUG_AT_RECEIVE_DATA
                Serial.print("[INFO-2] data frame:");
                Serial.print(counter_receive_data_frame);
                Serial.print(" len_data:");
                Serial.print(len_data_frame);
                Serial.print(" bool:");
                Serial.println(real_ending_gif_code);
              #endif
              
              counter_receive_data_frame  = 0;
              receiveing_data_frame       = false;
              real_ending_gif_code        = true;
            }
           }
           
           if(got_hdr_describe_gif)
           {
             counter_to_data_frame++;
             if(counter_to_data_frame == pos_info_len_data)
             {
               len_data_frame             = buf_dst[idx_buf_gif_g];
               counter_to_data_frame      = 0;
               got_hdr_describe_gif       = false;
               receiveing_data_frame      = true;
             }
           }          
           
           if((buf_dst[idx_buf_gif_g] == 0xf9) && (buf_dst[idx_buf_gif_g - 1] == 0x21))
           {
              got_hdr_describe_gif  = true;
              counter_to_data_frame = 0;
              real_ending_gif_code  = false;
           }
             
           if((idx_buf_gif_g != 0) && (buf_dst[idx_buf_gif_g] == 0x3B) && ((buf_dst[idx_buf_gif_g - 1] == 0x00)) && (real_ending_gif_code))
           {
             gif_file_is_end_b    = true;
             real_ending_gif_code = false;
             break;
           }
           else
           {           
             idx_buf_gif_g++;  
           } 
        }
        else
        {
          continue;
        }
     }

     last_byte_for_next_g = 0;
   }
}
#endif

/**
 * 
 * @brief function to receive running text data then parsing it and collect it
 * 
*/
#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
void receive_text()
{
    uint8_t offset                 = 0;

    if(got_sequence_data_playlist_b)
    {
      offset = 2;
    }
    else
    {
      offset = 0;
    }

    count_failed_to_receive_g      = 0;
    receive_data_b                 = truee_val;
    notify_received_data_b         = true;

    memset(buf_receive_ble, 0, sizeof(buf_receive_ble));
    length_data_packet_g          = rxValue_g.length();

    Serial.printf("len: %d \r\n", length_data_packet_g);
  
    if (length_data_packet_g > 3) 
    {
        idx_buf = 0;

        rxValue_g.copy(buf_receive_ble, length_data_packet_g-offset, offset);
        
        if(!mode_kemalingan_b)
        {
          if(!got_sequence_data_playlist_b)
          {
            //memset_pointer(buff_text, 0, 25);
            memset(buff_universal_recv, 0, sizeof(buff_universal_recv));
            collect_data(0, buf_receive_ble, buff_universal_recv, length_data_packet_g - offset, data_text_d);

            receive_complete_g                = 3; 
            mode_display_led_g                = ST_ANIMATION;
            save_to_playlist_b                = true;
            tipe_data_received_g              = data_text_d;

            Serial.printf("save to playlist: %d \r\n", buff_universal_recv);
           
            type_data_to_show_g               = data_gif_d;
            close_gif(ST_ANIMATION, data_text_d);
          }                
          else
          {
            urutan_playlist_g++;
            //save_to_playlist_b                 = true;
            tipe_data_received_g               = data_text_d;

            while(save_to_playlist_b == true);
            memset(buff_textEEPROM, 0, sizeof(buff_textEEPROM)); 
            collect_data(0, buf_receive_ble, buff_textEEPROM, length_data_packet_g - offset, data_text_d);
            progres_status                     = progres_status + (float)(100.00/total_playlist_g);
            notify_progress_b                  = true;

            Serial.printf("save to sequences: %d \r\n", buff_universal_recv);
            
            add_file_to_playlist(&mem_addre_current_g, &buff_textEEPROM[0]);
          }
          
          vTaskDelay(200);
        }
        else
        {
          receive_failed_b = true;
        }
        
        timer0_counter_power_off_g          = 0;
        
    }
    else
    {
        mode_display_led_g = ST_ANIMATION;
    }
}
#else
void receive_text()
{
    bool kirim_data_b              = false;
    uint8_t offset                 = 0;

    if(got_sequence_data_playlist_b)
    {
      offset = 1;
      kirim_data_b = true;
    }
    else
    {
      offset = 0;
      kirim_data_b                 = decide_boolean(rxValue_g[0 + offset]);
    }

    count_failed_to_receive_g      = 0;
    receive_data_b                 = truee_val;
    notify_received_data_b         = true;

    memset(buf_receive_ble, 0, sizeof(buf_receive_ble));
    
    length_data_packet_g          = rxValue_g.length();
  
    if ((length_data_packet_g > 0) && (kirim_data_b)) 
    {
        rxValue_g.copy(buf_receive_ble, 30, 1 + offset);
  
        idx_buf = 0;
  
        memset(buff_text, 0, sizeof(buff_text));
        
        collect_data(0, buf_receive_ble, buff_text, (length_data_packet_g - (1 + offset)), data_text_d);
    
        //Serial.println();
        #ifdef DEBUG_AT_RECEIVE_DATA
        Serial.println("Received text: ");
        for (int i = 0; i <= length_data_packet_g; i++) 
        {
          Serial.print(buf_receive_ble[i]);
        }
        Serial.println();
        #endif
        
        if(!mode_kemalingan_b)
        {
          if(!got_sequence_data_playlist_b)
          {
            receive_complete_g                = 3; 
            mode_display_led_g                = show_animasi_d;
            save_to_playlist_b                = true;
            tipe_data_received_g              = data_text_d;

            close_gif(show_animasi_d, data_text_d);
          }                
          else
          {
            urutan_playlist_g++;
            save_to_playlist_b                 = true;
            tipe_data_received_g               = data_text_d;
            memset(buff_textEEPROM, 0, sizeof(buff_textEEPROM)); 
            memcpy(buff_textEEPROM, buff_text,sizeof(buff_textEEPROM)); 
            progres_status                     = progres_status + (float)(100.00/total_playlist_g);
            notify_progress_b                  = true;
            
            add_file_to_playlist(&mem_addre_current_g, &buff_textEEPROM[0]);
          }
          
          vTaskDelay(200);
        }
        else
        {
          receive_failed_b = true;
        }
        
        timer0_counter_power_off_g          = 0;
        
    }
    else
    {
        mode_display_led_g = show_animasi_d;
    }
}
#endif

/**
 * 
 * @brief function to receive image data then parsing it and collect it
 * 
*/
#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
  void receive_image()
  {
    uint8_t offset                 = 0;

    if(got_sequence_data_playlist_b) //differenciate between image from receive as image data standalone on inside playlist seq
    {
      offset = 2;
    }
    else
    {
      offset = 0;
    }

    sequence_kirim_data_g          = rxValue_g[0 + offset];
    length_data_packet_g           = rxValue_g.length();
    notify_received_data_b         = true;

    // Serial.print(sequence_kirim_data_g);
    // Serial.print("-");     
    // Serial.print(urutan_playlist_g);
    // Serial.print("-");        
    // Serial.println(total_playlist_g);  

    if(!frist_get_b)
    {
      if(sequence_kirim_data_g == 1)      //set failed status when receive 1 at the first seq, seq should start from 0
      {
        failed_flag_b = true;
      } 
    }

    memset(buf_receive_ble, 0, sizeof(buf_receive_ble));
      
    if(length_data_packet_g >= 90)
    {
      frist_get_b = true;

      if(sequence_kirim_data_g == 0)
      {
        idx_buf = 0;
        if(!got_sequence_data_playlist_b)
        {
          //memset_pointer(buff_img, 0, 768);
          memset(buff_universal_recv, 0, sizeof(buff_universal_recv));
        }
        else
        {
          // while(save_to_playlist_b == true)
          // {
          //   Serial.print("#");
          // }
          memset(buff_imgEEPROM, 0, sizeof(buff_imgEEPROM));
        }
      }

      rxValue_g.copy(buf_receive_ble, (length_data_packet_g-1-offset), offset);
          
      if(!got_sequence_data_playlist_b)
      {
        collect_data(sequence_kirim_data_g, buf_receive_ble, buff_universal_recv, (length_data_packet_g-1-offset), data_image_d);
        progres_status                 = (float)((float)(sequence_kirim_data_g+1)/8) * 100.0;
        notify_progress_b              = true;
      }
      else
      {
        collect_data(sequence_kirim_data_g, buf_receive_ble, buff_imgEEPROM, (length_data_packet_g-1-offset), data_image_d);
        progres_status          = progres_status + (float)((float)(1.0/8.0) * (100.00/total_playlist_g));
        notify_progress_b       = true;
      }

      if((sequence_kirim_data_g >= 7) && (sequence_kirim_data_g != 8) && (!failed_flag_b) && (!mode_kemalingan_b))
      {
          if(!got_sequence_data_playlist_b)
          {
            progres_status                     = 99;
            notify_progress_b                  = true;
            
            receive_complete_g                = 3; 
            mode_display_led_g                = ST_ANIMATION;
            save_to_playlist_b                = true;
            tipe_data_received_g              = data_image_d;
            frist_get_b                       = false;

            type_data_to_show_g               = data_gif_d;
            close_gif(ST_ANIMATION, data_image_d);
        
          }
          else
          {
            urutan_playlist_g++;

            //save_to_playlist_b         = true;
            tipe_data_received_g       = data_image_d;
            frist_get_b                = false;

            //progres_status              = progres_status + ((float)100.00/total_playlist_g);
            //notify_progress_b           = true;
            //save_to_playlist_b         = true;

            add_file_to_playlist(&mem_addre_current_g, &buff_imgEEPROM[0]);
            //add_file_to_playlist(&mem_addre_current_g, buff_imgEEPROM);
          } 
      }
      else
      {
          if(sequence_kirim_data_g >= 7)
          {
            receive_failed_b = true;
            failed_flag_b    = false;
          }

          if(mode_kemalingan_b)
          {
            receive_failed_b = true;
          }
      }
        
      timer0_counter_power_off_g          = 0;
    }
    else
    {
      mode_display_led_g = ST_ANIMATION;
    }
  }
#else
  void receive_image()
  {
    uint8_t temp_val[2];
    uint8_t offset                 = 0;

      if(got_sequence_data_playlist_b)
      {
        offset = 2;
      }
      else
      {
        offset = 0;
      }

    receive_data_b                 = truee_val;
    length_data_packet_g           = rxValue_g.length();
    count_failed_to_receive_g      = 0;
    notify_received_data_b         = true;

    temp_val[0]             = hex_to_val(rxValue_g[0 + offset]);
    temp_val[1]             = hex_to_val(rxValue_g[1 + offset]);
    sequence_kirim_data_g   = temp_val[0] * 10 + temp_val[1];

    if(!frist_get_b)
    {
      if(sequence_kirim_data_g == 1)
      {
        //length_data_packet_g = 0;
        failed_flag_b = true;
      } 
    }
    
    memset(buf_receive_ble, 0, sizeof(buf_receive_ble));
      
    if(length_data_packet_g >= 130)
    {
      frist_get_b = true;
      
      if(sequence_kirim_data_g == 0)
      {
          idx_buf = 0;
          memset(buff_img, 0, sizeof(buff_img));
      }

      rxValue_g.copy(buf_receive_ble, 128, 2 + offset);
      
      collect_data(sequence_kirim_data_g, buf_receive_ble, buff_img, (length_data_packet_g - (2 + offset)), data_image_d);

      #ifdef DEBUG_AT_RECEIVE_DATA
      Serial.print("sequence: ");
      Serial.print(sequence_kirim_data_g);
      Serial.print("length: ");
      Serial.println(length_data_packet_g);
      #endif
      
  //     Serial.println("Received: ");
  //     for (int i = 0; i < (length_data_packet_g-(2 + offset)); i++) 
  //     {
  //       Serial.print(buf_receive_ble[i]);
  //     }
  //     Serial.println();
      if(!got_sequence_data_playlist_b)
      {
          progres_status                 = (float)((float)sequence_kirim_data_g/15) * 100.0;
          notify_progress_b                  = true;
      }
    
      if((sequence_kirim_data_g >= 15) && (sequence_kirim_data_g != 16) && (!failed_flag_b) && !mode_kemalingan_b)
      {
          if(!got_sequence_data_playlist_b)
          {
            progres_status               = 99;
            notify_progress_b                  = true;
            
            receive_complete_g                = 3; 
            mode_display_led_g                = show_animasi_d;
            save_to_playlist_b                = true;
            tipe_data_received_g              = data_image_d;
            frist_get_b                       = false;

            close_gif(show_animasi_d, data_image_d);
        
          }
          else
          {
            urutan_playlist_g++;
            memset(buff_imgEEPROM, 0, sizeof(buff_imgEEPROM));
            memcpy(buff_imgEEPROM, buff_img, sizeof(buff_imgEEPROM));
            save_to_playlist_b                = true;
            tipe_data_received_g              = data_image_d;
            frist_get_b                       = false;

            progres_status                    = progres_status + ((float)100.00/total_playlist_g);
            notify_progress_b                 = true;

            add_file_to_playlist(&mem_addre_current_g, &buff_imgEEPROM[0]);
          } 
      }
      else
      {
          if(sequence_kirim_data_g >= 15)
          {
            receive_failed_b = true;
            failed_flag_b    = false;
          }

          if(mode_kemalingan_b)
          {
            receive_failed_b = true;
          }
      }
        
      timer0_counter_power_off_g          = 0;
    }
    else
    {
      mode_display_led_g = show_animasi_d;
    }
  }
#endif


/**
 * 
 * @brief function to receive animation data then parsing it and collect it
 * 
*/
#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
void receive_animasi()
{
    static uint8_t seq_frame        = 0;
    uint8_t offset                 = 0;

    if(got_sequence_data_playlist_b)
    {
      offset = 2;
    }
    else
    {
      offset = 0;
      got_sequence_data_playlist_b = false;
    }
    
    receive_data_b                    = truee_val;
    length_data_packet_g              = rxValue_g.length();
    count_failed_to_receive_g         = 0;
    notify_received_data_b            = true;
    
    if(length_data_packet_g >= 96)
    {

      fps_animasi_received_data_g  = rxValue_g[0 + offset];
      total_frame_animasi_g        = rxValue_g[1 + offset];
      frame_seq_receive_g          = rxValue_g[2 + offset];
      sequence_kirim_data_g        = rxValue_g[3 + offset];

//      Serial.print(frame_seq_receive_g);
//      Serial.print("-");
//      Serial.print(total_frame_animasi_g);
//      Serial.print("-");      
//      Serial.println(sequence_kirim_data_g);  

      if((frame_seq_receive_g == 0) && (sequence_kirim_data_g == 0))
      {
        idx_buf = 0;

        if(!got_sequence_data_playlist_b)
        {
          //memset_pointer(buff_animasi, 0, 15360);
          memset(buff_universal_recv, 0, sizeof(buff_universal_recv));
        }
        else
        {
          memset(buff_animasiEEPROM, 0, sizeof(buff_animasiEEPROM));
        }
      }
  
      rxValue_g.copy(buf_receive_ble, length_data_packet_g - (4 + offset), 4 + offset);

      if(!got_sequence_data_playlist_b)
      {
        collect_data(sequence_kirim_data_g, buf_receive_ble, buff_universal_recv, (length_data_packet_g-(4 + offset)), data_animasi_d);
        progres_status          = (float)((float)(frame_seq_receive_g+1)/total_frame_animasi_g) * 100.0;
        notify_progress_b       = true;
      }
      else
      {
        collect_data(sequence_kirim_data_g, buf_receive_ble, buff_animasiEEPROM, (length_data_packet_g-(4 + offset)), data_animasi_d);
        //progres_status          = progres_status + ((float)(100.00/total_playlist_g) / total_frame_animasi_g);
        if(frame_seq_receive_g != seq_frame)
        {
          progres_status          = progres_status + (float)((float)(1.0/total_frame_animasi_g)) * (100.00/total_playlist_g);
          notify_progress_b       = true;
          seq_frame = frame_seq_receive_g;
        }
      }

      if(sequence_kirim_data_g >= 7)
      {
        sequence_kirim_data_g               = 0;
        save_to_playlist_b                  = false;

        if(frame_seq_receive_g >= (total_frame_animasi_g - 1))
        {
          //Serial.println("1 animation ready to save");
          if(!mode_kemalingan_b)
          {
              if(!got_sequence_data_playlist_b)
              {
                receive_complete_g                = 3; 
                frame_seq_receive_g               = 0;
                sequence_kirim_data_g             = 0;
                tipe_data_received_g              = data_animasi_d;
                save_to_playlist_b                = true;
                data_size_animasi_g               = total_frame_animasi_g * 768;
                mode_display_led_g                = ST_ANIMATION;

                type_data_to_show_g               = data_gif_d;
                close_gif(ST_ANIMATION, data_animasi_d);
              }
              else
              {
                //Serial.print("prepare to save to playlist");
                tipe_data_received_g              = data_animasi_d;
                urutan_playlist_g++;
                
                //save_to_playlist_b = true;
                add_file_to_playlist(&mem_addre_current_g, buff_animasiEEPROM);
                //memset(buff_animasiEEPROM, 0, sizeof(buff_animasiEEPROM));
                //idx_buf = 0;
              } 
              
              frame_seq_receive_g             = 0;

              #if(DEBUG_SERIAL_ALL_RECEIVED_DATA == 1)
                Serial.print("[INFO] anim size: ");  
                Serial.println(data_size_animasi_g);
                Serial.print("[INFO] last message:");
                for (int i = 0; i < length_data_packet_g; i++) 
                {
                  Serial.printf(" %02x", rxValue_g[i]);
                }
                Serial.println();  
              #endif


          }
          else
          {
            receive_failed_b = true;
          }
        }
        else
        {
          //Serial.println("masuk sini");
          //receive_failed_b = true;
        }
      }
      
      timer0_counter_power_off_g          = 0;
    }
    else
    {
      mode_display_led_g = ST_ANIMATION;
    }

}
#else
void receive_animasi()
{
    static uint8_t seq_frame        = 0;
    uint8_t temp_val[2]            = {0};
    uint8_t offset                 = 0;

    if(got_sequence_data_playlist_b)
    {
      offset = 2;
    }
    else
    {
      offset = 0;
      got_sequence_data_playlist_b = false;
    }
    
    receive_data_b                    = truee_val;
    length_data_packet_g              = rxValue_g.length();
    count_failed_to_receive_g         = 0;
    notify_received_data_b            = true;
    
    if(length_data_packet_g >= 130)
    {
      if(!got_sequence_data_playlist_b)
      {
          fps_animasi_received_data_g  = check_got_ascii(rxValue_g[0]) - 0x30;
      
          temp_val[0]                  = hex_to_val(rxValue_g[1]);
          temp_val[1]                  = hex_to_val(rxValue_g[2]);
          total_frame_animasi_g        = temp_val[0] * 10 + temp_val[1];
      }
      else
      {
          temp_val[0]                  = hex_to_val(rxValue_g[0 + offset]);
          temp_val[1]                  = hex_to_val(rxValue_g[1 + offset]);
          total_frame_animasi_g        = temp_val[0] * 10 + temp_val[1];
          
          fps_animasi_received_data_g  = check_got_ascii(rxValue_g[2 + offset]) - 0x30;
      }
      
      temp_val[0]                  = hex_to_val(rxValue_g[3 + offset]);
      temp_val[1]                  = hex_to_val(rxValue_g[4 + offset]);
      sequence_kirim_data_g        = temp_val[0] * 10 + temp_val[1];
  
      if((frame_seq_receive_g == 0) && (sequence_kirim_data_g == 0))
      {
        //Serial.println("[INFO] idx_buf animasi reset to 0");
        idx_buf = 0;
        memset(buff_animasi, 0, sizeof(buff_animasi));
      }
  
      rxValue_g.copy(buf_receive_ble, 128, 5 + offset);
      //Serial.println();
      collect_data(sequence_kirim_data_g, buf_receive_ble, buff_animasi, (length_data_packet_g - (5 + offset)), data_animasi_d);
      //Serial.println();
      //memcpy

//      for (int i = 0; i < 10; i++) 
//      {
//        Serial.print(rxValue_g[i]);
//      }
//      Serial.println();  
//      Serial.print("fps: ");
//      Serial.print(fps_animasi_received_data_g);
//      Serial.print(" frame total: ");
//      Serial.print(rxValue_g[1]);
//      Serial.print(rxValue_g[2]);
//        Serial.print(" frm: ");
//        Serial.println(frame_seq_receive_g);
//      Serial.print(" total: "); 
//      Serial.println(total_frame_animasi_g);
//      Serial.print(" length: ");
//      Serial.println(length_data_packet_g);
//      Serial.println("Received: ");
//      for(uint8_t zz = 0; zz < 5; zz++)
//      {
//        Serial.print(buff_animasi[idx_buf - 64 + zz], HEX);
//        Serial.print(" ");
//      }
//      Serial.println();
      
//      for (int i = 0; i < length_data_packet_g; i++) 
//      {
//        Serial.print(rxValue_g[i]);
//      }
//      Serial.println();
  
      
      if(sequence_kirim_data_g >= 15)
      {
        frame_seq_receive_g++;
        sequence_kirim_data_g               = 0;
        save_to_playlist_b                  = false;

        if(!got_sequence_data_playlist_b)
        {
          progres_status      = (float)((float)frame_seq_receive_g/total_frame_animasi_g) * 100.0;
          notify_progress_b       = true;
        }
        else
        {
          progres_status      = progres_status + ((float)(100.00/total_playlist_g) / total_frame_animasi_g);
          notify_progress_b       = true;
        }
//        Serial.print(frame_seq_receive_g);
//        Serial.print(" - ");
//        Serial.println(total_frame_animasi_g);
        //sisa_frame
        
//        Serial.print("progress: (%)");
//        Serial.println(progres_status);
        
        if(frame_seq_receive_g >= total_frame_animasi_g)
        {
          //Serial.println("masuk sini 1");
          if(!mode_kemalingan_b)
          {
              if(!got_sequence_data_playlist_b)
              {
                receive_complete_g                = 3; 
                frame_seq_receive_g               = 0;
                sequence_kirim_data_g             = 0;
                tipe_data_received_g              = data_animasi_d;
                save_to_playlist_b                = true;
                data_size_animasi_g               = total_frame_animasi_g * 1024;
                mode_display_led_g                = show_animasi_d;

                close_gif(show_animasi_d, data_animasi_d);
              }
              else
              {
                tipe_data_received_g              = data_animasi_d;
                urutan_playlist_g++;
                //clear_LED(16,16);
                memset(buff_animasiEEPROM, 0, sizeof(buff_animasiEEPROM));
                memcpy(buff_animasiEEPROM, buff_animasi, sizeof(buff_animasiEEPROM));
                save_to_playlist_b = true;
                add_file_to_playlist(&mem_addre_current_g, &buff_animasiEEPROM[0]);
              } 
              
              frame_seq_receive_g             = 0;
          }
          else
          {
            receive_failed_b = true;
          }
        }
        else
        {
          //Serial.println("masuk sini");
          //receive_failed_b = true;
        }
      }
      
      timer0_counter_power_off_g          = 0;
    }
    else
    {
      mode_display_led_g = show_animasi_d;
    }
}
#endif

/**
 * 
 * @brief function to receive gif file data then parsing it and collect it
 * 
*/
#if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
void receive_GIF()
{
   static bool seq_reach_hundred = false;
   static bool first_recv_gif_data = false;
   uint8_t last_gif_buff_arr[100];
   uint8_t offset_idx     = 1;

   
   if((size_gif_file_received + 96) <= MAX_GIF_SIZE)
   {
        receive_data_b                 = truee_val;
        length_data_packet_g           = rxValue_g.length();
        count_failed_to_receive_g      = 0;
        notify_received_data_b         = true;
      
        sequence_kirim_data_g          = rxValue_g[0];

        memset(buf_receive_ble, 0, sizeof(buf_receive_ble));
       
        rxValue_g.copy(buf_receive_ble, (length_data_packet_g - offset_idx), offset_idx);

        #if(SPECIAL_DEBUG_FOR_GIF == 1)
          Serial.printf("%d - %d - %d - %d - %d -%d\r\n", length_data_packet_g, sequence_kirim_data_g, times_sent, first_recv_gif_data, block_rest_of_gif_file_b, gif_file_is_end_b);
        #endif

        if((sequence_kirim_data_g == 0))
        {
            this_is_gif_header_b          = check_incoming_gif_header(buf_receive_ble, offset_idx);
            //times_sent = times_sent + 1;
            if(this_is_gif_header_b)
            {
              //Serial.println("got first header of GIF");
              mode_display_led_g          = ST_ANIMATION;
              gif_show_enable              = false;
              //tipe_data_received_g        = data_gif_d;
              idx_buf                     = 0;
              gif_file_is_end_b           = false;
              block_rest_of_gif_file_b    = false;
              type_data_to_show_g         = data_animasi_default;
              
              if(file_gif_opened)
              {   
                file_gif_opened               = false;   
                play_animasi_playlist_g       = false;          
                gif_h.close();
              }
            
              progres_status              = 0;
              memset(buff_universal_recv, 0, sizeof(buff_universal_recv));
            }
            else
            {
              //Serial.println("No header");
              gif_file_is_end_b           = true;
              block_rest_of_gif_file_b    = true;
            }
        }
        else
        {
          // if((sequence_kirim_data_g == 0) && (times_sent == 1))
          // {
          //   Serial.println("time sent reset to 0");
          //   times_sent = 0;
          // }

          // if(!first_recv_gif_data && (sequence_kirim_data_g != 0))
          // {
          //   first_recv_gif_data = true;
          //   len_last_gif_msg    = length_data_packet_g - offset_idx;
          //   total_max_data_est  = ((sequence_kirim_data_g+1) * 96) - (96 - len_last_gif_msg);
            
          //   for(uint8_t x = 0; x < len_last_gif_msg; x++)
          //   {
          //     last_gif_buff_arr[x] = buf_receive_ble[x];
          //   }

          //   Serial.print(last_gif_buff_arr[len_last_gif_msg-2]);
          //   Serial.print("-");
          //   Serial.print(last_gif_buff_arr[len_last_gif_msg-1]);
          //   Serial.print("  size gif would be received: ");
          //   Serial.println(total_max_data_est);
            
          //   //block_rest_of_gif_file_b    = true;
          // }
          //by past incomeing file gif sequence below 99-- 
        }
        
        if(!block_rest_of_gif_file_b)
        {     
            // if(times_sent == 0)
            // {
            //   block_rest_of_gif_file_b = true;
            // }

            if(!gif_file_is_end_b)
            {
                //Serial.println("gif file is end not found");
                collect_data(sequence_kirim_data_g, buf_receive_ble, buff_universal_recv, (length_data_packet_g - offset_idx), data_gif_d);
                size_gif_file_received        = idx_buf;
                progres_status                = (float)(((sequence_kirim_data_g + 1.0) * 96.0)/(float)MAX_GIF_SIZE) * 100.0;
                notify_progress_b             = true;
            }
            
            #if(SPECIAL_DEBUG_FOR_GIF == 1)
              Serial.print("last GIF: ");
              Serial.println(gif_file_is_end_b);
            #endif

            if(gif_file_is_end_b)
            {
                if((check_header_gif(buff_universal_recv)) && (!mode_kemalingan_b))
                {
                  //copy rest of last gif msg
                  // for(uint8_t y = 0; y < len_last_gif_msg; y++)
                  // {
                  //   buff_universal_recv[idx_buf + y] = last_gif_buff_arr[y];
                  // }

                  progres_status                    = 99;
                  notify_progress_b                 = true;
                  first_recv_gif_data               = false;                  
                  
                  receive_complete_g                = 3;
                  mode_display_led_g                = ST_ANIMATION;
                  //gif_show_enable                   = true;
                  tipe_data_received_g              = data_gif_d;
                  //size_gif_file_received            = size_gif_file_received + 1;
                  size_gif_file_received            = idx_buf + (length_data_packet_g - 1);
                  //animation_gif_repeat              = 20;
                  block_rest_of_gif_file_b          = true;
                  save_to_playlist_b                = true;
                  total_max_data_est                = 0x00;

                  #if(SPECIAL_DEBUG_FOR_GIF == 1)
                    Serial.print("====== FINISH ================ GIF SIZE: ");
                    Serial.println(size_gif_file_received);

                    Serial.print("[INFO] last message:");
                    for (int i = 0; i < rxValue_g.length(); i++) 
                    {
                      Serial.print(rxValue_g[i], HEX);
                    }
                    Serial.println();  
                  #endif

                }
                else
                {
                  receive_failed_b = true;
                }
              }
              else
              {
                  block_rest_of_gif_file_b          = false;
              }  
              timer0_counter_power_off_g          = 0;
        }
        else
        {
          #if(SPECIAL_DEBUG_FOR_GIF == 1)
          Serial.print("rest of GIF --> ");
          for (uint16_t i = 0; i < rxValue_g.length(); i++) 
          {
              Serial.print(rxValue_g[i], HEX);
          }
          Serial.println(); 
          #endif
          //by past incomeing file gif -- rest of it to prevent from copy unnecesarry byte to gif file
        }

   }
   else
   {
     mode_display_led_g = ST_ANIMATION;
   }
}
#else
void receive_GIF()
{
   uint8_t temp_val[2];
   static bool seq_reach_hundred = false;
   uint8_t offset_idx     = 2;
   
   if(size_gif_file_received <= MAX_GIF_SIZE)
   {
        receive_data_b                 = truee_val;
        length_data_packet_g           = rxValue_g.length();
        count_failed_to_receive_g      = 0;
        notify_received_data_b         = true;
      
        if(!seq_reach_hundred)
        {
          temp_val[0]                    = hex_to_val(rxValue_g[0]);
          temp_val[1]                    = hex_to_val(rxValue_g[1]);
          sequence_kirim_data_g          = temp_val[0] * 10 + temp_val[1];
          
          if(sequence_kirim_data_g == 99)
          {
            seq_reach_hundred              = true;
          }
          
          offset_idx                     = 2;
        }
        else
        {
          temp_val[0]                    = hex_to_val(rxValue_g[0]);
          temp_val[1]                    = hex_to_val(rxValue_g[1]);
          temp_val[2]                    = hex_to_val(rxValue_g[2]);
          sequence_kirim_data_g          = temp_val[0] * 100 + temp_val[1] * 10 + temp_val[2]; 
          offset_idx                     = 3;
        }       

        memset(buf_receive_ble, 0, sizeof(buf_receive_ble));
       
        if(sequence_kirim_data_g == 1)
        {
            rxValue_g.copy(buf_receive_ble, (length_data_packet_g - offset_idx), offset_idx);
            this_is_gif_header_b          = check_incoming_gif_header(buf_receive_ble, offset_idx);
            if(this_is_gif_header_b)
            {
              mode_display_led_g          = show_animasi_d;
              tipe_data_received_g        = type_data_to_show_g;
              gif_file_is_end_b           = false;
              idx_buf_gif_g               = 0;
              block_rest_of_gif_file_b    = false;

              
              if(file_gif_opened)
              {
                gif_show_enable               = false;
                file_gif_opened               = false;                
                gif_h.close();
              }

              progres_status              = 0;
              memset(buffer_GIF, 0, sizeof(buffer_GIF));
            }
            else
            {
              gif_file_is_end_b           = true;
              block_rest_of_gif_file_b    = true;
            }
        }
        else
        {
          //by past incomeing file gif sequence below 99-- 
        }
        
        if(!block_rest_of_gif_file_b)
        {
            rxValue_g.copy(buf_receive_ble, (length_data_packet_g - offset_idx), offset_idx);

            #ifdef DEBUG_AT_RECEIVE_DATA
            Serial.print("seq: ");
            Serial.print(sequence_kirim_data_g);
            Serial.print(" --len: ");
            Serial.print(length_data_packet_g);
            Serial.print(" --size: ");
            Serial.print(size_gif_file_received);
            Serial.print(" -->99: ");
            Serial.println(seq_reach_hundred);            
          
            for (int i = 0; i < 5; i++) 
            {
              Serial.print(rxValue_g[i]);
            }
            Serial.println();   
            #endif
          
          
            if(!gif_file_is_end_b)
            {
                collect_gif(sequence_kirim_data_g, buf_receive_ble, buffer_GIF, (length_data_packet_g - offset_idx));
                size_gif_file_received        = idx_buf_gif_g;
                progres_status                = (float)((float)(sequence_kirim_data_g*70)/MAX_GIF_SIZE) * 100.0;
                notify_progress_b             = true;
            }
          
            
            if(gif_file_is_end_b)
            {
                if(check_header_gif(buffer_GIF) and !mode_kemalingan_b)
                {
                  progres_status                    = 99;
                  notify_progress_b                 = true;
                  
                  receive_complete_g                = 3;
                  mode_display_led_g                = show_animasi_d;
                  gif_show_enable                   = true;
                  tipe_data_received_g              = data_gif_d;
                  size_gif_file_received            = size_gif_file_received + 1;
                  animation_gif_repeat              = 15;
                  seq_reach_hundred                 = false;
                  block_rest_of_gif_file_b          = true;
                  
                  Serial.print("====== FINISH ================ GIF SIZE: ");
                  Serial.println(size_gif_file_received);

                  #ifdef DEBUG_AT_RECEIVE_DATA
                  Serial.print("[INFO] last message:");
                  for (int i = 0; i < rxValue_g.length(); i++) 
                  {
                    Serial.print(rxValue_g[i]);
                  }
                  Serial.println();  
                  #endif

                }
                else
                {
                  receive_failed_b = true;
                }
            }
            else
            {
                block_rest_of_gif_file_b          = false;
            }  
            timer0_counter_power_off_g          = 0;
        }
        else
        {
          #ifdef DEBUG_AT_RECEIVE_DATA
          Serial.print("rest of GIF --> ");
          for (int i = 0; i < rxValue_g.length(); i++) 
          {
              Serial.print(rxValue_g[i]);
          }
          Serial.println(); 
          #endif
          //by past incomeing file gif -- rest of it to prevent from copy unnecesarry byte to gif file
        }

   }
   else
   {
    mode_display_led_g = show_animasi_d;
   }
}
#endif

//OK
