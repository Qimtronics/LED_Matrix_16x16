/**
 * 
 * @brief function for getting boot of esp32 and all state machine functions
 * 
*/
int boot_up_and_setup(int curr_state)
{
  sprintf(text_animasi_g, "Ciqa2");
  ble_config();
  gif_h.begin(LITTLE_ENDIAN_PIXELS);

  mode_display_led_g =  ST_ANIMATION;

//memainkan animasi pembuka ("Running text")
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(5);
  matrix->setTextColor(matrix->Color(color_table_global_g[6][0], color_table_global_g[6][1], color_table_global_g[6][2]));
  
  show_text_animation_b           = true;
  total_pixel_led_used_g          = (String(text_animasi_g).length() * 11) + 32;
  
  while(show_text_animation_b)
  {
    matrix->fillScreen(0);
    matrix->setCursor(cursor_x_text_g, cursor_y_text_g);
    matrix->print(text_animasi_g);
    matrix->setFont(&Dialog_bold_13);
    matrix->setTextColor(matrix->Color(color_table_global_g[6][0], color_table_global_g[6][1], color_table_global_g[6][2]));
    if(--cursor_x_text_g < -total_pixel_led_used_g) {
      cursor_x_text_g = matrix->width();
      show_text_animation_b = false;
    }
    matrix->show();
    delay(25);//speed_animasi_text_g);
  }
  
  return mode_display_led_g;
}

int animation_show(int curr_state)
{
    if(current_state != prev_state)
    {
        clear_LED(WIDTH_, HEIGHT_);
    }

    if((save_to_playlist_b) && (!got_sequence_data_playlist_b) && (!flash_on_read_process))// && (!notify_progress_b))
    {
        protect_eeprom_memory(false);
        uint32_t desire_addres = 0x00000000;
        flash_mem.eraseBlock64K(desire_addres);// hapus 64K dari alamat 0x00
        vTaskDelay(200/portTICK_PERIOD_MS);
        desire_addres = desire_addres + 0x00010000;
        flash_mem.eraseBlock64K(desire_addres);// hapus 64K  dari alamat 0xFA00 --> 
        vTaskDelay(200/portTICK_PERIOD_MS);
        protect_eeprom_memory(true);

        vTaskDelay(2/portTICK_PERIOD_MS);

        mem_addre_current_g         = 0x000000;
        amount_of_playlist_show_g   = 0;
        mem_addre_playlist_g        = mem_addre_current_g;

        //Serial.println("[INFO] SIMPAN KE PLAYLIST");
        switch(tipe_data_received_g)
        {
            case data_text_d:
                if(check_available_sector_size(&mem_addre_current_g, data_text_d))
                {
                    add_file_to_playlist(&mem_addre_current_g, &buff_universal_recv[0]);
                }
                //memset_pointer(buff_text, 0, 25); 
                //memset(buff_text, 0, sizeof(buff_text));   
            break;
            case data_image_d:
                if(check_available_sector_size(&mem_addre_current_g, data_image_d))
                {
                    add_file_to_playlist(&mem_addre_current_g, &buff_universal_recv[0]); 
                }
                //memset_pointer(buff_img, 0, 768); 
                //memset(buff_img, 0, sizeof(buff_img));   
            break;
            case data_animasi_d:

                //show last message 
                //#if(SPECIAL_DEBUG_FOR_ANIMATION == 1)
                
                //Serial.print("[INFO] animation: saved.......... ");
                // for (uint16_t i = 0; i < data_size_animasi_g; i++) 
                // {
                //     Serial.print(buff_universal_recv[i], HEX);
                //     Serial.print("-");
                //     if(((i+1) % 48) == 0)
                //     {
                //     Serial.println();
                //     }
                // }
                // Serial.println();  
                //#endif

                if(check_available_sector_size(&mem_addre_current_g, data_animasi_d))
                {
                    add_file_to_playlist(&mem_addre_current_g, &buff_universal_recv[0]);
                }
                //memset_pointer(buff_animasi, 0, 15360);   
                //memset(buff_animasi, 0, sizeof(buff_animasi));  
            break;

            case data_gif_d:
                if(check_available_sector_size(&mem_addre_current_g, data_gif_d))
                {
                    add_file_to_playlist(&mem_addre_current_g, &buff_universal_recv[0]);
                }           
            break;

            default:
            break;
        }

        memset(buff_universal_recv, 0, sizeof(buff_universal_recv));
        save_to_playlist_b       = false; 
    }
    else
    {
        /*
        if((save_to_playlist_b) && (got_sequence_data_playlist_b))
        {
            Serial.println("playlist prepare to save to eeprom");
            switch(tipe_data_received_g)
            {
                case data_text_d:
                    if(check_available_sector_size(&mem_addre_current_g, data_text_d))
                    {
                        add_file_to_playlist(&mem_addre_current_g, &buff_textEEPROM[0]);
                    }
                break;
                case data_image_d:
                    if(check_available_sector_size(&mem_addre_current_g, data_image_d))
                    {
                        add_file_to_playlist(&mem_addre_current_g, &buff_imgEEPROM[0]); 
                    }
                break;
                case data_animasi_d:
                    if(check_available_sector_size(&mem_addre_current_g, data_animasi_d))
                    {
                        add_file_to_playlist(&mem_addre_current_g, &buff_animasiEEPROM[0]);
                    }
                break;
        
                default:
                break;
            }            
            save_to_playlist_b = false;
        }
        */

        if((!gif_show_enable) && (!got_sequence_data_playlist_b))
        {
            read_playlist();
    //      play_animasi_playlist_g = false;
        }
        else
        {
            packet_show_b          = true;
            //type_data_to_show_g    = tipe_data_received_g;
    //      Serial.println("[INFO] ready for showing GIF file");
        }

        //if(play_animasi_playlist_g)
        //{
            if(packet_show_b)
            { 
                switch(type_data_to_show_g)
                { 
                    case data_text_d:
                        show_animation(type_data_to_show_g, buff_textEEPROM);
                        prev_type_data_to_show_g = data_text_d;     
                    break;

                    case data_image_d:
                        show_animation(type_data_to_show_g, buff_imgEEPROM);
                        prev_type_data_to_show_g = data_image_d;  
                    break;

                    case data_animasi_d:
                        show_animation(type_data_to_show_g, buff_animasiEEPROM);
                        prev_type_data_to_show_g = data_animasi_d;  
                    break;

                    case data_gif_d:
                        if(gif_show_enable)
                        {
                            show_animation_GIF();
                        }
                        prev_type_data_to_show_g = data_gif_d;
                    break;

                    default:
                    //Serial.println("[INFO] default position type to show");
                        #if(CHK_AND_DEBUG_PLAYLIST == 1)
                        Serial.println("[INFO] default animation is playing");
                        #endif

                        //play_animasi_playlist_g   = false;
                        animation_default(&next_animation_g);
                        frame_num_playlist_g      = 0;
                        repeat_animasi_image_g    = 0;
                        show_text_animation_b     = false;
                        type_data_to_show_g       = data_animasi_default;  
                        mem_addre_playlist_g      = 0x00000;
                        playlist_seq              = 0;	
                    break;
                }

                //mode_display_led_g = ST_ANIMATION;
            }            
        //}
        //else
        //{
            // #if(CHK_AND_DEBUG_PLAYLIST == 1)
            // Serial.println("[INFO] default animation is playing");
            // #endif

            // packet_show_b             = false;
            // play_animasi_playlist_g   = false;
            // animation_default(&next_animation_g);
            // frame_num_playlist_g      = 0;
            // repeat_animasi_image_g    = 0;
            // show_text_animation_b     = false;
            // type_data_to_show_g       = 0;  
            // mem_addre_playlist_g      = 0x00000;
            // playlist_seq              = 0;								  
        //}            
    }
      
    return mode_display_led_g;
}

int widget_show(int curr_state)
{
    if(new_entry_b)
    {
        clear_LED(WIDTH_, HEIGHT_);
        //Serial.println("clear sebelum widget");
    }

    switch(tipe_widget_g)
    {
        case widget_clock_d:
                if(!timer3_aktif_b)
                {
                    clear_LED(WIDTH_, HEIGHT_);
                    h_timer3.restartTimer();
                    timer3_aktif_b            = true;
                    ganti_tampilan_waktu_b    = true;
                    new_entry_b               = false;
                    widget_clock();
                }
                else
                {
                    widget_clock();
                }

        break;
        case widget_stopwatch_d:
                if(!timer3_aktif_b)
                {
                    clear_LED(WIDTH_, HEIGHT_);
                    h_timer3.restartTimer();
                    timer3_aktif_b            = true;
                    ganti_tampilan_waktu_b    = true;

                    if(new_entry_b)
                    {
                    time_minutes_g            = 0;
                    time_seconds_g            = 0;
                    new_entry_b               = false;
                    }
                    widget_stopwatch();
                }
                else
                {
                    widget_stopwatch();
                }

        break;
        case widget_timer_d:
                if(!timer3_aktif_b)
                {
                    clear_LED(WIDTH_, HEIGHT_);
                    h_timer3.restartTimer();
                    timer3_aktif_b            = true;
                    ganti_tampilan_waktu_b    = true;
                    new_entry_b               = false;
                    widget_timer();
                }
                else
                {
                    widget_timer();
                }
        
        break;
        case widget_scoreboard_d:
                if(new_entry_b)
                {
                    clear_LED(WIDTH_, HEIGHT_);
                    prev_score_a_g  = 0;
                    prev_score_b_g  = 0;
                    widget_score_board();
                }
                else
                {                                                   
                    widget_score_board();
                }
                
        break;
    }

    prev_tipe_widget_g = tipe_widget_g;
    
    return mode_display_led_g;
}

int game_show(int curr_state)
{
    if(prev_games_mode_g != games_mode_g)
    {
        loadingFlag_b  = true;
        clear_LED(WIDTH_, HEIGHT_);
    }
    
    switch(games_mode_g)
    {
        case games_snake_d:
                snakeRoutine();
        break;
        case games_tetris_d:
                tetrisRoutine();
        break;
        case games_flappy_bird_d:
                flappyRoutine();
        break;
        case games_arkanoid_d:
                arkanoidRoutine(); 
        break;
    }  
    prev_games_mode_g  = games_mode_g;

    return mode_display_led_g;
}

int remote_show(int curr_state)
{
    if(command_mpu_calib_b)
    {
        calibrate_mpu(); 
    }
    else
    {
        Serial.println("cmd remote received");
        current_state_core0 = ST_MPU_CALC;      //determine state of core 0

        switch(remote_sign_direction_g)
        {
            case remote_up_d:
                SIGN_ANIMATION              = ON;
                DIRECTION                   = FORWARD;
                prev_sign_stop_or_fwd       = remote_up_d;
                //Serial.println("[INFO] FORWARD DIRECTION by remote....");
            break;
            case remote_stop_d:
                DIRECTION                   = REM_STOP;
                SIGN_ANIMATION              = ON;
                prev_sign_stop              = true;
            break;
            case remote_right_d:
                SIGN_ANIMATION              = ON;
                DIRECTION                   = RIGHT;
                prev_sign_stop_or_fwd       = remote_right_d;
                //Serial.print("[INFO] RIGHT DIRECTION by remote.... timer: ");  
                //Serial.println(timer2_tick_g);                        
            break;
            case remote_left_d:
                SIGN_ANIMATION              = ON;
                DIRECTION                   = LEFT;
                prev_sign_stop_or_fwd       = remote_left_d;
                //Serial.print("[INFO] LEFT DIRECTION by remote.... timer: ");
                //Serial.println(timer2_tick_g);
            break;
            case remote_off_d:
                SIGN_ANIMATION              = OFF;
                DIRECTION                   = NO_ACTION;

                if(prev_state == ST_REMOTE)
                {
                    mode_display_led_g          = ST_ANIMATION;
                }

                prev_sign_stop_or_fwd       = remote_off_d;
                //Serial.println("[INFO] sign off by remote");
            break;
            default:   
                //Serial.println("[INFO] No dir");
            break;
        }

        set_sign_animation(&prev_direction,&DIRECTION);
        action_base_on_prediction();
    }

    return mode_display_led_g;
}

int thief_alarm(int curr_state)
{
    static bool task_suspended = false;
    current_state_core0 = ST_THIEF_ALARM;      //determine state of core 0

    Serial.print("[INFO] HMSpace: ");
    Serial.println(ESP.getFreeHeap());
    
    tone_alarm_maling_blink_on();
    //buzzer_alarm_thief();
    vTaskDelay(250/portTICK_PERIOD_MS);

    EEPROM.writeChar(kemalingan_status_addr , 1);
    EEPROM.commit();

    device_shutdown_b  = false;

    if(erase_eeprom_b)
    {
        clear_LED(WIDTH_, HEIGHT_);            
        play_animasi_playlist_g      = false;
        delete_files_from_playlist();

        erase_eeprom_b               = false;
        next_animation_g             = 0;
        notif_kemalingan             = 0;

        noTone(BUZZER_PIN, BUZZER_CHANNEL); 

        clear_LED(WIDTH_, HEIGHT_);
        EEPROM.writeChar(kemalingan_status_addr , 0);
        EEPROM.commit();

        g_cek_maling_counter         = 0;
        g_jmlh_spike_x               = 0;
        g_jmlh_spike_z               = 0;

        mode_kemalingan_b            = false;
        
        vTaskDelay(1000/portTICK_PERIOD_MS);
        //ESP.restart();

        task_suspended = false;
        mode_display_led_g = ST_ANIMATION;
        //vTaskResume(Task0);
    }
    else
    {
       if(mode_kemalingan_b)
       {
          mode_display_led_g = ST_THIEF_ALARM;
       }
       else
       {
          mode_display_led_g = ST_ANIMATION;
          noTone(BUZZER_PIN, BUZZER_CHANNEL); 
       }

       if(!task_suspended)
       {
          task_suspended = true;
          //vTaskSuspend(Task0);
       }
    }

    if(notif_kemalingan > 0)
    {
        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
            buf_send_notify_g[0]  = 20;
            p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
        #else
            p_widget_characteristic->setValue("20");
        #endif

        p_widget_characteristic->notify(true);
        //Serial.println("notifikais anti maling");
        notif_kemalingan--;
    }


    return mode_display_led_g;
}

int idle_core_0(int curr_state)
{
    vTaskDelay(500/portTICK_PERIOD_MS);
    return current_state_core0;
}

int mpu_calc(int curr_state)
{
    if(!mode_kemalingan_b)
    {
      if(sampling_data_mpu_b)
      {
          portENTER_CRITICAL_ISR(&timerMux_mpu);
          sampling_data_mpu_b          = false;
          //Serial.print("MPU interrupt ");  
          portEXIT_CRITICAL_ISR(&timerMux_mpu);
          //h_timer1.stopTimer();
          
              if(mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
              {      
                  get_accel_gyro_buff(buf_idx_mpu);
                  buf_idx_mpu++; 
                  
                  if((buf_idx_mpu >= TOTAL_UPDATED_DATA))
                  {
                    if(mpu_calibrated_b) //if statement untuk automatic sign
                    {    
                        split_into_segment(data_accx_th, &accx_seg[0][0]);
                        split_into_segment(data_accy_th, &accy_seg[0][0]);
                        split_into_segment(data_accz_th, &accz_seg[0][0]);
                    
                        //Serial.println("data MPU calculated");           
                        if(!got_sequence_data_playlist_b and !notify_progress_b)  //jika tidak sedang mengirim playlist
                        {
                          predict_action();
                        }
                    }
                    else //if statement untuk alarm feature (tanpa harus kalibrasi)
                    {
                        if(!got_sequence_data_playlist_b and !notify_progress_b)  //jika tidak sedang mengirim playlist
                        {
                          predict_action();
                        }
                    }
                                      
                    buf_idx_mpu                  = 0;
                    decision_diproses_b          = true;
                  }
              }
              else
              {
                  buf_idx_mpu              = 0;
                  if(!got_sequence_data_playlist_b and !notify_progress_b)  //jika tidak sedang mengirim playlist
                  {
                    predict_action();
                  }                
              }
  
          if((mode_safety_enabled_b && !mode_remote_enabled) || (timer2_enabled_for_safety))//agar tone masuk dan keluar mode alarm tidak delay
          {
              vTaskDelay(10/portTICK_PERIOD_MS);    
          }
          else
          {
              vTaskDelay(DELAY_BACA_MPU/portTICK_PERIOD_MS);
          }
      }
      else
      {
          vTaskDelay(250/portTICK_PERIOD_MS);
      }
    }
    else
    {
      Serial.print("[INFO] A HMSpace: ");
      Serial.println(ESP.getFreeHeap());
      vTaskDelay(500/portTICK_PERIOD_MS);
    }

    return current_state_core0;
}

void reading_MPU__()
{
    
    if(!mode_kemalingan_b)
    {
      //Serial.printf("npu:  %d %d %d %d %d %d\r\n", sampling_data_mpu_b, buf_idx_mpu, mpu_calibrated_b, mode_remote_enabled, SIGN_ANIMATION, timer2_tick_g);

      // we have to split up between alarm features and smart sign to prevent and confugsing while both of them is enabled by user. the priority one is alarm feature.
      // once alarm feature activated then user will not got a smart sign feature is working properly
      if(mode_safety_enabled_b)
      {
          if(sampling_data_mpu_b)
          {
              portENTER_CRITICAL_ISR(&timerMux_mpu);
              sampling_data_mpu_b          = false;
              portEXIT_CRITICAL_ISR(&timerMux_mpu);
              
              if(mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
              {  
                 detect_movement_alarm_feature(&buf_idx_mpu);
                 buf_idx_mpu++;
                 
                if((buf_idx_mpu >= TOTAL_UPDATED_DATA))
                {
                    if(mpu_calibrated_b) 
                    {    
                        split_into_segment(data_accx_th, &accx_seg[0][0]);
                        split_into_segment(data_accy_th, &accy_seg[0][0]);
                        split_into_segment(data_accz_th, &accz_seg[0][0]);
                    
                        //Serial.println("data MPU calculated");           
                        if(!got_sequence_data_playlist_b && !notify_progress_b)  //jika tidak sedang mengirim playlist
                        {
                          predict_action();
                        }
                    }
                    
                    buf_idx_mpu = 0; 
                }
              }
          }       
      }
      else
      {
          if(!got_sequence_data_playlist_b && !notify_progress_b)  //jika tidak sedang mengirim playlist
          {

             if(mpu.dmpGetCurrentFIFOPacket(fifoBuffer))
             {  
                detect_stop_sign_by_mpu(&buf_idx_mpu, true);
             }
             else
             {
               detect_stop_sign_by_mpu(&buf_idx_mpu, false);
             }

             //this counter buffer is checke inside predict_action() function.
             buf_idx_mpu++;
             
             predict_action();
          }        
      }
    }
    else
    {
      Serial.print("[INFO] A HMSpace: ");
      Serial.println(ESP.getFreeHeap());
      //vTaskDelay(500/portTICK_PERIOD_MS);
    }
}

void check_special_command(uint8_t *buff_data_serial)
{
    switch(buff_data_serial[2])
    {
    case CHANGE_ID_BLE:
        for(uint8_t i = start_addres_ID; i<(start_addres_ID + 10); i++)
        {
            EEPROM.write(i, buff_data_serial[4 + i]);
        }
        EEPROM.commit();   

        Serial.println("Command Chng ID received");
    break;

    case RESET_SCORE:
        high_score_arkanoid = 0;
        high_score_snake = 0;
        high_score_tetris = 0;
        high_score_flappy_bird = 0;

        for(uint8_t i = score_game_arkanoid_addr; i < (score_game_arkanoid_addr + 8); i++)
        {
            EEPROM.write(i, 0x00);
        }
        EEPROM.commit();         
        Serial.println("Command reset score received");     
    break;

    default:
    break;
    }
}

//OK--