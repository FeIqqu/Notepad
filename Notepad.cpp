#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <thread>
#include <ctime>
#include <windows.h>
#include "graphics.h"

using namespace std;

//===============���������� ����������===============
const int max_lines = 27; // ���������, �������� ������������ ���������� �����
const int max_length = 71; // ���������, �������� ������������ ���������� �������� � ������
const int line_height = 20; // ���������, �������� ������ ������ ������
const int width = 11; // ���������, �������� ������ ������ ������� � ������ ������ �� ������ ���� ���� 
const int indent = 38; // ���������, �������� ������ ������ ������ ��������� �� �������� ���� ����
int SUM = 0; // ���������� �������� � ���������
int numline = 0; // ����� ������
int linelength = 0; // ����� ������
int top = 0, bottom = 27; // ������� ������� � ������ ������, �������������� �� ������
int x, y; // ���������� ��������� ����
int xk = width, yk = indent, heightk = 15; // ���������� ��������� �������, � ������
int POS = 1; // ������� ��������� ������� � ������� �����
bool DOC_MODIFIED = true; // ����, ���������� �� ��, ��� �� � ������ ����
HWND hwnd; // ���������� ��������� ����
string DOC_NAME = "��� �����";
string DIRECTORY = "";
bool flags[7] = {true, true, true, true, true, true, true}; // ����� �������� ���� � �������
IMAGE *images[26]; // ������ ����������� ������
vector<string> input(27, ""); // ������ �����, �������� ���� �������� �����
//================================================

//=================��������� �������================
void Initializewindow(); // ������������� ����
void keyboard_thread(); // ����� �� ����� �������� � ����������
void HandleInput(int ch); // ��������� ��������� �������, � ����������� ������� ��������� ������ � ���������� �������� �� �����
void EnterInput(); // ���� ������ ������� "Enter"
void BackspaceInput(); // ���� ������ ������� "Backspace"
void LeftInput(); // ���� ������ ������� �����
void RightInput(); // ���� ������ ������� ������
void UpInput(); // ���� ������ ������� �����
void DownInput(); // ���� ������ ������� ����
void DefaultInput(int ch); // ���� ������ ������ �������
void drawtext(); // ����� ��������� ���������� ������ � ���������� �������� �� �����
void mouse_thread(); // ����� �� ����������� ��������� ��������� ����
void buttons(); // ��������� ������ ��������� ������������ ��������� ��������� ���� � ������� ���
void savemenu(); // ���� "���������"
int save(); // ������� ���������� ���������
void saveasmenu(); // ������� "��������� ���"
void changemenu(); // ���� "��������"
void findmenu(); // ������� "�����"
void findall(string &str, int &f, int &f_err, int &length, int &left, int &xf); // ����� ���� ��������� ��������� str � ��������� � �� �������������
void replacemenu(); // ������� "������"
void replace(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int (&left)[2], int &xf); // ���������� ��������� � � ������
void replaceall(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int (&left)[2], int &xf); // ���������� ���� �������� � �� ������
void aboutmenu(); // ���� "� ���������"
void exit(); // ���� ����� �������
void exitmenu(); // ����, ������������ ��� �������� ���� �������������, ���� ���� �� �������
bool AllFlagsTrue(); // ������� ��� �������� ����, ��� �� ����� � ������� flags ��������� �������� true
bool ValidChar(int ch); // ������� ��� �������� ����, ������������� �� ������ �����
//================================================

void (*menuFunctions[7])() = {savemenu, changemenu, aboutmenu, findmenu, replacemenu, exitmenu, saveasmenu}; // ������������� ������� ���������� �� �������

int main(){
   Initializewindow(); // ������������� ����
   thread t1(mouse_thread); // ������� ������ ����� � �������� � ���� ������� mouse_thread
   thread t2(keyboard_thread); // ������� ������ ����� � �������� � ���� ������� keyboard_thread
   t1.join(); // ������� ���������� ������� ������
   t2.join(); // ������� ���������� ������� ������
   return 0;
}

void Initializewindow(){
   initwindow(800, 600, "��� ����� � �������", 100, 100, false, false); // ������������� ����
   hwnd = GetForegroundWindow(); // ��������� ����������� ��������� ����
   for(int i = input.size(); i > 1; i--){
      input.erase(input.begin() + i); // ������� ������
   }
   setfillstyle(SOLID_FILL, LIGHTGRAY, WHITE); // ������������� ���� ���������� � ���� ����
   clearviewport(); // ������� ����
   setcolor(BLACK); // ������� ����� �����
   line(0, 30, 800, 30), line(0, 580, 800, 580); // ��������� �����
   settextstyle(COMPLEX_FONT, 0, 1); // ������������� �����
   outtextxy(10, 581, "����������: 0, "), outtextxy(10 + textwidth("����������: 0, "), 581, "������: 1");
   for(int i = 0; i < 26; i++){ // ���� ����������� ������ � ������
      char filename[25];
      sprintf(filename, ".\\buttons\\button%d.bmp", i + 1);
      images[i] = loadBMP(filename);
   }
   putimage(0, 0, images[0], COPY_PUT), putimage(56, 0, images[2], COPY_PUT), putimage(136, 0, images[4], COPY_PUT); // ��������� ����������� ������
} // ������������� ����

void HandleInput(int ch){
   POS = xk / width - 1; // ��������� ������� ��������� ������� � ������� �����
   linelength = input[numline].length();
   switch(ch){
      case KEY_ENTER: // ���� ������ ������� "Enter"
         EnterInput();
         break;
      case KEY_BACKSPACE: // ���� ������ ������� "Backspace"
         BackspaceInput();
         break;
      case static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL): // ���� ������ ������� �����
         LeftInput();
         break;
      case static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL): // ���� ������ ������� ������
         RightInput();
         break;
      case static_cast<int>(KEY_UP) + static_cast<int>(KEY_SPECIAL): // ���� ������ ������� �����
         UpInput();
         break;
      case static_cast<int>(KEY_DOWN) + static_cast<int>(KEY_SPECIAL): // ���� ������ ������� ����
         DownInput();
         break;
      case WINDOW_CLOSE: // ���� ����� �������
         exit();
         break;
      default: // ���� ������ ������ �������
         DefaultInput(ch);
         break;
   }
   if(DOC_MODIFIED == false){ // ���� ���� ��� ������
      string doc_name = DOC_NAME  + "* � �������";
      SetWindowText(hwnd, doc_name.c_str()); // ��������� ������ ����� ����
   }
   if(flags[5]) drawtext(); // ����� ��������� ���������� ������ �� �����,  ���������� ��������(���� ������� ���� ������)
} // ��������� ��������� �������, � ����������� ������� ��������� ������ � ���������� �������� �� �����

void EnterInput(){
   DOC_MODIFIED = false;
   numline++; // ��������� �� ��������� ������
   xk = width;
   if(numline == bottom){
      top++, bottom++;
   }
   else{
      yk += line_height; // ��������� �������� ��������� ��������� �������, ����� ��������� �� �� ����� ������
   }
   input.insert(input.begin() + numline, ""); // ��������� ����� ������
   input[numline] = input[numline - 1].substr(POS, linelength); // ��������� ��� ������� ���������� ������, ����������� ������ �� �������, �� ����� ������ ������
   input[numline - 1] = input[numline - 1].erase(POS, linelength); // ������� ��� ������� �� ������� ������
} // ���� ������ ������� "Enter"

void BackspaceInput(){
   if(POS > 0){ // ���� ������� �� ��������� � ������� ����� ������� ������
      DOC_MODIFIED = false;
      SUM--;
      input[numline].erase(POS - 1, 1); // ������� ������ �� ������
      xk -= width; // ���������� ��������� �������
   }
   else if(numline > 0){
      DOC_MODIFIED = false;
      if(numline != top){
         numline--; // ��������� �� ���������� ������
         linelength = input[numline].length(); // �������� ����� ���������� ������
         input[numline] = input[numline] + input[numline + 1].substr(0,  max_length - linelength); // ��������� � ������� ������ �����, ������� ����� � ��� ���������� �� ������ ������
         input.erase(input.begin() + numline + 1); // ������� ������
         xk = width + linelength * width, yk -= line_height; // ���������� ��������� �������
      }
      else{
         numline--; // ��������� �� ���������� ������
         top--, bottom--;
         linelength = input[numline].length(); // �������� ����� ���������� ������
         input[numline] = input[numline] + input[numline + 1].substr(0,  max_length - linelength); // ��������� � ������� ������ �����, ������� ����� � ��� ���������� �� ������ ������
         input.erase(input.begin() + numline + 1); // ������� ������
         xk = width + linelength * width; // ���������� ��������� �������
      }
   }
} // ���� ������ ������� "Backspace"

void LeftInput(){
   if(POS > 0){ // ���� ������� ������� ������� �� �������� ����� ����� � ������
      xk -= width; // ���������� ��������� �������
   }
   else if(numline > 0){ // ���� ������� ������� ������� �������� ����� ����� � ������ � ��� �� ������
      numline--; // ��������� �� ���������� ������
      linelength = input[numline].length(); // �������� ����� ���������� ������
      xk = width + linelength * width; // ���������� ��������� �������
      if(numline + 1 != top){
         yk -= line_height; // ���������� ��������� �������
      }
      else{
         top--, bottom--;
      }
   }
} // ���� ������ ������� �����

void RightInput(){
   if(POS < linelength){ // ���� ������� ������� ������� �� �������� ����� ������ � ������
      xk += width; // ���������� ��������� �������
   }
   else if(numline < static_cast<int>(input.size()) - 1){ // ���� ������� ������� ������� �������� ����� ������ � ������ � ��� �� ���������
      numline++; // ��������� �� ��������� ������
      xk = width; // ���������� ��������� �������
      if(numline != bottom){
         yk += line_height; // ���������� ��������� �������
      }
      else{
         top++, bottom++;
      }
   }
} // ���� ������ ������� ������

void UpInput(){
   if(numline > 0){
      int length_top_string = input[numline - 1].length(); // ��������� ����� ������� ������
      if(numline != top){ // ���� ������� �� ��������� �� ����� ������� ������������ ������
         if(POS <= length_top_string){ // ���� �� ������� ������ ���� ����� ��� ����������� �������
            numline--;
            yk -= line_height; // ���������� ��������� �������
         }
         else if(linelength >= length_top_string){ // ���� �� ������� ������ ��� ����� ��� ����������� �������
            numline--;
            xk = width + length_top_string * width, yk -= line_height; // ���������� ������� � ����� ������� ������
         }
      }
      else{ // ���� ������� ��������� �� ����� ������� ������������ ������
         top--, bottom--;
         if(POS <= length_top_string){ // ���� �� ������� ������ ���� ����� ��� ����������� �������
            numline--;
         }
         else if(linelength >= length_top_string){ // ���� �� ������� ������ ��� ����� ��� ����������� �������
            numline--;
            xk = width + length_top_string * width; // ���������� ������� � ����� ������� ������
         }
      }
   }
} // ���� ������ ������� �����

void DownInput(){
   if(numline < static_cast<int>(input.size() - 1)){
      int length_bottom_string = input[numline + 1].length(); // ��������� ����� ������ ������
      if(numline != bottom - 1){ // ���� ������� �� ��������� �� ����� ������ ������������ ������
         if(POS <= length_bottom_string){ // ���� �� ������ ������ ���� ����� ��� ����������� �������
            numline++; // ��������� �� ��������� ������
            yk += line_height; // ���������� ��������� �������
         }
         else if(linelength >= length_bottom_string){ // ���� �� ������ ������ ��� ����� ��� ����������� �������
            numline++;
            xk = width + length_bottom_string * width, yk += line_height; // ���������� ������� � ����� ������ ������
         }
      }
      else{ // ���� ������� ��������� �� ����� ������ ������������ ������
         top++, bottom++;
         if(POS <= length_bottom_string){ // ���� �� ������ ������ ���� ����� ��� ����������� �������
            numline++;
         }
         else if(linelength >= length_bottom_string){ // ���� �� ������ ������ ��� ����� ��� ����������� �������
            numline++;
            xk = width + length_bottom_string * width; // ���������� ������� � ����� ������ ������
         }
      }
   }
} // ���� ������ ������� ����

void DefaultInput(int ch){
   if(ValidChar(ch)){
      if(linelength < max_length){ // ����������� �� ����� ������
         DOC_MODIFIED = false;
         SUM++; // ����������� �������� ���������� �������� � ���������
         input[numline].insert(POS, 1, ch); // ��������� ������ � ������ �� ������� �������
         xk += width; // ���������� ��������� �������
      }
      else if(linelength == POS){
         DOC_MODIFIED = false;
         SUM++; // ����������� �������� ���������� �������� � ���������
         numline++; // ��������� �� ��������� ������
         xk = width * 2; // ���������� ��������� �������
         if(numline == bottom){
            top++, bottom++;
         }
         else{
            yk += line_height; // ��������� �������� ��������� ��������� �������, ����� ��������� �� �� ����� ������
         }
         input.insert(input.begin() + numline, ""); // ��������� ����� ������
         input[numline].push_back(ch); // ��������� ������ � ����� ����� ������
      }
   }
} // ���� ������ ������ �������

void drawtext(){
   char str1[20]; sprintf(str1, "����������: %d, ", SUM); // ���������� "����������: SUM, " ��� ������
   char str2[15]; sprintf(str2, "������: %d", numline + 1); // ����������  "������: numline + 1" ��� ������
   setcolor(BLACK); // ���� ������
   setfillstyle(SOLID_FILL, WHITE);
   bar(0, 31, 800, 579), bar(0, 581, 400, 600);
   setfillstyle(SOLID_FILL, LIGHTGRAY);
   int j = 0;
   for(int i = top; i < bottom; i++){
      if(!input[i].empty()) outtextxy(width, indent + j * line_height, input[i].c_str()); // ������� ���� ������� �����
      j++;
   }
   outtextxy(width, 581, str1), outtextxy(width + textwidth(str1), 581, str2); // ������� ���������� �������� � ����� ������� ������
} // ����� ��������� ���������� ������ � ���������� �������� �� �����

void buttons(){
   static int state[3] = {0, 0, 0}; // ��������� ���������
   int buttonbounds[3][4] = {{0, 55, 0, 30}, {56, 135, 0, 30}, {136, 240, 0, 30}}; // ������� ������
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(AllFlagsTrue()){ // ���� �� ������� �����-���� �� ���� � �������
      if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
         getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
         for(int i = 0; i < 3; i++){ // ��������: ���� �� ������ ��� �� ������, ���� - ��, �� �������� ��������������� ������
            if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][1] && ym >= buttonbounds[i][2] && ym <= buttonbounds[i][3]){
               switch(i){ // �������� ������ ������� ��� ������ ������
                     case 0:
                        savemenu(); // �������� ������� ��� ������ ������
                        break;
                     case 1:
                        changemenu(); // �������� ������� ��� ������ ������
                        break;
                     case 2:
                        aboutmenu(); // �������� ������� ��� ������� ������
                        break;
               }
            }
         }
         if(ym > 30 && ym < 580){ // ���� ������ � ������� ������
            setcolor(WHITE); // ���� ����
            line(xk, yk, xk, yk + heightk); // ������� �������
            setcolor(BLACK); // ���������� ���� �������
            if((ym - 30) / line_height + top < static_cast<int>(input.size())){
               numline = (ym - 30) / line_height + top;
               xm -= xm % width;
               if(xm < width + static_cast<int>(input[numline].length()) * width) xk = width + xm;
               else xk = width + input[numline].length() * width;
            }
            else{
               numline = input.size() - 1;
               xk = width + input[numline].length() * width;
            }
            yk = indent + (numline - top) * line_height;
            line(xk, yk, xk, yk + heightk); // ������ �������
            setfillstyle(SOLID_FILL, WHITE), bar(0, 581, 400, 600), setfillstyle(SOLID_FILL, LIGHTGRAY);
            char str1[20]; sprintf(str1, "����������: %d, ", SUM); // ���������� "����������: SUM, " ��� ������
            char str2[15]; sprintf(str2, "������: %d", numline + 1); // ����������  "������: numline + 1" ��� ������
            outtextxy(width, 581, str1), outtextxy(width + textwidth(str1), 581, str2); // ������� ���������� �������� � ����� ������� ������
         }
      }
      for(int i = 0; i < 3; i++){
         if(x >= buttonbounds[i][0] && x <= buttonbounds[i][1] && y >= buttonbounds[i][2] && y <= buttonbounds[i][3] && state[i] == 0){
            state[i] = 1; // ������ ��������� ������� ������
            putimage(buttonbounds[i][0], 0, images[i*2+1], COPY_PUT); // ��������� ����������� ������
         }
         else if(!(x >= buttonbounds[i][0] && x <= buttonbounds[i][1] && y >= buttonbounds[i][2] && y <= buttonbounds[i][3]) && state[i] == 1){
            state[i] = 0; // ������ ��������� ������� ������
            putimage(buttonbounds[i][0], 0, images[i*2], COPY_PUT); // ��������� ����������� ������
         }
      }
   }
   for(int i = 0; i < 7; i++){
      if(flags[i] == 0){
         menuFunctions[i](); // �������� ������� ���������������� ����, ���� ��� �������
         break;
      }
   }
} // ��������� ������ ��������� ������������ ��������� ��������� ���� � ������� ���

void savemenu(){
   static int state[2] = {0, 0}, f = 0; // ��������� ��������� � ����, ���������� �� ��, ���� �� ���������� ����
   static int fs = 0; // ����, ���������� �� ��, ����� ��� ������ ������������� ������ "���������" ���������� ���� "��������� ���"
   int buttonbounds[2][4] = {{0, 31, 115, 60}, {0, 61, 115, 90}}; // ������� ��� ������
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[0] = 0, f = 1;
      putimage(0, 31, images[10], COPY_PUT), putimage(0, 61, images[12], COPY_PUT); // ��������� ����������� ���������� ����
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      flags[0] = 1, f = 0;
      drawtext();
      for(int i = 0; i < 2; i++){
         if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
            switch(i){
               case 0:
                  if(fs) save(); // ��� ������ "���������" 
                  else fs = 1, saveasmenu(); // ���� "��������� ���"
                  break;
               case 1:
                  fs = 1, saveasmenu(); // ��� ������ "��������� ���"
                  break;
            }
         }
      }
   }
   for(int i = 0; i < 2; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // ���� �� ������ ��� ����� � ���� ��� ������
            putimage(0, buttonbounds[i][1], images[i*2 + 11], COPY_PUT); // �������� ����������� �� ������
            state[i] = 1; // ��������� ����, ��� ������ ���� ��������� � ���� ��� ������
         }
      }
      else if(state[i]){ // ���� �� ������ ��� ����� �������� �� ���� ��� ������
         putimage(0, buttonbounds[i][1], images[i*2 + 10], COPY_PUT);  // ��������������� �������� �����������
         state[i] = 0; // ���������� ����, ��� ������ ���� ��������� � ���� ��� ������
      }
   }
} // ���� "���������"

int save(){
   string FILEPATH = DIRECTORY + DOC_NAME + ".txt";
   if(!DIRECTORY.empty() && !filesystem::is_directory(DIRECTORY)) return 3;
   ofstream file(FILEPATH);
   int last_non_empty_line = input.size();
   int f = 0;
   for(int i = input.size() - 1; i >= 0; i--){
      if(!input[i].empty()){
         last_non_empty_line = i;
         f = 1;
         break;
      }
   }
   if(f == 0) last_non_empty_line = 0;
   for(int i = 0; i <= last_non_empty_line; i++){
      file << input[i] << endl;
   }
   file.close();
   if(file.fail()) return 2;
   DOC_MODIFIED = true;
   string doc_name = DOC_NAME  + " � �������";
   SetWindowText(hwnd, doc_name.c_str()); // ��������� ������ ����� ����
   return 1;
} // ������� ���������� ���������

void saveasmenu(){
   static int state[3] = {0, 0, 0}, f = 0, f_err = 0; // ��������� ��������� � �����, ���������� �� ��, ���� �� ���������� ����
   static int fstr = 0; // ����, ���������� �� ��, � ����� ���� ���������� ������
   static string str1, str2; // ������ ����� ���� "��������� ���"
   string& selectedstring = fstr ? str2 : str1;
   static int length = 0;
   static int left[2] = {0, 0}; // ������� ������������ ������
   static int xf = 271; // ���������� ��������� �������
   int buttonbounds[4][4] = {{555, 81, 600, 105}, {210, 175, 330, 194}, {265, 115, 582, 135}, {265, 145, 582, 165}}; // ������� ��� ������ � ����� �����
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[6] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 205);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 205), setcolor(COLOR(2, 224, 202)), rectangle(265, 115, 582, 135), setcolor(BLACK), rectangle(265, 145, 582, 165); // ��������� ���� ���� � ����� �����
      putimage(555, 81, images[14]), putimage(210, 175, images[22]);
      outtextxy(210, 90, "��������� ���"), outtextxy(207, 117, "���:"), outtextxy(207, 147, "����:");
      line(xf, 117, xf, 132); // ������ �������
   }
   if(kbhit()){ // ���� ������ �������
      int ch = getch(1); //���������� ������� �������
      int pos = left[fstr] + (xf - 271) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // ���� ������ ������� "Backspace" � ������� �� ��������� � ������� ����� ������� ������
         if(pos == left[fstr]){ // ���� ������� ��������� � ������� ����� ������� ������������ ������, � ���� ������ �� �����
            selectedstring.erase(pos - 1, 1);
            left[fstr]--;
         }
         else{
            selectedstring.erase(pos - 1, 1);
            xf -= width;
         }
         length--, f2 = 1;
      }
      else if(ch == static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL) && pos > 0){
         if(pos == left[fstr]) left[fstr]--; // ���� ������� ��������� � ������� ����� ������� ������������ ������, � ���� ������ �� ����� �����
         else xf -= width;
      }
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length){
         if(pos == left[fstr] + 28) left[fstr]++; // ���� ������� ��������� � ������� ������ ������� ������������ ������, � ���� ������ �� ����� ������
         else xf += width;
      }
      else if(ValidChar(ch)){ // ���� ������ ��������
         if(selectedstring.substr(left[fstr], 28).length() < 28){ // ���� ����� ������� ������ �� ��������� �����������
            selectedstring.insert(pos, 1, ch);
            xf += width;
         }
         else{ // ���� ����� ������� ������ ��������� �����������
            selectedstring.insert(pos, 1, ch);
            left[fstr]++;
         }
         length++, f2 = 1;
      }
      if(f_err && f2){
         f_err = 0;
         setcolor(COLOR(2, 224, 202)), rectangle(265, fstr ? 145 : 115, 582, fstr ? 165 : 135), setcolor(BLACK);
         setfillstyle(SOLID_FILL, WHITE), bar(331, 166, 599, 204), setfillstyle(SOLID_FILL, LIGHTGRAY);
      }
      setfillstyle(SOLID_FILL, WHITE);
      bar(266, fstr ? 146 : 116, 581, fstr ? 164 : 134);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      outtextxy(270, fstr ? 147 : 117, selectedstring.substr(left[fstr], 28).c_str()); // ������� ������
      line(xf, fstr ? 147 : 117, xf, fstr ? 162 : 132); // ������ �������
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      if(xm >= buttonbounds[0][0] && xm <= buttonbounds[0][2] && ym >= buttonbounds[0][1] && ym <= buttonbounds[0][3]){ // ���� ����� �������
         flags[6] = 1, f = 0, fstr = 0;
         str1 = "", str2 = "", left[0] = 0, left[1] = 0, length = 0, xf = 271; // ������� ������, �������� �� ����� � ��������� ������� � ������ ������ ������
         drawtext();
      }
      else if(xm >= buttonbounds[2][0] && xm <= buttonbounds[2][2] && ym >= buttonbounds[2][1] && ym <= buttonbounds[2][3]){ // ���� ������ � ������ ���� "���:"
         fstr = 0;
         setcolor(COLOR(2, 224, 202)), rectangle(265, 115, 582, 135), setcolor(BLACK), rectangle(265, 145, 582, 165);
         setfillstyle(SOLID_FILL, WHITE), bar(331, 166, 599, 204), setfillstyle(SOLID_FILL, LIGHTGRAY);
         setcolor(WHITE), line(xf, 147, xf, 162), line(xf, 117, xf, 132), setcolor(BLACK); // ������� �������
         outtextxy(270, 117, str1.substr(left[fstr], 28).c_str()); // ������� ������
         length = str1.length();
         xm -= (xm - 271) % width;
         if(xm < 271 + static_cast<int>(str1.substr(left[fstr], 28).length()) * width) xf = xm;
         else xf = 271 + static_cast<int>(str1.substr(left[fstr], 28).length()) * width;
         line(xf, 117, xf, 132); // ���������� �������
      }
      else if(xm >= buttonbounds[3][0] && xm <= buttonbounds[3][2] && ym >= buttonbounds[3][1] && ym <= buttonbounds[3][3]){ // ���� ������ � ������ ���� "����:"
         fstr = 1;
         setcolor(COLOR(2, 224, 202)), rectangle(265, 145, 582, 165), setcolor(BLACK), rectangle(265, 115, 582, 135);
         setfillstyle(SOLID_FILL, WHITE), bar(331, 166, 599, 204), setfillstyle(SOLID_FILL, LIGHTGRAY);
         setcolor(WHITE), line(xf, 117, xf, 132), line(xf, 147, xf, 162), setcolor(BLACK); // ������� �������
         outtextxy(270, 147, str2.substr(left[fstr], 28).c_str()); // ������� ������
         length = str2.length();
         xm -= (xm - 271) % width;
         if(xm < 271 + static_cast<int>(str2.substr(left[fstr], 28).length()) * width) xf = xm;
         else xf = 271 + static_cast<int>(str2.substr(left[fstr], 28).length()) * width;
         line(xf, 147, xf, 162); // ���������� �������
      }
      else{
         if(xm >= buttonbounds[1][0] && xm <= buttonbounds[1][2] && ym >= buttonbounds[1][1] && ym <= buttonbounds[1][3]){
            if(!str1.empty()){ // ���� ������ � ��������� ����� �� �����
               DOC_NAME = str1, DIRECTORY = str2;
               int i = save();
               if(i == 1){
                  flags[6] = 1, f = 0, fstr = 0;
                  str1 = "", str2 = "", left[0] = 0, left[1] = 0, length = 0, xf = 271; // ������� ������, �������� �� ����� � ��������� ������� � ������ ������ ������
                  drawtext();
               }
               else{
                  f_err = 1;
                  setcolor(COLOR(255, 0, 0));
                  if(i == 3) outtextxy(360, 175, "������������ ����");
                  else outtextxy(360, 175, "������ ����������");
                  DOC_NAME = "��� �����", DIRECTORY = "";
                  setcolor(BLACK);
               }
            }
            else{
               f_err = 1;
               setcolor(COLOR(255, 0, 0)), outtextxy(360, 175, "������� ��� �����"), rectangle(265, 115, 582, 135), setcolor(BLACK);
            }
         }
      }
   }
   for(int i = 0; i < 2; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // ���� �� ������ ��� ����� � ���� ��� ������
            if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[15], COPY_PUT); // �������� ����������� �� ������
            else putimage(buttonbounds[i][0], 175, images[(i - 1)*2 + 23], COPY_PUT); // �������� ����������� �� ������
            state[i] = 1; // ��������� ����, ��� ������ ���� ��������� � ���� ��� ������
         }
      }
      else if(state[i]){ // ���� �� ������ ��� ����� �������� �� ���� ��� ������
         if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[14], COPY_PUT);  // ��������������� �������� �����������
         else putimage(buttonbounds[i][0], 175, images[(i - 1)*2 + 22], COPY_PUT);  // ��������������� �������� �����������
         state[i] = 0; // ���������� ����, ��� ������ ���� ��������� � ���� ��� ������
      }
   }
} // ���� "��������� ���"

void changemenu(){
   static int state[2] = {0, 0}, f = 0; // ��������� ��������� � ����, ���������� �� ��, ���� �� ���������� ����
   int buttonbounds[2][4] = {{56, 31, 155, 60}, {56, 61, 155, 90}}; // ������� ��� ������
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[1] = 0, f = 1;
      putimage(56, 31, images[6], COPY_PUT), putimage(56, 61, images[8], COPY_PUT); // ��������� ����������� ���������� ����
   }
   if(ismouseclick(WM_LBUTTONDOWN)){
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      flags[1] = 1, f = 0;
      drawtext();
      for(int i = 0; i < 2; i++){
         if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
            switch(i){
               case 0:
                  findmenu(); // ������� "�����"
                  break;
               case 1:
                  replacemenu(); // ������� "������"
                  break;
            }
         }
      }
   }
   for(int i = 0; i < 2; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // ���� �� ������ ��� ����� � ���� ��� ������
            putimage(56, buttonbounds[i][1], images[i*2 + 7], COPY_PUT); // �������� ����������� �� ������
            state[i] = 1; // ��������� ����, ��� ������ ���� ��������� � ���� ��� ������
         }
      }
      else if(state[i]){ // ���� �� ������ ��� ����� �������� �� ���� ��� ������
         putimage(56, buttonbounds[i][1], images[i*2 + 6], COPY_PUT);  // ��������������� �������� �����������
         state[i] = 0; // ���������� ����, ��� ������ ���� ��������� � ���� ��� ������
      }
   }
} // ���� "��������"

void findmenu(){
   static int state[2] = {0, 0}, f = 0, f_err = 0; // ��������� ��������� � �����, ���������� �� ��, ���� �� ���������� ����
   static string str; // ������ ���� ���� "�����"
   static int length = 0;
   static int left = 0; // ������ ������������ ������
   static int xf = 261; // ���������� ��������� �������
   int buttonbounds[3][4] = {{555, 81, 600, 105}, {210, 145, 330, 165}, {255, 115, 582, 135}}; // ������� ��� ������
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[3] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 175);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 175), setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK); // ��������� ���� ���� � ���� �����
      line(xf, 117, xf, 132); // ������ �������
      putimage(555, 81, images[14]), putimage(210, 145, images[16]);
      outtextxy(210, 90, "�����"), outtextxy(210, 117, "���:");
   }
   if(kbhit()){ // ���� ������ �������
      int ch = getch(1); //���������� ������� �������
      int pos = left + (xf - 261) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // ���� ������ ������� "Backspace" � ������� �� ��������� � ������� ����� ������� ������
         if(pos == left){ // ���� ������� ��������� � ������� ����� ������� ������������ ������, � ���� ������ �� �����
            str.erase(pos - 1, 1);
            left--;
         }
         else{
            str.erase(pos - 1, 1);
            xf -= width;
         }
         length--, f2 = 1;
      }
      else if(ch == static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL) && pos > 0){
         if(pos == left) left--; // ���� ������� ��������� � ������� ����� ������� ������������ ������, � ���� ������ �� ����� �����
         else xf -= width;
      }
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length){
         if(pos == left + 29) left++; // ���� ������� ��������� � ������� ������ ������� ������������ ������, � ���� ������ �� ����� ������
         else xf += width;
      }
      else if(ValidChar(ch)){ // ���� ������ ��������
         if(str.substr(left, 29).length() < 29){ // ���� ����� ������� ������ �� ��������� �����������
            str.insert(pos, 1, ch);
            xf += width;
         }
         else{ // ���� ����� ������� ������ ��������� �����������
            str.insert(pos, 1, ch);
            left++;
         }
         length++, f2 = 1;
      }
      if(f_err && f2){ // ���� ��� ������������ ��������� �� ������ ���������� ��������� ������
         f_err = 0;
         setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK);
         setfillstyle(SOLID_FILL, WHITE), bar(360, 140, 550, 170), setfillstyle(SOLID_FILL, LIGHTGRAY);
      }
      setfillstyle(SOLID_FILL, WHITE);
      bar(256, 116, 581, 134);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      outtextxy(260, 117, str.substr(left, 29).c_str()); // ������� ������
      line(xf, 117, xf, 132); // ������ �������
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      if(xm >= buttonbounds[0][0] && xm <= buttonbounds[0][2] && ym >= buttonbounds[0][1] && ym <= buttonbounds[0][3]){ // ���� ����� �������
         flags[3] = 1, f = 0;
         str = "", left = 0, length = 0, xf = 261; // ������� ������ ������, �������� � ����� � ��������� ������� � ������ ������
         drawtext();
      }
      else if(xm >= buttonbounds[2][0] && xm <= buttonbounds[2][2] && ym >= buttonbounds[2][1] && ym <= buttonbounds[2][3]){ // ���� ������ � ������ ���� �����
         setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK);
         setfillstyle(SOLID_FILL, WHITE), bar(360, 140, 550, 170), setfillstyle(SOLID_FILL, LIGHTGRAY);
         setcolor(WHITE), line(xf, 117, xf, 132), setcolor(BLACK); // ������� �������
         xm -= (xm - 261) % width;
         if(xm < 261 + static_cast<int>(str.substr(left, 29).length()) * width) xf = xm;
         else xf = 261 + static_cast<int>(str.substr(left, 29).length()) * width;
         line(xf, 117, xf, 132); // ������ �������
      }
      else if(xm >= buttonbounds[1][0] && xm <= buttonbounds[1][2] && ym >= buttonbounds[1][1] && ym <= buttonbounds[1][3]){ // ���� ������ ������ "����� ���"
         if(!str.empty()) findall(str, f, f_err, length, left, xf); // ����� ���� ��������� ��������� str � ��������� � �� �������������
      }
   }
   for(int i = 0; i < 2; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // ���� �� ������ ��� ����� � ���� ��� ������
            if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[15], COPY_PUT); // �������� ����������� �� ������
            else putimage(buttonbounds[i][0], 145, images[(i - 1)*2 + 17], COPY_PUT); // �������� ����������� �� ������
            state[i] = 1; // ��������� ����, ��� ������ ���� ��������� � ���� ��� ������
         }
      }
      else if(state[i]){ // ���� �� ������ ��� ����� �������� �� ���� ��� ������
         if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[14], COPY_PUT);  // ��������������� �������� �����������
         else putimage(buttonbounds[i][0], 145, images[(i - 1)*2 + 16], COPY_PUT);  // ��������������� �������� �����������
         state[i] = 0; // ���������� ����, ��� ������ ���� ��������� � ���� ��� ������
      }
   }
} // ������� "�����"

void findall(string &str, int &f, int &f_err, int &length, int &left, int &xf){
   bool f1 = false; // ����, ���������� �� ��, ������� �� ���� ���� ��������� ��������� � ���������
   int strlength = str.size();
   int index = 0;
   for(int i = top; i < bottom; i++){ // �������� �� ���� ������� ������� ���������
      size_t pos = 0;
      while((pos = input[i].find(str, pos)) != string::npos){
         if(f1 == false){
            f1 = true;
            drawtext();
            setbkcolor(LIGHTGRAY); // ������ ��� ������
         }
         outtextxy((pos + 1) * width, indent + index * line_height, str.c_str()); // ������� ��������� ���������
         pos += strlength;
      }
      index++;
   }
   if(f1){
      setbkcolor(WHITE); // ������ ��� ������
      while(!ismouseclick(WM_LBUTTONDOWN)){ // ��� ������� ���
         if(kbhit()) getch(); // ���� ������ ������� - ��������� �
         continue;
         delay(5); // �������� ���������� �����
      }
      flags[3] = 1, f = 0;
      str = "", length = 0, left = 0, xf = 261; // ������� ������ ������, �������� � ����� � ��������� ������� � ������ ������
      drawtext();
   }
   else{
      findmenu();
      f_err = 1;
      setcolor(COLOR(255, 0, 0)), rectangle(255, 115, 582, 135);
      outtextxy(360, 145, "������ �� �������");
      setcolor(BLACK);
   }
} // ����� ���� ��������� ��������� str � ��������� � �� �������������

void replacemenu(){
   static int state[3] = {0, 0, 0}, f = 0, f_err = 0; // ��������� ��������� � �����, ���������� �� ��, ���� �� ���������� ����
   static int fstr = 0; // ����, ���������� �� ��, � ����� ���� ���������� ������
   static string str1, str2; // ������ ����� ���� "������"
   string& selectedstring = fstr ? str2 : str1;
   static int length = 0;
   static int left[2] = {0, 0}; // ������� ������������ ������
   static int xf = 261; // ���������� ��������� �������
   int buttonbounds[5][4] = {{555, 81, 600, 105}, {210, 175, 330, 194}, {340, 175, 460, 194}, {255, 115, 582, 135}, {255, 145, 582, 165}}; // ������� ��� ������ � ����� �����
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[4] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 205);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 205), setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK), rectangle(255, 145, 582, 165); // ��������� ���� ���� � ����� �����
      putimage(555, 81, images[14]), putimage(210, 175, images[18]), putimage(340, 175, images[20]);
      outtextxy(210, 90, "������"), outtextxy(210, 117, "���:"), outtextxy(210, 147, "���:");
      line(xf, 117, xf, 132); // ������ �������
   }
   if(kbhit()){ // ���� ������ �������
      int ch = getch(1); //���������� ������� �������
      int pos = left[fstr] + (xf - 261) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // ���� ������ ������� "Backspace" � ������� �� ��������� � ������� ����� ������� ������
         if(pos == left[fstr]){ // ���� ������� ��������� � ������� ����� ������� ������������ ������, � ���� ������ �� �����
            selectedstring.erase(pos - 1, 1);
            left[fstr]--;
         }
         else{
            selectedstring.erase(pos - 1, 1);
            xf -= width;
         }
         length--, f2 = 1;
      }
      else if(ch == static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL) && pos > 0){
         if(pos == left[fstr]) left[fstr]--; // ���� ������� ��������� � ������� ����� ������� ������������ ������, � ���� ������ �� ����� �����
         else xf -= width;
      }
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length){
         if(pos == left[fstr] + 29) left[fstr]++; // ���� ������� ��������� � ������� ������ ������� ������������ ������, � ���� ������ �� ����� ������
         else xf += width;
      }
      else if(ValidChar(ch)){ // ���� ������ ��������
         if(selectedstring.substr(left[fstr], 29).length() < 29){ // ���� ����� ������� ������ �� ��������� �����������
            selectedstring.insert(pos, 1, ch);
            xf += width;
         }
         else{ // ���� ����� ������� ������ ��������� �����������
            selectedstring.insert(pos, 1, ch);
            left[fstr]++;
         }
         length++, f2 = 1;
      }
      if(f_err && f2){
         f_err = 0;
         setcolor(COLOR(2, 224, 202)), rectangle(255, fstr ? 145 : 115, 582, fstr ? 165 : 135), setcolor(BLACK);
         rectangle(255, fstr ? 115 : 145, 582, fstr ? 135 : 165);
      }
      setfillstyle(SOLID_FILL, WHITE);
      bar(256, fstr ? 146 : 116, 581, fstr ? 164 : 134);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      outtextxy(260, fstr ? 147 : 117, selectedstring.substr(left[fstr], 29).c_str()); // ������� ������
      line(xf, fstr ? 147 : 117, xf, fstr ? 162 : 132); // ������ �������
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      if(xm >= buttonbounds[0][0] && xm <= buttonbounds[0][2] && ym >= buttonbounds[0][1] && ym <= buttonbounds[0][3]){ // ���� ����� �������
         flags[4] = 1, f = 0, fstr = 0;
         str1 = "", str2 = "", length = 0, left[0] = 0, left[1] = 0, xf = 261; // ������� ������, �������� �� ����� � ��������� ������� � ������ ������ ������
         drawtext();
      }
      else if(xm >= buttonbounds[3][0] && xm <= buttonbounds[3][2] && ym >= buttonbounds[3][1] && ym <= buttonbounds[3][3]){ // ���� ������ � ������ ���� "���:"
         fstr = 0;
         setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK), rectangle(255, 145, 582, 165);
         setcolor(WHITE), line(xf, 147, xf, 162), line(xf, 117, xf, 132), setcolor(BLACK); // ������� �������
         outtextxy(260, 117, str1.substr(left[fstr], 29).c_str()); // ������� ������
         length = str1.length(); 
         xm -= (xm - 261) % width;
         if(xm < 261 + static_cast<int>(str1.substr(left[fstr], 29).length()) * width) xf = xm;
         else xf = 261 + static_cast<int>(str1.substr(left[fstr], 29).length()) * width;
         line(xf, 117, xf, 132); // ���������� �������
      }
      else if(xm >= buttonbounds[4][0] && xm <= buttonbounds[4][2] && ym >= buttonbounds[4][1] && ym <= buttonbounds[4][3]){ // ���� ������ � ������ ���� "���:"
         fstr = 1;
         setcolor(COLOR(2, 224, 202)), rectangle(255, 145, 582, 165), setcolor(BLACK), rectangle(255, 115, 582, 135);
         setcolor(WHITE), line(xf, 117, xf, 132), line(xf, 147, xf, 162), setcolor(BLACK); // ������� �������
         outtextxy(260, 147, str2.substr(left[fstr], 29).c_str()); // ������� ������
         length = str2.length(); 
         xm -= (xm - 261) % width;
         if(xm < 261 + static_cast<int>(str2.substr(left[fstr], 29).length()) * width) xf = xm;
         else xf = 261 + static_cast<int>(str2.substr(left[fstr], 29).length()) * width;
         line(xf, 147, xf, 162); // ���������� �������
      }
      else{
         for(int i = 1; i < 3; i++){
            if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
               if(!str1.empty()){ // ���� ������ ������ �� �����
                  switch(i){
                     case 1: // ���� ������ ������ "�������� ���"
                        replaceall(str1, str2, f, fstr, f_err, length, left, xf); // ���������� ���� �������� � �� ������
                        break;
                     case 2: // ���� ������ ������ "��������"
                        replace(str1, str2, f, fstr, f_err, length, left, xf); // ���������� ��������� � � ������
                        break;
                  
                  }
               }
            }
         }
      }
   }
   for(int i = 0; i < 3; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // ���� �� ������ ��� ����� � ���� ��� ������
            if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[15], COPY_PUT); // �������� ����������� �� ������
            else putimage(buttonbounds[i][0], 175, images[(i - 1)*2 + 19], COPY_PUT); // �������� ����������� �� ������
            state[i] = 1; // ��������� ����, ��� ������ ���� ��������� � ���� ��� ������
         }
      }
      else if(state[i]){ // ���� �� ������ ��� ����� �������� �� ���� ��� ������
         if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[14], COPY_PUT);  // ��������������� �������� �����������
         else putimage(buttonbounds[i][0], 175, images[(i - 1)*2 + 18], COPY_PUT);  // ��������������� �������� �����������
         state[i] = 0; // ���������� ����, ��� ������ ���� ��������� � ���� ��� ������
      }
   }
} // ������� "������"

void replace(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int (&left)[2], int &xf){
   bool f1 = false; // ����, ���������� �� ��, ������� �� ���� ���� ��������� ��������� � ���������
   int strlength1 = str1.size(), strlength2 = str2.size();
   int docsize = input.size();
   for(int i = 0; i < docsize; i++){ // �������� �� ���� ������� ���������
      size_t pos = 0;
      while((pos = input[i].find(str1, pos)) != string::npos){
         if(f1 == false && input[i].size() + ((strlength1 > strlength2) ? (strlength1 - strlength2) : (strlength2 - strlength1)) <= max_length){
            f1 = true;
            input[i].erase(pos, strlength1), input[i].insert(pos, str2); // ��������
            xk = (input[i].length() + 1) * width, yk = indent + i * line_height, numline = i; // ���������� ������� � ����� ������, � ������� ��������� ������
            drawtext();
            break;
         }
         pos += strlength1;
      }
      if(f1) break;
   }
   if(f1){
      flags[4] = 1, f = 0, fstr = 0;
      str1 = "", str2 = "", length = 0, left[0] = 0, left[1] = 0, xf = 261; // ������� ������, �������� �� ����� � ��������� ������� � ������ ������ ������
   }
   else{
      replacemenu();
      f_err = 1;
      setcolor(COLOR(255, 0, 0));
      rectangle(255, 145, 582, 165), rectangle(255, 115, 582, 135);
      setcolor(BLACK);
   }
} // ���������� ��������� � � ������

void replaceall(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int (&left)[2], int &xf){
   bool f1 = false; // ����, ���������� �� ��, ���� �� ����������� ���� ���� ������
   int strlength1 = str1.size(), strlength2 = str2.size();
   int last_replaced_string = 0;
   int docsize = input.size();
   for(int i = 0; i < docsize; i++){ // �������� �� ���� ������� ���������
      size_t pos = 0;
      while((pos = input[i].find(str1, pos)) != string::npos){
         if(input[i].size() + ((strlength1 > strlength2) ? (strlength1 - strlength2) : (strlength2 - strlength1)) <= max_length){
            if(f1 == false) f1 = true;
            input[i].erase(pos, strlength1), input[i].insert(pos, str2); // ��������
         }
         pos += strlength1, last_replaced_string = i;
      }
   }
   if(f1){
      flags[4] = 1, f = 0, fstr = 0;
      str1 = "", str2 = "", length = 0, left[0] = 0, left[1] = 0, xf = 261; // ������� ������, �������� �� ����� � ��������� ������� � ������ ������ ������
      drawtext();
      xk = (input[last_replaced_string].length() + 1) * width, yk = indent + last_replaced_string * line_height, numline = last_replaced_string; // ���������� ������� � ����� ������, � ������� ��������� ��������� ������
   }
   else{
      replacemenu();
      f_err = 1;
      setcolor(COLOR(255, 0, 0));
      rectangle(255, 145, 582, 165), rectangle(255, 115, 582, 135);
      setcolor(BLACK);
   }
} // ���������� ���� �������� � �� ������

void aboutmenu(){
   static int state = 0, f = 0; // ��������� ��������� � ����, ���������� �� ��, ���� �� ���������� ����
   int buttonbounds[4] = {605, 81, 650, 105};
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[2] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(150, 80, 650, 240);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      rectangle(150, 80, 650, 240); // ��������� ���� ����
      outtextxy(160, 90, "��������� �������� <�������>");
      outtextxy(160, 130, "����������: �������� ��������� ���������");
      outtextxy(160, 150, "������: ��-112");
      outtextxy(160, 170, "����������: ������ ����������������");
      outtextxy(160, 190, "�������������: ��������� �.�.");
      outtextxy(325, 210, "������� �.�.");
      putimage(136, 0, images[4], COPY_PUT), putimage(605, 81, images[14]);
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      if(xm >= buttonbounds[0] && xm <= buttonbounds[2] && ym >= buttonbounds[1] && ym <= buttonbounds[3]){ // ���� ����� �������
         flags[2] = 1, f = 0;
         drawtext();
      }
   }
   if(x >= buttonbounds[0] && x <= buttonbounds[2] && y >= buttonbounds[1] && y <= buttonbounds[3] && state == 0){
      state = 1; // ������ ��������� ������� ������
      putimage(buttonbounds[0], buttonbounds[1], images[15], COPY_PUT); // �������� ����������� �� ������
   }
   else if(!(x >= buttonbounds[0] && x <= buttonbounds[2] && y >= buttonbounds[1] && y <= buttonbounds[3]) && state == 1){
      state = 0; // ������ ��������� ������� ������
      putimage(buttonbounds[0], buttonbounds[1], images[14], COPY_PUT);  // ��������������� �������� �����������
   }
} // ���� "� ���������"

void exit(){
   if(DOC_MODIFIED == false){ // ���� ���� �� �������� � ���� ������ �������
      exitmenu();
   }
   else{ // ���� ���� ��������
      closegraph(); // ��������� ����
      exit(0);
   }
} // ����, ������������ ��� �������� ���� �������������, ���� ���� �� �������

void exitmenu(){
   static int state[3] = {0, 0, 0}, f = 0; // ��������� ��������� � ����, ���������� �� ��, ���� �� ���������� ����
   int buttonbounds[3][4] = {{635, 171, 680, 205}, {270, 240, 390, 260}, {410, 240, 530, 260}}; // ������� ��� ������
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(kbhit()) getch(); // ���� ������ ������� � ����������  - ��������� �
   if(f == 0){
      flags[5] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(120, 170, 680, 275);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      setcolor(BLACK);
      rectangle(120, 170, 680, 275); // ��������� ���� ����
      putimage(635, 171, images[14]), putimage(270, 240, images[22]), putimage(410, 240, images[24]);
      string FILEPATH = DOC_NAME + ".txt?";
      if(FILEPATH.length() < 55){
         outtextxy(193, 190, "�� ������ ��������� ��������� � �����"); // ������� ���������
         outtextxy(400 - textwidth(FILEPATH.c_str()) / 2, 210, FILEPATH.c_str()); // ������� ���������
      }
      else outtextxy(193, 197, "�� ������ ��������� ��������� � �����?"); // ������� ���������
      setcolor(WHITE);
   }
   if(kbhit() && getch() == WINDOW_CLOSE){ // ���� ������� ���� �������� � ���������� ������� ��������
      save();
      closegraph(); // ��������� ����
      exit(0);
   }
   if(ismouseclick(WM_LBUTTONDOWN)){
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      if(xm > buttonbounds[0][0] && xm < buttonbounds[0][2] && ym > buttonbounds[0][1] && ym < buttonbounds[0][3]){ // ���� ����� �������
         flags[5] = 1, f = 0;
         drawtext();
      }
      else{
         for(int i = 1; i < 3; i++){
            if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
               switch(i - 1){
                  case 0: // ���� ������ ������ "���������"
                     save();
                     closegraph(); // ��������� ����
                     exit(0);
                     break;
                  case 1: // ���� ������ ������ "�� ���������"
                     closegraph(); // ��������� ����
                     exit(0);
                     break;
               }
            }
         }
      }
   }
   for(int i = 0; i < 3; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // ���� �� ������ ��� ����� � ���� ��� ������
            if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[15], COPY_PUT); // �������� ����������� �� ������
            else putimage(buttonbounds[i][0], 240, images[(i - 1)*2 + 23], COPY_PUT); // �������� ����������� �� ������
            state[i] = 1; // ��������� ����, ��� ������ ���� ��������� � ���� ��� ������
         }
      }
      else if(state[i]){ // ���� �� ������ ��� ����� �������� �� ���� ��� ������
         if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[14], COPY_PUT);  // ��������������� �������� �����������
         else putimage(buttonbounds[i][0], 240, images[(i - 1)*2 + 22], COPY_PUT);  // ��������������� �������� �����������
         state[i] = 0; // ���������� ����, ��� ������ ���� ��������� � ���� ��� ������
      }
   }
} // ����, ������������ ��� �������� ���� �������������, ���� ���� �� �������

bool AllFlagsTrue(){
   for(int i = 0; i < 7; i++){
      if(flags[i] == false) return false;
   }
   return true;
} // ������� ��� �������� ����, ��� �� ����� � ������� flags ��������� �������� true

bool ValidChar(int ch){
   if(ch >= ' ' && ch <= 255) return true;
   return false;
} // ������� ��� �������� ����, ������������� �� ������ �����

void mouse_thread(){
   while(1){
      x = mousex(), y = mousey(); // ����������� ��������� ��������� ����
      buttons(); // ��������� ������ ��������� ������������ ��������� ��������� ���� � ������� ���
      delay(5); // �������� ���������� �����
   }
} // ����� �� ����������� ��������� ��������� ����

void keyboard_thread(){
   clock_t start_time = clock(); // ���������� ����� ������ �����
   while(1){
      if(AllFlagsTrue()){ // ���� �� ������� �����-���� �� ���� � �������
         if(kbhit()){ // ���� ������ ������� � �� ������� �����-���� �� ����
            int ch = getch(1); //���������� ������� �������
            setcolor(WHITE), line(xk, yk, xk, yk + heightk); // ������� �������
            HandleInput(ch); // ��������� ��������� �������, � ����������� ������� ��������� ������ � ���������� �������� �� �����
            line(xk, yk, xk, yk + heightk); // ������ �������
            start_time = clock(); // ��������� ����� ������ �����
         }
         else if((clock() - start_time) > 500){ // ���� ������ ����� 500 ����������� � ������� ���������� ������� �������
            line(xk, yk, xk, yk + heightk); // ������ �������
            delay(250); // ��������, ����� ������� ������
            setcolor(WHITE); // ���� ����
            line(xk, yk, xk, yk + heightk); // ������� �������
            setcolor(BLACK); // ���������� ���� �������
            delay(250); // ��������, ����� ������� ������
            start_time = clock(); // ��������� ����� ������ �����
         }
      }
      else if(kbhit() && flags[3] && flags[4] && flags[5] && flags[6]) getch(); // ���� ������� ������ � �����-���� �� ����(� ������� ���������� ���������� ������� ������) - ��������� �
      delay(5); // �������� ���������� �����
   }
} // ����� �� ����� �������� � ���������� � ����������� ��������� �������