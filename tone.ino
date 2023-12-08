void tone_alarm_maling_blink_on()
{
    static uint8_t counter_to_stop;
    static bool stop_buzzering = false;

    counter_to_stop++;
    if(!stop_buzzering)
    {
      if(counter_to_stop > 2)
      {
        for(uint16_t idx_led = 0; idx_led < (HEIGHT_ * WIDTH_); idx_led++)
        {
           leds[idx_led] =  CRGB::Red;
        }
        FastLED.show();

        tone_always_on(BUZZER_PIN, NOTE_G4, BUZZER_CHANNEL);
        //tone(BUZZER_PIN, NOTE_G4, 10, BUZZER_CHANNEL);

        counter_to_stop   = 0;
        stop_buzzering    = true;
      }
      Serial.printf("A buzz state: %d %d \r\n", counter_to_stop, stop_buzzering);
    }
    else
    {
      if(counter_to_stop > 5)
      {      
        for(uint16_t idx_led = 0; idx_led < (HEIGHT_ * WIDTH_); idx_led++)
        {
            leds[idx_led] =  CRGB::Black;  
        }
        FastLED.show();

        noTone(BUZZER_PIN, BUZZER_CHANNEL); 
        
        counter_to_stop   = 0;
        stop_buzzering    = false;
      }
      Serial.printf("B buzz state: %d %d \r\n", counter_to_stop, stop_buzzering);
    }
}

void buzzer_alarm_thief()
{
    digitalWrite(BUZZER_PIN, HIGH);
    for(uint16_t idx_led = 0; idx_led < (HEIGHT_ * WIDTH_); idx_led++)
    {
       leds[idx_led] =  CRGB::Red;
    }
    FastLED.show();

    delay(1000); 
    
    digitalWrite(BUZZER_PIN, LOW);
    for(uint16_t idx_led = 0; idx_led < (HEIGHT_ * WIDTH_); idx_led++)
    {
        leds[idx_led] =  CRGB::Black;
    }
    FastLED.show();
}

void tone_warning()
{
     tone(BUZZER_PIN, NOTE_G4, 80, BUZZER_CHANNEL);
     noTone(BUZZER_PIN, BUZZER_CHANNEL);
     delay(80);
     tone(BUZZER_PIN, NOTE_G4, 80, BUZZER_CHANNEL);
     noTone(BUZZER_PIN, BUZZER_CHANNEL);
     delay(80);
     tone(BUZZER_PIN, NOTE_G4, 80, BUZZER_CHANNEL);
     noTone(BUZZER_PIN, BUZZER_CHANNEL);
     delay(80);
}

void tone_mode_safety()
{
     tone(BUZZER_PIN, NOTE_C4, 100, BUZZER_CHANNEL);
     noTone(BUZZER_PIN, BUZZER_CHANNEL);
     delay(80);
     tone(BUZZER_PIN, NOTE_D4, 100, BUZZER_CHANNEL);
     noTone(BUZZER_PIN, BUZZER_CHANNEL);
     delay(80);
     tone(BUZZER_PIN, NOTE_E4, 100, BUZZER_CHANNEL);
     noTone(BUZZER_PIN, BUZZER_CHANNEL);
}

void tone_mode_setting()
{
    tone(BUZZER_PIN, NOTE_C4, 80, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    delay(80);
}

void tone_timer_end(uint8_t repeat)
{
    for(uint8_t x = 0; x < repeat; x++)
    {
      tone(BUZZER_PIN, NOTE_G4, 80, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      delay(80);
      tone(BUZZER_PIN, NOTE_A4, 80, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      delay(80);
      tone(BUZZER_PIN, NOTE_B4, 80, BUZZER_CHANNEL);
      noTone(BUZZER_PIN, BUZZER_CHANNEL);
      delay(80);
    }
}

void tone_game_control()
{
    tone(BUZZER_PIN, NOTE_D4, 80, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    //vTaskDelay(80);
}

void tone_game_over()
{
    tone(BUZZER_PIN, NOTE_E4, 100, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);
    tone(BUZZER_PIN, NOTE_F4, 80, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);
    tone(BUZZER_PIN, NOTE_E4, 100, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);
    tone(BUZZER_PIN, NOTE_F4, 1000, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);
}

void show_info_high_score()
{
  FastLED.clear();
  delay(100);
  sprintf(text_info_animasi_g, "High score!!");
  //memainkan animasi pembuka ("Running text")
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(5);
  matrix->setTextColor(matrix->Color(color_table_global_g[6][0], color_table_global_g[6][1], color_table_global_g[6][2]));
  
  show_text_animation_b           = true;
  total_pixel_led_used_g          = (String(text_info_animasi_g).length() * 11) + 32;
  
  while(show_text_animation_b)
  {
    matrix->fillScreen(0);
    matrix->setCursor(cursor_x_text_g, cursor_y_text_g);
    matrix->print(text_info_animasi_g);
    matrix->setFont(&Dialog_bold_13);
    matrix->setTextColor(matrix->Color(color_table_global_g[6][0], color_table_global_g[6][1], color_table_global_g[6][2]));
    if(--cursor_x_text_g < -total_pixel_led_used_g) {
      cursor_x_text_g = matrix->width();
      show_text_animation_b = false;
    }
    matrix->show();
    delay(25);//speed_animasi_text_g);
  }
}

void tone_high_score()
{
    tone(BUZZER_PIN, NOTE_E4, 1000, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);
    tone(BUZZER_PIN, NOTE_D4, 80, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);
    tone(BUZZER_PIN, NOTE_F4, 100, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);
    tone(BUZZER_PIN, NOTE_D4, 80, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);
    tone(BUZZER_PIN, NOTE_F4, 500, BUZZER_CHANNEL);
    noTone(BUZZER_PIN, BUZZER_CHANNEL);
    vTaskDelay(80);   
}

//OK--