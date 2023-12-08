uint8_t devStatus   = 0;

/**
 * 
 * @brief Serial configuration function
 * 
*/
void serial_config()
{
    Serial.begin(115200);
    delay(500);

    Serial.println(branch_id);
    Serial.println(unique_id);
}

/**
 * 
 * @brief gpio configuration function
 * 
*/
void gpio_config()
{
  pinMode(en_boost_5v_pin_d, OUTPUT);
  pinMode(en_supply_3v3_pin_d, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUZZER_PIN_2, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(BUZZER_PIN_2, LOW);
}

/**
 * 
 * @brief eeprom internal of esp32 chip configuration function
 * 
*/
void eeprom_config()
{
  EEPROM.begin(EEPROM_SIZE); 
}

/**
 * 
 * @brief flash memory configuration function, this is a function for setup eeprom external
 * 
*/
void flash_memory_config()
{
    /*
     * 
     * WP and HOLD pin need to be configured as HIGH level output due to situation where eeprom chip need to be able to write
     * and write the data. Once those are two pins set to be LOW level output, the eeprom chip will enter protection mode means 
     * it could not be writen or even read.
     * 
     * to activated those two function just call protect_eeprom_memory(false); 
     * 
    */
    pinMode(WP_PIN, OUTPUT);
    pinMode(HOLD_PIN, OUTPUT);

    protect_eeprom_memory(false);

   if(flash_mem.error()) 
   {
      Serial.println(flash_mem.error(VERBOSE));
   }

    //flash_mem.begin(MB(4), &SPIPins[0]);
    flash_mem.begin();
//    Serial.println("[INFO] Reading Memory info");  

    uint8_t id[5] = {0,0,0,0,0};

    uint32_t JEDEC_ID = flash_mem.getJEDECID();

//    Serial.print("JEDEC ID: 0x");
//    Serial.println(JEDEC_ID, HEX);
//    Serial.print("Man ID: 0x");
//    Serial.println(uint8_t(JEDEC_ID >> 16), HEX);
//    Serial.print("Memory ID: 0x");
//    Serial.println(uint8_t(JEDEC_ID >> 8), HEX);
//    Serial.print("[INFO] block size mem: ");
//    Serial.print(flash_mem.getMaxPage());
//    Serial.println(F(" bytes"));

    if(((JEDEC_ID >> 16 & 0xFF) == used_eeprom_id_g[0]) && 
      ((JEDEC_ID >> 8 & 0xFF)  == used_eeprom_id_g[1]) &&
      ((JEDEC_ID & 0xFF)  == used_eeprom_id_g[2]))
    {
      Serial.println("[INFO] MEMORY ID VALID");
      if(flash_mem.getCapacity() > 0)
      {
        Serial.println("[INFO] MEMORY IS VALID");
      }
      else
      {
        Serial.println("[INFO] MEMORY IS NOT VALID");
      }    
    }
    else
    {
        Serial.println("[INFO] MEMORY IS NOT VALID");
    }
    protect_eeprom_memory(true);
}


/**
 * 
 * @brief led matrix ws2812b configuration
 * 
*/
void led_matrix_config()
{
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness_level_g);  
    clear_LED(WIDTH_, HEIGHT_);
}

/**
 * 
 * @brief mpu6500 configuration
 * 
*/
void mpu_config()
{
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  mpu.initialize();
  if(mpu.testConnection())
  {
    #if(enable_debug_serial == 1)
      Serial.println("SENS OK 68");
    #endif
  }
  else
  {
    mpu.set_address_MPU(MPU6050_ADDRESS_AD0_HIGH);
    mpu.initialize();

    if(mpu.testConnection())
    {
      #if(enable_debug_serial == 1)
        Serial.println("SENS OK 69");
      #endif
    }
    else
    {      
      #if(enable_debug_serial == 1)
        Serial.println("SENS NO 89");
      #endif   
    }    
  }

  devStatus = mpu.dmpInitialize();
  mpu.setDMPEnabled(true);
  mpu.dmpGetFIFOPacketSize();
  
  if (devStatus == 0) 
  {
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    dmpReady_b = true;
  }
  else
  {
    //mpu.setDMPEnabled(false);
  }
  
}

/**
 * 
 * @brief device configuration number
 * 
*/
void get_device_config()
{
    uint8_t idx_pair = 0;
    #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
      pairing_buffer[idx_pair]          = 'V';
      pairing_buffer[++idx_pair]        = version_firm_break;
      pairing_buffer[++idx_pair]        = version_firm_feature;  
      pairing_buffer[++idx_pair]        = version_firm_fix;
      pairing_buffer[++idx_pair]        = 16;  
      pairing_buffer[++idx_pair]        = 16; 
      pairing_buffer[++idx_pair]        = 99; 
    #else
      pairing_buffer           = "";
      pairing_buffer           = "V.2.3.0,16,16,99,";    
    #endif
      
    uint8_t idx = 0;
    for(uint8_t i = brigthness_addr; i < (brigthness_addr + size_data_config); i++)
    {
      settings_config[idx] = EEPROM.read(i);
      
      #ifdef DEBUG_AT_STARTUP
        Serial.printf(" %02X",settings_config[idx]);
      #endif

      idx++;
    }
    #ifdef DEBUG_AT_STARTUP
    Serial.println();
    #endif


    //set up brightness
    if(settings_config[0] <= 10)
    {
      brightness_level_g    = (uint8_t) DEFAULT_BRIGHTNESS * settings_config[0];// * DEFAULT_BRIGHTNESS;
      
      #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
        pairing_buffer[++idx_pair]        = settings_config[0];
      #else
        pairing_buffer = pairing_buffer + String(settings_config[0], DEC) + String(settings_config[1], DEC) + ",";
      #endif
    }
    else
    { 
      settings_config[0] = 0;
      
      #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
        pairing_buffer[++idx_pair]        = 1;
      #else
        pairing_buffer = pairing_buffer + "01,";
      #endif
      
      brightness_level_g = DEFAULT_BRIGHTNESS;
    }

     #ifdef DEBUG_AT_STARTUP
        Serial.println("==============================================");
        Serial.printf("%d ", brightness_level_g);
     #endif
    
    //check if safety anti thief wheater active or not
    mode_safety_enabled_b      = (uint8_t) settings_config[1];
    if(mode_safety_enabled_b)
    {
        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
          pairing_buffer[++idx_pair]        = 1;
        #else
          pairing_buffer = pairing_buffer + "1,";
        #endif
        mode_safety_enabled_b   = true;
    }
    else
    {
        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
          pairing_buffer[++idx_pair]        = 0;
        #else
          pairing_buffer = pairing_buffer + "0,";
        #endif
        mode_safety_enabled_b   = false;
    }

     #ifdef DEBUG_AT_STARTUP
        Serial.printf("%d ",mode_safety_enabled_b);
     #endif
    
    //check if automatic sign wheater active or not
    automatic_sign_b        = (uint8_t) settings_config[2];
    if(automatic_sign_b)
    {
      #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
        pairing_buffer[++idx_pair]        = 1;
      #else
        pairing_buffer = pairing_buffer + "1,";
      #endif
      automatic_sign_b = true;
    }
    else
    {
      #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
        pairing_buffer[++idx_pair]        = 0;
      #else
        pairing_buffer = pairing_buffer + "0,";
      #endif
      automatic_sign_b = false;
    }

     #ifdef DEBUG_AT_STARTUP
        Serial.printf("%d ",automatic_sign_b);
     #endif
    
    //check if auto power off led wheater active or not
    auto_power_off_led_b    = settings_config[3];
    if(auto_power_off_led_b)
    {
        auto_power_off_time_g   = (uint8_t) settings_config[4];
        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
          pairing_buffer[++idx_pair]         = 1;
          pairing_buffer[++idx_pair]        = auto_power_off_time_g;
        #else
          pairing_buffer = pairing_buffer + "1," + String(settings_config[5], DEC) + String(settings_config[6], DEC) + String(settings_config[7], DEC);
        #endif
    }
    else
    {
        auto_power_off_time_g   = off_time_never_d;
        settings_config[4]      = off_time_never_d;

        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
          pairing_buffer[++idx_pair]        = 0;
          pairing_buffer[++idx_pair]        = 20;
        #else
          pairing_buffer = pairing_buffer + "0,020";
        #endif
    }

    //check mode if it enter thief status or not
    if(settings_config[5] == 1)
    {
      mode_kemalingan_b = false;
    }
    else
    {
      mode_kemalingan_b = false;
    }

    pairing_buffer[++idx_pair]        = 0;

     //setting up interval time for automatic sign to turn off
     if(settings_config[6] == 0xFF)
     {
        interval_ticks_sign = 20 * 4;
        EEPROM.write(interval_time_sign_addr, 20);
        EEPROM.commit();        
     }
     else
     {
        interval_ticks_sign = settings_config[6] * 4;
     }

     if((settings_config[7] != 0xFF) && (settings_config[8] != 0xFF))
     {
       high_score_arkanoid = (uint16_t)(settings_config[7] << 8 | settings_config[8]);
     }
     else
     {
       high_score_arkanoid = 0;
     }

     if((settings_config[9] != 0xFF) && (settings_config[10] != 0xFF))
     {
       high_score_snake = (uint16_t)(settings_config[9] << 8 | settings_config[10]);
     }
     else
     {
       high_score_snake = 0;
     }
     
     if((settings_config[11] != 0xFF) && (settings_config[12] != 0xFF))
     {
       high_score_tetris = (uint16_t)(settings_config[11] << 8 | settings_config[12]);
     }
     else
     {
       high_score_tetris = 0;
     }
     
     if((settings_config[13] != 0xFF) && (settings_config[14] != 0xFF))
     {
       high_score_flappy_bird = (uint16_t)(settings_config[13] << 8 | settings_config[14]);
     }     
     else
     {
       high_score_flappy_bird = 0;
     }
          
     #ifdef DEBUG_AT_STARTUP
        Serial.printf("%d %d %d\r\n",interval_ticks_sign, auto_power_off_led_b, auto_power_off_time_g);
  
        #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
          for(uint8_t x = 0; x < data_pairing_length; x++)
          {
            Serial.print(pairing_buffer[x]);
            Serial.print("-");
          }
          Serial.println("");
        #else
          Serial.println(pairing_buffer.c_str());
        #endif
        
         Serial.printf("hs %d %d %d %d\r\n",high_score_arkanoid, high_score_snake, high_score_tetris, high_score_flappy_bird);
     #endif         
}

/**
 * 
 * @brief ble of esp32 configuration
 * 
*/
void ble_config()
{
    for(uint8_t i = start_addres_ID; i<(start_addres_ID + 10); i++)
    {
      ID_device_g[i] = EEPROM.readChar(i);
    }
    
    #ifdef DEBUG_AT_STARTUP
      Serial.println();
      Serial.println("BLE ID: ");\
      Serial.println(ID_device_g.c_str());
      Serial.println();
    #endif  
 
#if(BLE_LIB_IS_NIMBLE == 1)
    //NimBLEDevice::init(ID_device_g.c_str());
    NimBLEDevice::init("Ciqa2ku_09");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setMTU(160);

    NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    NimBLEService *p_ble_services = pServer->createService(NimBLEUUID(SERVICE_SERVICE_UUID), 33, 0);
    
    p_pairing_characteristic = p_ble_services->createCharacteristic(
                        CHAR_PAIRING_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE | 
                        NIMBLE_PROPERTY::NOTIFY
                      );
    
    p_widget_characteristic = p_ble_services->createCharacteristic(
                        CHAR_WIDGET_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE | 
                        NIMBLE_PROPERTY::NOTIFY
                      );
  
    p_remote_characteristic = p_ble_services->createCharacteristic(
                        CHAR_REMOTE_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY
                      );
    
    p_games_characteristic = p_ble_services->createCharacteristic(
                        CHAR_GAMES_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY
                      );    
    
    p_text_characteristic = p_ble_services->createCharacteristic(
                        CHAR_TEXT_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY 
                    );  
                       
    p_setting_characteristic = p_ble_services->createCharacteristic(
                        CHAR_SETTING_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY
                      );   
      
    p_design_characteristic = p_ble_services->createCharacteristic(
                        CHAR_DESIGN_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY
                    );  
      
    p_image_characteristic = p_ble_services->createCharacteristic(
                        CHAR_IMAGE_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY
                      );   
    
    p_animation_characteristic = p_ble_services->createCharacteristic(
                        CHAR_ANIMATION_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY
                    );  
                                     
    p_playlist_characteristic = p_ble_services->createCharacteristic(
                        CHAR_PLAYLIST_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY
                      );      

    p_uploadGif_characteristic = p_ble_services->createCharacteristic(
                        CHAR_UPLOADGIF_UUID,
                        NIMBLE_PROPERTY::READ | 
                        NIMBLE_PROPERTY::WRITE 
//                        | 
//                        NIMBLE_PROPERTY::NOTIFY
                      );                                                                                    
  
//    p_pairing_characteristic    ->addDescriptor(new BLE2902());
//    p_widget_characteristic 	  ->addDescriptor(new BLE2902());

    p_widget_characteristic    ->setCallbacks(new widget_callback());
    p_remote_characteristic    ->setCallbacks(new remote_callback());
    p_games_characteristic     ->setCallbacks(new games_callback());
    p_text_characteristic      ->setCallbacks(new text_callback());
    p_setting_characteristic   ->setCallbacks(new setting_callback());
    p_design_characteristic    ->setCallbacks(new design_callback());
    p_image_characteristic     ->setCallbacks(new image_callback());
    p_animation_characteristic ->setCallbacks(new animation_callback());
    p_playlist_characteristic  ->setCallbacks(new playlist_callback());
    p_uploadGif_characteristic ->setCallbacks(new uploadGIF_callback());
  
    //Start the service
    p_ble_services->start();

    // Start advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // 0x06 functions that help with iPhone connections issue
    pAdvertising->setMaxPreferred(0xf0);
    NimBLEDevice::startAdvertising();

#else
    //BLEDevice::init(ID_device_g.c_str()); 
    BLEDevice::init("Ciqa2-V2"); 
	  //BLEDevice::init("Ciqa2-tester"); 
    BLEDevice::setMTU(160);
    //BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
  
    // Create the BLE Server and security
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
  
    //-----------------------------------------------------------------------------------------------//
    BLEService *p_ble_services = pServer->createService(BLEUUID((uint32_t)SERVICE_SERVICE_UUID), 33, 0);
    
    p_pairing_characteristic = p_ble_services->createCharacteristic(
                        CHAR_PAIRING_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE | 
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
    
    p_widget_characteristic = p_ble_services->createCharacteristic(
                        CHAR_WIDGET_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE | 
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
  
    p_remote_characteristic = p_ble_services->createCharacteristic(
                        CHAR_REMOTE_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY
                      );
    
    p_games_characteristic = p_ble_services->createCharacteristic(
                        CHAR_GAMES_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY
                      );    
    
    p_text_characteristic = p_ble_services->createCharacteristic(
                        CHAR_TEXT_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY 
                    );  
                       
    p_setting_characteristic = p_ble_services->createCharacteristic(
                        CHAR_SETTING_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY
                      );   
      
    p_design_characteristic = p_ble_services->createCharacteristic(
                        CHAR_DESIGN_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY
                    );  
      
    p_image_characteristic = p_ble_services->createCharacteristic(
                        CHAR_IMAGE_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY
                      );   
    
    p_animation_characteristic = p_ble_services->createCharacteristic(
                        CHAR_ANIMATION_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY
                    );  
                                     
    p_playlist_characteristic = p_ble_services->createCharacteristic(
                        CHAR_PLAYLIST_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY
                      );      

    p_uploadGif_characteristic = p_ble_services->createCharacteristic(
                        CHAR_UPLOADGIF_UUID,
                        BLECharacteristic::PROPERTY_READ | 
                        BLECharacteristic::PROPERTY_WRITE 
//                        | 
//                        BLECharacteristic::PROPERTY_NOTIFY
                      );                                                                                    
  
    p_pairing_characteristic    ->addDescriptor(new BLE2902());
    p_widget_characteristic 	  ->addDescriptor(new BLE2902());
//    p_remote_characteristic   ->addDescriptor(new BLE2902());
//    p_games_characteristic    ->addDescriptor(new BLE2902()); 
//    p_text_characteristic     ->addDescriptor(new BLE2902());
//    p_setting_characteristic  ->addDescriptor(new BLE2902());
//    p_design_characteristic   ->addDescriptor(new BLE2902());
//    p_image_characteristic    ->addDescriptor(new BLE2902());
//    p_animation_characteristic->addDescriptor(new BLE2902());
//    p_playlist_characteristic ->addDescriptor(new BLE2902());

//    p_pairing_characteristic    ->setCallbacks(new pairing_callback());
    p_widget_characteristic    ->setCallbacks(new widget_callback());
    p_remote_characteristic    ->setCallbacks(new remote_callback());
    p_games_characteristic     ->setCallbacks(new games_callback());
    p_text_characteristic      ->setCallbacks(new text_callback());
    p_setting_characteristic   ->setCallbacks(new setting_callback());
    p_design_characteristic    ->setCallbacks(new design_callback());
    p_image_characteristic     ->setCallbacks(new image_callback());
    p_animation_characteristic ->setCallbacks(new animation_callback());
    p_playlist_characteristic  ->setCallbacks(new playlist_callback());
    p_uploadGif_characteristic ->setCallbacks(new uploadGIF_callback());
  
    //Start the service
    p_ble_services->start();
  
    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // 0x06 functions that help with iPhone connections issue
    pAdvertising->setMaxPreferred(0xf0);
    BLEDevice::startAdvertising();
#endif
}

/**
 * 
 * @brief hardware timer of esp32 configuration
 * 
*/
void timer_config()
{
    if (h_timer0.attachInterruptInterval(timer0_interval_ms_d * 1000, timer0_handler))
    {
      h_timer0.stopTimer();
      #ifdef DEBUG_AT_STARTUP
        //Serial.println("[INFO] Timer0 dibuat berhenti terlebih dahulu . . . ");
        //Serial.println("[INFO] Timer0 started . . . ");
      #endif
      h_timer0.restartTimer();
    }
  
    if (h_timer1.attachInterruptInterval(timer1_interval_ms_d * 1000, timer1_handler))
    {
      h_timer1.stopTimer();
      #ifdef DEBUG_AT_STARTUP
        //Serial.println("[INFO] Timer1 dibuat berhenti terlebih dahulu . . . ");
      #endif
    }
//  
    if (h_timer2.attachInterruptInterval(timer2_interval_ms_d * 1000, timer2_handler))
    {
      h_timer2.stopTimer();
      #ifdef DEBUG_AT_STARTUP
        //Serial.println("[INFO] Timer2 dibuat berhenti terlebih dahulu . . . ");
      #endif
    }
  
    if (h_timer3.attachInterruptInterval(timer3_interval_ms_d * 1000, timer3_handler))
    {
      h_timer3.stopTimer();
      #ifdef DEBUG_AT_STARTUP
        //Serial.println("[INFO] Timer3 dibuat berhenti terlebih dahulu . . . ");
      #endif
      timer3_aktif_b = false;
    }
}

//OK