uint16_t buf_led_widget_g[256];
const byte angka_3x5    [0xA][3] = {{0x1F, 0x11, 0x1F}, //0
                                    {0x00, 0x02, 0x1F}, //1
                                    {0x1D, 0x15, 0x17}, //2
                                    {0x15, 0x15, 0x1F}, //3
                                    {0x07, 0x04, 0x1F}, //4
                                    {0x17, 0x15, 0x1D}, //5
                                    {0x1F, 0x15, 0x1D}, //6
                                    {0x01, 0x01, 0x1F}, //7
                                    {0x1F, 0x15, 0x1F}, //8
                                    {0x17, 0x15, 0x1F}}; //9  
                                        
void tampilan_border(uint8_t border_color)
{
  for(uint8_t sumbu_x = 0; sumbu_x < 15; sumbu_x++)
  {
    leds[getPixelNumber(sumbu_x, 0)] = CRGB(color_table_global_g[border_color][0], color_table_global_g[border_color][1], color_table_global_g[border_color][2]);
  }

  for(uint8_t sumbu_y = 0; sumbu_y < 15; sumbu_y++)
  {
    leds[getPixelNumber(15, sumbu_y)] = CRGB(color_table_global_g[border_color][0], color_table_global_g[border_color][1], color_table_global_g[border_color][2]);
  }

  for(uint8_t sumbu_x = 15; sumbu_x > 0; sumbu_x--)
  {
    leds[getPixelNumber(sumbu_x, 15)] = CRGB(color_table_global_g[border_color][0], color_table_global_g[border_color][1], color_table_global_g[border_color][2]);
  }

  for(uint8_t sumbu_y = 15; sumbu_y > 0; sumbu_y--)
  {
    leds[getPixelNumber(0, sumbu_y)] = CRGB(color_table_global_g[border_color][0], color_table_global_g[border_color][1], color_table_global_g[border_color][2]);
  }
}

void parsing_puluhan_satuan(uint8_t digit, uint8_t *p_puluhan, uint8_t* p_satuan)
{
    *p_puluhan = digit/10;
    *p_satuan  = digit%10;                                            
}

void clear_spesifik_area(uint8_t posisi_x, uint8_t posisi_y, uint8_t len_x, uint8_t len_y)
{
  for(uint8_t pos_x = posisi_x; pos_x < (posisi_x + len_x); pos_x++)
  {
    for(uint8_t pos_y = posisi_y; pos_y < (posisi_y + len_y); pos_y++)
    {
       leds[getPixelNumber(pos_x, 15-pos_y)] = CRGB(0, 0, 0); 
    }
  }
}

void tampilkan_di_led()
{
   FastLED.show();
}

void konversi_digit_to_array (uint8_t angka, uint8_t posisi_x, uint8_t posisi_y, uint8_t color)
{
   uint8_t bit_status         = 0; 
   uint8_t posisi_y_increment = posisi_y;
   
   for(uint8_t idx = 0; idx < 3;idx++)
   {    
        posisi_y_increment = posisi_y;
        
        for(uint8_t bit_array = 0; bit_array < 5; bit_array++)
        {
            bit_status                               = angka_3x5[angka][idx] >> bit_array & 0x01; 

            if(bit_status == 1)
            {            
              leds[getPixelNumber(posisi_x, 15-posisi_y_increment)] = CRGB(color_table_global_g[color][0], color_table_global_g[color][1], color_table_global_g[color][2]);
            }
            else
            {
              leds[getPixelNumber(posisi_x, 15-posisi_y_increment)] = CRGB(color_table_global_g[0][0], color_table_global_g[0][1], color_table_global_g[0][2]);
            }
            
            posisi_y_increment++;
        }
        posisi_x++;
   }
}

void led_seconds_indicator(bool b_on_off)
{
   switch(b_on_off)
   {
      case true:
        leds[getPixelNumber(12, 15-3)] = CRGB(color_table_global_g[1][0], color_table_global_g[1][1], color_table_global_g[1][2]);
        leds[getPixelNumber(12, 15-5)] = CRGB(color_table_global_g[1][0], color_table_global_g[1][1], color_table_global_g[1][2]);
        tampilkan_di_led();
      break;
      case false:
        clear_spesifik_area(12, 3, 1, 3);
        tampilkan_di_led();
      break;
   }
}

void widget_clock()
{  
   uint8_t puluhan                = 0;
   uint8_t satuan                 = 0;

   if(ganti_tampilan_waktu_b)
   { 
     if(time_minutes_g == 0)
     {
        clear_spesifik_area(4,15-2,11,7);
        tampilkan_di_led();
     }
     //clear_LED(WIDTH, HEIGHT_);
     tampilan_border(2);
     parsing_puluhan_satuan(time_hours_g, &puluhan, &satuan);
     konversi_digit_to_array(puluhan, 4, 2, 2);
     konversi_digit_to_array(satuan,  8, 2, 2);
     parsing_puluhan_satuan(time_minutes_g, &puluhan, &satuan);
     konversi_digit_to_array(puluhan, 4, 8, 2);
     konversi_digit_to_array(satuan,  8, 8, 2);
     tampilkan_di_led();
     ganti_tampilan_waktu_b = false;
   }
   else
   {
     if(timer3_tick_g)
     {
       if((time_seconds_g % 2) == 0)
       {
          led_seconds_indicator(true);
       }
       else
       {
          led_seconds_indicator(false);
       }

      #if(DEBUG_WIDGET_RESULT == 1)
       Serial.print("[INFO] Jam saat ini: ");
       Serial.print(time_hours_g);
       Serial.print(" : ");
       Serial.print(time_minutes_g);
       Serial.print(" : ");
       Serial.println(time_seconds_g);
      #endif
       
       timer3_tick_g = false;
     }
   }
}

void widget_timer()
{
    uint8_t puluhan                = 0;
    uint8_t satuan                 = 0;
    
    if(timer3_tick_g)
    {
       //clear_LED(WIDTH_, HEIGHT_);
       tampilan_border(5);       
       parsing_puluhan_satuan(time_minutes_g, &puluhan, &satuan);
       konversi_digit_to_array(puluhan, 4, 2, 5);
       konversi_digit_to_array(satuan,  8, 2, 5);
       parsing_puluhan_satuan(time_seconds_g, &puluhan, &satuan);
       konversi_digit_to_array(puluhan, 4, 8, 5);
       konversi_digit_to_array(satuan,  8, 8, 5);
       tampilkan_di_led();

       if((time_minutes_g == 0) && (time_seconds_g == 0))
       {
          tone_timer_end(4);
       }
       
       timer3_tick_g = false;

      #if(DEBUG_WIDGET_RESULT == 1)
       Serial.print("[INFO] Timer saat ini: ");
       Serial.print(time_minutes_g);
       Serial.print(" : ");
       Serial.println(time_seconds_g);
      #endif
       
    }
}

void widget_stopwatch()
{
    uint8_t puluhan                = 0;
    uint8_t satuan                 = 0;
    
    if(timer3_tick_g)
    {
       //clear_LED(WIDTH_, HEIGHT_);
       tampilan_border(1);
       parsing_puluhan_satuan(time_minutes_g, &puluhan, &satuan);
       konversi_digit_to_array(puluhan, 4, 2, 1);
       konversi_digit_to_array(satuan,  8, 2, 1);
       parsing_puluhan_satuan(time_seconds_g, &puluhan, &satuan);
       konversi_digit_to_array(puluhan, 4, 8, 1);
       konversi_digit_to_array(satuan,  8, 8, 1);
       tampilkan_di_led();
       timer3_tick_g = false;

      #if(DEBUG_WIDGET_RESULT == 1)
       Serial.print("[INFO] stopwatch saat ini: ");
       Serial.print(time_minutes_g);
       Serial.print(" : ");
       Serial.println(time_seconds_g);
      #endif
       
    }  
}

void widget_score_board()
{   
    uint8_t puluhan                = 0;
    uint8_t satuan                 = 0;
         
    if((prev_score_a_g != score_tim_a_g) || (prev_score_b_g != score_tim_b_g) || (new_entry_b))
    {

       tampilan_border(7);

       if((score_tim_a_g > 99))
       {
         score_tim_a_g = 0;
       }

       if((score_tim_b_g > 99))
       {
         score_tim_b_g = 0;
       }
              
       prev_score_a_g  = score_tim_a_g;
       prev_score_b_g  = score_tim_b_g;  
       
       parsing_puluhan_satuan(score_tim_a_g, &puluhan, &satuan);
       konversi_digit_to_array(puluhan, 4, 2, 4);
       konversi_digit_to_array(satuan,  8, 2, 4);
       parsing_puluhan_satuan(score_tim_b_g, &puluhan, &satuan);
       konversi_digit_to_array(puluhan, 4, 8, 3);
       konversi_digit_to_array(satuan,  8, 8, 3);
      
       tampilkan_di_led();
       
       new_entry_b      = false;
//       score_a_g = 0;
//       score_b_g = 0;
     #if(DEBUG_WIDGET_RESULT == 1)
       Serial.print("[INFO] score saat ini: A: ");
       Serial.print(score_tim_a_g);
       Serial.print(" B: ");
       Serial.println(score_tim_b_g);       
     #endif   
    }
}


//OK--