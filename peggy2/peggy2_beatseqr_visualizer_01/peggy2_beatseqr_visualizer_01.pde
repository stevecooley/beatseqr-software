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

unsigned short x = 0;
unsigned short y = 0;
void loop()
{ 
  for(int i=1; i<=8; i++)
  {
    switch(i)
    {
    case 1:
      {
        voice1_rgbw();
        break;
      }
    case 2:
      {
        voice2_rgbw();
        break;
      }
    case 3:
      {
        voice3_rgbw();
        break;
      }
    case 4:
      {
        voice4_rgbw();
        break;
      }
    case 5:
      {
        voice5_rgbw();
        break;
      }
    case 6:
      {
        voice6_rgbw();
        break;
      }
    case 7:
      {
        voice7_rgbw();
        break;
      }
    case 8:
      {
        voice8_rgbw();
        break;
      }
    }
     beatseqrVisualize.RefreshAll(50); //Draw frame buffer one time
delay(200);
  beatseqrVisualize.Clear();
  }

 
}


void voice1_rgbw()
{

  for(int y = 0; y <= 12; y++)
  {
    for(int x = 0; x <= 5; x++)
    {
      beatseqrVisualize.SetPoint(x, y);
    }
  }
}

void voice2_rgbw()
{

  for(int y = 0; y <= 12; y++)
  {
    for(int x = 6; x <= 12; x++)
    {
      beatseqrVisualize.SetPoint(x, y);
    }
  }
}

void voice3_rgbw()
{

  for(int y = 0; y <= 12; y++)
  {
    for(int x = 13; x <= 19; x++)
    {
      beatseqrVisualize.SetPoint(x, y);
    }
  }
}

void voice4_rgbw()
{

  for(int y = 0; y <= 12; y++)
  {
    for(int x = 20; x <= 25; x++)
    {
      beatseqrVisualize.SetPoint(x, y);
    }
  }
}

void voice5_rgbw()
{

  for(int y = 13; y <= 24; y++)
  {
    for(int x = 0; x <= 5; x++)
    {
      beatseqrVisualize.SetPoint(x, y);
    }
  }
}

void voice6_rgbw()
{

  for(int y = 13; y <= 24; y++)
  {
    for(int x = 6; x <= 12; x++)
    {
      beatseqrVisualize.SetPoint(x, y);
    }
  }
}

void voice7_rgbw()
{

  for(int y = 13; y <= 24; y++)
  {
    for(int x = 13; x <= 19; x++)
    {
      beatseqrVisualize.SetPoint(x, y);
    }
  }
}

void voice8_rgbw()
{

  for(int y = 13; y <= 24; y++)
  {
    for(int x = 20; x <= 25; x++)
    {
      beatseqrVisualize.SetPoint(x, y);
    }
  }
}

