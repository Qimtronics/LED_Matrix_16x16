/**
 * 
 * @brief interrupt function timer 0 to calculate for vsupply value in volt to esp32 and counter to power off led 
 * 
 */
void IRAM_ATTR timer0_handler(void)  //100 ms
{
    timer0_counter_vsupply_status_g++;
    timer0_counter_power_off_g++;
}

/**
 * 
 * @brief interrupt function timer 1 for mpu sampling
 * 
 */
void IRAM_ATTR timer1_handler(void) //50 ms
{    
     portENTER_CRITICAL_ISR(&timerMux_mpu); 
     sampling_data_mpu_b  = true;
     portEXIT_CRITICAL_ISR(&timerMux_mpu);
}

/**
 * 
 * @brief interrupt function timer 2 to calulate duration of right and left active remain active once automatic smart sign is activated
 * 
 */
void IRAM_ATTR timer2_handler(void) //250 ms
{
  if(b_timer2_active)
  {
     timer2_tick_g++;
     if(timer2_tick_g >= 65535)
     {
       timer2_tick_g = 0;
     }
  }
}

/**
 * 
 * @brief interrupt function timer 3 to calculate clock support of widget feature
 * 
 */
void IRAM_ATTR timer3_handler(void) //1000 ms
{
   timer3_tick_g = true;
   
   if(auto_power_off_time_g != off_time_never_d)
   {
      timer3_counter_power_off_g++;
   }

   if((mode_display_led_g == ST_WIDGET) && (tipe_widget_g != widget_timer_d))
   {
       time_seconds_g++;
       if(tipe_widget_g == widget_stopwatch_d)
       {
         ganti_tampilan_waktu_b = true;
       }
       
       if(time_seconds_g > 59)
       {
          ganti_tampilan_waktu_b = true;
          time_minutes_g++;
          time_seconds_g = 0;
          if(time_minutes_g > 59)
          {
             time_minutes_g = 0;
             time_seconds_g = 0;
             time_hours_g++;
             if(time_hours_g > 23)
             {
                time_hours_g   = 0;
                time_minutes_g = 0;
                time_seconds_g = 0;
             }
          }
       }
   }
   else if((mode_display_led_g == ST_WIDGET) && (tipe_widget_g == widget_timer_d))
   {
      time_seconds_g--;
      if(time_seconds_g < 0)
       {  
          time_minutes_g--;
          time_seconds_g = 59;
          if(time_minutes_g < 0)
          {
             time_minutes_g = 59;
             time_seconds_g = 59;
             time_hours_g--;
//             if(time_hours_g < 0)
//             {
//                time_hours_g   = 59;
//                time_minutes_g = 59;
//                time_seconds_g = 59;
//             }
          }
       }
   }
}

void IRAM_ATTR dmpDataReady() {
    mpuInterrupt_g = true;
}

//OK
