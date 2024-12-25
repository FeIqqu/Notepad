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

//===============Глобальные переменные===============
const int max_lines = 27; // Константа, задающая максимальное количество строк
const int max_length = 71; // Константа, задающая максимальное количество символов в строке
const int line_height = 20; // Константа, задающая высоту каждой строки
const int width = 11; // Константа, задающая ширину одного символа и отступ строки от левого края окна 
const int indent = 38; // Константа, задающая отступ первой строки документа от верхнего края окна
int SUM = 0; // Количество символов в документе
int numline = 0; // Номер строки
int linelength = 0; // длина строки
int top = 0, bottom = 27; // Индексы верхней и нижней строки, отображающихся на экране
int x, y; // Координаты указателя мыши
int xk = width, yk = indent, heightk = 15; // Координаты текстовой каретки, её высота
int POS = 1; // Позиция текстовой каретки в массиве строк
bool DOC_MODIFIED = true; // Флаг, отвечающий за то, был ли и изменён файл
HWND hwnd; // Дескриптор активного окна
string DOC_NAME = "Без имени";
string DIRECTORY = "";
bool flags[7] = {true, true, true, true, true, true, true}; // Флаги открытия меню и подменю
IMAGE *images[26]; // Массив изображений кнопок
vector<string> input(27, ""); // Вектор строк, хранящий весь введённый текст
//================================================

//=================Прототипы функций================
void Initializewindow(); // Инициализация окна
void keyboard_thread(); // Поток по вводу символов с клавиатуры
void HandleInput(int ch); // Обработка введенной клавиши, с последующим выводом изменённой строки и количества символов на экран
void EnterInput(); // Если нажата клавиша "Enter"
void BackspaceInput(); // Если нажата клавиша "Backspace"
void LeftInput(); // Если нажата стрелка влево
void RightInput(); // Если нажата стрелка вправо
void UpInput(); // Если нажата стрелка вверх
void DownInput(); // Если нажата стрелка вниз
void DefaultInput(int ch); // Если нажата другая клавиша
void drawtext(); // Вывод последней измененной строки и количества символов на экран
void mouse_thread(); // Поток по определению положения указателя мыши
void buttons(); // Обработка кнопок программы относительно положения указателя мыши и нажатия ЛКМ
void savemenu(); // Меню "Сохранить"
int save(); // Функция сохранения документа
void saveasmenu(); // Подменю "Сохранить как"
void changemenu(); // Меню "Изменить"
void findmenu(); // Подменю "Найти"
void findall(string &str, int &f, int &f_err, int &length, int &left, int &xf); // Поиск всех вхождений подстроки str в документе и их подсвечивание
void replacemenu(); // Подменю "Замена"
void replace(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int (&left)[2], int &xf); // Нахождение подстроки и её замена
void replaceall(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int (&left)[2], int &xf); // Нахождение всех подстрок и их замена
void aboutmenu(); // Меню "О программе"
void exit(); // Если нажат крестик
void exitmenu(); // Меню, вызывающееся при закрытии окна пользователем, если файл не сохранён
bool AllFlagsTrue(); // Функция для проверки того, все ли флаги в массиве flags принимают значение true
bool ValidChar(int ch); // Функция для проверки того, удовлетворяет ли символ вводу
//================================================

void (*menuFunctions[7])() = {savemenu, changemenu, aboutmenu, findmenu, replacemenu, exitmenu, saveasmenu}; // Инициализация массива указателей на функции

int main(){
   Initializewindow(); // Инициализация окна
   thread t1(mouse_thread); // Создаем первый поток и передаем в него функцию mouse_thread
   thread t2(keyboard_thread); // Создаем второй поток и передаем в него функцию keyboard_thread
   t1.join(); // Ожидаем завершения первого потока
   t2.join(); // Ожидаем завершения второго потока
   return 0;
}

void Initializewindow(){
   initwindow(800, 600, "Без имени – Блокнот", 100, 100, false, false); // Инициализация окна
   hwnd = GetForegroundWindow(); // Получение дескриптора активного окна
   for(int i = input.size(); i > 1; i--){
      input.erase(input.begin() + i); // Удаляем строки
   }
   setfillstyle(SOLID_FILL, LIGHTGRAY, WHITE); // Устанавливаем цвет заполнения и цвет фона
   clearviewport(); // Задание фона
   setcolor(BLACK); // Задание цвета линий
   line(0, 30, 800, 30), line(0, 580, 800, 580); // Рисование линий
   settextstyle(COMPLEX_FONT, 0, 1); // Устанавливаем шрифт
   outtextxy(10, 581, "Количество: 0, "), outtextxy(10 + textwidth("Количество: 0, "), 581, "строка: 1");
   for(int i = 0; i < 26; i++){ // Ввод изображений кнопок в массив
      char filename[25];
      sprintf(filename, ".\\buttons\\button%d.bmp", i + 1);
      images[i] = loadBMP(filename);
   }
   putimage(0, 0, images[0], COPY_PUT), putimage(56, 0, images[2], COPY_PUT), putimage(136, 0, images[4], COPY_PUT); // Вставляем изображения кнопок
} // Инициализация окна

void HandleInput(int ch){
   POS = xk / width - 1; // Вычисляем позицию текстовой каретки в массиве строк
   linelength = input[numline].length();
   switch(ch){
      case KEY_ENTER: // Если нажата клавиша "Enter"
         EnterInput();
         break;
      case KEY_BACKSPACE: // Если нажата клавиша "Backspace"
         BackspaceInput();
         break;
      case static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL): // Если нажата стрелка влево
         LeftInput();
         break;
      case static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL): // Если нажата стрелка вправо
         RightInput();
         break;
      case static_cast<int>(KEY_UP) + static_cast<int>(KEY_SPECIAL): // Если нажата стрелка вверх
         UpInput();
         break;
      case static_cast<int>(KEY_DOWN) + static_cast<int>(KEY_SPECIAL): // Если нажата стрелка вниз
         DownInput();
         break;
      case WINDOW_CLOSE: // Если нажат крестик
         exit();
         break;
      default: // Если нажата другая клавиша
         DefaultInput(ch);
         break;
   }
   if(DOC_MODIFIED == false){ // Если файл был изменён
      string doc_name = DOC_NAME  + "* – Блокнот";
      SetWindowText(hwnd, doc_name.c_str()); // Установка нового имени окна
   }
   if(flags[5]) drawtext(); // Вывод последней измененной строки на экран,  количества символов(Если закрыто меню выхода)
} // Обработка введенной клавиши, с последующим выводом изменённой строки и количества символов на экран

void EnterInput(){
   DOC_MODIFIED = false;
   numline++; // Переходим на следующую строку
   xk = width;
   if(numline == bottom){
      top++, bottom++;
   }
   else{
      yk += line_height; // Обновляем значения координат текстовой каретки, чтобы перевести ее на новую строку
   }
   input.insert(input.begin() + numline, ""); // Вставляем новую строку
   input[numline] = input[numline - 1].substr(POS, linelength); // Переносим все символы предыдущей строки, находящиеся справа от каретки, на новую нижнюю строку
   input[numline - 1] = input[numline - 1].erase(POS, linelength); // Удаляем эти символы из верхней строки
} // Если нажата клавиша "Enter"

void BackspaceInput(){
   if(POS > 0){ // Если каретка не находится в крайней левой позиции строки
      DOC_MODIFIED = false;
      SUM--;
      input[numline].erase(POS - 1, 1); // Удаляем символ из строки
      xk -= width; // Перемещаем текстовую каретку
   }
   else if(numline > 0){
      DOC_MODIFIED = false;
      if(numline != top){
         numline--; // Переходим на предыдущую строку
         linelength = input[numline].length(); // Получаем длину предыдущей строки
         input[numline] = input[numline] + input[numline + 1].substr(0,  max_length - linelength); // Добавляем к верхней строке часть, которая может в ней уместиться от нижней строки
         input.erase(input.begin() + numline + 1); // Удаляем строку
         xk = width + linelength * width, yk -= line_height; // Перемещаем текстовую каретку
      }
      else{
         numline--; // Переходим на предыдущую строку
         top--, bottom--;
         linelength = input[numline].length(); // Получаем длину предыдущей строки
         input[numline] = input[numline] + input[numline + 1].substr(0,  max_length - linelength); // Добавляем к верхней строке часть, которая может в ней уместиться от нижней строки
         input.erase(input.begin() + numline + 1); // Удаляем строку
         xk = width + linelength * width; // Перемещаем текстовую каретку
      }
   }
} // Если нажата клавиша "Backspace"

void LeftInput(){
   if(POS > 0){ // Если текущая позиция каретки не является самой левой в строке
      xk -= width; // Перемещаем текстовую каретку
   }
   else if(numline > 0){ // Если текущая позиция каретки является самой левой в строке и она не первая
      numline--; // Переходим на предыдущую строку
      linelength = input[numline].length(); // Получаем длину предыдущей строки
      xk = width + linelength * width; // Перемещаем текстовую каретку
      if(numline + 1 != top){
         yk -= line_height; // Перемещаем текстовую каретку
      }
      else{
         top--, bottom--;
      }
   }
} // Если нажата стрелка влево

void RightInput(){
   if(POS < linelength){ // Если текущая позиция каретки не является самой правой в строке
      xk += width; // Перемещаем текстовую каретку
   }
   else if(numline < static_cast<int>(input.size()) - 1){ // Если текущая позиция каретки является самой правой в строке и она не последняя
      numline++; // Переходим на следующую строку
      xk = width; // Перемещаем текстовую каретку
      if(numline != bottom){
         yk += line_height; // Перемещаем текстовую каретку
      }
      else{
         top++, bottom++;
      }
   }
} // Если нажата стрелка вправо

void UpInput(){
   if(numline > 0){
      int length_top_string = input[numline - 1].length(); // Вычисляем длину верхней строки
      if(numline != top){ // Если каретка не находится на самой верхней отображаемой строке
         if(POS <= length_top_string){ // Если на верхней строке есть место для перемещения каретки
            numline--;
            yk -= line_height; // Перемещаем текстовую каретку
         }
         else if(linelength >= length_top_string){ // Если на верхней строке нет места для перемещения каретки
            numline--;
            xk = width + length_top_string * width, yk -= line_height; // Перемещаем каретку в конец верхней строки
         }
      }
      else{ // Если каретка находится на самой верхней отображаемой строке
         top--, bottom--;
         if(POS <= length_top_string){ // Если на верхней строке есть место для перемещения каретки
            numline--;
         }
         else if(linelength >= length_top_string){ // Если на верхней строке нет места для перемещения каретки
            numline--;
            xk = width + length_top_string * width; // Перемещаем каретку в конец верхней строки
         }
      }
   }
} // Если нажата стрелка вверх

void DownInput(){
   if(numline < static_cast<int>(input.size() - 1)){
      int length_bottom_string = input[numline + 1].length(); // Вычисляем длину нижней строки
      if(numline != bottom - 1){ // Если каретка не находится на самой нижней отображаемой строке
         if(POS <= length_bottom_string){ // Если на нижней строке есть место для перемещения каретки
            numline++; // Переходим на следующую строку
            yk += line_height; // Перемещаем текстовую каретку
         }
         else if(linelength >= length_bottom_string){ // Если на нижней строке нет места для перемещения каретки
            numline++;
            xk = width + length_bottom_string * width, yk += line_height; // Перемещаем каретку в конец нижней строки
         }
      }
      else{ // Если каретка находится на самой нижней отображаемой строке
         top++, bottom++;
         if(POS <= length_bottom_string){ // Если на нижней строке есть место для перемещения каретки
            numline++;
         }
         else if(linelength >= length_bottom_string){ // Если на нижней строке нет места для перемещения каретки
            numline++;
            xk = width + length_bottom_string * width; // Перемещаем каретку в конец нижней строки
         }
      }
   }
} // Если нажата стрелка вниз

void DefaultInput(int ch){
   if(ValidChar(ch)){
      if(linelength < max_length){ // Ограничение на длину строки
         DOC_MODIFIED = false;
         SUM++; // Увеличиваем значение количества символов в документе
         input[numline].insert(POS, 1, ch); // Вставляем символ в строку на позицию каретки
         xk += width; // Перемещаем текстовую каретку
      }
      else if(linelength == POS){
         DOC_MODIFIED = false;
         SUM++; // Увеличиваем значение количества символов в документе
         numline++; // Переходим на следующую строку
         xk = width * 2; // Перемещаем текстовую каретку
         if(numline == bottom){
            top++, bottom++;
         }
         else{
            yk += line_height; // Обновляем значения координат текстовой каретки, чтобы перевести ее на новую строку
         }
         input.insert(input.begin() + numline, ""); // Вставляем новую строку
         input[numline].push_back(ch); // Добавляем символ в конец новой строки
      }
   }
} // Если нажата другая клавиша

void drawtext(){
   char str1[20]; sprintf(str1, "Количество: %d, ", SUM); // Записываем "Количество: SUM, " как строку
   char str2[15]; sprintf(str2, "строка: %d", numline + 1); // Записываем  "строка: numline + 1" как строку
   setcolor(BLACK); // Цвет текста
   setfillstyle(SOLID_FILL, WHITE);
   bar(0, 31, 800, 579), bar(0, 581, 400, 600);
   setfillstyle(SOLID_FILL, LIGHTGRAY);
   int j = 0;
   for(int i = top; i < bottom; i++){
      if(!input[i].empty()) outtextxy(width, indent + j * line_height, input[i].c_str()); // Выводим весь видимый текст
      j++;
   }
   outtextxy(width, 581, str1), outtextxy(width + textwidth(str1), 581, str2); // Выводим количество символов и номер текущей строки
} // Вывод последней измененной строки и количества символов на экран

void buttons(){
   static int state[3] = {0, 0, 0}; // Начальные состояния
   int buttonbounds[3][4] = {{0, 55, 0, 30}, {56, 135, 0, 30}, {136, 240, 0, 30}}; // Границы кнопок
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(AllFlagsTrue()){ // Если не открыто какое-либо из меню и подменю
      if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
         getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
         for(int i = 0; i < 3; i++){ // Проверка: была ли нажата ЛКМ по кнопке, если - да, то вызываем соответствующую кнопку
            if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][1] && ym >= buttonbounds[i][2] && ym <= buttonbounds[i][3]){
               switch(i){ // Вызываем нужную функцию для данной кнопки
                     case 0:
                        savemenu(); // Вызываем функцию для первой кнопки
                        break;
                     case 1:
                        changemenu(); // Вызываем функцию для второй кнопки
                        break;
                     case 2:
                        aboutmenu(); // Вызываем функцию для третьей кнопки
                        break;
               }
            }
         }
         if(ym > 30 && ym < 580){ // Если нажата в области текста
            setcolor(WHITE); // Цвет фона
            line(xk, yk, xk, yk + heightk); // Стираем каретку
            setcolor(BLACK); // Возвращаем цвет каретки
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
            line(xk, yk, xk, yk + heightk); // Рисуем каретку
            setfillstyle(SOLID_FILL, WHITE), bar(0, 581, 400, 600), setfillstyle(SOLID_FILL, LIGHTGRAY);
            char str1[20]; sprintf(str1, "Количество: %d, ", SUM); // Записываем "Количество: SUM, " как строку
            char str2[15]; sprintf(str2, "строка: %d", numline + 1); // Записываем  "строка: numline + 1" как строку
            outtextxy(width, 581, str1), outtextxy(width + textwidth(str1), 581, str2); // Выводим количество символов и номер текущей строки
         }
      }
      for(int i = 0; i < 3; i++){
         if(x >= buttonbounds[i][0] && x <= buttonbounds[i][1] && y >= buttonbounds[i][2] && y <= buttonbounds[i][3] && state[i] == 0){
            state[i] = 1; // Меняем состояние нажатия кнопки
            putimage(buttonbounds[i][0], 0, images[i*2+1], COPY_PUT); // Вставляем изображение кнопки
         }
         else if(!(x >= buttonbounds[i][0] && x <= buttonbounds[i][1] && y >= buttonbounds[i][2] && y <= buttonbounds[i][3]) && state[i] == 1){
            state[i] = 0; // Меняем состояние нажатия кнопки
            putimage(buttonbounds[i][0], 0, images[i*2], COPY_PUT); // Вставляем изображение кнопки
         }
      }
   }
   for(int i = 0; i < 7; i++){
      if(flags[i] == 0){
         menuFunctions[i](); // Вызываем функцию соответствующего меню, если оно открыто
         break;
      }
   }
} // Обработка кнопок программы относительно положения указателя мыши и нажатия ЛКМ

void savemenu(){
   static int state[2] = {0, 0}, f = 0; // Начальные состояния и флаг, отвечающий за то, было ли отрисовано меню
   static int fs = 0; // Флаг, отвечающий за то, чтобы при первом использовании кнопки "Сохранить" вызывалось меню "Сохранить как"
   int buttonbounds[2][4] = {{0, 31, 115, 60}, {0, 61, 115, 90}}; // Границы под кнопок
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[0] = 0, f = 1;
      putimage(0, 31, images[10], COPY_PUT), putimage(0, 61, images[12], COPY_PUT); // Вставляем изображения подпунктов меню
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      flags[0] = 1, f = 0;
      drawtext();
      for(int i = 0; i < 2; i++){
         if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
            switch(i){
               case 0:
                  if(fs) save(); // Под кнопка "Сохранить" 
                  else fs = 1, saveasmenu(); // Меню "Сохранить как"
                  break;
               case 1:
                  fs = 1, saveasmenu(); // Под кнопка "Сохранить как"
                  break;
            }
         }
      }
   }
   for(int i = 0; i < 2; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // Если мы только что зашли в зону под кнопки
            putimage(0, buttonbounds[i][1], images[i*2 + 11], COPY_PUT); // Заменяем изображение на другое
            state[i] = 1; // Установим флаг, что курсор мыши находится в зоне под кнопки
         }
      }
      else if(state[i]){ // Если мы только что вышли курсором из зоны под кнопки
         putimage(0, buttonbounds[i][1], images[i*2 + 10], COPY_PUT);  // Восстанавливаем исходное изображение
         state[i] = 0; // Сбрасываем флаг, что курсор мыши находится в зоне под кнопки
      }
   }
} // Меню "Сохранить"

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
   string doc_name = DOC_NAME  + " – Блокнот";
   SetWindowText(hwnd, doc_name.c_str()); // Установка нового имени окна
   return 1;
} // Функция сохранения документа

void saveasmenu(){
   static int state[3] = {0, 0, 0}, f = 0, f_err = 0; // Начальные состояния и флаги, отвечающие за то, было ли отрисовано меню
   static int fstr = 0; // Флаг, отвечающий за то, в каком поле происходит запись
   static string str1, str2; // Строки полей меню "Сохранить как"
   string& selectedstring = fstr ? str2 : str1;
   static int length = 0;
   static int left[2] = {0, 0}; // Индексы отображаемой строки
   static int xf = 271; // Координата текстовой каретки
   int buttonbounds[4][4] = {{555, 81, 600, 105}, {210, 175, 330, 194}, {265, 115, 582, 135}, {265, 145, 582, 165}}; // Границы под кнопок и полей ввода
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[6] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 205);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 205), setcolor(COLOR(2, 224, 202)), rectangle(265, 115, 582, 135), setcolor(BLACK), rectangle(265, 145, 582, 165); // Отрисовка окна меню и полей ввода
      putimage(555, 81, images[14]), putimage(210, 175, images[22]);
      outtextxy(210, 90, "Сохранить как"), outtextxy(207, 117, "Имя:"), outtextxy(207, 147, "Путь:");
      line(xf, 117, xf, 132); // Рисуем каретку
   }
   if(kbhit()){ // Если нажата клавиша
      int ch = getch(1); //Запоминаем нажатую клавишу
      int pos = left[fstr] + (xf - 271) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // Если нажата клавиша "Backspace" и каретка не находится в крайней левой позиции строки
         if(pos == left[fstr]){ // Если каретка находится в крайней левой позиции отображаемой строки, а сама строка не пуста
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
         if(pos == left[fstr]) left[fstr]--; // Если каретка находится в крайней левой позиции отображаемой строки, а сама строка не пуста левее
         else xf -= width;
      }
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length){
         if(pos == left[fstr] + 28) left[fstr]++; // Если каретка находится в крайней правой позиции отображаемой строки, а сама строка не пуста правее
         else xf += width;
      }
      else if(ValidChar(ch)){ // Если символ подходит
         if(selectedstring.substr(left[fstr], 28).length() < 28){ // Если длина текущей строки не превышает ограничение
            selectedstring.insert(pos, 1, ch);
            xf += width;
         }
         else{ // Если длина текущей строки превышает ограничение
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
      outtextxy(270, fstr ? 147 : 117, selectedstring.substr(left[fstr], 28).c_str()); // Выводим строку
      line(xf, fstr ? 147 : 117, xf, fstr ? 162 : 132); // Рисуем каретку
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      if(xm >= buttonbounds[0][0] && xm <= buttonbounds[0][2] && ym >= buttonbounds[0][1] && ym <= buttonbounds[0][3]){ // Если нажат крестик
         flags[6] = 1, f = 0, fstr = 0;
         str1 = "", str2 = "", left[0] = 0, left[1] = 0, length = 0, xf = 271; // Очищаем строки, обнуляем их длину и переносим каретку в начало первой строки
         drawtext();
      }
      else if(xm >= buttonbounds[2][0] && xm <= buttonbounds[2][2] && ym >= buttonbounds[2][1] && ym <= buttonbounds[2][3]){ // Если нажата в районе поля "Имя:"
         fstr = 0;
         setcolor(COLOR(2, 224, 202)), rectangle(265, 115, 582, 135), setcolor(BLACK), rectangle(265, 145, 582, 165);
         setfillstyle(SOLID_FILL, WHITE), bar(331, 166, 599, 204), setfillstyle(SOLID_FILL, LIGHTGRAY);
         setcolor(WHITE), line(xf, 147, xf, 162), line(xf, 117, xf, 132), setcolor(BLACK); // Стираем каретку
         outtextxy(270, 117, str1.substr(left[fstr], 28).c_str()); // Выводим строку
         length = str1.length();
         xm -= (xm - 271) % width;
         if(xm < 271 + static_cast<int>(str1.substr(left[fstr], 28).length()) * width) xf = xm;
         else xf = 271 + static_cast<int>(str1.substr(left[fstr], 28).length()) * width;
         line(xf, 117, xf, 132); // Перемещаем каретку
      }
      else if(xm >= buttonbounds[3][0] && xm <= buttonbounds[3][2] && ym >= buttonbounds[3][1] && ym <= buttonbounds[3][3]){ // Если нажата в районе поля "Путь:"
         fstr = 1;
         setcolor(COLOR(2, 224, 202)), rectangle(265, 145, 582, 165), setcolor(BLACK), rectangle(265, 115, 582, 135);
         setfillstyle(SOLID_FILL, WHITE), bar(331, 166, 599, 204), setfillstyle(SOLID_FILL, LIGHTGRAY);
         setcolor(WHITE), line(xf, 117, xf, 132), line(xf, 147, xf, 162), setcolor(BLACK); // Стираем каретку
         outtextxy(270, 147, str2.substr(left[fstr], 28).c_str()); // Выводим строку
         length = str2.length();
         xm -= (xm - 271) % width;
         if(xm < 271 + static_cast<int>(str2.substr(left[fstr], 28).length()) * width) xf = xm;
         else xf = 271 + static_cast<int>(str2.substr(left[fstr], 28).length()) * width;
         line(xf, 147, xf, 162); // Перемещаем каретку
      }
      else{
         if(xm >= buttonbounds[1][0] && xm <= buttonbounds[1][2] && ym >= buttonbounds[1][1] && ym <= buttonbounds[1][3]){
            if(!str1.empty()){ // Если строка с названием файла не пуста
               DOC_NAME = str1, DIRECTORY = str2;
               int i = save();
               if(i == 1){
                  flags[6] = 1, f = 0, fstr = 0;
                  str1 = "", str2 = "", left[0] = 0, left[1] = 0, length = 0, xf = 271; // Очищаем строки, обнуляем их длину и переносим каретку в начало первой строки
                  drawtext();
               }
               else{
                  f_err = 1;
                  setcolor(COLOR(255, 0, 0));
                  if(i == 3) outtextxy(360, 175, "Некорректный путь");
                  else outtextxy(360, 175, "Ошибка сохранения");
                  DOC_NAME = "Без имени", DIRECTORY = "";
                  setcolor(BLACK);
               }
            }
            else{
               f_err = 1;
               setcolor(COLOR(255, 0, 0)), outtextxy(360, 175, "Введите имя файла"), rectangle(265, 115, 582, 135), setcolor(BLACK);
            }
         }
      }
   }
   for(int i = 0; i < 2; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // Если мы только что зашли в зону под кнопки
            if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[15], COPY_PUT); // Заменяем изображение на другое
            else putimage(buttonbounds[i][0], 175, images[(i - 1)*2 + 23], COPY_PUT); // Заменяем изображение на другое
            state[i] = 1; // Установим флаг, что курсор мыши находится в зоне под кнопки
         }
      }
      else if(state[i]){ // Если мы только что вышли курсором из зоны под кнопки
         if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[14], COPY_PUT);  // Восстанавливаем исходное изображение
         else putimage(buttonbounds[i][0], 175, images[(i - 1)*2 + 22], COPY_PUT);  // Восстанавливаем исходное изображение
         state[i] = 0; // Сбрасываем флаг, что курсор мыши находится в зоне под кнопки
      }
   }
} // Меню "Сохранить как"

void changemenu(){
   static int state[2] = {0, 0}, f = 0; // Начальные состояния и флаг, отвечающий за то, было ли отрисовано меню
   int buttonbounds[2][4] = {{56, 31, 155, 60}, {56, 61, 155, 90}}; // Границы под кнопок
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[1] = 0, f = 1;
      putimage(56, 31, images[6], COPY_PUT), putimage(56, 61, images[8], COPY_PUT); // Вставляем изображения подпунктов меню
   }
   if(ismouseclick(WM_LBUTTONDOWN)){
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      flags[1] = 1, f = 0;
      drawtext();
      for(int i = 0; i < 2; i++){
         if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
            switch(i){
               case 0:
                  findmenu(); // Подменю "Найти"
                  break;
               case 1:
                  replacemenu(); // Подменю "Замена"
                  break;
            }
         }
      }
   }
   for(int i = 0; i < 2; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // Если мы только что зашли в зону под кнопки
            putimage(56, buttonbounds[i][1], images[i*2 + 7], COPY_PUT); // Заменяем изображение на другое
            state[i] = 1; // Установим флаг, что курсор мыши находится в зоне под кнопки
         }
      }
      else if(state[i]){ // Если мы только что вышли курсором из зоны под кнопки
         putimage(56, buttonbounds[i][1], images[i*2 + 6], COPY_PUT);  // Восстанавливаем исходное изображение
         state[i] = 0; // Сбрасываем флаг, что курсор мыши находится в зоне под кнопки
      }
   }
} // Меню "Изменить"

void findmenu(){
   static int state[2] = {0, 0}, f = 0, f_err = 0; // Начальные состояния и флаги, отвечающие за то, было ли отрисовано меню
   static string str; // Строка поля меню "Найти"
   static int length = 0;
   static int left = 0; // Индекс отображаемой строки
   static int xf = 261; // Координата текстовой каретки
   int buttonbounds[3][4] = {{555, 81, 600, 105}, {210, 145, 330, 165}, {255, 115, 582, 135}}; // Границы под кнопок
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[3] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 175);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 175), setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK); // Отрисовка окна меню и поля ввода
      line(xf, 117, xf, 132); // Рисуем каретку
      putimage(555, 81, images[14]), putimage(210, 145, images[16]);
      outtextxy(210, 90, "Найти"), outtextxy(210, 117, "Что:");
   }
   if(kbhit()){ // Если нажата клавиша
      int ch = getch(1); //Запоминаем нажатую клавишу
      int pos = left + (xf - 261) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // Если нажата клавиша "Backspace" и каретка не находится в крайней левой позиции строки
         if(pos == left){ // Если каретка находится в крайней левой позиции отображаемой строки, а сама строка не пуста
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
         if(pos == left) left--; // Если каретка находится в крайней левой позиции отображаемой строки, а сама строка не пуста левее
         else xf -= width;
      }
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length){
         if(pos == left + 29) left++; // Если каретка находится в крайней правой позиции отображаемой строки, а сама строка не пуста правее
         else xf += width;
      }
      else if(ValidChar(ch)){ // Если символ подходит
         if(str.substr(left, 29).length() < 29){ // Если длина текущей строки не превышает ограничение
            str.insert(pos, 1, ch);
            xf += width;
         }
         else{ // Если длина текущей строки превышает ограничение
            str.insert(pos, 1, ch);
            left++;
         }
         length++, f2 = 1;
      }
      if(f_err && f2){ // Если при отрисованном сообщении об ошибке происходит изменение строки
         f_err = 0;
         setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK);
         setfillstyle(SOLID_FILL, WHITE), bar(360, 140, 550, 170), setfillstyle(SOLID_FILL, LIGHTGRAY);
      }
      setfillstyle(SOLID_FILL, WHITE);
      bar(256, 116, 581, 134);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      outtextxy(260, 117, str.substr(left, 29).c_str()); // Выводим строку
      line(xf, 117, xf, 132); // Рисуем каретку
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      if(xm >= buttonbounds[0][0] && xm <= buttonbounds[0][2] && ym >= buttonbounds[0][1] && ym <= buttonbounds[0][3]){ // Если нажат крестик
         flags[3] = 1, f = 0;
         str = "", left = 0, length = 0, xf = 261; // Очищаем строку поиска, обнуляем её длину и переносим каретку в начало строки
         drawtext();
      }
      else if(xm >= buttonbounds[2][0] && xm <= buttonbounds[2][2] && ym >= buttonbounds[2][1] && ym <= buttonbounds[2][3]){ // Если нажата в районе поля ввода
         setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK);
         setfillstyle(SOLID_FILL, WHITE), bar(360, 140, 550, 170), setfillstyle(SOLID_FILL, LIGHTGRAY);
         setcolor(WHITE), line(xf, 117, xf, 132), setcolor(BLACK); // Стираем каретку
         xm -= (xm - 261) % width;
         if(xm < 261 + static_cast<int>(str.substr(left, 29).length()) * width) xf = xm;
         else xf = 261 + static_cast<int>(str.substr(left, 29).length()) * width;
         line(xf, 117, xf, 132); // Рисуем каретку
      }
      else if(xm >= buttonbounds[1][0] && xm <= buttonbounds[1][2] && ym >= buttonbounds[1][1] && ym <= buttonbounds[1][3]){ // Если нажата кнопка "Найти все"
         if(!str.empty()) findall(str, f, f_err, length, left, xf); // Поиск всех вхождений подстроки str в документе и их подсвечивание
      }
   }
   for(int i = 0; i < 2; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // Если мы только что зашли в зону под кнопки
            if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[15], COPY_PUT); // Заменяем изображение на другое
            else putimage(buttonbounds[i][0], 145, images[(i - 1)*2 + 17], COPY_PUT); // Заменяем изображение на другое
            state[i] = 1; // Установим флаг, что курсор мыши находится в зоне под кнопки
         }
      }
      else if(state[i]){ // Если мы только что вышли курсором из зоны под кнопки
         if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[14], COPY_PUT);  // Восстанавливаем исходное изображение
         else putimage(buttonbounds[i][0], 145, images[(i - 1)*2 + 16], COPY_PUT);  // Восстанавливаем исходное изображение
         state[i] = 0; // Сбрасываем флаг, что курсор мыши находится в зоне под кнопки
      }
   }
} // Подменю "Найти"

void findall(string &str, int &f, int &f_err, int &length, int &left, int &xf){
   bool f1 = false; // Флаг, отвечающий за то, найдено ли хоть одно вхождение подстроки в документе
   int strlength = str.size();
   int index = 0;
   for(int i = top; i < bottom; i++){ // Проходим по всем видимым строкам документа
      size_t pos = 0;
      while((pos = input[i].find(str, pos)) != string::npos){
         if(f1 == false){
            f1 = true;
            drawtext();
            setbkcolor(LIGHTGRAY); // Меняем фон текста
         }
         outtextxy((pos + 1) * width, indent + index * line_height, str.c_str()); // Выводим найденную подстроку
         pos += strlength;
      }
      index++;
   }
   if(f1){
      setbkcolor(WHITE); // Меняем фон текста
      while(!ismouseclick(WM_LBUTTONDOWN)){ // Ждём нажатия ЛКМ
         if(kbhit()) getch(); // Если нажата клавиша - считываем её
         continue;
         delay(5); // Задержка выполнения цикла
      }
      flags[3] = 1, f = 0;
      str = "", length = 0, left = 0, xf = 261; // Очищаем строку поиска, обнуляем её длину и переносим каретку в начало строки
      drawtext();
   }
   else{
      findmenu();
      f_err = 1;
      setcolor(COLOR(255, 0, 0)), rectangle(255, 115, 582, 135);
      outtextxy(360, 145, "Ничего не найдено");
      setcolor(BLACK);
   }
} // Поиск всех вхождений подстроки str в документе и их подсвечивание

void replacemenu(){
   static int state[3] = {0, 0, 0}, f = 0, f_err = 0; // Начальные состояния и флаги, отвечающие за то, было ли отрисовано меню
   static int fstr = 0; // Флаг, отвечающий за то, в каком поле происходит запись
   static string str1, str2; // Строки полей меню "Замена"
   string& selectedstring = fstr ? str2 : str1;
   static int length = 0;
   static int left[2] = {0, 0}; // Индексы отображаемой строки
   static int xf = 261; // Координата текстовой каретки
   int buttonbounds[5][4] = {{555, 81, 600, 105}, {210, 175, 330, 194}, {340, 175, 460, 194}, {255, 115, 582, 135}, {255, 145, 582, 165}}; // Границы под кнопок и полей ввода
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[4] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 205);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 205), setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK), rectangle(255, 145, 582, 165); // Отрисовка окна меню и полей ввода
      putimage(555, 81, images[14]), putimage(210, 175, images[18]), putimage(340, 175, images[20]);
      outtextxy(210, 90, "Замена"), outtextxy(210, 117, "Что:"), outtextxy(210, 147, "Чем:");
      line(xf, 117, xf, 132); // Рисуем каретку
   }
   if(kbhit()){ // Если нажата клавиша
      int ch = getch(1); //Запоминаем нажатую клавишу
      int pos = left[fstr] + (xf - 261) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // Если нажата клавиша "Backspace" и каретка не находится в крайней левой позиции строки
         if(pos == left[fstr]){ // Если каретка находится в крайней левой позиции отображаемой строки, а сама строка не пуста
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
         if(pos == left[fstr]) left[fstr]--; // Если каретка находится в крайней левой позиции отображаемой строки, а сама строка не пуста левее
         else xf -= width;
      }
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length){
         if(pos == left[fstr] + 29) left[fstr]++; // Если каретка находится в крайней правой позиции отображаемой строки, а сама строка не пуста правее
         else xf += width;
      }
      else if(ValidChar(ch)){ // Если символ подходит
         if(selectedstring.substr(left[fstr], 29).length() < 29){ // Если длина текущей строки не превышает ограничение
            selectedstring.insert(pos, 1, ch);
            xf += width;
         }
         else{ // Если длина текущей строки превышает ограничение
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
      outtextxy(260, fstr ? 147 : 117, selectedstring.substr(left[fstr], 29).c_str()); // Выводим строку
      line(xf, fstr ? 147 : 117, xf, fstr ? 162 : 132); // Рисуем каретку
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      if(xm >= buttonbounds[0][0] && xm <= buttonbounds[0][2] && ym >= buttonbounds[0][1] && ym <= buttonbounds[0][3]){ // Если нажат крестик
         flags[4] = 1, f = 0, fstr = 0;
         str1 = "", str2 = "", length = 0, left[0] = 0, left[1] = 0, xf = 261; // Очищаем строки, обнуляем их длину и переносим каретку в начало первой строки
         drawtext();
      }
      else if(xm >= buttonbounds[3][0] && xm <= buttonbounds[3][2] && ym >= buttonbounds[3][1] && ym <= buttonbounds[3][3]){ // Если нажата в районе поля "Что:"
         fstr = 0;
         setcolor(COLOR(2, 224, 202)), rectangle(255, 115, 582, 135), setcolor(BLACK), rectangle(255, 145, 582, 165);
         setcolor(WHITE), line(xf, 147, xf, 162), line(xf, 117, xf, 132), setcolor(BLACK); // Стираем каретку
         outtextxy(260, 117, str1.substr(left[fstr], 29).c_str()); // Выводим строку
         length = str1.length(); 
         xm -= (xm - 261) % width;
         if(xm < 261 + static_cast<int>(str1.substr(left[fstr], 29).length()) * width) xf = xm;
         else xf = 261 + static_cast<int>(str1.substr(left[fstr], 29).length()) * width;
         line(xf, 117, xf, 132); // Перемещаем каретку
      }
      else if(xm >= buttonbounds[4][0] && xm <= buttonbounds[4][2] && ym >= buttonbounds[4][1] && ym <= buttonbounds[4][3]){ // Если нажата в районе поля "Чем:"
         fstr = 1;
         setcolor(COLOR(2, 224, 202)), rectangle(255, 145, 582, 165), setcolor(BLACK), rectangle(255, 115, 582, 135);
         setcolor(WHITE), line(xf, 117, xf, 132), line(xf, 147, xf, 162), setcolor(BLACK); // Стираем каретку
         outtextxy(260, 147, str2.substr(left[fstr], 29).c_str()); // Выводим строку
         length = str2.length(); 
         xm -= (xm - 261) % width;
         if(xm < 261 + static_cast<int>(str2.substr(left[fstr], 29).length()) * width) xf = xm;
         else xf = 261 + static_cast<int>(str2.substr(left[fstr], 29).length()) * width;
         line(xf, 147, xf, 162); // Перемещаем каретку
      }
      else{
         for(int i = 1; i < 3; i++){
            if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
               if(!str1.empty()){ // Если первая строка не пуста
                  switch(i){
                     case 1: // Если нажата кнопка "Заменить все"
                        replaceall(str1, str2, f, fstr, f_err, length, left, xf); // Нахождение всех подстрок и их замена
                        break;
                     case 2: // Если нажата кнопка "Заменить"
                        replace(str1, str2, f, fstr, f_err, length, left, xf); // Нахождение подстроки и её замена
                        break;
                  
                  }
               }
            }
         }
      }
   }
   for(int i = 0; i < 3; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // Если мы только что зашли в зону под кнопки
            if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[15], COPY_PUT); // Заменяем изображение на другое
            else putimage(buttonbounds[i][0], 175, images[(i - 1)*2 + 19], COPY_PUT); // Заменяем изображение на другое
            state[i] = 1; // Установим флаг, что курсор мыши находится в зоне под кнопки
         }
      }
      else if(state[i]){ // Если мы только что вышли курсором из зоны под кнопки
         if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[14], COPY_PUT);  // Восстанавливаем исходное изображение
         else putimage(buttonbounds[i][0], 175, images[(i - 1)*2 + 18], COPY_PUT);  // Восстанавливаем исходное изображение
         state[i] = 0; // Сбрасываем флаг, что курсор мыши находится в зоне под кнопки
      }
   }
} // Подменю "Замена"

void replace(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int (&left)[2], int &xf){
   bool f1 = false; // Флаг, отвечающий за то, найдено ли хоть одно вхождение подстроки в документе
   int strlength1 = str1.size(), strlength2 = str2.size();
   int docsize = input.size();
   for(int i = 0; i < docsize; i++){ // Проходим по всем строкам документа
      size_t pos = 0;
      while((pos = input[i].find(str1, pos)) != string::npos){
         if(f1 == false && input[i].size() + ((strlength1 > strlength2) ? (strlength1 - strlength2) : (strlength2 - strlength1)) <= max_length){
            f1 = true;
            input[i].erase(pos, strlength1), input[i].insert(pos, str2); // Заменяем
            xk = (input[i].length() + 1) * width, yk = indent + i * line_height, numline = i; // Перемещаем каретку в конец строки, в которой произошла замена
            drawtext();
            break;
         }
         pos += strlength1;
      }
      if(f1) break;
   }
   if(f1){
      flags[4] = 1, f = 0, fstr = 0;
      str1 = "", str2 = "", length = 0, left[0] = 0, left[1] = 0, xf = 261; // Очищаем строки, обнуляем их длину и переносим каретку в начало первой строки
   }
   else{
      replacemenu();
      f_err = 1;
      setcolor(COLOR(255, 0, 0));
      rectangle(255, 145, 582, 165), rectangle(255, 115, 582, 135);
      setcolor(BLACK);
   }
} // Нахождение подстроки и её замена

void replaceall(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int (&left)[2], int &xf){
   bool f1 = false; // Флаг, отвечающий за то, было ли произведено хоть одна замена
   int strlength1 = str1.size(), strlength2 = str2.size();
   int last_replaced_string = 0;
   int docsize = input.size();
   for(int i = 0; i < docsize; i++){ // Проходим по всем строкам документа
      size_t pos = 0;
      while((pos = input[i].find(str1, pos)) != string::npos){
         if(input[i].size() + ((strlength1 > strlength2) ? (strlength1 - strlength2) : (strlength2 - strlength1)) <= max_length){
            if(f1 == false) f1 = true;
            input[i].erase(pos, strlength1), input[i].insert(pos, str2); // Заменяем
         }
         pos += strlength1, last_replaced_string = i;
      }
   }
   if(f1){
      flags[4] = 1, f = 0, fstr = 0;
      str1 = "", str2 = "", length = 0, left[0] = 0, left[1] = 0, xf = 261; // Очищаем строки, обнуляем их длину и переносим каретку в начало первой строки
      drawtext();
      xk = (input[last_replaced_string].length() + 1) * width, yk = indent + last_replaced_string * line_height, numline = last_replaced_string; // Перемещаем каретку в конец строки, в которой произошла последняя замена
   }
   else{
      replacemenu();
      f_err = 1;
      setcolor(COLOR(255, 0, 0));
      rectangle(255, 145, 582, 165), rectangle(255, 115, 582, 135);
      setcolor(BLACK);
   }
} // Нахождение всех подстрок и их замена

void aboutmenu(){
   static int state = 0, f = 0; // Начальные состояния и флаг, отвечающий за то, было ли отрисовано меню
   int buttonbounds[4] = {605, 81, 650, 105};
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[2] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(150, 80, 650, 240);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      rectangle(150, 80, 650, 240); // Отрисовка окна меню
      outtextxy(160, 90, "Текстовый редактор <Блокнот>");
      outtextxy(160, 130, "Разработал: Дорофеев Александр Сергеевич");
      outtextxy(160, 150, "Группа: ЕТ-112");
      outtextxy(160, 170, "Дисциплина: Методы программирования");
      outtextxy(160, 190, "Преподаватели: Сартасова М.Ю.");
      outtextxy(325, 210, "Демидов А.К.");
      putimage(136, 0, images[4], COPY_PUT), putimage(605, 81, images[14]);
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      if(xm >= buttonbounds[0] && xm <= buttonbounds[2] && ym >= buttonbounds[1] && ym <= buttonbounds[3]){ // Если нажат крестик
         flags[2] = 1, f = 0;
         drawtext();
      }
   }
   if(x >= buttonbounds[0] && x <= buttonbounds[2] && y >= buttonbounds[1] && y <= buttonbounds[3] && state == 0){
      state = 1; // Меняем состояние нажатия кнопки
      putimage(buttonbounds[0], buttonbounds[1], images[15], COPY_PUT); // Заменяем изображение на другое
   }
   else if(!(x >= buttonbounds[0] && x <= buttonbounds[2] && y >= buttonbounds[1] && y <= buttonbounds[3]) && state == 1){
      state = 0; // Меняем состояние нажатия кнопки
      putimage(buttonbounds[0], buttonbounds[1], images[14], COPY_PUT);  // Восстанавливаем исходное изображение
   }
} // Меню "О программе"

void exit(){
   if(DOC_MODIFIED == false){ // Если файл не сохранен и меню выхода закрыто
      exitmenu();
   }
   else{ // Если файл сохранен
      closegraph(); // Закрываем окно
      exit(0);
   }
} // Меню, вызывающееся при закрытии окна пользователем, если файл не сохранён

void exitmenu(){
   static int state[3] = {0, 0, 0}, f = 0; // Начальные состояния и флаг, отвечающий за то, было ли отрисовано меню
   int buttonbounds[3][4] = {{635, 171, 680, 205}, {270, 240, 390, 260}, {410, 240, 530, 260}}; // Границы под кнопок
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(kbhit()) getch(); // Если нажата клавиша с клавиатуры  - считываем её
   if(f == 0){
      flags[5] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(120, 170, 680, 275);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      setcolor(BLACK);
      rectangle(120, 170, 680, 275); // Отрисовка окна меню
      putimage(635, 171, images[14]), putimage(270, 240, images[22]), putimage(410, 240, images[24]);
      string FILEPATH = DOC_NAME + ".txt?";
      if(FILEPATH.length() < 55){
         outtextxy(193, 190, "Вы хотите сохранить изменения в файле"); // выводим сообщение
         outtextxy(400 - textwidth(FILEPATH.c_str()) / 2, 210, FILEPATH.c_str()); // выводим сообщение
      }
      else outtextxy(193, 197, "Вы хотите сохранить изменения в файле?"); // выводим сообщение
      setcolor(WHITE);
   }
   if(kbhit() && getch() == WINDOW_CLOSE){ // Если открыто меню закрытия и происходит нажатие крестика
      save();
      closegraph(); // Закрываем окно
      exit(0);
   }
   if(ismouseclick(WM_LBUTTONDOWN)){
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      if(xm > buttonbounds[0][0] && xm < buttonbounds[0][2] && ym > buttonbounds[0][1] && ym < buttonbounds[0][3]){ // Если нажат крестик
         flags[5] = 1, f = 0;
         drawtext();
      }
      else{
         for(int i = 1; i < 3; i++){
            if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
               switch(i - 1){
                  case 0: // Если нажата кнопка "Сохранить"
                     save();
                     closegraph(); // Закрываем окно
                     exit(0);
                     break;
                  case 1: // Если нажата кнопка "Не сохранять"
                     closegraph(); // Закрываем окно
                     exit(0);
                     break;
               }
            }
         }
      }
   }
   for(int i = 0; i < 3; i++){
      if(x >= buttonbounds[i][0] && x <= buttonbounds[i][2] && y >= buttonbounds[i][1] && y <= buttonbounds[i][3]){
         if(!state[i]){ // Если мы только что зашли в зону под кнопки
            if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[15], COPY_PUT); // Заменяем изображение на другое
            else putimage(buttonbounds[i][0], 240, images[(i - 1)*2 + 23], COPY_PUT); // Заменяем изображение на другое
            state[i] = 1; // Установим флаг, что курсор мыши находится в зоне под кнопки
         }
      }
      else if(state[i]){ // Если мы только что вышли курсором из зоны под кнопки
         if(i == 0) putimage(buttonbounds[i][0], buttonbounds[i][1], images[14], COPY_PUT);  // Восстанавливаем исходное изображение
         else putimage(buttonbounds[i][0], 240, images[(i - 1)*2 + 22], COPY_PUT);  // Восстанавливаем исходное изображение
         state[i] = 0; // Сбрасываем флаг, что курсор мыши находится в зоне под кнопки
      }
   }
} // Меню, вызывающееся при закрытии окна пользователем, если файл не сохранён

bool AllFlagsTrue(){
   for(int i = 0; i < 7; i++){
      if(flags[i] == false) return false;
   }
   return true;
} // Функция для проверки того, все ли флаги в массиве flags принимают значение true

bool ValidChar(int ch){
   if(ch >= ' ' && ch <= 255) return true;
   return false;
} // Функция для проверки того, удовлетворяет ли символ вводу

void mouse_thread(){
   while(1){
      x = mousex(), y = mousey(); // Определение координат указателя мыши
      buttons(); // Обработка кнопок программы относительно положения указателя мыши и нажатия ЛКМ
      delay(5); // Задержка выполнения цикла
   }
} // Поток по определению положения указателя мыши

void keyboard_thread(){
   clock_t start_time = clock(); // Запоминаем время начала цикла
   while(1){
      if(AllFlagsTrue()){ // Если не открыто какое-либо из меню и подменю
         if(kbhit()){ // Если нажата клавиша и не открыто какое-либо из меню
            int ch = getch(1); //Запоминаем нажатую клавишу
            setcolor(WHITE), line(xk, yk, xk, yk + heightk); // Стираем каретку
            HandleInput(ch); // Обработка введенной клавиши, с последующим выводом изменённой строки и количества символов на экран
            line(xk, yk, xk, yk + heightk); // Рисуем каретку
            start_time = clock(); // Обновляем время начала цикла
         }
         else if((clock() - start_time) > 500){ // Если прошло более 500 миллисекунд с момента последнего нажатия клавиши
            line(xk, yk, xk, yk + heightk); // Рисуем каретку
            delay(250); // Задержка, чтобы каретка мигала
            setcolor(WHITE); // Цвет фона
            line(xk, yk, xk, yk + heightk); // Стираем каретку
            setcolor(BLACK); // Возвращаем цвет каретки
            delay(250); // Задержка, чтобы каретка мигала
            start_time = clock(); // Обновляем время начала цикла
         }
      }
      else if(kbhit() && flags[3] && flags[4] && flags[5] && flags[6]) getch(); // Если клавиша нажата в каком-либо из меню(в которых происходит считывание нажатия клавиш) - считываем её
      delay(5); // Задержка выполнения цикла
   }
} // Поток по вводу символов с клавиатуры и отображению текстовой каретки