#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <ctime>
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
int x, y; // ���������� ��������� ����
int xk = 11, yk = indent, heightk = 15; // ���������� ��������� �������, � ������
int POS = 1; // ������� ��������� ������� � ������� �����
bool DOC_MODIFIED = true; // ����, ���������� �� ��, ��� �� � ������ ����
bool flags[6] = {true, true, true, true, true, true}; // ����� �������� ���� � �������
IMAGE *images[26]; // ������ ����������� ������
string input[max_lines]; // ������ �����, �������� ���� �������� �����
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
void save(); // ��� ������ "���������"
void saveas(); // ������� "��������� ���"
void changemenu(); // ���� "��������"
void findmenu(); // ������� "�����"
void findall(string &str, int &f, int &f_err, int &length, int &xf); // ����� ���� ��������� ��������� str � ��������� � �� �������������
void replacemenu(); // ������� "������"
void replace(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int &xf); // ���������� ��������� � � ������
void replaceall(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int &xf); // ���������� ���� �������� � �� ������
void aboutmenu(); // ���� "� ���������"
void exit(); // ���� ����� �������
void exitmenu(); // ����, ������������ ��� �������� ���� �������������, ���� ���� �� �������
bool AllFlagsTrue(); // ������� ��� �������� ����, ��� �� ����� � ������� flags ��������� �������� true
bool ValidChar(int ch); // ������� ��� �������� ����, ������������� �� ������ �����
//================================================

void (*menuFunctions[6])() = {savemenu, changemenu, aboutmenu, findmenu, replacemenu, exitmenu}; // ������������� ������� ���������� �� �������

int main(){
   Initializewindow(); // ������������� ����
   thread t1(mouse_thread); // ������� ������ ����� � �������� � ���� ������� mouse_thread
   thread t2(keyboard_thread); // ������� ������ ����� � �������� � ���� ������� keyboard_thread
   t1.join(); // ������� ���������� ������� ������
   t2.join(); // ������� ���������� ������� ������
   return 0;
}

void Initializewindow(){
   initwindow(800, 600, "�������", 100, 100, false, false); // ������������� ����
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
   if(flags[5]) drawtext(); // ����� ��������� ���������� ������ �� �����,  ���������� ��������(���� ������� ���� ������)
} // ��������� ��������� �������, � ����������� ������� ��������� ������ � ���������� �������� �� �����

void EnterInput(){
   if(input[max_lines - 1].empty() && numline < max_lines - 1){ // ���� ��������� ������ ����� � ������� �� ��������� �� ���
      DOC_MODIFIED = false;
      numline++; // ��������� �� ��������� ������ � ����������� �������� ������������ ��������������� ������
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, indent + (numline - 1)*line_height, 800, 579);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      setcolor(BLACK); // ���� ������
      for(int i = max_lines - 1; i > numline; i--){
         input[i] = input[i - 1]; // ��������� ��� ������ ���� ������� ������ ���� �� ���� �������
         outtextxy(width, indent + i * line_height, input[i].c_str()); // ������� ������
      }
      input[numline] = input[numline - 1].substr(POS, linelength); // ��������� ��� ������� ���������� ������, ����������� ������ �� �������, �� ����� ������ ������
      input[numline - 1] = input[numline - 1].erase(POS, linelength); // ������� ��� ������� �� ������� ������
      outtextxy(width, indent + (numline - 1) * line_height, input[numline - 1].c_str()); // ������� ������� ������
      xk = width, yk += line_height; // ��������� �������� ��������� ��������� �������, ����� ��������� �� �� ����� ������
   }
} // ���� ������ ������� "Enter"

void BackspaceInput(){
   if(POS > 0){ // ���� ������� �� ��������� � ������� ����� ������� ������
      DOC_MODIFIED = false;
      SUM--;
      input[numline].erase(POS - 1, 1); // ������� ������ �� ������
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, indent + numline * line_height, 800, indent + (numline + 1) * line_height);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      xk -= width; // ���������� ��������� �������
   }
   else if(numline > 0){ // ���� ������� ��������� � ������� ����� ������� � ������ � �� ��������� �� ������ ������
      DOC_MODIFIED = false;
      numline--; // ��������� �� ���������� ������
      linelength = input[numline].length(); // �������� ����� ���������� ������
      input[numline] = input[numline] + input[numline + 1].substr(0,  max_length - linelength); // ��������� � ������� ������ �����, ������� ����� � ��� ���������� �� ������ ������
      input[numline + 1].erase(0,  max_length - linelength); // ������� ��� ����� �� ������ ������
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, indent + numline*line_height, 800, 579);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      setcolor(BLACK); // ���� ������
      for(int i = numline + 1; i < max_lines; i++){ // ��������� � ������� ��� ������ ���� ������� ������ ���� �� ���� �������
         input[i] = input[i + 1];
         outtextxy(width, indent + i * line_height, input[i].c_str());
      }
      xk = width + linelength*width, yk -= line_height; // ���������� ��������� �������
   }
} // ���� ������ ������� "Backspace"

void LeftInput(){
   if(POS > 0){ // ���� ������� ������� ������� �� �������� ����� ����� � ������
      xk -= width; // ���������� ��������� �������
   }
   else if(numline > 0){ // ���� ������� ������� ������� �������� ����� ����� � ������ � ��� �� ������
      numline--; // ��������� �� ���������� ������
      linelength = input[numline].length(); // �������� ����� ���������� ������
      xk = width + linelength*width, yk -= line_height; // ���������� ��������� �������
   }
} // ���� ������ ������� �����

void RightInput(){
   if(POS < linelength){ // ���� ������� ������� ������� �� �������� ����� ������ � ������
      xk += width; // ���������� ��������� �������
   }
   else if(numline < max_lines - 1){ // ���� ������� ������� ������� �� �������� ����� ������ � ������, ������ �� ��������� � ������ ������ �� �����
      numline++;
      xk = width, yk += line_height; // ���������� ��������� �������
   }
} // ���� ������ ������� ������

void UpInput(){
   if(numline > 0){ // ���� ������� �� ��������� �� ����� ������ ������
      int length_top_string = input[numline - 1].length(); // ��������� ����� ������� ������
      if(POS <= length_top_string){ // ���� �� ������� ������ ���� ����� ��� ����������� �������
         numline--;
         yk -= line_height; // ���������� ��������� �������
      }
      else if(linelength >= length_top_string){ // ���� �� ������� ������ ��� ����� ��� ����������� �������
         numline--;
         xk = width + length_top_string*width, yk -= line_height; // ���������� ������� � ����� ������� ������
      }
   }
} // ���� ������ ������� �����

void DownInput(){
   if(numline < max_lines - 1){ // ���� ������� �� ��������� �� ����� ��������� ������
      int length_bottom_string = input[numline + 1].length(); // ��������� ����� ������ ������
      if(POS <= length_bottom_string){ // ���� �� ������ ������ ���� ����� ��� ����������� �������
         numline++; // ��������� �� ��������� ������
         yk += line_height; // ���������� ��������� �������
      }
      else if(linelength >= length_bottom_string){ // ���� �� ������ ������ ��� ����� ��� ����������� �������
         numline++;
         xk = width + length_bottom_string*width, yk += line_height; // ���������� ������� � ����� ������ ������
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
      else if(input[max_lines - 1].empty() && numline < max_lines - 1 && POS == linelength){ // ���� ��������� ������ ����� � ������� ��������� � ����� ������, �� �� ���������
         DOC_MODIFIED = false;
         SUM++; // ����������� �������� ���������� �������� � ���������
         numline++; // ��������� �� ��������� ������
         setfillstyle(SOLID_FILL, WHITE);
         bar(0, indent + numline*line_height, 800, 579);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
         setcolor(BLACK); // ���� ������
         for(int i = max_lines - 1; i > numline; i--){ // ��������� � ������� ��� ������ ���� ������� ������ ���� �� ���� �������
            input[i] = input[i - 1];
            outtextxy(width, indent + i * line_height, input[i].c_str());
         }
         input[numline] = ""; // ������� ������� ������
         input[numline].push_back(ch); // ��������� ������ � ����� ����� ������
         xk = width * 2, yk += line_height; // ���������� ��������� �������
      }
   }
} // ���� ������ ������ �������

void drawtext(){
   setcolor(BLACK); // ���� ������
   outtextxy(width, indent + numline * line_height, input[numline].c_str()); // ������� ��������� ��������� ������
   char str1[20]; sprintf(str1, "����������: %d, ", SUM); // ���������� "����������: SUM, " ��� ������
   char str2[15]; sprintf(str2, "������: %d", numline + 1); // ����������  "������: numline + 1" ��� ������
   setfillstyle(SOLID_FILL, WHITE);
   bar(0, 581, 400, 600);
   setfillstyle(SOLID_FILL, LIGHTGRAY);
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
   for(int i = 0; i < 6; i++){
      if(flags[i] == 0){
         menuFunctions[i](); // �������� ������� ���������������� ����, ���� ��� �������
         break;
      }
   }
} // ��������� ������ ��������� ������������ ��������� ��������� ���� � ������� ���

void savemenu(){
   static int state[2] = {0, 0}, f = 0; // ��������� ��������� � ����, ���������� �� ��, ���� �� ���������� ����
   int buttonbounds[2][4] = {{0, 31, 115, 60}, {0, 61, 115, 90}}; // ������� ��� ������
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[0] = 0, f = 1;
      putimage(0, 31, images[10], COPY_PUT), putimage(0, 61, images[12], COPY_PUT); // ��������� ����������� ���������� ����
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      flags[0] = 1, f = 0;
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, 31, 800, 31 + 3 * line_height);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      for(int i = 0; i < 3; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // ���������� �����, ���������� ����
      for(int i = 0; i < 2; i++){
         if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
            switch(i){
               case 0:
                  save(); // ��� ������ "���������"
                  break;
               case 1:
                  saveas(); // ��� ������ "��������� ���"
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

void save(){
   DOC_MODIFIED = true;
   ofstream file("output.txt");
   int last_non_empty_line = max_lines - 1;
   for(int i = max_lines - 1; i >= 0; i--){
      if(!input[i].empty()){
          last_non_empty_line = i;
          break;
      }
   }
   for(int i = 0; i <= last_non_empty_line ; i++){
      if(!input[i].empty()) file << input[i] << endl;
      else file << "" << endl;
   }
   file.close();
} // ��� ������ "���������"

void saveas(){} // ��� ���� "��������� ���"

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
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, 31, 800, 31 + 3 * line_height);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      for(int i = 0; i < 3; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // ���������� �����, ���������� ����
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
   static int xf = 261; // ���������� ��������� �������
   int buttonbounds[2][4] = {{555, 81, 600, 105}, {210, 145, 330, 165}}; // ������� ��� ������
   int xm, ym; // ���������� ���������� ����� ������� ���
   cout << f_err << endl;
   if(f == 0){
      flags[3] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 175);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 175), rectangle(255, 115, 582, 135); // ��������� ���� ���� � ���� �����
      line(xf, 117, xf, 132); // ������ �������
      putimage(555, 81, images[14]), putimage(210, 145, images[16]);
      outtextxy(210, 90, "�����"), outtextxy(210, 117, "���:");
   }
   if(kbhit()){ // ���� ������ �������
      int ch = getch(1); //���������� ������� �������
      int pos = (xf - 261) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // ���� ������ ������� "Backspace" � ������� �� ��������� � ������� ����� ������� ������
         str.erase(pos - 1, 1);
         length--, xf -= width, f2 = 1;
      }
      else if(ch == static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL) && pos > 0) xf -= width;
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length) xf += width;
      else if(ValidChar(ch) && length < 29){ // ���� ������ �������� � ����� ������� ������ �� ��������� �����������
         str.insert(pos, 1, ch);
         length++, xf += width, f2 = 1;
      }
      if(f_err && f2){ // ���� ��� ������������ ��������� �� ������ ���������� ��������� ������
         f_err = 0;
         rectangle(255, 115, 582, 135);
         setfillstyle(SOLID_FILL, WHITE);
         bar(360, 140, 550, 170);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
      }
      setfillstyle(SOLID_FILL, WHITE);
      bar(256, 116, 581, 134);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      outtextxy(260, 117, str.c_str()); // ������� ������
      line(xf, 117, xf, 132); // ������ �������
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      if(xm > buttonbounds[0][0] && xm < buttonbounds[0][2] && ym > buttonbounds[0][1] && ym < buttonbounds[0][3]){ // ���� ����� �������
         flags[3] = 1, f = 0;
         str = "", length = 0, xf = 261; // ������� ������ ������, �������� � ����� � ��������� ������� � ������ ������
         setfillstyle(SOLID_FILL, WHITE);
         bar(200, 80, 600, 175);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
         for(int i = 2; i < 7; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // ���������� �����, ���������� ����
      }
      else if(xm >= buttonbounds[1][0] && xm <= buttonbounds[1][2] && ym >= buttonbounds[1][1] && ym <= buttonbounds[1][3]){ // ���� ������ ������ "����� ���"
         if(!str.empty()) findall(str, f, f_err, length, xf); // ����� ���� ��������� ��������� str � ��������� � �� �������������
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

void findall(string &str, int &f, int &f_err, int &length, int &xf){
   bool f1 = false; // ����, ���������� �� ��, ������� �� ���� ���� ��������� ��������� � ���������
   int strlength = str.size();
   for(int i = 0; i < max_lines; i++){ // �������� �� ���� ������� ���������
      size_t pos = 0;
      while((pos = input[i].find(str, pos)) != string::npos){
         if(f1 == false){
            f1 = true;
            setfillstyle(SOLID_FILL, WHITE);
            bar(200, 80, 600, 175);
            setfillstyle(SOLID_FILL, LIGHTGRAY);
            for(int j = 2; j < 7; j++) outtextxy(width, indent + j * line_height, input[j].c_str()); // ���������� �����, ���������� ����
            setbkcolor(LIGHTGRAY); // ������ ��� ������
         }
         outtextxy((pos + 1)*width, indent + i * line_height, str.c_str()); // ������� ��������� ���������
         pos += strlength;
      }
   }
   if(f1){
      setbkcolor(WHITE); // ������ ��� ������
      while(!ismouseclick(WM_LBUTTONDOWN)){ // ��� ������� ���
         if(kbhit()) getch(); // ���� ������ ������� - ��������� �
         continue;
         delay(5); // �������� ���������� �����
      }
      flags[3] = 1, f = 0;
      str = "", length = 0, xf = 261; // ������� ������ ������, �������� � ����� � ��������� ������� � ������ ������
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, 31, 800, 579);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      for(int i = 0; i < max_lines; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // ���������� �����
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
   static int xf = 261; // ���������� ��������� �������
   int buttonbounds[5][4] = {{555, 81, 600, 105}, {210, 175, 330, 194}, {340, 175, 460, 194}, {255, 115, 582, 135}, {255, 145, 582, 165}}; // ������� ��� ������ � ����� �����
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[4] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 205);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 205), rectangle(255, 115, 582, 135), rectangle(255, 145, 582, 165); // ��������� ���� ���� � ����� �����
      putimage(555, 81, images[14]), putimage(210, 175, images[18]), putimage(340, 175, images[20]);
      outtextxy(210, 90, "������"), outtextxy(210, 117, "���:"), outtextxy(210, 147, "���:");
      line(xf, 117, xf, 132); // ������ �������
   }
   if(kbhit()){ // ���� ������ �������
      int ch = getch(1); //���������� ������� �������
      int pos = (xf - 261) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // ���� ������ ������� "Backspace" � ������� �� ��������� � ������� ����� ������� ������
         selectedstring.erase(pos - 1, 1);
         length--, xf -= width, f2 = 1;
      }
      else if(ch == static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL) && pos > 0) xf -= width;
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length) xf += width;
      else if(ValidChar(ch) && length < 29){ // ���� ������ �������� � ����� ������� ������ �� ��������� �����������
         selectedstring.insert(pos, 1, ch);
         length++, xf += width, f2 = 1;
      }
      if(f_err && f2){
         f_err = 0;
         rectangle(255, 145, 582, 165), rectangle(255, 115, 582, 135);
      }
      setfillstyle(SOLID_FILL, WHITE);
      bar(256, fstr ? 146 : 116, 581, fstr ? 164 : 134);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      outtextxy(260, fstr ? 147 : 117, selectedstring.c_str()); // ������� ������
      line(xf, fstr ? 147 : 117, xf, fstr ? 162 : 132); // ������ �������
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // ���������, ���� �� ����������� ������� �� ���
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // ���������� ���������� ����� ������� ���
      if(xm > buttonbounds[0][0] && xm < buttonbounds[0][2] && ym > buttonbounds[0][1] && ym < buttonbounds[0][3]){ // ���� ����� �������
         flags[4] = 1, f = 0, fstr = 0;
         str1 = "", str2 = "", length = 0, xf = 261; // ������� ������, �������� �� ����� � ��������� ������� � ������ ������ ������
         setfillstyle(SOLID_FILL, WHITE);
         bar(200, 80, 600, 205);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
         for(int i = 2; i < 8; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // ���������� �����, ���������� ����
      }
      else if(xm > buttonbounds[3][0] && xm < buttonbounds[3][2] && ym > buttonbounds[3][1] && ym < buttonbounds[3][3]){ // ���� ������ � ������ ���� "���:"
         fstr = 0;
         setcolor(WHITE), line(xf, 147, xf, 162), setcolor(BLACK); // ������� �������
         outtextxy(260, 117, str1.c_str()); // ������� ������
         length = str1.length(), xf = 261 + length * width;
         line(xf, 117, xf, 132); // ���������� �������
      }
      else if(xm > buttonbounds[4][0] && xm < buttonbounds[4][2] && ym > buttonbounds[4][1] && ym < buttonbounds[4][3]){ // ���� ������ � ������ ���� "���:"
         fstr = 1;
         setcolor(WHITE), line(xf, 117, xf, 132), setcolor(BLACK); // ������� �������
         outtextxy(260, 147, str2.c_str()); // ������� ������
         length = str2.length(), xf = 261 + length*width;
         line(xf, 147, xf, 162); // ���������� �������
      }
      else{
         for(int i = 1; i < 3; i++){
            if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
               if(!str1.empty()){ // ���� ������ ������ �� �����
                  switch(i){
                     case 1: // ���� ������ ������ "�������� ���"
                        replaceall(str1, str2, f, fstr, f_err, length, xf); // ���������� ���� �������� � �� ������
                        break;
                     case 2: // ���� ������ ������ "��������"
                        replace(str1, str2, f, fstr, f_err, length, xf); // ���������� ��������� � � ������
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

void replace(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int &xf){
   bool f1 = false; // ����, ���������� �� ��, ������� �� ���� ���� ��������� ��������� � ���������
   int strlength1 = str1.size(), strlength2 = str2.size();
   for(int i = 0; i < max_lines; i++){
      size_t pos = 0;
      while((pos = input[i].find(str1, pos)) != string::npos){
         if(f1 == false && input[i].size() + ((strlength1 > strlength2) ? (strlength1 - strlength2) : (strlength2 - strlength1)) <= max_length){
            f1 = true;
            input[i].erase(pos, strlength1), input[i].insert(pos, str2); // ��������
            xk = (input[i].length() + 1) * width, yk = indent + i * line_height, numline = i; // ���������� ������� � ����� ������, � ������� ��������� ������
            setfillstyle(SOLID_FILL, WHITE);
            bar(200, 80, 600, 205);
            bar(0, indent + i * line_height, 800, indent + (i + 1) * line_height);
            setfillstyle(SOLID_FILL, LIGHTGRAY);
            for(int j = 2; j < 8 && j != i; j++) outtextxy(width, indent + j * line_height, input[j].c_str()); // ���������� �����, ���������� ����
            outtextxy(width, indent + i * line_height, input[i].c_str()); // ������� ��������� ������
            break;
         }
         pos += strlength1;
      }
      if(f1) break;
   }
   if(f1){
      flags[4] = 1, f = 0, fstr = 0;
      str1 = "", str2 = "", length = 0, xf = 261; // ������� ������, �������� �� ����� � ��������� ������� � ������ ������ ������
   }
   else{
      replacemenu();
      f_err = 1;
      setcolor(COLOR(255, 0, 0));
      rectangle(255, 145, 582, 165), rectangle(255, 115, 582, 135);
      setcolor(BLACK);
   }
} // ���������� ��������� � � ������

void replaceall(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int &xf){
   bool f1 = false; // ����, ���������� �� ��, ���� �� ����������� ���� ���� ������
   int strlength1 = str1.size(), strlength2 = str2.size();
   int last_replaced_string = 0;
   for(int i = 0; i < max_lines; i++){
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
      str1 = "", str2 = "", length = 0, xf = 261; // ������� ������, �������� �� ����� � ��������� ������� � ������ ������ ������
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, 31, 800, 579);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      xk = (input[last_replaced_string].length() + 1) * width, yk = indent + last_replaced_string * line_height, numline = last_replaced_string; // ���������� ������� � ����� ������, � ������� ��������� ��������� ������
      for(int i = 0; i < max_lines; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // ���������� �����
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
   flags[2] = 0;
   clearviewport();
   while(flags[2] == 0){
      outtextxy(300, 200, "�����");
      if(getch()){ // ���������, ���� �� ����������� ������� ������� � ����������
         flags[2] = 1;
         clearviewport();
         setcolor(BLACK); // ������������� ����
         line(0, 30, 800, 30); line(0, 580, 800, 580); // ��������� �����
         putimage(0, 0, images[0], COPY_PUT), putimage(56, 0, images[2], COPY_PUT), putimage(136, 0, images[4], COPY_PUT); // ������ �������� ������
         for(int i = 0; i < max_lines; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // ���������� �����, ���������� ����
         drawtext(); // ����� ��������� ���������� ������ �� �����,  ���������� ��������
      }
      delay(5); // �������� ���������� �����
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
   int buttonbounds[3][4] = {{635, 181, 680, 205}, {270, 240, 390, 260}, {410, 240, 530, 260}}; // ������� ��� ������
   int xm, ym; // ���������� ���������� ����� ������� ���
   if(f == 0){
      flags[5] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(120, 180, 680, 275);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      setcolor(BLACK);
      rectangle(120, 180, 680, 275); // ��������� ���� ����
      putimage(635, 181, images[14]), putimage(270, 240, images[22]), putimage(410, 240, images[24]);
      outtextxy(130, 210, "�� ������ ��������� ��������� � ����� output.txt?"); // ������� ���������
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
         setfillstyle(SOLID_FILL, WHITE);
         bar(120, 180, 680, 275);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
         for(int i = 7; i < 12; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // ���������� �����, ���������� ����
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
   for(int i = 0; i < 6; i++){
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
            setcolor(WHITE); line(xk, yk, xk, yk + heightk); // ������� �������
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
      else if(kbhit() && flags[3] && flags[4] && flags[5]) getch(); // ���� ������� ������ � �����-���� �� ����(� ������� ���������� ���������� ������� ������) - ��������� �
      delay(5); // �������� ���������� �����
   }
} // ����� �� ����� �������� � ���������� � ����������� ��������� �������