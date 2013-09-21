/* 
 
 Beatseqr visualizer for Peggy 2.x
 
 Copyright (c) 2011 Steve Cooley.  All right reserved.
 
 This example is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This software is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this software; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include <Peggy2Serial.h>

Peggy2 beatseqrVisualize;


void setup()                  
{
  beatseqrVisualize.HardwareInit();   
}


unsigned short y_start = 0;
unsigned short x_start = 0;
unsigned short x_end = 0;
unsigned short y_end = 0;

byte r;
byte g;
byte b;
byte w;

int counter;


void loop()
{ 
  for(int i=1; i<=8; i++)
  {

    r = counter % 2;
    g = counter % 3;
    b = counter % 5;
    w = counter % 7;

    visualize(i, r, g, b, w, 0);

    beatseqrVisualize.RefreshAll(250); //Draw frame buffer one time
    delay(100);
    beatseqrVisualize.Clear();
    counter++;
    if(counter>1000)
    {
      counter=0;
    }
  }


}


void visualize(int voice, int red, int green, int blue, int white, int sections)
{

  switch(voice)
  {
  case 1:
    {
      y_start = 0; 
      y_end   = 12;
      x_start = 0; 
      x_end   = 5;
      break;
    }
  case 2:
    {
      y_start = 0;
      y_end   = 12;
      x_start = 6; 
      x_end   = 11;
      break;
    }
  case 3:
    {
      y_start = 0;
      y_end   = 12;
      x_start = 13; 
      x_end   = 19;
      break;
    }
  case 4:
    {
      y_start = 0;
      y_end   = 12;
      x_start = 20; 
      x_end   = 24;
      break;
    }
  case 5:
    {
      y_start = 13;
      y_end   = 24;
      x_start = 0; 
      x_end   = 5;
      break;
    }
  case 6:
    {
      y_start = 13;
      y_end   = 24;
      x_start = 6; 
      x_end   = 11;
      break;
    }
  case 7:
    {
      y_start = 13;
      y_end   = 24;
      x_start = 13; 
      x_end   = 19;
      break;
    }
  case 8:
    {
      y_start = 13;
      y_end   = 24;
      x_start = 20; 
      x_end   = 24;
      break;
    }

  }

  for(int i = y_start; i<=y_end; i=i+2)
  {
    for(int j = x_start; j <= x_end; j=j+2)
    {
      if(red == 1)
      {
        beatseqrVisualize.SetPoint(j, i);
      }
      if(green == 1)
      {
        beatseqrVisualize.SetPoint(j, i+1);
      }
      if(blue == 1)
      {
        beatseqrVisualize.SetPoint(j+1, i);
      }
      if(white == 1)
      {
        beatseqrVisualize.SetPoint(j+1, i+1);
      }
    }
  }
}


