/*
This file is part of lrc2cdg.

lrc2cdg is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

lrc2cdg is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with lrc2cdg.  If not, see <https://www.gnu.org/licenses/>.
*/
/*
Creates a LRC from a CDG. Build with make, install with make install.
Usage: lrc2cdg infile.lrc outfile.cdg r(optional) g(optional) b(optional)
CDG specifiation: https://jbum.com//cdg_revealed.html
*/
#include <vector>
#include "values.h"
#include <bitset>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
//a com is a word
typedef struct {
  float stretch; //duration of word
  std::string text; //word
  bool newline; //if this word starts on a new line
  float time; //start time of this word
} com;
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color;
std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}
//cdg buffer
std::vector<char> fl;
//initcdg loads the color table and clears screen. The passed color is the desired color of the text
void initcdg(color color) {
  //input RGB888 Colors, convert to RGB444
  uint8_t r = color.r;
  uint8_t g = color.g;
  uint8_t b = color.b;
  float rr = (float)r/(float)255;
  float rg = (float)g/(float)255;
  float rb = (float)b/(float)255;
  std::bitset<4> r4(rr*15);
  std::bitset<4> g4(rg*15);
  std::bitset<4> b4(rb*15);
  //convert to cdg format
  std::string rs = r4.to_string();
  std::string gs = g4.to_string();
  std::string gh1 = gs.substr(0, gs.length()/2);
  std::string gh2 = gs.substr(gs.length()/2);
  std::string bs = b4.to_string();
  std::bitset<8> final1("00" + rs + gh1);
  std::bitset<8> final2("00" + gh2 + bs);
  unsigned long d1 = final1.to_ulong();
  unsigned char c1 = static_cast<unsigned char>( d1 );
  unsigned long d2 = final2.to_ulong();
  unsigned char c2 = static_cast<unsigned char>( d2 );
  header[1930] = c1;
  header[1931] = c2;
  //add cdg headers
  copy(&header[0], &header[headersize], back_inserter(fl));
}
//draw block to screen. block is a 2d array, as CDG blocks are 12x6 px. Char blocks are stored in values.h
void draw(bool block[12][6],char row,char col,bool blue) {
  //draw 12x6 bitmap
  //basic command stuff
  char command[24] = {0x09,0x26,0x00,0x00,0x00,0x01,row,col};
  if(blue) {
    command[5] = 0x02;
  }
  for(int i = 0; i < 12; i++) {
    std::bitset<8> bits;
    for(int d = 0; d < 6; d++) {
      bits.set(5-d,block[i][d]);
    }
    unsigned long d = bits.to_ulong();
    unsigned char c = static_cast<unsigned char>( d );
    command[8+i] = c;
  }
  copy(&command[0], &command[24], back_inserter(fl));
}

//wait(targetpos) Waits until a time in the song. First argument is time in seconds

void wait(float targetpos) {
  float currentpos = ((float)(fl.size()/24)/(float)300);
  float correction = targetpos - currentpos;
  for(int x = 0; x < correction*300; x++) {
    for(int j = 0; j < 24; j++) {
      fl.push_back(0x00);
    }
  }

}

//write cdg buffer to file
void write(const char* filename) {
  std::ofstream cdgout(filename, std::ios::out | std::ios::binary);
  cdgout.write((const char*)&fl[0], fl.size());
  cdgout.close();
}

//draw string onto cdg(string,row(really uint8_t but char is easier to use),col,blue(if the text is colored(the chosen color) or white))

char drawString(std::string str, char row, char col, bool blue) {
  std::transform(str.begin(), str.end(), str.begin(), ::tolower);

  for(std::string::iterator it = str.begin(); it != str.end(); ++it) {

      int position = *it - 'a';

      if(*it == '\'') {
        position = 26;
      } else if(*it == ',') {
        position = 27;
      } else if(*it == '.') {
        position = 28;
      } else if(*it == '*') {
        position = 29;
      } else if(*it == ' ') {
        position = 30;
      }
      if(position > 30 || position < 0) {
        std::cout << *it << "(" << (int)*it << ") is not found in characters" << std::endl;
        position = 30;
      }

      bool b1[12][6] = {0};
      bool b2[12][6] = {0};
      bool b3[12][6] = {0};
      bool b4[12][6] = {0};
      for(int x = 0; x < 24; x++) {
        for(int a = 0; a < 12; a++) {
          if(x < 12 && a < 6) {
            //b1
            b1[x][a] = alphabet[position][x][a];
          } else if(x < 12 && a >= 6) {
            //b2
            b2[x][a-6] = alphabet[position][x][a];
          } else if(x >= 12 && a < 6) {
            //b3

            b3[x-12][a] = alphabet[position][x][a];
          } else if(x >= 12 && a >= 6) {
            //b4
            b4[x-12][a-6] = alphabet[position][x][a];
          }
        }
      }
      /*std::cout << std::endl;
      for(int i = 0; i < 12; i++) {
        for(int d = 0; d < 6; d++) {
          std::cout << b1[i][d] << ",";
        }
        for(int d = 0; d < 6; d++) {
          std::cout << b2[i][d] << ",";
        }
        std::cout << std::endl;
      }
      for(int i = 0; i < 12; i++) {
        for(int d = 0; d < 6; d++) {
          std::cout << b3[i][d] << ",";
        }
        for(int d = 0; d < 6; d++) {
          std::cout << b4[i][d] << ",";
        }
        std::cout << std::endl;
      }*/
      draw(b1,row,col,blue);
      draw(b2,row,static_cast<char>(col + 1),blue);
      draw(b3,static_cast<char>(row + 1),col,blue);
      draw(b4,static_cast<char>(row + 1),static_cast<char>(col + 1),blue);
      /*wait(10);
      write("test.cdg");
      exit(1);*/
      col = static_cast<char>(col + 2);
  }
  return col;
}
// clear screen
void clrScr() {
  char command[24] = {0x09,0x01};
  copy(&command[0], &command[24], back_inserter(fl));
}

void convlrc2cdg(const char* flin,const char* flout,int r, int g, int b) {
  std::ifstream infile(flin);
  std::string line;
  //one "command" is one word
  std::vector<com> commands;
  //roll counts the amount of words on the current line
  int roll = 0;
  //parse LRC file
  while (std::getline(infile, line))
  {
    //lrc says the first timestamp needs to be formatted like [], and subsequent ones in the line like <>. Just make it simpler
    while(line.find('[') != std::string::npos) {
      std::replace( line.begin(), line.end(), '<', '[');
      std::replace( line.begin(), line.end(), '>', ']');
      int first = line.find('[');
      int last = line.find(']');
      std::string timestamp = line.substr (first+1,last-first-1);

      line.erase(0,last+1);
      first = line.find('[');
      bool newline = false;
      //determines whether or not we need to create a new line
      if(first == std::string::npos) {
        newline = true;
        roll = 0;
        first = line.length()-1;
      } else if(roll == 2) {
        newline = true;
        roll = 0;
      }
      roll++;
      std::string text = line.substr(0,first+1);
      std::replace( text.begin(), text.end(), '[', ' ');
      line.erase(0,first);
      float lasttime;
      com command;
      //get current word start time, aka the end of the last word
      if(commands.size() == 0) {
        lasttime = 0;
      } else {
        lasttime = commands.back().time;
      }
      //some lrc files have extra data at the top. Ignore this data
      if(!isdigit(timestamp.at(0))) {
        continue;
      }
      std::vector<std::string> time = split(timestamp,':');
      float d = ((float)atoi(time[0].c_str())*60) + stof(time[1],nullptr);
      command.time = d;
      command.stretch = command.time - lasttime;
      command.text = text;
      command.newline = newline;
      commands.push_back(command);
    }

  }
  color blue;
  blue.r=r;
  blue.g=g;
  blue.b=b;

  initcdg(blue);
  std::vector<com> l1; //line 1 buffer
  std::vector<com> l2; //line 2 buffer
  bool draw = false;
  //work out what words will go on what line
  for(int x = 0; x < commands.size(); x++) {
    com command = commands[x];
    if(draw) {
      l2.push_back(command);
    } else {
      l1.push_back(command);
    }

    if(command.newline) {
      if(draw) {
        //draw to cdg
        std::string total; // line 1 converted to string
        std::string total2; // line 2 converted to string
        for(int d = 0; d < l1.size(); d++) {
          com f = l1[d];
          total += f.text;
        }
        for(int d = 0; d < l2.size(); d++) {
          com f = l2[d];
          total2 += f.text;
        }
        //draw "white" text
        drawString(total,0x05,0x03,false);
        drawString(total2,0x08,0x03,false);
        int pos_left = 0x03; // our cursor position
        //color in text gradually as song progresses
        for(int d = 0; d < l1.size(); d++) {
          com f = l1[d];
          wait(f.time);
          pos_left = drawString(f.text,0x05,pos_left,true);
        }
        pos_left = 0x03;
        for(int d = 0; d < l2.size(); d++) {
          com f = l2[d];
          wait(f.time);
          pos_left = drawString(f.text,0x08,pos_left,true);
        }
        l1.clear();
        l2.clear();
        clrScr();
      }
      draw = !draw;
    }
  }
  write(flout);

}
#ifndef __EMSCRIPTEN__
int main(int argc, char** argv) {
  if(argc != 3 && argc != 6) {
    std::cout << "lrc2cdg infile.lrc outfile.cdg r g b" << std::endl;
    return 1;
  }
  //default RGB values(if not specified)
  int r = 68;
  int g = 204;
  int b = 255;
  if(argc == 6) {
    r = atoi(argv[3]);
    g = atoi(argv[4]);
    b = atoi(argv[5]);
  }

  convlrc2cdg(argv[1],argv[2],r,g,b);
  return 0;
}
#endif
#ifdef __EMSCRIPTEN__
extern "C" {
  void convlrc2cdgjs(const char* flin,const char* flout, int r, int g, int b) {
    convlrc2cdg(flin,flout,r,g,b);
  }
}
#endif
