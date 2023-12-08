/** 
 *
 *  @brief function untuk sampling data mpu
 * @param -idx : element counter of data accel x,y, and z array buffer
 * 
 */
void detect_movement_alarm_feature(uint16_t * idx)
{       
    mpu.dmpGetQuaternion(&qrtn, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &qrtn);
    mpu.dmpGetYawPitchRoll(ypr, &qrtn, &gravity);
    
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    f_ax = (1 - LPF_CONSTANTA) * f_ax + LPF_CONSTANTA * ax;
    f_ay = (1 - LPF_CONSTANTA) * f_ay + LPF_CONSTANTA * ay;
    f_az = (1 - LPF_CONSTANTA) * f_az + LPF_CONSTANTA * az;
    
    data_accx_th[*idx]  = (int16_t) f_ax * 1.0; 
    data_accy_th[*idx]  = (int16_t) f_ay * 1.0;
    data_accz_th[*idx]  = (int16_t) f_az * 1.0;

    submitted_ypr[*idx] = ypr[1];     
}


/**
 * 
 * @brief functon to detect stop sign
 * For now sign left and right will active manually by pressing button on apps, then
 * will stop animating when timer counter is exceed the defined value
 * 
 */
void detect_stop_sign_by_mpu(uint16_t *idx, bool dmp_)
{
    if(dmp_)
    {
      mpu.dmpGetQuaternion(&qrtn, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &qrtn);
      mpu.dmpGetYawPitchRoll(ypr, &qrtn, &gravity);
    }
    
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    f_ax = (1 - LPF_CONSTANTA) * f_ax + LPF_CONSTANTA * ax;
    f_ay = (1 - LPF_CONSTANTA) * f_ay + LPF_CONSTANTA * ay;
    f_az = (1 - LPF_CONSTANTA) * f_az + LPF_CONSTANTA * az;
    
    gfx = (float) f_ax / SCALE_FOR_ACCEL;
    gfy = (float) f_ay / SCALE_FOR_ACCEL;
    gfz = (float) f_az / SCALE_FOR_ACCEL;
    
    only_z_axis = gfz - offset_calibration;
    submitted_ypr[*idx] = ypr[1];      //aslinya yaw ada di index 0 kalo index 1 adalah pitch

    #ifndef DEBUG_DATA_DUMMY_MPU_FOR_STOP_SIGN
      //do nothing
    #else
      float data_gf_new      = LPF_OUTPUT_GF * result_gf_average + (1 - LPF_OUTPUT_GF) * prev_result_total_gf;
      prev_result_total_gf  = data_gf_new;
      
      if(mpu_calibrated_b && mode_remote_enabled)
      {
        String data_d         = String(sudden_spike, DEC) + "," + String(automatic_sign_b, DEC)+ "," + String(automatic_sign_b, DEC) + "," + String(mode_remote_enabled, DEC) + "," + String(count_val_below_gf, DEC) + "," + String(dummy_data[idx_data], 3) + "," + String(data_gf_new, 3) + "," + String(result_gf_average, 3) ;
        Serial.println(data_d.c_str());
        result_total_gf       = dummy_data[idx_data];
        idx_data++;

        if(idx_data >= 1222)
        {
          idx_data = 0;
        }
      }
    #endif

    #ifdef CHK_TASK_DURATION_EACH_CORE    
      digitalWrite(CORE_TASK_1_PIN, HIGH);
    #endif

    #ifdef DEBUG_SEND_DATA_MPU_VIA_BLE_FOR_STOP_SIGN
      if(mpu_calibrated_b && mode_remote_enabled)
      {
        String data_d         = String(only_z_axis, 3) + "," + String(threshold_value_var, 3) + "," + String(min_val_happen_var, DEC) + "," + String(abs(gfy), 1);
        p_pairing_characteristic->setValue(std::string(data_d.c_str()));
        #if(BLE_LIB_IS_NIMBLE == 1)
         p_pairing_characteristic->notify(true);
        #else
         p_pairing_characteristic->notify();
        #endif
        
        #ifdef CHK_TASK_DURATION_EACH_CORE
          digitalWrite(CORE_TASK_1_PIN, LOW);
        #endif
      }
    #endif


    //Serial.printf("[INFO] z_axis stop sign: %0.2f %0.2f %d %d\r\n", only_z_axis, threshold_value_var, count_val_below_gf, count_val_above_gf);
    if(only_z_axis < threshold_value_var)
    {
      count_val_below_gf = 0;      
      count_val_above_gf++;
      if(count_val_above_gf >= min_val_happen_var)
      {  
          if(automatic_sign_b && mode_remote_enabled && mpu_calibrated_b)
          {
            sudden_spike = true;
            mode_display_led_g        = ST_REMOTE;
            remote_sign_direction_g   = remote_stop_d;
            Serial.printf("give stop sign A %0.3f %d %d\r\n", only_z_axis, count_val_above_gf, count_val_below_gf);
          }
      }
    }
    else
    {
      if(count_val_above_gf >= min_val_happen_var)
      {
          if(automatic_sign_b && mode_remote_enabled && mpu_calibrated_b)
          {
            sudden_spike = true;
            mode_display_led_g        = ST_REMOTE;
            remote_sign_direction_g   = remote_stop_d;
            Serial.printf("give stop sign B %0.3f %d %d\r\n", only_z_axis, count_val_above_gf, count_val_below_gf);
          }
      }
      
      count_val_below_gf++;    
      if(count_val_below_gf > 7)
      {
        sudden_spike = false;
        count_val_above_gf = 0;
      }
      Serial.printf("check idle pos %0.3f %0.3f %d %d\r\n", only_z_axis, threshold_value_var, count_val_above_gf, count_val_below_gf);
    }

    #ifdef DEBUG_STOP_SIGN_DATA_MPU_FEATURE
      Serial.print(gfx,3);
      Serial.print(",");
      Serial.print(gfy, 3);
      Serial.print(",");
      Serial.print(gfz,3);
      Serial.print(",  ");
      Serial.print(only_z_axis, 3);  
      Serial.print("  ,"); 
      Serial.print(threshold_value_var, 3);  
      Serial.print("  ,");  
      Serial.print(count_val_below_gf);  
      Serial.print("  ,"); 
      Serial.print(count_val_above_gf);  
      Serial.print("  ,");                      
      Serial.println(sudden_spike);  
      //Serial.print("  ,");    
      //Serial.println(cntr_same);
    #endif
}


void split_into_segment(int16_t *data_full, int16_t *segmented_data)
{
    int16_t segment     = 0;
    int16_t idx         = 0;

    for (int8_t i = 0 ; i < TOTAL_UPDATED_DATA ; i++)
    {  
       
       if(i >= ((segment+1) * (TOTAL_UPDATED_DATA/TOTAL_SEGMENT)))
       {
          segment++;
          idx = 0;
       }

       *((segmented_data + segment * (TOTAL_UPDATED_DATA/TOTAL_SEGMENT)) + idx) = data_full[i];
       idx++;       
    }
}

void get_mean_value_accel(int16_t *p_data_array, int16_t *p_mean_val, uint16_t p_total_data)
{ 
      int32_t temp_val = 0;
  
      for(int8_t idx = 0; idx < p_total_data; idx++)
      {
          temp_val += p_data_array[idx];//*( + idx);
      }

      *p_mean_val = (temp_val / p_total_data);
}

void get_threshold_value(int16_t *p_mean_val, int16_t *p_thr_top, int16_t *p_thr_btm)
{
    *p_thr_top = *p_mean_val + g_th_value;
    *p_thr_btm = *p_mean_val - g_th_value;
}

bool cek_posisi_nilai(int16_t *p_mean_val, int16_t *p_b_atas, int16_t *p_b_bawah)
{
  
  if((*p_mean_val > *p_b_bawah) && (*p_mean_val < *p_b_atas))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void get_determine_spike(int16_t *p_data_array, int16_t *p_b_atas, int16_t *p_b_bawah, uint8_t *jmlh_spike)
{
      int16_t value = 0;
      
      for(int8_t idx = 0; idx < TOTAL_UPDATED_DATA; idx++)
      {  
          value = p_data_array[idx];
          if( !cek_posisi_nilai(&value, p_b_atas, p_b_bawah))
          {
             *jmlh_spike = *jmlh_spike + 1;
             //Serial.println("ada spike");
          }
          else
          {
             //Serial.println("tak ade spike");
          }
          //delay(10);
      }
}

int16_t mpu_avrg_ypr(float *ypr_val)
{   
   //float value_tempy = 0.00;
   float value_tempp360 = 0.00;
   float value_tempp    = 0.00;
   //float value_avrg = 0.00;
   
   for(uint8_t i = 0; i < TOTAL_UPDATED_DATA; i++)
   {
     value_tempp360 = value_tempp360 + ((-1 * ypr_val[i] * 180/M_PI) + 180);
     value_tempp    = value_tempp + ( ypr_val[i] * 180/M_PI);
     //Serial.println(ypr_val[i] * 180/M_PI);
   }
//   Serial.print("[INFO] value_tempp360: "); Serial.println(value_tempp360);
//   Serial.print("[INFO] value_tempp: "); Serial.println(value_tempp);
   if(value_tempp360 != 0 || value_tempp != 0)
   {
      return MAX_val((value_tempp360/TOTAL_UPDATED_DATA),(value_tempp/TOTAL_UPDATED_DATA)); 
   }
   else
   {
      return ERROR_CODE_CALIB_MPU;
   }							  
}

void set_sign_animation(uint8_t *prev_dir, uint8_t *new_dir)
{
    //cek apakah mendapatkan perintah untuk menampilkan arah panah ynag baru atau tidak dengan membandingkan data arah panah yang tampil (sebelumnya) dengan perintah tanda panah yang baru.
    if(*prev_dir != *new_dir)
    {
       Serial.printf("[INFO] ----------------------------- new dir &d \r\n", remote_sign_direction_g);
       
       //clear a current animation, whatever animation is it, then clear also memory buffer for animation
       clear_LED(WIDTH_, HEIGHT_);
       memset(buf_led, 0, sizeof(buf_led)); 
       //viewLED(buf_led);

       //reest all variable to count repeated animation
       g_next_part_animation           = 0;
       g_repeat_animation              = 0;

       //give status animation to apps when it connected with led and automatic sign is enabled
       if((device_connected_b && apps_connected_b) && automatic_sign_b)
       {  
          switch(remote_sign_direction_g)
          {
            case remote_up_d:
               buf_send_notify_g[0] = 31;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
               #if(BLE_LIB_IS_NIMBLE == 1)
                  p_widget_characteristic->notify(true);
               #else
                  p_widget_characteristic->notify();
               #endif
            break;
            case remote_left_d:
               buf_send_notify_g[0] = 32;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
               #if(BLE_LIB_IS_NIMBLE == 1)
                  p_widget_characteristic->notify(true);
               #else
                  p_widget_characteristic->notify();
               #endif
            break;
            case remote_right_d:
               buf_send_notify_g[0] = 34;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
               #if(BLE_LIB_IS_NIMBLE == 1)
                  p_widget_characteristic->notify(true);
               #else
                  p_widget_characteristic->notify();
               #endif
            break;
            case remote_stop_d:
               buf_send_notify_g[0] = 33;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
               #if(BLE_LIB_IS_NIMBLE == 1)
                  p_widget_characteristic->notify(true);
               #else
                  p_widget_characteristic->notify();
               #endif
            break;
            default:
               buf_send_notify_g[0] = 30;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
               #if(BLE_LIB_IS_NIMBLE == 1)
                  p_widget_characteristic->notify(true);
               #else
                  p_widget_characteristic->notify();
               #endif
            break;
          }
           
           //reset tick timer2 untuk rating kanan/kiri (otomatis)
           //saat mode otomatis rating kanan dan kiri akan mati berdasarkan timer --> logic mematikannya ada di fungsi predict_action() -->(sub) determine_bike_direction()
           if((remote_sign_direction_g == remote_right_d) || (remote_sign_direction_g == remote_left_d))
           {                                     
              if(!b_timer2_active)
              {
                h_timer2.restartTimer();
                b_timer2_active         = true;
                timer2_tick_g           = 0;
              }
              else
              {
                timer2_tick_g           = 0;
              }
           }
       }
       
        switch(*new_dir)
        {
        case FORWARD:
           g_frame        = 0;
           b_blank_array  = 0;
        break;
        case LEFT:
           g_frame        = 0;
           b_blank_array  = 0;
        break;
        case RIGHT:
           g_frame        = WIDTH_ - 1;
           b_blank_array  = 0;
        break;
        case REM_STOP:
           //g_frame        = 5;
           b_blank_array  = 0;
        break;
        default:
           //Serial.println("[INFO - 332] UNKNOW-----");
        break;
       }     
    }
    else
    {
      //jika arah panah yang tampil adalah mwnunjukan ke atas (maju) maka animasi ini akan tampil dengan diulang sebanyak 20 kali baik saat mode otomatis aktif
      //ataupun mode otomatis tidak aktif
      if(*new_dir == FORWARD)
      {
        if(g_repeat_animation > MAX_REPEAT_FORWARD_ANIMATION)
        {
//           if(automatic_sign_b && (*new_dir != NO_ACTION))
//           {
//             if(device_connected_b && apps_connected_b)
//             {
//               buf_send_notify_g[0] = 30;
//               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
//                  #if(BLE_LIB_IS_NIMBLE == 1)
//                     p_widget_characteristic->notify(true);
//                  #else
//                     p_widget_characteristic->notify();
//                  #endif
//             }
//           }
           
           //*new_dir                     = FORWARD;
           //prev 
           //g_repeat_animation        = 0;
           //Serial.println(mode_display_led_g);
           
           if(mode_display_led_g         == ST_REMOTE)
           {
               SIGN_ANIMATION            = OFF;
               forward_has_over_b        = true;
               mode_display_led_g        = ST_ANIMATION;
               remote_sign_direction_g   = remote_up_d;

               if(device_connected_b && apps_connected_b)
               {
                  buf_send_notify_g[0] = 30;
                  p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
                  #if(BLE_LIB_IS_NIMBLE == 1)
                     p_widget_characteristic->notify(true);
                  #else
                     p_widget_characteristic->notify();
                  #endif
               }
           }
        }
      }
      //handle logic jika menampilkan tanda stop maka saat mode otmatis diaktifkan, akan menampilakan tanda stop sebanyak 6 kali, jika mode otomatis tidak aktif
      //maka tanda stop akan terus menerus aktif hingga tombol stop (hp/remote) dinon aktifkan
      else if(*new_dir == REM_STOP)
      {
        //ini utk handle logic tampilan animasi stopnya agar berhenti menampilkan tanda stop secara otomatis
        if(g_repeat_animation >= MAX_REPEAT_STOP_ANIMATION && automatic_sign_b)
        {
          g_repeat_animation        = 0;
          forward_has_over_b        = false;
          mode_display_led_g        = ST_REMOTE;
          remote_sign_direction_g   = remote_up_d;
        }
      }
      else
      {
        //do nothing
      }   
    }
    
    Serial.print("repeat ");
    Serial.println(g_repeat_animation); 
    *prev_dir = *new_dir;
}

/**
 * @brief function to determine cycling direction automatically
 * 
 */
void determine_bike_direction()
{     
   if((abs(gfy) > abs(gfz)) && (abs(gfy) > abs(gfx)))
   {    
        if (sudden_spike) // && (prev_sign_stop_or_fwd != remote_stop_d)
        {
          if(automatic_sign_b && mode_remote_enabled)
          {
            mode_display_led_g        = ST_REMOTE;
            remote_sign_direction_g   = remote_stop_d;
            forward_has_over_b        = false;
            prev_sign_stop            = true;
            prev_sign_stop_or_fwd     = remote_stop_d;
            Serial.println("[INFO - 133] STOPING THE BIKE....");
          }
        }
        else if(g_sudut_kemiringan > (STEADY_VAL_MIN + THRESHOLD_DEG))
        {          
//           if(automatic_sign_b && mode_remote_enabled)
//           {
//             mode_display_led_g        = ST_REMOTE;
//             remote_sign_direction_g   = remote_left_d;         
//             forward_has_over_b        = false;
//             //Serial.println("[INFO - 141] LEFT DIRECTION....");
//           }
        }
        else if (g_sudut_kemiringan < (STEADY_VAL_MIN - THRESHOLD_DEG))
        {
//           if(automatic_sign_b && mode_remote_enabled)
//           {
//             mode_display_led_g        = ST_REMOTE;
//             remote_sign_direction_g   = remote_right_d;
//             forward_has_over_b        = false;
//             //Serial.println("[INFO - 149] RIGHT DIRECTION....");
//           }
        }
        else if (((g_sudut_kemiringan >= (STEADY_VAL_MIN - THRESHOLD_DEG)) && (g_sudut_kemiringan <= (STEADY_VAL_MIN + THRESHOLD_DEG))))
        {
           if(automatic_sign_b && mode_remote_enabled)
           {
             if(!forward_has_over_b && prev_sign_stop)
             {
                if(remote_sign_direction_g != remote_stop_d)
                { 
                  mode_display_led_g        = ST_REMOTE;
                  remote_sign_direction_g   = remote_up_d;
                  prev_sign_stop            = false;
                  prev_sign_stop_or_fwd     = remote_up_d;
                }
             }
             //Serial.println("[INFO -156] FORWARD DIRECTION....");
           }
        }
        else
        { 
          //b_animasi_diproses = false;
          //Serial.println("[INFO - 279] Unknown DIRECTION....");
        }
   }
   //when the led (means bag, cover bag or what ever) is laying down, we don't show any animation then it would stop every animation displayed on leds
   //animation will stop when automatic feature is enabled otherwise it will countinue to show
   else if((abs(gfz) > abs(gfy)) && (abs(gfz) > abs(gfx)))
   {
      if((g_repeat_animation > 6))
      {
         if(automatic_sign_b && (DIRECTION != NO_ACTION))
         {
           if(device_connected_b && apps_connected_b)
           {
               buf_send_notify_g[0] = 30;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
               #if(BLE_LIB_IS_NIMBLE == 1)
                  p_widget_characteristic->notify(true);
               #else
                  p_widget_characteristic->notify();
               #endif
           }


           DIRECTION                     = NO_ACTION;
           
           if(mode_display_led_g         == ST_REMOTE)
           {
               SIGN_ANIMATION            = OFF;
               mode_display_led_g        = ST_ANIMATION;
  
              //nonaktifkan timer2 untuk hitung rating-kanan-kiri
              if(b_timer2_active)
              {
                h_timer2.stopTimer();
                b_timer2_active         = false;
                timer2_tick_g           = 0;
              }
           }
         }
      }
      else
      {
         //do nothing
      }
   }
   else
   {
     // do nothing
   }

   //cek durasi timer rating kanan dan kiri, jika sudah melebihi durasi yang ditentukan maka diganti menjadi animasi forward (maju)
   //Serial.printf("ring2: %d, %d, %d, %d, %d, %d\r\n", SIGN_ANIMATION, DIRECTION, automatic_sign_b, mode_remote_enabled, timer2_tick_g, interval_ticks_sign);
   if((SIGN_ANIMATION == ON) && ((DIRECTION == RIGHT) || (DIRECTION == LEFT)))
   {
      if(automatic_sign_b && mode_remote_enabled)
      {
         if(timer2_tick_g >= interval_ticks_sign)
         {
            Serial.println("change direction");
            mode_display_led_g        = ST_REMOTE;
            remote_sign_direction_g   = remote_up_d;
            
            if(device_connected_b && apps_connected_b)
            {
               buf_send_notify_g[0] = 30;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
               #if(BLE_LIB_IS_NIMBLE == 1)
                  p_widget_characteristic->notify(true);
               #else
                  p_widget_characteristic->notify();
               #endif
            }     
         }
      }
   }
   //prev_direction = DIRECTION;
}

/**
 * @brief function untuk melakukan aksi berupa tampilan animasi di led sesuai olah data hasil prediction
 */
void action_base_on_prediction()
{ 
   if(mode_safety_enabled_b && !mode_remote_enabled)
   {
      if(b_deteksi_dipindahkan)
      {     
        
          Serial.println(timer2_tick_g);

          if((g_mean_value[g_coor_z] > g_mean_value[g_coor_y]) && (g_mean_value[g_coor_z] > g_mean_value[g_coor_x]))
          {             
              if(timer2_tick_g >= g_max_timer_counter)
              {
//                 Serial.print("Jumlah spike x ");
//                 Serial.print(g_jmlh_spike_x);
//                 Serial.print(" -Jumlah spike y ");
//                 Serial.print(g_jmlh_spike_y);
//                 Serial.print(" -Jumlah spike z ");
//                 Serial.println(g_jmlh_spike_z);
                 Serial.println("[INFO - 383] tick2 >");
                 if(g_jmlh_spike_x >= thr_value)
                 {
                    g_cek_maling_counter++;
                    Serial.print("BUZZER WARNING    =>  ");
                    Serial.println(g_cek_maling_counter);
                    tone_warning();
                    if(g_cek_maling_counter >= g_jmlh_counter_maling)
                    {
                       mode_kemalingan_b     = true;
                       mode_display_led_g    = ST_THIEF_ALARM;
                       notif_kemalingan      = 2;
                    }
                 }
                 else
                 {
                    g_cek_maling_counter     = 0;
                    b_determine_stable_value = true;
                 }

                 g_jmlh_spike   = 0;
                 timer2_tick_g    = 0;
                 g_jmlh_spike_x = 0;
                 g_jmlh_spike_y = 0;
                 g_jmlh_spike_z = 0;
              }
              else
              {
                 Serial.println("[INFO -413] tick2 <");
                 if(g_jmlh_spike_x >= thr_value)
                 {
                    Serial.println("[INFO] bergerak");
                    tone_warning();
                    //g_counter = 0; 
                  //   g_jmlh_spike   =  g_jmlh_spike + g_jmlh_spike_x;
                  //   g_jmlh_spike_x = 0; 
                 }
                 
                 //g_jmlh_spike   =  g_jmlh_spike + g_jmlh_spike_x;
              }
          }
          else if((g_mean_value[g_coor_y] > g_mean_value[g_coor_z]) && (g_mean_value[g_coor_y] > g_mean_value[g_coor_x]))   
          {             
              if(timer2_tick_g >= g_max_timer_counter)
              {
//                 Serial.print("Jumlah spike x ");
//                 Serial.print(g_jmlh_spike_x);
//                 Serial.print(" -Jumlah spike y ");
//                 Serial.print(g_jmlh_spike_y);
//                 Serial.print(" -Jumlah spike z ");
//                 Serial.println(g_jmlh_spike_z);
                 Serial.println("[INFO - 427] tick2 >");
                 if(g_jmlh_spike_z >= thr_value)
                 {
                    g_cek_maling_counter++;
                    Serial.print("BUZZER WARNING    =>  ");
                    Serial.println(g_cek_maling_counter);
                    tone_warning();
                    if(g_cek_maling_counter >= g_jmlh_counter_maling)
                    {
                       mode_kemalingan_b     = true;
                       mode_display_led_g    = ST_THIEF_ALARM;
                       notif_kemalingan      = 2;
                    }
                 }
                 else
                 {  
                    g_cek_maling_counter     = 0;
                    b_determine_stable_value = true;
                 }
      
                 g_jmlh_spike   = 0;
                 timer2_tick_g    = 0;
                 g_jmlh_spike_x = 0;
                 g_jmlh_spike_y = 0;
                 g_jmlh_spike_z = 0;
              }
              else
              {
                 Serial.println("[INFO - 452] tick2 <");
                 if(g_jmlh_spike_z >= thr_value)
                 {
                    Serial.println("[INFO] bergerak");
                    tone_warning();  
                    //g_counter = 0; 
                  //   g_jmlh_spike   =  g_jmlh_spike + g_jmlh_spike_x;
                  //   g_jmlh_spike_z = 0;                 
                 }  
                 
                 //g_jmlh_spike   =  g_jmlh_spike + g_jmlh_spike_z;              
              }
          }
          else
          {
            b_determine_stable_value = true;
          }
          b_deteksi_dipindahkan = false;
      }
      else
      {
//         g_jmlh_spike   = 0;
      //   g_jmlh_spike_x = 0;
//         g_jmlh_spike_y = 0;
      //   g_jmlh_spike_z = 0;
      }
  }
   else
   {
      g_jmlh_spike  = 0;
      //Serial.println("[INFO] play_animation");
      play_animation();
   }
}

/**
 * 
 * @brief function untuk memprediksi animasi led yang akan ditampilkan (sign kanan, kiri, maju, stop, kemalingan)
 * 
 */
void predict_action()
{
  if(mode_safety_enabled_b && !mode_remote_enabled)
  {
    if(!timer2_enabled_for_safety)
    {
      tone_mode_safety(); 
      if(!b_timer2_active)
      {			   
          h_timer2.restartTimer();
          b_timer2_active = true;
      }
      timer2_enabled_for_safety = true;
    }
    
    if(b_determine_stable_value)
    {     
      //clear_LED(WIDTH_, HEIGHT_);
      Serial.print("g_counter: ");
      Serial.println(g_counter );
      get_mean_value_accel(data_accx_th, &g_mean_val_x[g_counter], TOTAL_UPDATED_DATA);
      get_mean_value_accel(data_accy_th, &g_mean_val_y[g_counter], TOTAL_UPDATED_DATA);
      get_mean_value_accel(data_accz_th, &g_mean_val_z[g_counter], TOTAL_UPDATED_DATA);

      g_counter++;
      
      if(g_counter >= g_max_counter)
      {
          get_mean_value_accel(g_mean_val_x, &g_mean_value[g_coor_x], g_max_counter);
          get_threshold_value(&g_mean_value[g_coor_x], &g_value_th_top[g_coor_x], &g_value_th_btm[g_coor_x]);  
        
          get_mean_value_accel(g_mean_val_y, &g_mean_value[g_coor_y], g_max_counter);
          get_threshold_value(&g_mean_value[g_coor_y], &g_value_th_top[g_coor_y], &g_value_th_btm[g_coor_y]);  
        
          get_mean_value_accel(g_mean_val_z, &g_mean_value[g_coor_z], g_max_counter);
          get_threshold_value(&g_mean_value[g_coor_z], &g_value_th_top[g_coor_z], &g_value_th_btm[g_coor_z]);  
         
          b_determine_stable_value = false;
         
          action_base_on_prediction();

          timer2_tick_g   = 0;
          g_counter     = 0;
      }
    }
    else
    {
       get_determine_spike(data_accx_th, &g_value_th_top[g_coor_x], &g_value_th_btm[g_coor_x], &g_jmlh_spike_x);
       get_determine_spike(data_accy_th, &g_value_th_top[g_coor_y], &g_value_th_btm[g_coor_y], &g_jmlh_spike_y);
       get_determine_spike(data_accz_th, &g_value_th_top[g_coor_z], &g_value_th_btm[g_coor_z], &g_jmlh_spike_z);
       
       b_deteksi_dipindahkan = true;
       action_base_on_prediction();
    }
   
     memset(data_accx_th, 0, sizeof(data_accx_th));
     memset(data_accy_th, 0, sizeof(data_accy_th));
     memset(data_accz_th, 0, sizeof(data_accz_th));  
  }
  else
  {     
      if(b_timer2_active && timer2_enabled_for_safety)
      {
		      tone_mode_setting();					   
          h_timer2.stopTimer();
          b_timer2_active = false;
          b_determine_stable_value = true;
          timer2_tick_g   = 0;    
          timer2_enabled_for_safety = false;
//          g_counter     = 0;
          memset(g_mean_value  , 0 , sizeof(g_mean_value));
          memset(g_value_th_top, 0 , sizeof(g_value_th_top));
          memset(g_value_th_btm, 0 , sizeof(g_value_th_btm));           
      }

      if((buf_idx_mpu >= TOTAL_UPDATED_DATA))
      {
          g_sudut_kemiringan  = (int16_t)mpu_avrg_ypr(submitted_ypr);    
          memset(submitted_ypr, 0, sizeof(submitted_ypr));   
          buf_idx_mpu = 0;  
      }
    
      //Serial.println("[INFO] determine_bike_direction");
      determine_bike_direction();
  }
}


/**
 * 
 * @brief function to calibrated mpu position on its current position
 * 
*/
void calibrate_mpu()
{
   clear_LED(WIDTH_, HEIGHT_);
   Serial.println("[INFO] Calib proses...");

   float sum_gf_calib = 0.0;
   for(uint8_t idx_mpu = 0; idx_mpu < TOTAL_UPDATED_DATA; idx_mpu++)
   {
      submitted_ypr[idx_mpu] = ypr[1];
      sum_gf_calib += gfz;
      delay(100);

      if(device_connected_b && apps_connected_b)
      {
          //notify_progress_b = true;
          progres_status    = (float)((float)idx_mpu/TOTAL_UPDATED_DATA) * 100;

          memset(buf_send_notify_g, 0, sizeof(buf_send_notify_g));
          
          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 0)
             if(progres_status < 10)
             {
               sprintf(buf_send_notify_g, "%%0%d", progres_status);
             }
             else
             {
               if(progres_status >= 100)
               { 
                  progres_status = 99;
               }
               sprintf(buf_send_notify_g, "%%%d", progres_status);
             }
          #else
             buf_send_notify_g[0] = '%';
             buf_send_notify_g[1] = progres_status;
          #endif
    
          if(progres_status >= 99)
          {
             progres_status = 0;
          }
          
          //Serial.println(buf_send_notify_g);    
          #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
             p_widget_characteristic->setValue(&buf_send_notify_g[0], 2);
          #else         
             p_widget_characteristic->setValue(buf_send_notify_g);
          #endif
    
          #if(BLE_LIB_IS_NIMBLE == 1)
             p_widget_characteristic->notify(true);
          #else
             p_widget_characteristic->notify();
          #endif
          notify_progress_b       = false;
      }
   }


   offset_calibration = (float)sum_gf_calib/TOTAL_UPDATED_DATA;
   STEADY_VAL_MIN = (int16_t)mpu_avrg_ypr(submitted_ypr); 
   Serial.print("STEADY_VAL_MIN:");
   Serial.println(STEADY_VAL_MIN);
   
   if(STEADY_VAL_MIN != ERROR_CODE_CALIB_MPU)
   {
      if(device_connected_b && apps_connected_b)
      {
         #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
               buf_send_notify_g[0]  = 20;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
         #else
               p_widget_characteristic->setValue("20");
         #endif

         #if(BLE_LIB_IS_NIMBLE == 1)
            p_widget_characteristic->notify(true);
         #else
            p_widget_characteristic->notify();
         #endif
      }
      mpu_calibrated_b = true;
   }
   else
   {
      if(device_connected_b && apps_connected_b)
      {
         #if(EXCHANGE_DATA_ON_INT_NOT_STR == 1)
               buf_send_notify_g[0]  = 21;
               p_widget_characteristic->setValue(&buf_send_notify_g[0], 1);
         #else
               p_widget_characteristic->setValue("21");
         #endif

         #if(BLE_LIB_IS_NIMBLE == 1)
            p_widget_characteristic->notify(true);
         #else
            p_widget_characteristic->notify();
         #endif
      }
      
      mpu_calibrated_b = false;
   }
}

//OK--