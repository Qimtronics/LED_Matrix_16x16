
/**
 * 
 *  @brief callback connection status
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class MyServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
#else
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
#endif
      if((pServer->getConnectedCount()-1) == 0)
      {
        device_connected_b = true; 
        count_conn_device = pServer->getConnectedCount() - 1;
        Serial.print(count_conn_device);     
        Serial.println(" connected");        
      }

      #ifdef DEBUG_AT_RECEIVE_DATA
        Serial.print("pServer->getConnectedCount: ");
        Serial.println(pServer->getConnectedCount());
      #endif

#if(BLE_LIB_IS_NIMBLE == 1)
        NimBLEDevice::startAdvertising();
#else
        BLEDevice::startAdvertising();
#endif
    };

#if(BLE_LIB_IS_NIMBLE == 1)
    void onDisconnect(NimBLEServer* pServer) {
#else
    void onDisconnect(BLEServer* pServer) {
#endif
      if(pServer->getConnectedCount() <= 1)
      {
        device_connected_b = false;
        count_conn_device = pServer->getConnectedCount(); 
        Serial.print(count_conn_device);     
        Serial.println(" Disconnected");
      }
    } 
};
 
 /* 
 *  --------------------------------------------------------------------//
 *  functions callbacks BLE
 *  --------------------------------------------------------------------//
 */
/**
 * 
 *  @brief widget callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class widget_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_widget_characteristic) {
#else
class widget_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_widget_characteristic) {
#endif
      std::string rxValue = p_widget_characteristic->getValue();

      #if(DEBUG_DATA_RAW == 1)
        Serial.println("widget");

        for(uint16_t x = 0; x < rxValue.length(); x++)
        {
            Serial.print(rxValue[x], HEX);
            Serial.print("-");
        }
        Serial.println();
      #endif

      if((rxValue[0] == 'C') && (rxValue[1] == 'Q'))
      {
        #if(RECV_SPECS_CMD == 1) 
          //do something here...
        #endif
      }
      else
      {
        #if(DISABLE_PROCESSING_RECV_DATA == 0)
          tipe_widget_g          = rxValue[0];
          mode_remote_enabled    = false;
          gif_show_enable        = false;
          packet_show_b          = false;
          
          if((prev_mode_display_g != mode_display_led_g) || (prev_tipe_widget_g != tipe_widget_g) || (!timer3_aktif_b))
          {
            new_entry_b = true;
          }
          
          length_data_packet_g          = rxValue.length();
          
          if((tipe_widget_g > 0) && (tipe_widget_g <= 4) && (length_data_packet_g > 0))
          {
            
            play_timer_b                = rxValue[6];
            if((mode_display_led_g == ST_WIDGET) && (prev_tipe_widget_g == widget_stopwatch_d) && (tipe_widget_g == widget_stopwatch_d))
            {
              if(!play_timer_b)
              {
                h_timer3.restartTimer();
              }
              else
              {
                h_timer3.stopTimer();
              }
            }
            else
            {
              time_hours_g              = rxValue[1];
              time_minutes_g            = rxValue[2];
              time_seconds_g            = rxValue[3];
            }
            
            score_tim_a_g               = rxValue[4];
            score_tim_b_g               = rxValue[5];

            close_gif(ST_WIDGET, data_animasi_default);
            mode_display_led_g          = ST_WIDGET;
            timer0_counter_power_off_g  = 0;
          }
          else
          {
            h_timer3.stopTimer();
            timer3_aktif_b              = false;
            mode_display_led_g          = ST_ANIMATION;
          }    
        #endif 
      
        if((rxValue[0] == 'Q') && (rxValue[1] == 'I') && (rxValue[2] == 'M'))
        {
          uint16_t sum = 0;
          for(uint8_t j = 0; j < (rxValue.length() - 2); j++)
          {
            sum = sum + rxValue[j];
          }

          sum = sum ^ 0xFFFF;
          if(((sum >> 8) & 0xFF) == rxValue[5] && (sum & 0xFF) == rxValue[6])
          {
            if(rxValue[3] == 0x10)
            {
              p_setting_characteristic->setValue(std::string(unique_id.c_str()));
            }
          }
        }
      
      }
    }
};

/**
 * 
 *  @brief remote callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class remote_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_remote_characteristic) {
#else
class remote_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_remote_characteristic) {
#endif
      std::string rxValue = p_remote_characteristic->getValue();

      //#if(DEBUG_DATA_RAW == 1)
        Serial.println("remote: ");
        for(uint16_t x = 0; x < rxValue.length(); x++)
        {
            Serial.print(rxValue[x], HEX);
            Serial.print("-");
        }
        Serial.println();
      //#endif

      #if(DISABLE_PROCESSING_RECV_DATA == 0)
        //char temp_val[2];
        //mode_remote_enabled    = true;
        static bool remote_enabled_prev = false;

        gif_show_enable        = false;
        close_gif(ST_REMOTE, data_animasi_default);
        
        remote_mode_g           = rxValue[0];//check_got_ascii(rxValue[0]) - 0x30;
        length_data_packet_g    = rxValue.length();

        if(remote_mode_g >= 1)
        {
          mode_remote_enabled = true;
        }
        else
        {
          mode_remote_enabled = false;
          //mpu_calibrated_b    = false;
        }

        if(mode_remote_enabled)
        {
          if((remote_mode_g <= 1) && (length_data_packet_g > 0) && (remote_enabled_prev))
          {
            automatic_sign_b        = rxValue[2];//decide_boolean(rxValue[2]);
            auto_stop_sign_b        = rxValue[3];//decide_boolean(rxValue[3]);
            remote_sign_direction_g = rxValue[4];//check_got_ascii(rxValue[4]) - 0x30;
            alert_alarm_sign_b      = rxValue[5];//check_got_ascii(rxValue[5]) - 0x30;
            
            if(alert_alarm_sign_b >= 1)
            {
              mode_kemalingan_b = true;
              notif_kemalingan  = 2;
              EEPROM.writeChar(kemalingan_status_addr ,rxValue[5]);
              EEPROM.commit();

              mode_display_led_g = ST_THIEF_ALARM;
              //vTaskSuspend(Task0);
            }
            else
            {
              mode_kemalingan_b = false;
              notif_kemalingan  = 0;
              EEPROM.writeChar(kemalingan_status_addr ,rxValue[5]);
              EEPROM.commit();

              mode_display_led_g = ST_REMOTE;
              //vTaskResume(Task0);
            }
    
            if(rxValue[6])//(decide_boolean(rxValue[6]))
            {
              command_mpu_calib_b = true;
              //mode_display_led_g  = ST_REMOTE;
              //mpu_calibrated_b    = false;
            }
            else
            {
              command_mpu_calib_b = false;
              //mode_display_led_g  = ST_REMOTE;
              //mode_display_led_g  = show_calibrate_mpu_d;
            }

            #ifdef DEBUG_AT_RECEIVE_DATA
              Serial.print("remote mode: ");
              Serial.print(remote_mode_g, HEX);
              Serial.print("-");
              Serial.print(alert_alarm_sign_b, HEX);
              Serial.print("-");
              Serial.print(automatic_sign_b, HEX);
              Serial.print("-");
              Serial.print(auto_stop_sign_b, HEX);
              Serial.print("direct: ");
              Serial.print(remote_sign_direction_g, HEX);
              Serial.print("calib: ");
              Serial.println();
              //Serial.println(decide_boolean(rxValue[6]));
            #endif
    
            forward_has_over_b          = false;
            //mode_display_led_g          = show_remote_d;
            timer0_counter_power_off_g  = 0;
            prev_direction              = NO_ACTION;
          }
          else
          {
            mode_display_led_g = ST_ANIMATION;
          }
        }
        else
        {
          mode_display_led_g = ST_ANIMATION;
        }

        remote_enabled_prev = mode_remote_enabled;
      #endif

    }
};

/**
 * 
 *  @brief games callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class games_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_remote_characteristic) {
#else
class games_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_remote_characteristic) {
#endif
      std::string rxValue = p_remote_characteristic->getValue();

      #if(DEBUG_DATA_RAW == 1)
        Serial.println("games");

        for(uint16_t x = 0; x < rxValue.length(); x++)
        {
            Serial.print(rxValue[x], HEX);
            Serial.print("-");
        }
        Serial.println();
      #endif

      #if(DISABLE_PROCESSING_RECV_DATA == 0)
        games_mode_g            = rxValue[0];
        length_data_packet_g    = rxValue.length();
        mode_remote_enabled     = false;
        gif_show_enable         = false;
        packet_show_b           = false;

        if((games_mode_g > 0) && (games_mode_g <= 4) && (length_data_packet_g > 0))
        {

          control_games_g         = rxValue[1];

          if(!got_new_command_g)
          {
            got_new_command_g   =  true;
          }

          close_gif(ST_GAME, data_animasi_default);
          mode_display_led_g          = ST_GAME;
          timer0_counter_power_off_g  = 0;
        }
        else
        {
          mode_display_led_g = ST_ANIMATION;
        }
      #endif

    }
};


/**
 * 
 *  @brief running text callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class text_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_text_characteristic) {
#else
class text_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_text_characteristic) {
#endif
      rxValue_g = p_text_characteristic->getValue();

      //#if(DEBUG_DATA_RAW == 1)
        Serial.println("text");

        for(uint16_t x = 0; x < rxValue_g.length(); x++)
        {
            Serial.print(rxValue_g[x], HEX);
            Serial.print("-");
        }
        Serial.println();
      //#endif

      #if(DISABLE_PROCESSING_RECV_DATA == 0)
        mode_remote_enabled           = false;
        gif_show_enable               = false;
        got_sequence_data_playlist_b  = false;
        
        receive_text();
      #endif
    }
};


/**
 * 
 *  @brief setting callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class setting_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_setting_characteristic) {
#else
class setting_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_setting_characteristic) {
#endif
      std::string rxValue = p_setting_characteristic->getValue();

      #if(DEBUG_DATA_RAW == 1)
        Serial.println("setting");
        for(uint16_t x = 0; x < rxValue.length(); x++)
        {
            Serial.print(rxValue[x], HEX);
            Serial.print("-");
        }
        Serial.println();
      #endif

      mode_remote_enabled     = false;

      //Serial.println("setup:");
      for(uint8_t d = 0; d < rxValue.length(); d++)
      {
        Serial.print(rxValue[d]);
        Serial.print("-");
      }
      Serial.println();

      if(rxValue.length() > 0)
      {
        erase_eeprom_b           = rxValue[0];

        if(!erase_eeprom_b && !mode_kemalingan_b)
        {    

          //enable and disable anti thief    
          alert_alarm_sign_b      = rxValue[1];
          if(alert_alarm_sign_b >= 1)
          {             
              mpu_calibrated_b        = false;
              command_mpu_calib_b     = true;

              mode_safety_enabled_b   = true;
              mode_remote_enabled     = false;
                            
              EEPROM.write(alert_config_addr, 1);
          }
          else
          {
              mode_safety_enabled_b   = false;
              mode_remote_enabled     = false;
              EEPROM.write(alert_config_addr, 0);
          }

          //enable and disable automatic stop sign
          automatic_sign_b        = rxValue[2];
          EEPROM.write(auto_smart_sign_addr, (char)rxValue[2]);

          //set up auto power off
          auto_power_off_led_b    = rxValue[3];
          EEPROM.write(auto_power_off_addr, (char)rxValue[3]);  
          if(auto_power_off_led_b)
          {
              auto_power_off_time_g   = rxValue[4];
              EEPROM.write(auto_power_time_addr, (char)rxValue[4]);                                                                                                            
          }
          else
          {
              auto_power_off_time_g   = off_time_never_d;
          }

          //set up led brightness
          if(rxValue[5] != 0)
          {                                   
              brightness_level_g    = DEFAULT_BRIGHTNESS * rxValue[5];// * DEFAULT_BRIGHTNESS;
              EEPROM.write(brigthness_addr    , (char)rxValue[5]);                                                                   
          }

          //reverse for future use
          if(rxValue.length() > 6)
          {
            //reverse for future use, setting up timer for smart sign to shut off
            if(rxValue[6] == 1)
            {
              EEPROM.write(interval_time_sign_addr, (char)rxValue[7]);
              interval_ticks_sign = rxValue[7] * 4; 
            }
            else
            {
              uint8_t interval_timer_sign = 20;
              EEPROM.write(interval_time_sign_addr, (char)interval_timer_sign);
            }   
            
            //reverse for future use, setting up acceleration threshold to make stop sign is turn on
            if(rxValue[8] == 1)
            {
              threshold_value_var = (float)((float)hex_to_val(rxValue[9])/1.00) + ((float)hex_to_val(rxValue[10])/10.00) + ((float)hex_to_val(rxValue[11])/100.00) + ((float)hex_to_val(rxValue[12])/1000.00);
              if(hex_to_val(rxValue[13]) == 1)
              {
                threshold_value_var = threshold_value_var * (1);
              }
              else
              {
                threshold_value_var = threshold_value_var * (-1);
              }
            }

            //reverse for future use, setting up max value to count an acceleration threshold to turn on stop sign
            if(rxValue[14] == 1)
            {
              min_val_happen_var = rxValue[15]; 
            }
          }
        }
        
        EEPROM.commit();  

        #ifdef DEBUG_AT_RECEIVE_DATA  
          Serial.print("settings_config: ");
          Serial.print(erase_eeprom_b);
          Serial.print(" - ");
          Serial.print(alert_alarm_sign_b, DEC);
          Serial.print(" - ");
          Serial.print(automatic_sign_b, DEC);
          Serial.print(" - ");
          Serial.print(auto_power_off_led_b, DEC);
          Serial.print(" - ");
          Serial.print(auto_power_off_time_g, DEC);
          Serial.print(" - ");
          Serial.print(brightness_level_g, DEC);
          Serial.print(" - ");
          Serial.println(interval_ticks_sign, DEC);
        #endif

        get_device_config();
        
        setting_receive_b   = true;

        timer0_counter_power_off_g  = 0;
      }
      //#endif
    }
};


/**
 * 
 *  @brief design image manually feature callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class design_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_design_characteristic) {
#else
class design_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_design_characteristic) {
#endif
      rxValue_g = p_design_characteristic->getValue();

      ///#if(DEBUG_DATA_RAW == 1)
        //Serial.println("design");
        // length_data_packet_g   = rxValue_g.length();

         for(uint16_t x = 0; x < length_data_packet_g; x++)
         {
             Serial.print(rxValue_g[x], HEX);
             Serial.print(" ");
         }
         Serial.println();

      //#else

      mode_remote_enabled          = false;
      gif_show_enable              = false;
      length_data_packet_g         = rxValue_g.length();
      got_sequence_data_playlist_b = false;
      receive_design_not_image_b   = true;
      
      if(length_data_packet_g > 0)
      {
        if(design_stream_b)
        {
            led_array_idx_g         = rxValue_g[0];
            deisgn_color_rgb_g[0]   = rxValue_g[1];
            deisgn_color_rgb_g[1]   = rxValue_g[2];
            deisgn_color_rgb_g[2]   = rxValue_g[3];
            deisgn_color_rgb_g[3]   = rxValue_g[4];
        }
        else
        {
           receive_image(); 
        }
      }
    
      //#endif
    }
};


/**
 * 
 *  @brief image callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class image_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_image_characteristic) {
#else
class image_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_image_characteristic) {
#endif
      rxValue_g = p_image_characteristic->getValue();

      //#if(DEBUG_DATA_RAW == 1)
//        Serial.println("image");
//         length_data_packet_g   = rxValue_g.length();
//         for(uint16_t x = 0; x < length_data_packet_g; x++)
//         {
//             Serial.print(rxValue_g[x], HEX);
//             Serial.print("-");
//         }
//         Serial.println();

      //#endif
       mode_remote_enabled          = false;
       gif_show_enable              = false;
       got_sequence_data_playlist_b = false;
       receive_design_not_image_b   = false;

       receive_image();
      //#endif
    }
};

/**
 * 
 *  @brief existing animation callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class animation_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_animation_characteristic) {
#else
class animation_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_animation_characteristic) {
#endif
      rxValue_g = p_animation_characteristic->getValue();

      //#if(DEBUG_DATA_RAW == 1)
        //Serial.println("animation");
        // length_data_packet_g   = rxValue_g.length();

        // for(uint16_t x = 0; x < rxValue_g.length(); x++)
        // {
        //     Serial.print(rxValue_g[x], HEX);
        //     Serial.print("-");
        // }
        // Serial.println();

      //#endif

      mode_remote_enabled           = false;
      gif_show_enable               = false;
      got_sequence_data_playlist_b  = false;
      
      receive_animasi();
      //#endif
   }
};

/**
 * 
 *  @brief playlist animation callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class playlist_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_playlist_characteristic) {
#else
class playlist_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_playlist_characteristic) {
#endif
      rxValue_g = p_playlist_characteristic->getValue();

//        Serial.println("playlist");
//      #if(DEBUG_DATA_RAW == 1)
//        // length_data_packet_g   = rxValue_g.length();
        for(uint16_t x = 0; x < rxValue_g.length(); x++)
        {
            Serial.print(rxValue_g[x], HEX);
            Serial.print(" ");
        }
        Serial.println();
//      #endif

      mode_remote_enabled          = true;
      uint8_t tipe_received_data   = 0;
      type_data_to_show_g          = data_animasi_default;
      mode_display_led_g           = ST_ANIMATION;
      play_animasi_playlist_g      = false;
      total_playlist_g             = rxValue_g[0];
      got_sequence_data_playlist_b = true;
      receive_design_not_image_b   = false;

      //clear_LED(16,16);
      
      if((total_playlist_g > 0) && (total_playlist_g <= 5))
      {
          tipe_received_data      = rxValue_g[1];

          if(total_playlist_g >= 1)
          {
            if(!first_playlist_b)
            {
              Serial.println("PLAYLIST......(1)");
              protect_eeprom_memory(false);
              uint32_t desire_addres1 = 0x00000000;
              flash_mem.eraseBlock64K(desire_addres1);// hapus 64K dari alamat 0x00
              vTaskDelay(200/portTICK_PERIOD_MS);
              desire_addres1 = desire_addres1 + 0x00010000;
              flash_mem.eraseBlock64K(desire_addres1);// hapus 64K  dari alamat 0xFA00 --> 
              vTaskDelay(200/portTICK_PERIOD_MS);
              desire_addres1 = desire_addres1 + 0x00010000;
              flash_mem.eraseBlock64K(desire_addres1);// hapus 64K  dari alamat 0xFA00 --> 
              vTaskDelay(200/portTICK_PERIOD_MS);
              protect_eeprom_memory(true);
              
              first_playlist_b             = true;
              progres_status              = 0;
              mem_addre_current_g         = 0x000000;
              amount_of_playlist_show_g   = 0;
              mem_addre_playlist_g        = mem_addre_current_g;
            }
          }
          else
          {
            //first_playlist_b             = false;
          }

          if(tipe_received_data == 1)
          {
            receive_text();
          }
          else if(tipe_received_data == 2)
          {
            receive_image();
          }
          else if(tipe_received_data == 3)
          {
            receive_animasi();
          }
          else
          {
             Serial.println("[INFO] uncategorized");
          }

          save_to_playlist_b                = false;
          if(urutan_playlist_g >= total_playlist_g)
          {
            progres_status                 = 99;
            notify_progress_b              = true;
            receive_complete_g             = 3;
            //got_sequence_data_playlist_b = false;
            //urutan_playlist_g            = 0;
            //mode_display_led_g           = show_animasi_d;
            got_sequence_data_playlist_b = false;
            urutan_playlist_g            = 0;
            total_playlist_g             = 0;
            mode_display_led_g           = ST_ANIMATION;
            play_animasi_playlist_g      = true;
            first_playlist_b             = false;            
          }                      
      }
      else
      {
        mode_display_led_g            = ST_ANIMATION;
      }

      //#endif
   }
};

/**
 * 
 *  @brief GIF file animation callback function
 * 
 */
#if(BLE_LIB_IS_NIMBLE == 1)
class uploadGIF_callback: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *p_uploadGif_characteristic) {
#else
class uploadGIF_callback: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *p_uploadGif_characteristic) {
#endif

      //#if(DEBUG_DATA_RAW == 1)
        //Serial.println("GIF");
        
        rxValue_g = p_uploadGif_characteristic->getValue();
        length_data_packet_g   = rxValue_g.length();

        // for(uint16_t x = 0; x < rxValue_g.length(); x++)
        // {
        //     Serial.print(rxValue_g[x], HEX);
        //     Serial.print("-");
        // }
        // Serial.println();

      //#endif
      got_sequence_data_playlist_b = false;
      mode_remote_enabled          = false;
      //gif_show_enable              = false;
      receive_GIF();

      //#endif
   }
};

//OK