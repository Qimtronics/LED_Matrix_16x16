uint16_t general_counter_idx      = 0;
bool preview_text_b               = false;

//void view_on_led(uint8_t *buf)
//{ 
//  uint16_t idx = 0;
//  for(uint8_t coord_x = 0; coord_x < 16; coord_x++)
//  {
//    for(uint8_t coord_y = 0; coord_y < 16; coord_y++)
//    { 
//      idx = getPixelNumber_IMGframe(coord_x, coord_y) * 4;
//      leds[getPixelNumber(coord_x, coord_y)] =  CRGB(buf[idx + 1],  buf[idx + 2],  buf[idx + 3]);   
//    }
//  }
//  FastLED.show(); 
//}

//void view_on_led_animasi(uint8_t *buf, uint8_t frm)
//{ 
//  uint16_t idx = 0;
//  for(uint8_t coord_x = 0; coord_x < 16; coord_x++)
//  {
//    for(uint8_t coord_y = 0; coord_y < 16; coord_y++)
//    { 
//        idx = (frm * 1024) + getPixelNumber_IMGframe(coord_x, coord_y) * 4;
//        leds[getPixelNumber(coord_x, coord_y)] =  CRGB(buf[idx + 1],  buf[idx + 2],  buf[idx + 3]);   
//    }
//  }
//  FastLED.show(); 
//}


void clear_LED(uint8_t width_led, uint8_t height_led)
{
  for(uint8_t coord_x = 0; coord_x < WIDTH_; coord_x++)
  {
    for(uint8_t coord_y = 0; coord_y < HEIGHT_; coord_y++)
    {
      leds[getPixelNumber(coord_x, coord_y)] =  CRGB(0,0,0);
    }
  }
  FastLED.show();
}

//void view_GIF_on_led(uint8_t *data_GIF)
//{
//  for(uint8_t coord_x = frame_properties_t.start_drawing_x; coord_x < WIDTH_; coord_x++)
//  {
//    for(uint8_t coord_y = frame_properties_t.start_drawing_y; coord_y < HEIGHT__; coord_y++)
//    {
//      leds[flip_image(rotate_image(rotate_image(getPixelNumber(coord_x, coord_y), -90), -90), vertical_d)] =  CRGB(
//                                                                                             data_GIF[(encoded_data_p[getPixelNumber_GIFframe(coord_x, coord_y)]*3) + 13 + 0],
//                                                                                             data_GIF[(encoded_data_p[getPixelNumber_GIFframe(coord_x, coord_y)]*3) + 13 + 1],
//                                                                                             data_GIF[(encoded_data_p[getPixelNumber_GIFframe(coord_x, coord_y)]*3) + 13 + 2]
//                                                                                             );
//    }
//  }
//  FastLED.show();
//}

/**
 * 
 * @brief GIF draw (draw to LED) callback functions
 * CHECKED
 * 
 */
// Draw a line of image directly on the LED
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320]; //tadinya 320 -> asumsi lebar max dirubah ke 16x3
    int x, y;
    uint32_t color;
    
    if(file_gif_opened and gif_show_enable)
    {
        usPalette = pDraw->pPalette;
        y = pDraw->iY + pDraw->y; // current line
        
        s = pDraw->pPixels;
        if (pDraw->ucDisposalMethod == 2) // restore to background color
        {
          for (x=0; x<pDraw->iWidth; x++)
          {
            if (s[x] == pDraw->ucTransparent)
               s[x] = pDraw->ucBackground;
          }
          pDraw->ucHasTransparency = 0;
        }
        else if(pDraw->ucDisposalMethod == 4)
        {
          gif_show_enable   = false;
          receive_failed_b  = true;
        }
        else
        {
          
        }
        
        // Apply the new pixels to the main image
        if (pDraw->ucHasTransparency) // if transparency used
        {
          uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
          int iCount;
          pEnd = s + pDraw->iWidth;
          x = 0;
          iCount = 0; // count non-transparent pixels
          while(x < pDraw->iWidth)
          {
            c = ucTransparent-1;
            d = usTemp;
            while (c != ucTransparent && s < pEnd)
            {
              c = *s++;
              if (c == ucTransparent) // done, stop
              {
                s--; // back up to treat it like transparent
              }
              else // opaque
              {
                 *d++ = usPalette[c];
                 iCount++;
              }
            } // while looking for opaque pixels
            if (iCount) // any opaque pixels?
            {
              x = pDraw->iX;
    //          Serial.print(iCount);
    //          Serial.print(" - ");
    //          Serial.print(pDraw->iWidth);
    //          Serial.print(" - ");
    //          Serial.print(pDraw->iHeight);
    //          Serial.print(" - ");
    //          Serial.print(pDraw->iX);
    //          Serial.print(" - ");
    //          Serial.print(pDraw->iY);
    //          Serial.print(" - ");
    //          Serial.print(pDraw->ucDisposalMethod);          
    //          Serial.print(" - ");
    //          Serial.print(x);
    //          Serial.print(" - ");
    //          Serial.println(y);          
              for(int xOffset = 0; xOffset < iCount; xOffset++ ){
                color = expandColor(usTemp[xOffset]);
                leds[flip_image(rotate_image(rotate_image(getPixelNumber(x + xOffset, y), -90), -90), vertical_d)] = CRGB((color >> 16 & 0xFF), (color >> 8 & 0xFF), (color & 0xFF));
                //leds[getPixelNumber(x + xOffset, y)] = CRGB((color >> 16 & 0xFF), (color >> 8 & 0xFF), (color & 0xFF));
              }
              x += iCount;
              iCount = 0;
            }
            // no, look for a run of transparent pixels
            c = ucTransparent;
            while (c == ucTransparent && s < pEnd)
            {
              c = *s++;
              if (c == ucTransparent)
              {
                 iCount++;
              }
              else
              {
                 s--; 
              }
            }
            if (iCount)
            {
              x += iCount; // skip these
              iCount = 0;
            }
          }
        }
        else
        { 
          s = pDraw->pPixels;
          for (x=0; x<pDraw->iWidth; x++)
          {
            color = expandColor(usPalette[*s++]);
            leds[flip_image(rotate_image(rotate_image(getPixelNumber(x, y), -90), -90), vertical_d)] = CRGB((color >> 16 & 0xFF), (color >> 8 & 0xFF), (color & 0xFF));
          }
        }
        FastLED.show();
    }
    else
    {
      //Serial.println("Callback Draw GIF stopped...");
      FastLED.clear(true);
      FastLED.clearData();
    }
    
} /* GIFDraw() */

/**
 * @brief function to decode data text, parse its speed, color, and its text.
 * 
 * */
void decode_text(uint8_t *buf)
{
    general_counter_idx = 0;
    
    for(general_counter_idx = 0; general_counter_idx < size_buffer_text_d; general_counter_idx++)
    {
       if(general_counter_idx == 0) //take text's speed
       {
          speed_animasi_text_g = buf[general_counter_idx];
          Serial.printf("spd_text: %d \r\n", speed_animasi_text_g);
       }
       else if ((general_counter_idx >= 1) && (general_counter_idx < 4))  //take text's color
       { 
          text_color_rgb_g[general_counter_idx - 1] = buf[general_counter_idx];
       }
       else //take the real text
       {
          text_animasi_g[general_counter_idx - 4]   = buf[general_counter_idx];
       }
    }
}

void decode_animasi(uint8_t *buf)
{
    fps_animasi_eeprom_g    = buf[0];
    total_frame_dr_eeprom_g = buf[1];        
    repeat_frame_animasi    = (1000/fps_animasi_eeprom_g)/20;
    #if(SPECIAL_DEBUG_FOR_ANIMATION == 1)
      Serial.print("[INFO] total frame:");
      Serial.println(total_frame_dr_eeprom_g);  

      Serial.print("[INFO] fps:");
      Serial.println(fps_animasi_eeprom_g);
    #endif

    for (general_counter_idx = 0; general_counter_idx < size_buffer_animasi_d; general_counter_idx++)
    {
        buf[general_counter_idx]  = buf[general_counter_idx + 2];
    }
}

/** @brief function to show data pixel to led 
 * 
 * pixel led and data saved on buffer are on different index value based on how led is connected
 * for led known and usually used by us 16x16 used a sequence index such this, 
 * 
 * (start from left top corner)
 *   0	16	32	48	64	80	96	112	128	144	160	176	192	208	224	240 
 *   1	17	33	49	65	81	97	113	129	145	161	177	193	209	225	241
 *   2	18	34	50	66	82	98	114	130	146	162	178	194	210	226	242
 *   3	19	35	51	67	83	99	115	131	147	163	179	195	211	227	243
 *   4	20	36	52	68	84	100	116	132	148	164	180	196	212	228	244
 *   5	21	37	53	69	85	101	117	133	149	165	181	197	213	229	245
 *   6	22	38	54	70	86	102	118	134	150	166	182	198	214	230	246
 *   7	23	39	55	71	87	103	119	135	151	167	183	199	215	231	247
 *   8	24	40	56	72	88	104	120	136	152	168	184	200	216	232	248
 *   9	25	41	57	73	89	105	121	137	153	169	185	201	217	233	249
 *   10	26	42	58	74	90	106	122	138	154	170	186	202	218	234	250
 *   11	27	43	59	75	91	107	123	139	155	171	187	203	219	235	251
 *   12	28	44	60	76	92	108	124	140	156	172	188	204	220	236	252
 *   13	29	45	61	77	93	109	125	141	157	173	189	205	221	237	253
 *   14	30	46	62	78	94	110	126	142	158	174	190	206	222	238	254
 *   15	31	47	63	79	95	111	127	143	159	175	191	207	223	239	255 (right bottom corner)
 *
 * */
void show_animation(uint8_t jenis_data, uint8_t *buf_to_show)
{  
   uint16_t idx_color       = 0;
   int8_t loops             = 0;

   switch(jenis_data)
   {
     case data_text_d:              
          if(show_text_animation_b)
          {
            matrix->fillScreen(0);
            matrix->setCursor(cursor_x_text_g, cursor_y_text_g);
            matrix->print(text_animasi_g);
            matrix->setFont(&Dialog_bold_13);                 
            matrix->setTextColor(matrix->Color(text_color_rgb_g[0], text_color_rgb_g[1], text_color_rgb_g[2]));
            //matrix->setTextColor(matrix->Color(gamma_correction_ciqa2(text_color_rgb_g[1], R_color), gamma_correction_ciqa2(text_color_rgb_g[2], G_color), gamma_correction_ciqa2(text_color_rgb_g[3], B_color)));
            if(--cursor_x_text_g < -total_pixel_led_used_g) {
              cursor_x_text_g = matrix->width();
              show_text_animation_b = false;
            }
            matrix->show();

            switch(speed_animasi_text_g)
            {
              case 1: delay(200); break;
              case 2: delay(180); break;
              case 3: delay(160); break;
              case 4: delay(140); break;
              case 5: delay(120); break;
              case 6: delay(100); break;
              case 7: delay(80); break;
              case 8: delay(60); break;
              case 9: delay(40); break;
              case 10: delay(20); break;
              default: delay(120); break;                
            }
          }
     break;
     
     case data_image_d: 
          for(uint8_t coord_x = 0; coord_x < 16; coord_x++)
          {
            for(uint8_t coord_y = 0; coord_y < 16; coord_y++)
            {              
              idx_color = getPixelNumber_IMGframe(coord_x, coord_y) * 3; 
              // result value of getPixelNumber_IMGframe times 3 because we have 3 type color saved on eeprom R, G, and B, 
              // if we have A value then times 4

              //uncomment line below to know exactly the value which is going to be shown and its index ony led
              // Serial.print(design_not_img_eeprom_b);
              // Serial.print("-");              
              // Serial.print(rotate_image(getPixelNumber(coord_x, coord_y), 90));
              // Serial.print("-");
              // Serial.print(idx_color);
              // Serial.print("-");              
              // Serial.print(buf_to_show[idx_color + 0]);
              // Serial.print("-");
              // Serial.print(buf_to_show[idx_color + 1]);
              // Serial.print("-");
              // Serial.println(buf_to_show[idx_color + 2]);

              if(design_not_img_eeprom_b)
              {
                leds[rotate_image(getPixelNumber(coord_x, coord_y), 90)] =  CRGB(gamma_correction_ciqa2(buf_to_show[idx_color + 0], R_color),  gamma_correction_ciqa2(buf_to_show[idx_color + 1], G_color),  gamma_correction_ciqa2(buf_to_show[idx_color + 2], B_color));   
              }
              else
              {
                leds[flip_image(getPixelNumber(coord_x, coord_y), vertical_d)] =  CRGB(gamma_correction_ciqa2(buf_to_show[idx_color + 0], R_color),  gamma_correction_ciqa2(buf_to_show[idx_color + 1], G_color), gamma_correction_ciqa2(buf_to_show[idx_color + 2], B_color));   
              }                  
            }
          }

          FastLED.show(); 
          vTaskDelay(duration_show_img_default/portTICK_PERIOD_MS);
          
          repeat_animasi_image_g--;
          if(repeat_animasi_image_g == 0)
          {
              playlist_seq++;
              if(playlist_seq >= amount_of_playlist_show_g)
              {
                 mem_addre_playlist_g = 0x00000;
                 playlist_seq         = 0;
              } 
          }         
     break;
     
     case data_animasi_d: 
           //Serial.println("prepare show animation");
           for(uint8_t coord_x = 0; coord_x < 16; coord_x++)
            {
              for(uint8_t coord_y = 0; coord_y < 16; coord_y++)
              { 
                  idx_color = (frame_num_playlist_g * 768) + getPixelNumber_IMGframe(coord_x, coord_y) * 3;                                                   
                  leds[flip_image(getPixelNumber(coord_x, coord_y), vertical_d)] =  CRGB(gamma_correction_ciqa2(buf_to_show[idx_color + 0], R_color),  gamma_correction_ciqa2(buf_to_show[idx_color + 1], G_color),  gamma_correction_ciqa2(buf_to_show[idx_color + 2], B_color));   
              }
            }

          #if(SPECIAL_DEBUG_FOR_ANIMATION == 1)
            Serial.print("[INFO] animayion: show....... ");
            for (uint16_t i = 0; i < sizeof(buff_animasiEEPROM); i++) 
            { 
              Serial.print(buf_to_show[i], HEX);
              Serial.print("-");
              if(((i+1) % 48) == 0)
              {
                Serial.println();
              }
            }
            Serial.println();
            
            //Serial.print("[INFO] animayion:"); 
            Serial.println(frame_num_playlist_g); 
          #endif

            FastLED.show(); 
            vTaskDelay(10/portTICK_PERIOD_MS);

            repeat_frame_animasi--;
            if(repeat_frame_animasi <= 0)
            {
              repeat_frame_animasi    = (1000/fps_animasi_eeprom_g)/20;
           
              frame_num_playlist_g++;
              if(frame_num_playlist_g >= total_frame_dr_eeprom_g)
              {
                  frame_num_playlist_g = 0;
                  repeat_animasi_image_g--;
                  
                  if(repeat_animasi_image_g == 0)
                  {
                      playlist_seq++;
                  }
                
                  if(playlist_seq >= amount_of_playlist_show_g)
                  {
                      mem_addre_playlist_g = 0x00000;
                      playlist_seq         = 0;
                  }
              }
            }
     break;
     
     default:
     break;
   }
}


//OK--