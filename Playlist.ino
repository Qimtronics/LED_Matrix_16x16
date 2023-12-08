uint16_t size_(uint8_t tipe_data_)
{
   switch(tipe_data_)
   {
     case data_text_d: 
        return size_buffer_text_d;
     break;
     case data_image_d: 
        return size_buffer_gambar_d;
     break;
     case data_animasi_d: 
        return data_size_animasi_g;
     break;
     case data_gif_d: 
        return size_gif_file_received;
     break;
     default:
        return 1;
     break;
   }
}

uint16_t size_sectore_used_d(uint8_t tipe_data_)
{
   switch(tipe_data_)
   {
     case data_text_d: 
        return 1;
     break;
     case data_image_d: 
        return 4;
     break;
     case data_animasi_d: 
        //return (data_size_animasi / 256) + 1;
        return 61;
     break;
     case data_gif_d: 
        return 61;
     break;
     default:
        return 0;
     break;
   }
}

void protect_eeprom_memory(bool b_yes_or_no)
{
    if(b_yes_or_no)
    {
        digitalWrite(WP_PIN, LOW) ;
        digitalWrite(HOLD_PIN, LOW); 
    }
    else
    {
        digitalWrite(WP_PIN, HIGH) ;
        digitalWrite(HOLD_PIN, HIGH); 
    }
}

uint8_t get_data_info_from_memory(uint32_t *mem_addres, uint8_t * buff)
{ 

    #if(CHK_AND_DEBUG_PLAYLIST == 1)
      Serial.print("[INFO] info data ->>> alamat memori: ");
      Serial.print(*mem_addres, HEX);  
      Serial.print(" -- buf0 ");
      Serial.print(buff[0], HEX); 
      Serial.print(" -- buf1 ");
      Serial.print(buff[1], HEX); 
      Serial.print(" -- buf2 ");
      Serial.println(buff[2], HEX);  
    #endif

    if((buff[0] == header_file_eeprom_g[0]) && (buff[1] == header_file_eeprom_g[1]) && (buff[2] == header_file_eeprom_g[2]))
    {         
        return true;
    }
    else
    {
        return false;
    }
}

/*
uint8_t check_data_header_eeprom(uint32_t *mem_addres)
{
    uint8_t dest_addres = *mem_addres;
    uint8_t header[3]   = {0,0,0};

    header[0] = flash.readByte(dest_addres);
    header[1] = flash.readByte(++dest_addres);
    header[2] = flash.readByte(++dest_addres);

    dest_addres = dest_addres + 3;
    type_data_to_show_g         = flash.readByte(dest_addres);

    #if(CHK_AND_DEBUG_PLAYLIST == 1)
      Serial.print("[INFO] info data ->>> alamat memori: ");
      Serial.print(*mem_addres, HEX);  
      Serial.print(" -- buf0 ");
      Serial.print(header[0], HEX); 
      Serial.print(" -- buf1 ");
      Serial.print(header[1], HEX); 
      Serial.print(" -- buf2 ");
      Serial.println(header[2], HEX);  
      Serial.print("type_data_to_show_g ");
      Serial.println(type_data_to_show_g);               
    #endif


    if((header[0] == header_file_eeprom_g[0]) && (header[1] == header_file_eeprom_g[1]) && (header[2] == header_file_eeprom_g[2]))
    {         
        return true;
    }
    else
    {
        return false;
    }
}
*/

void check_saved_data_in_eeprom(uint8_t size_sector, uint32_t addres)
{
   xcount_mem = 0;
   
   protect_eeprom_memory(false);
   Serial.println("size sector: ");
   Serial.println(size_sector);
   Serial.println("last address: ");
   Serial.println(addres);
   Serial.println("total used page: ");
   Serial.println((size_sector/0x100));
   for(xcount_mem = 0; xcount_mem < size_sector; xcount_mem++)
   {   
       memset(bufs_for_check_and_save_g, 0, sizeof(bufs_for_check_and_save_g));
       flash_mem.readByteArray(((addres - (size_sector * page_memory_size_d)) + (xcount_mem * page_memory_size_d)), bufs_for_check_and_save_g, page_memory_size_d);
       
       Serial.print("[INFO] alamat sector (alamat awal penyimpanan):");
       Serial.println(((addres - (size_sector * page_memory_size_d)) + (xcount_mem * page_memory_size_d)), HEX);
       
       ycount_mem = 0;
       Serial.print("[INFO] isi data: ");
       for(ycount_mem = 0; ycount_mem < 256; ycount_mem++)
        {
            Serial.print(bufs_for_check_and_save_g[ycount_mem], HEX);
            Serial.print("-");
        }
        
        Serial.println();
        //read_page((addres - size_sector) + xcount_mem);
   }
   
   protect_eeprom_memory(true);
}

bool check_available_sector_size(uint32_t *current_address, uint8_t type_data)
{
  switch(type_data)
  {
    case data_text_d:
        if(MAX_ADDRESS_MEMORY > (*current_address + (size_sectore_used_d(data_text_d) * page_memory_size_d)))
        {
           return true;
        }
        else
        {
          return false;
        }
    break;
    case data_image_d:
        if(MAX_ADDRESS_MEMORY > (*current_address + (size_sectore_used_d(data_image_d) * page_memory_size_d)))
        {
           return true;
        }
        else
        {
          return false;
        }
    break;
    case data_animasi_d:
        if(MAX_ADDRESS_MEMORY > (*current_address + (size_sectore_used_d(data_animasi_d) * page_memory_size_d)))
        {
           return true;
        }
        else
        {
          return false;                                              
        }
    break;
    case data_gif_d:
        if(MAX_ADDRESS_MEMORY > (*current_address + (size_sectore_used_d(data_gif_d) * page_memory_size_d)))
        {
           return true;
        }
        else
        {
          return false;                                              
        }
    break;
    default:
        return false;
    break;
  }
}

uint32_t check_available_addr_memory(uint32_t mem_addr)
{
    bool data_exist_b           = true;
  
    protect_eeprom_memory(false);
    
    memset(bufs_for_check_and_save_g, 0, sizeof(bufs_for_check_and_save_g));
    
    while(data_exist_b)
    {
      flash_mem.readByteArray(mem_addr, bufs_for_check_and_save_g, page_memory_size_d);
      data_exist_b = get_data_info_from_memory(&mem_addr, bufs_for_check_and_save_g);
  
      if(data_exist_b)
      {
        mem_addr = mem_addr + (size_sectore_used_d(bufs_for_check_and_save_g[5]) * page_memory_size_d); 
        amount_of_playlist_show_g++;
      }
    }
    
    protect_eeprom_memory(true);
    return mem_addr;
}

void add_file_to_playlist(uint32_t *addrs, uint8_t *data_buf)
{ 
  uint16_t checksum                    = 0x0000;
  uint8_t  size_sector_tosave          = size_sectore_used_d(tipe_data_received_g);
  uint16_t pos_idx_copied_arr          = 0;
  uint16_t total_data_required_to_save = size_(tipe_data_received_g);
  uint32_t prev_address                = *addrs;
  bool  save_sukses_b                  = false;

  memset(bufs_for_check_and_save_g, 0, sizeof(bufs_for_check_and_save_g));
  
  bufs_for_check_and_save_g[0] = header_file_eeprom_g[0];  //header Q
  bufs_for_check_and_save_g[1] = header_file_eeprom_g[1];  //header I
  bufs_for_check_and_save_g[2] = header_file_eeprom_g[2];  //header M

  switch(tipe_data_received_g)
  {
    case data_text_d:
      bufs_for_check_and_save_g[3] = 0x00;                 //data length MSB
      bufs_for_check_and_save_g[4] = 0x1E;                 //data length LSB
    break;
    case data_image_d:
      if(receive_design_not_image_b)
      {
        bufs_for_check_and_save_g[3] = 0x03;                 //data length MSB
        bufs_for_check_and_save_g[4] = 0x04;                 //data length LSB  0x06 -> image
      }
      else
      {
        bufs_for_check_and_save_g[3] = 0x03;                 //data length MSB
        bufs_for_check_and_save_g[4] = 0x06;                 //data length LSB  0x06 -> image
      }
    break;
    case data_animasi_d:
      bufs_for_check_and_save_g[3] = (total_data_required_to_save >> 8) & 0xFF;           //data length MSB
      bufs_for_check_and_save_g[4] =  total_data_required_to_save & 0xFF;                 //data length LSB
    break;
    case data_gif_d:
      bufs_for_check_and_save_g[3] = (total_data_required_to_save >> 8) & 0xFF;           //data length MSB
      bufs_for_check_and_save_g[4] =  total_data_required_to_save & 0xFF;                 //data length LSB
    break;
    default:
    break;
  }

//   Serial.println("===================================");
//   Serial.print(bufs_for_check_and_save_g[3], HEX);
//   Serial.println(bufs_for_check_and_save_g[4], HEX);
//   Serial.println("===================================");
  
  bufs_for_check_and_save_g[5] = tipe_data_received_g; 
  
  checksum                     = checksum + tipe_data_received_g;
  
  #ifdef DEBUG_PLAYLIST_EEPROM  
    Serial.print("[INFO] size data yang disimpan + header memori: ");
    Serial.println(total_data_required_to_save); 
    Serial.print("[INFO] alamat penyimpanan ");
    Serial.println(*addrs, HEX); 
    Serial.print("[INFO] total sektor dipakai ");
    Serial.println(size_sector_tosave);  
    Serial.print("[INFO] Chek header tambahan: ");
    for(byte q = 0; q < addres_header_size_eeprom_d; q++)
    {
        Serial.print(bufs_for_check_and_save_g[q], HEX); 
        Serial.print("-"); 
    }
    Serial.println();
  #endif

  protect_eeprom_memory(false);
  
  xcount_mem = 0;
  for(xcount_mem = 0; xcount_mem < size_sector_tosave; xcount_mem++)
  {   
      ycount_mem = 0;
      for(ycount_mem = 0; ycount_mem < page_memory_size_d; ycount_mem++)
      {
         if((xcount_mem == 0) && (ycount_mem < addres_header_size_eeprom_d))
         {
            //do nothing
         }
         else
         {
              switch(tipe_data_received_g)
              {
                case data_text_d:
                  if(pos_idx_copied_arr < size_buffer_text_d)
                  { 
                    bufs_for_check_and_save_g[ycount_mem] = *( data_buf + pos_idx_copied_arr);
                    checksum                              = checksum + bufs_for_check_and_save_g[ycount_mem];
                  }
                  else if((pos_idx_copied_arr >= size_buffer_text_d) && (pos_idx_copied_arr < (size_buffer_text_d + 1)))
                  {
                    bufs_for_check_and_save_g[ycount_mem] = 0x3B;  
                    checksum                              = checksum + 0x3B;
                  
                  }
                  else if((pos_idx_copied_arr >= (size_buffer_text_d + 1)) && (pos_idx_copied_arr < (size_buffer_text_d + 2)))
                  {
                    checksum                              = 0xFFFF - checksum;
                    bufs_for_check_and_save_g[ycount_mem] = (checksum >> 8) & 0xFF; 
                  }
                  else if((pos_idx_copied_arr > (size_buffer_text_d + 2)) && (pos_idx_copied_arr == (size_buffer_text_d + 3)))
                  {
                    bufs_for_check_and_save_g[ycount_mem] = checksum & 0xFF; 
                  }
                  else
                  {
                    bufs_for_check_and_save_g[ycount_mem] = 0x00; 
                  }
                  
                break;
                case data_image_d:
                   if(pos_idx_copied_arr < size_buffer_gambar_d)
                   {
                      bufs_for_check_and_save_g[ycount_mem] = *( data_buf + pos_idx_copied_arr);
                      checksum                              = checksum + bufs_for_check_and_save_g[ycount_mem];
                   }
                  else if((pos_idx_copied_arr >= size_buffer_gambar_d) && (pos_idx_copied_arr < (size_buffer_gambar_d + 1)))
                  {
                    bufs_for_check_and_save_g[ycount_mem] = 0x3B;  
                    checksum                              = checksum + 0x3B;
                  
                  }
                  else if((pos_idx_copied_arr >= (size_buffer_gambar_d + 1)) && (pos_idx_copied_arr < (size_buffer_gambar_d + 2)))
                  {
                    checksum                              = 0xFFFF - checksum;
                    bufs_for_check_and_save_g[ycount_mem] = (checksum >> 8) & 0xFF; 
                  }
                  else if((pos_idx_copied_arr > (size_buffer_gambar_d + 2)) && (pos_idx_copied_arr == (size_buffer_gambar_d + 3)))
                  {
                    bufs_for_check_and_save_g[ycount_mem] = checksum & 0xFF; 
                  }
                  else
                  {
                    bufs_for_check_and_save_g[ycount_mem] = 0x00; 
                  }
                break;
                
                case data_animasi_d:
                   if(pos_idx_copied_arr == 0)
                   {
                      bufs_for_check_and_save_g[ycount_mem] = fps_animasi_received_data_g;
                   }
                   else if (pos_idx_copied_arr == 1)
                   {
                      bufs_for_check_and_save_g[ycount_mem] = total_frame_animasi_g;
                   }
                   else if((pos_idx_copied_arr - 2) < size_buffer_animasi_d)
                   {  
                      bufs_for_check_and_save_g[ycount_mem] = *( data_buf + (pos_idx_copied_arr - 2));
                      checksum                              = checksum + bufs_for_check_and_save_g[ycount_mem];
                   }
                  else if(((pos_idx_copied_arr - 2) >= size_buffer_animasi_d) && ((pos_idx_copied_arr - 2) < (size_buffer_animasi_d + 1)))
                  {
                    bufs_for_check_and_save_g[ycount_mem] = 0x3B;  
                    checksum                              = checksum + 0x3B;
                  }
                  else if(((pos_idx_copied_arr - 2) >= (size_buffer_animasi_d + 1)) && ((pos_idx_copied_arr - 2) < (size_buffer_animasi_d + 2)))
                  {
                    checksum                              = 0xFFFF - checksum;
                    bufs_for_check_and_save_g[ycount_mem] = (checksum >> 8) & 0xFF; 
                  }
                  else if(((pos_idx_copied_arr - 2) > (size_buffer_animasi_d + 2)) && ((pos_idx_copied_arr - 2) < (size_buffer_animasi_d + 3)))
                  {
                    bufs_for_check_and_save_g[ycount_mem] = checksum & 0xFF; 
                  }
                  else
                  {
                    bufs_for_check_and_save_g[ycount_mem] = 0x00; 
                  }
                break;

                case data_gif_d:
                  if(pos_idx_copied_arr < size_buffer_gif_d)
                  {
                    bufs_for_check_and_save_g[ycount_mem] = *( data_buf + pos_idx_copied_arr);
                    checksum                              = checksum + bufs_for_check_and_save_g[ycount_mem];
                  }
                  else if((pos_idx_copied_arr >= size_buffer_gif_d) && (pos_idx_copied_arr < (size_buffer_gif_d + 1)))
                  {
                    bufs_for_check_and_save_g[ycount_mem] = 0x3B;  
                    checksum                              = checksum + 0x3B;
                  
                  }
                  else if((pos_idx_copied_arr >= (size_buffer_gif_d + 1)) && (pos_idx_copied_arr < (size_buffer_gif_d + 2)))
                  {
                    checksum                              = 0xFFFF - checksum;
                    bufs_for_check_and_save_g[ycount_mem] = (checksum >> 8) & 0xFF; 
                  }
                  else if((pos_idx_copied_arr > (size_buffer_gif_d + 2)) && (pos_idx_copied_arr == (size_buffer_gif_d + 3)))
                  {
                    bufs_for_check_and_save_g[ycount_mem] = checksum & 0xFF; 
                  }
                  else
                  {
                    bufs_for_check_and_save_g[ycount_mem] = 0x00; 
                  }                
                break;

                default:
                    Serial.println("[INFO] uncategorized data yang akan disimpan");
                break;
              }
              

            
            #ifdef DEBUG_PLAYLIST_CHECK_VAL 
              Serial.print("[INFO] process simpan data, alamat: ");
              Serial.print(*addrs, HEX);
              Serial.print("-- data: ");
              Serial.println(bufs_for_check_and_save_g[ycount_mem], HEX);
            #endif
            
            pos_idx_copied_arr++;
         }
      }

      //erase sector before write on it      
      //  if((*addrs + page_memory_size_d) >= (total_used_sector_g * 4096))
      // {
      //   flash_mem.eraseBlock64K(*addrs + page_memory_size_d);
      //   total_used_sector_g++;
      // }
      
      if(flash_mem.writeByteArray(*addrs, bufs_for_check_and_save_g, page_memory_size_d))
      {
        save_sukses_b = true;
      }
      else
      {
        save_sukses_b = false;
        break;
      } 
      vTaskDelay(1/portTICK_PERIOD_MS);

      *addrs = *addrs + page_memory_size_d;
      memset(bufs_for_check_and_save_g, 0, sizeof(bufs_for_check_and_save_g));

      vTaskDelay(1/portTICK_PERIOD_MS);
  }
  
  protect_eeprom_memory(true);

  if(save_sukses_b)
  {
    amount_of_playlist_show_g = amount_of_playlist_show_g + 1;
    mem_addre_playlist_g      = prev_address;
    playlist_seq              = amount_of_playlist_show_g - 1;

    if(!got_sequence_data_playlist_b)
    {
      play_animasi_playlist_g   = true;
    }

    frame_num_playlist_g      = 0;
    repeat_animasi_image_g    = 0;
    show_text_animation_b     = false;
    //mode_display_led_g        = ST_ANIMATION;
    Serial.println("MEMORY - success to save");
    //check_saved_data_in_eeprom(size_sector_tosave, *addrs);

     if(file_gif_opened)
    {
      gif_show_enable               = false;
      file_gif_opened               = false;                
      gif_h.close();
    }
   
  }
  else
  {
    if(!got_sequence_data_playlist_b)
    {
      play_animasi_playlist_g   = false;
    }
    
    *addrs                   = *addrs - ((xcount_mem + 1) * page_memory_size_d);

    Serial.println("MEMORY - failed to save");
    
  }

  #if(CHK_DATA_SAVED_PLAYLIST == 1)
    vTaskDelay(1000/portTICK_PERIOD_MS);
    check_saved_data_in_eeprom(size_sector_tosave, *addrs);
  #endif
}

void delete_files_from_playlist()
{  
   protect_eeprom_memory(false);
   
   if(flash_mem.eraseBlock64K(0x00000000))     // hapus 64K dari alamat 0x00
   {
      progres_status       				 = 33.00;
      notify_progress_b                  = true;

      vTaskDelay(500);
      if(flash_mem.eraseBlock64K(0x00010000))  // hapus 64K  dari alamat 0xFA00 --> 
      {
          progres_status       				       = 66.00;
          notify_progress_b                  = true;
          
          vTaskDelay(500);
          if(flash_mem.eraseBlock64K(0x00020000))     // hapus 64K dari alamat 0x00
          {
            progres_status                     = 99.00;
            notify_progress_b                  = true;
            mem_addre_current_g                = 0x000000;
            amount_of_playlist_show_g          = 0;
            receive_complete_g                 = 3;
  		      vTaskDelay(1000);			   
            Serial.println("[INFO] PLAYLIST ERASED SUCCESS");
          }
          else
          {
             Serial.println("[INFO] PLAYLIST STILL EXIST (err_erase stage 3)");
             receive_failed_b            = true;            
          }
      }
      else
      {
         Serial.println("[INFO] PLAYLIST STILL EXIST (err_erase stage 2)");
         receive_failed_b            = true;
      }
   }
   else
   {
       Serial.println("[INFO] PLAYLIST STILL EXIST (err_erase stage 1)");
       receive_failed_b            = true;     
   }
   
   protect_eeprom_memory(true);
   total_used_sector_g = 0;
}

void get_buffer_data(uint32_t  *address_mem, uint8_t *buffered_data, uint8_t size_sector_toread)
{
    uint16_t pos_idx_arr       = 0;
    
    protect_eeprom_memory(false);
    xcount_mem = 0;
    for(xcount_mem = 0; xcount_mem < size_sector_toread; xcount_mem++)
    {
      
        memset(bufs_for_check_and_save_g, 0, sizeof(bufs_for_check_and_save_g));
        //_read_page(address_mem, bufs_for_check_and_save_g);
        flash_mem.readByteArray(*address_mem, bufs_for_check_and_save_g, page_memory_size_d);
   
        #if(CHK_DATA_SAVED_PLAYLIST == 1)
          Serial.print("data read: ");
          Serial.println(*address_mem); 
          for(uint16_t x = 0; x < page_memory_size_d; x++)
          {
            Serial.print(bufs_for_check_and_save_g[x]);
            Serial.print("-");
          }
          Serial.println();
        #endif

        ycount_mem = 0;
        for(ycount_mem = 0; ycount_mem < 256; ycount_mem++)
        {
           if((xcount_mem == 0) && (ycount_mem < addres_header_size_eeprom_d))
           {
              continue;
           }
           else
           {
              switch(type_data_to_show_g)
              {
                 case data_text_d:
                    if(pos_idx_arr >= sizeof(buff_textEEPROM))
                    {
                       break;
                    }
                 break;
                 
                 case data_image_d:
                    if(pos_idx_arr >= sizeof(buff_imgEEPROM))
                    {
                       break;
                    }
                 break;

                 case data_animasi_d:
                    if(pos_idx_arr >= sizeof(buff_animasiEEPROM))
                    {
                       break;
                    }                  
                 break;

                 case data_gif_d:
                    if(pos_idx_arr >= sizeof(buff_animasiEEPROM))
                    {
                       break;
                    }                  
                 break;                 

                 default:
                    Serial.println("[INFO] uncategorized data yang akan diambil");
                 break;
              }
              
              buffered_data[pos_idx_arr] = bufs_for_check_and_save_g[ycount_mem];
              
              pos_idx_arr++;
           }
        }
        *address_mem = *address_mem + page_memory_size_d;
    }
    protect_eeprom_memory(true);

  #if(enable_debug_serial == 1)
    Serial.println("------------------------------------");
    Serial.print("[INFO] jumlah data yang dibaca : ");
    Serial.println(pos_idx_arr);
    Serial.println("[INFO] data yang diterima: ");
    xcount_mem = 0;
    for(xcount_mem = 0; xcount_mem < pos_idx_arr; xcount_mem++)
    {
        Serial.print(buffered_data[xcount_mem], HEX);
        Serial.print("-");
    }
    Serial.println();
    Serial.print("[INFO] alamat selanjutnya : ");
    Serial.println(*address_mem, HEX);
  #endif
    
}

void read_playlist()
{
  bool data_exist                 = false;
  uint8_t amount_of_sector        = 0;

  #if(CHK_AND_DEBUG_PLAYLIST == 1)
    Serial.print("[INFO] play_animasi_playlist_g ");
    Serial.println(play_animasi_playlist_g);
    Serial.print("[INFO] frame_num_playlist_g ");
    Serial.println(frame_num_playlist_g);
    Serial.print("[INFO] repeat_animasi_image_g ");
    Serial.println(repeat_animasi_image_g);
    Serial.print("[INFO] show_text_animation_b ");
    Serial.println(show_text_animation_b);
    Serial.print("[INFO] save_to_playlist_b ");
    Serial.println(save_to_playlist_b);    
  #endif

  if((frame_num_playlist_g == 0) && (repeat_animasi_image_g == 0) && (!show_text_animation_b) && ((!save_to_playlist_b)))
  {
    flash_on_read_process = true;
    //vTaskDelay(20/portTICK_PERIOD_MS);
    memset(bufs_for_check_and_save_g, 0, sizeof(bufs_for_check_and_save_g));

    if(playlist_seq >= amount_of_playlist_show_g)
    {
        mem_addre_playlist_g = 0x00000;
        playlist_seq         = 0;
    }

    #if(CHK_AND_DEBUG_PLAYLIST == 1)
      Serial.print("[INFO] mem_addre_playlist_g ");
      Serial.println(mem_addre_playlist_g);
      Serial.print("[INFO] playlist_seq ");
      Serial.println(playlist_seq);    
    #endif    


    protect_eeprom_memory(false);
    //vTaskDelay(5/portTICK_PERIOD_MS);
    
    flash_mem.readByteArray(mem_addre_playlist_g, bufs_for_check_and_save_g, page_memory_size_d);  
    vTaskDelay(5/portTICK_PERIOD_MS);
    
    data_exist = get_data_info_from_memory(&mem_addre_playlist_g, bufs_for_check_and_save_g);
    //data_exist = check_data_header_eeprom(&mem_addre_playlist_g);

    protect_eeprom_memory(true);  
    
    //Serial.printf("data playlist: %x-%x-%d-%d\r\n", bufs_for_check_and_save_g[3], bufs_for_check_and_save_g[4], bufs_for_check_and_save_g[5], data_exist);

    if(data_exist)
    {
      type_data_to_show_g         = bufs_for_check_and_save_g[5];
      amount_of_sector            = size_sectore_used_d(type_data_to_show_g);

      if(file_gif_opened)
      {
        gif_show_enable               = false;
        file_gif_opened               = false;                
        gif_h.close();
      }

      switch(type_data_to_show_g)
      {
        case data_text_d:
          memset(buff_textEEPROM, 0, sizeof(buff_textEEPROM));
          //memset_pointer(buff_textEEPROM, 25, 0x00);
          get_buffer_data(&mem_addre_playlist_g, buff_textEEPROM, amount_of_sector);
          decode_text(buff_textEEPROM); 
          playlist_seq++;       
          show_text_animation_b           = true;
          total_pixel_led_used_g          = (String(text_animasi_g).length() * 11) + 32;
          cursor_x_text_g                 = matrix->width(); 																	   
        break;
        
        case data_image_d:
          if(bufs_for_check_and_save_g[4] == 0x04)
          {
            design_not_img_eeprom_b = true;
          }
          else
          {
            design_not_img_eeprom_b = false;
          }
          memset(buff_imgEEPROM, 0, sizeof(buff_imgEEPROM));
          //memset_pointer(buff_imgEEPROM, 768, 0x00);
          get_buffer_data(&mem_addre_playlist_g, buff_imgEEPROM, amount_of_sector);         
          //playlist_seq++;  
          repeat_animasi_image_g              = 180;                
        break;
        
        case data_animasi_d:
          //Serial.println("prepare read animation data");
          //data_size_animasi_eeprom    = ((bufs_for_check_and_save_g[3] << 8) | bufs_for_check_and_save_g[4]) & 0xFF;
          memset(buff_animasiEEPROM, 0, sizeof(buff_animasiEEPROM));
          //memset_pointer(buff_animasiEEPROM, 15368, 0x00);
          repeat_animasi_image_g              = 5;   
          get_buffer_data(&mem_addre_playlist_g, buff_animasiEEPROM, amount_of_sector);
          decode_animasi(buff_animasiEEPROM);
       
        break;

        case data_gif_d:
          data_size_animasi_eeprom    = ((bufs_for_check_and_save_g[3] << 8) | bufs_for_check_and_save_g[4]) & 0xFFFF;

          //#if(SPECIAL_DEBUG_FOR_GIF == 1)
          //Serial.println("read gif file from eeprom");
          //Serial.printf("data gif: %d-%d \r\n", data_size_animasi_eeprom, file_gif_opened);
          //#endif

          memset(buff_animasiEEPROM, 0, sizeof(buff_animasiEEPROM));
          get_buffer_data(&mem_addre_playlist_g, buff_animasiEEPROM, amount_of_sector);
          repeat_animasi_image_g              = 10;

          // for (uint16_t i = 0; i < data_size_animasi_eeprom; i++) 
          // {
          //     Serial.print(buff_animasiEEPROM[i], HEX);
          //     Serial.print(" ");

          //     if((i % 96) == 0)
          //     {
          //       Serial.println();
          //     }
          // }
          // Serial.println(); 
          gif_show_enable = true;
        break;
        
        default:
          Serial.println("[INFO] uncategorized data yang akan ditampilkan");
        break;
      }
                      
      #if(CHK_AND_DEBUG_PLAYLIST == 1)
        Serial.print("[INFO] Playlist ke:"); 
        Serial.println(playlist_seq+1);
        Serial.print("[INFO] type_data_to_show_g: "); 
        Serial.println(type_data_to_show_g);        
        Serial.print("[INFO] jumlah playlist: "); 
        Serial.println(amount_of_playlist_show_g);
        Serial.print("[INFO] alamat file (yang tampil): "); 
        Serial.println(mem_addre_playlist_g - (amount_of_sector * page_memory_size_d), HEX);
        Serial.println("------------------------------------");
      #endif
      
      //play_animasi_playlist_g             = true;
    }
    else
    {   
      #if(CHK_AND_DEBUG_PLAYLIST == 1) 
        Serial.print("data not exist");
        Serial.println("----------------------------------------------");
      #endif
      
      
      //play_animasi_playlist_g = false; 
      type_data_to_show_g    = data_animasi_default;
    }  

    packet_show_b                       = true;
    flash_on_read_process               = false;
  }
  else
  {
    #if(CHK_AND_DEBUG_PLAYLIST == 1)
      Serial.println("[INFO] else...");
    #endif
    
    //Serial.printf("repeat animation... %d -%d -%d -%d\r\n", repeat_animasi_image_g, playlist_seq, mem_addre_playlist_g, design_not_img_eeprom_b);
    packet_show_b                = true;
    //play_animasi_playlist_g      = true;
  }
}

//OK--