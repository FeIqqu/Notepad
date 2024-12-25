#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <ctime>
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
int x, y; // Координаты указателя мыши
int xk = 11, yk = indent, heightk = 15; // Координаты текстовой каретки, её высота
int POS = 1; // Позиция текстовой каретки в массиве строк
bool DOC_MODIFIED = true; // Флаг, отвечающий за то, был ли и изменён файл
bool flags[6] = {true, true, true, true, true, true}; // Флаги открытия меню и подменю
IMAGE *images[26]; // Массив изображений кнопок
string input[max_lines]; // Массив строк, хранящий весь введённый текст
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
void save(); // Под кнопка "Сохранить"
void saveas(); // Подменю "Сохранить как"
void changemenu(); // Меню "Изменить"
void findmenu(); // Подменю "Найти"
void findall(string &str, int &f, int &f_err, int &length, int &xf); // Поиск всех вхождений подстроки str в документе и их подсвечивание
void replacemenu(); // Подменю "Замена"
void replace(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int &xf); // Нахождение подстроки и её замена
void replaceall(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int &xf); // Нахождение всех подстрок и их замена
void aboutmenu(); // Меню "О программе"
void exit(); // Если нажат крестик
void exitmenu(); // Меню, вызывающееся при закрытии окна пользователем, если файл не сохранён
bool AllFlagsTrue(); // Функция для проверки того, все ли флаги в массиве flags принимают значение true
bool ValidChar(int ch); // Функция для проверки того, удовлетворяет ли символ вводу
//================================================

void (*menuFunctions[6])() = {savemenu, changemenu, aboutmenu, findmenu, replacemenu, exitmenu}; // Инициализация массива указателей на функции

int main(){
   Initializewindow(); // Инициализация окна
   thread t1(mouse_thread); // Создаем первый поток и передаем в него функцию mouse_thread
   thread t2(keyboard_thread); // Создаем второй поток и передаем в него функцию keyboard_thread
   t1.join(); // Ожидаем завершения первого потока
   t2.join(); // Ожидаем завершения второго потока
   return 0;
}

void Initializewindow(){
   initwindow(800, 600, "Блокнот", 100, 100, false, false); // Инициализация окна
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
   if(flags[5]) drawtext(); // Вывод последней измененной строки на экран,  количества символов(Если закрыто меню выхода)
} // Обработка введенной клавиши, с последующим выводом изменённой строки и количества символов на экран

void EnterInput(){
   if(input[max_lines - 1].empty() && numline < max_lines - 1){ // Если последняя строка пуста и каретка не находится на ней
      DOC_MODIFIED = false;
      numline++; // Переходим на следующую строку и увеличиваем значение максимальной задействованной строки
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, indent + (numline - 1)*line_height, 800, 579);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      setcolor(BLACK); // Цвет текста
      for(int i = max_lines - 1; i > numline; i--){
         input[i] = input[i - 1]; // Переносим все строки ниже текущей строки вниз на одну позицию
         outtextxy(width, indent + i * line_height, input[i].c_str()); // Выводим строку
      }
      input[numline] = input[numline - 1].substr(POS, linelength); // Переносим все символы предыдущей строки, находящиеся справа от каретки, на новую нижнюю строку
      input[numline - 1] = input[numline - 1].erase(POS, linelength); // Удаляем эти символы из верхней строки
      outtextxy(width, indent + (numline - 1) * line_height, input[numline - 1].c_str()); // Выводим верхнюю строку
      xk = width, yk += line_height; // Обновляем значения координат текстовой каретки, чтобы перевести ее на новую строку
   }
} // Если нажата клавиша "Enter"

void BackspaceInput(){
   if(POS > 0){ // Если каретка не находится в крайней левой позиции строки
      DOC_MODIFIED = false;
      SUM--;
      input[numline].erase(POS - 1, 1); // Удаляем символ из строки
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, indent + numline * line_height, 800, indent + (numline + 1) * line_height);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      xk -= width; // Перемещаем текстовую каретку
   }
   else if(numline > 0){ // Если каретка находится в крайней левой позиции в строке и не находится на первой строке
      DOC_MODIFIED = false;
      numline--; // Переходим на предыдущую строку
      linelength = input[numline].length(); // Получаем длину предыдущей строки
      input[numline] = input[numline] + input[numline + 1].substr(0,  max_length - linelength); // Добавляем к верхней строке часть, которая может в ней уместиться от нижней строки
      input[numline + 1].erase(0,  max_length - linelength); // Удаляем эту часть от нижней строки
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, indent + numline*line_height, 800, 579);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      setcolor(BLACK); // Цвет текста
      for(int i = numline + 1; i < max_lines; i++){ // Переносим и выводим все строки ниже текущей строки вниз на одну позицию
         input[i] = input[i + 1];
         outtextxy(width, indent + i * line_height, input[i].c_str());
      }
      xk = width + linelength*width, yk -= line_height; // Перемещаем текстовую каретку
   }
} // Если нажата клавиша "Backspace"

void LeftInput(){
   if(POS > 0){ // Если текущая позиция каретки не является самой левой в строке
      xk -= width; // Перемещаем текстовую каретку
   }
   else if(numline > 0){ // Если текущая позиция каретки является самой левой в строке и она не первая
      numline--; // Переходим на предыдущую строку
      linelength = input[numline].length(); // Получаем длину предыдущей строки
      xk = width + linelength*width, yk -= line_height; // Перемещаем текстовую каретку
   }
} // Если нажата стрелка влево

void RightInput(){
   if(POS < linelength){ // Если текущая позиция каретки не является самой правой в строке
      xk += width; // Перемещаем текстовую каретку
   }
   else if(numline < max_lines - 1){ // Если текущая позиция каретки не является самой правой в строке, строка не последняя и нижняя строка не пуста
      numline++;
      xk = width, yk += line_height; // Перемещаем текстовую каретку
   }
} // Если нажата стрелка вправо

void UpInput(){
   if(numline > 0){ // Если каретка не находится на самой первой строке
      int length_top_string = input[numline - 1].length(); // Вычисляем длину верхней строки
      if(POS <= length_top_string){ // Если на верхней строке есть место для перемещения каретки
         numline--;
         yk -= line_height; // Перемещаем текстовую каретку
      }
      else if(linelength >= length_top_string){ // Если на верхней строке нет места для перемещения каретки
         numline--;
         xk = width + length_top_string*width, yk -= line_height; // Перемещаем каретку в конец верхней строки
      }
   }
} // Если нажата стрелка вверх

void DownInput(){
   if(numline < max_lines - 1){ // Если каретка не находится на самой последней строке
      int length_bottom_string = input[numline + 1].length(); // Вычисляем длину нижней строки
      if(POS <= length_bottom_string){ // Если на нижней строке есть место для перемещения каретки
         numline++; // Переходим на следующую строку
         yk += line_height; // Перемещаем текстовую каретку
      }
      else if(linelength >= length_bottom_string){ // Если на нижней строке нет места для перемещения каретки
         numline++;
         xk = width + length_bottom_string*width, yk += line_height; // Перемещаем каретку в конец нижней строки
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
      else if(input[max_lines - 1].empty() && numline < max_lines - 1 && POS == linelength){ // Если последняя строка пуста и каретка находится в конце строки, но не последней
         DOC_MODIFIED = false;
         SUM++; // Увеличиваем значение количества символов в документе
         numline++; // Переходим на следующую строку
         setfillstyle(SOLID_FILL, WHITE);
         bar(0, indent + numline*line_height, 800, 579);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
         setcolor(BLACK); // Цвет текста
         for(int i = max_lines - 1; i > numline; i--){ // Переносим и выводим все строки ниже текущей строки вниз на одну позицию
            input[i] = input[i - 1];
            outtextxy(width, indent + i * line_height, input[i].c_str());
         }
         input[numline] = ""; // Очищаем текущую строку
         input[numline].push_back(ch); // Добавляем символ в конец новой строки
         xk = width * 2, yk += line_height; // Перемещаем текстовую каретку
      }
   }
} // Если нажата другая клавиша

void drawtext(){
   setcolor(BLACK); // Цвет текста
   outtextxy(width, indent + numline * line_height, input[numline].c_str()); // Выводим последнюю изменённую строку
   char str1[20]; sprintf(str1, "Количество: %d, ", SUM); // Записываем "Количество: SUM, " как строку
   char str2[15]; sprintf(str2, "строка: %d", numline + 1); // Записываем  "строка: numline + 1" как строку
   setfillstyle(SOLID_FILL, WHITE);
   bar(0, 581, 400, 600);
   setfillstyle(SOLID_FILL, LIGHTGRAY);
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
   for(int i = 0; i < 6; i++){
      if(flags[i] == 0){
         menuFunctions[i](); // Вызываем функцию соответствующего меню, если оно открыто
         break;
      }
   }
} // Обработка кнопок программы относительно положения указателя мыши и нажатия ЛКМ

void savemenu(){
   static int state[2] = {0, 0}, f = 0; // Начальные состояния и флаг, отвечающий за то, было ли отрисовано меню
   int buttonbounds[2][4] = {{0, 31, 115, 60}, {0, 61, 115, 90}}; // Границы под кнопок
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[0] = 0, f = 1;
      putimage(0, 31, images[10], COPY_PUT), putimage(0, 61, images[12], COPY_PUT); // Вставляем изображения подпунктов меню
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      flags[0] = 1, f = 0;
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, 31, 800, 31 + 3 * line_height);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      for(int i = 0; i < 3; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // Возвращаем текст, перекрытый меню
      for(int i = 0; i < 2; i++){
         if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
            switch(i){
               case 0:
                  save(); // Под кнопка "Сохранить"
                  break;
               case 1:
                  saveas(); // Под кнопка "Сохранить как"
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
} // Под кнопка "Сохранить"

void saveas(){} // Под меню "Сохранить как"

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
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, 31, 800, 31 + 3 * line_height);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      for(int i = 0; i < 3; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // Возвращаем текст, перекрытый меню
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
   static int xf = 261; // Координата текстовой каретки
   int buttonbounds[2][4] = {{555, 81, 600, 105}, {210, 145, 330, 165}}; // Границы под кнопок
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   cout << f_err << endl;
   if(f == 0){
      flags[3] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 175);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 175), rectangle(255, 115, 582, 135); // Отрисовка окна меню и поля ввода
      line(xf, 117, xf, 132); // Рисуем каретку
      putimage(555, 81, images[14]), putimage(210, 145, images[16]);
      outtextxy(210, 90, "Найти"), outtextxy(210, 117, "Что:");
   }
   if(kbhit()){ // Если нажата клавиша
      int ch = getch(1); //Запоминаем нажатую клавишу
      int pos = (xf - 261) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // Если нажата клавиша "Backspace" и каретка не находится в крайней левой позиции строки
         str.erase(pos - 1, 1);
         length--, xf -= width, f2 = 1;
      }
      else if(ch == static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL) && pos > 0) xf -= width;
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length) xf += width;
      else if(ValidChar(ch) && length < 29){ // Если символ подходит и длина текущей строки не превышает ограничение
         str.insert(pos, 1, ch);
         length++, xf += width, f2 = 1;
      }
      if(f_err && f2){ // Если при отрисованном сообщении об ошибке происходит изменение строки
         f_err = 0;
         rectangle(255, 115, 582, 135);
         setfillstyle(SOLID_FILL, WHITE);
         bar(360, 140, 550, 170);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
      }
      setfillstyle(SOLID_FILL, WHITE);
      bar(256, 116, 581, 134);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      outtextxy(260, 117, str.c_str()); // Выводим строку
      line(xf, 117, xf, 132); // Рисуем каретку
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      if(xm > buttonbounds[0][0] && xm < buttonbounds[0][2] && ym > buttonbounds[0][1] && ym < buttonbounds[0][3]){ // Если нажат крестик
         flags[3] = 1, f = 0;
         str = "", length = 0, xf = 261; // Очищаем строку поиска, обнуляем её длину и переносим каретку в начало строки
         setfillstyle(SOLID_FILL, WHITE);
         bar(200, 80, 600, 175);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
         for(int i = 2; i < 7; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // Возвращаем текст, перекрытый меню
      }
      else if(xm >= buttonbounds[1][0] && xm <= buttonbounds[1][2] && ym >= buttonbounds[1][1] && ym <= buttonbounds[1][3]){ // Если нажата кнопка "Найти все"
         if(!str.empty()) findall(str, f, f_err, length, xf); // Поиск всех вхождений подстроки str в документе и их подсвечивание
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

void findall(string &str, int &f, int &f_err, int &length, int &xf){
   bool f1 = false; // Флаг, отвечающий за то, найдено ли хоть одно вхождение подстроки в документе
   int strlength = str.size();
   for(int i = 0; i < max_lines; i++){ // Проходим по всем строкам документа
      size_t pos = 0;
      while((pos = input[i].find(str, pos)) != string::npos){
         if(f1 == false){
            f1 = true;
            setfillstyle(SOLID_FILL, WHITE);
            bar(200, 80, 600, 175);
            setfillstyle(SOLID_FILL, LIGHTGRAY);
            for(int j = 2; j < 7; j++) outtextxy(width, indent + j * line_height, input[j].c_str()); // Возвращаем текст, перекрытый меню
            setbkcolor(LIGHTGRAY); // Меняем фон текста
         }
         outtextxy((pos + 1)*width, indent + i * line_height, str.c_str()); // Выводим найденную подстроку
         pos += strlength;
      }
   }
   if(f1){
      setbkcolor(WHITE); // Меняем фон текста
      while(!ismouseclick(WM_LBUTTONDOWN)){ // Ждём нажатия ЛКМ
         if(kbhit()) getch(); // Если нажата клавиша - считываем её
         continue;
         delay(5); // Задержка выполнения цикла
      }
      flags[3] = 1, f = 0;
      str = "", length = 0, xf = 261; // Очищаем строку поиска, обнуляем её длину и переносим каретку в начало строки
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, 31, 800, 579);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      for(int i = 0; i < max_lines; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // Возвращаем текст
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
   static int xf = 261; // Координата текстовой каретки
   int buttonbounds[5][4] = {{555, 81, 600, 105}, {210, 175, 330, 194}, {340, 175, 460, 194}, {255, 115, 582, 135}, {255, 145, 582, 165}}; // Границы под кнопок и полей ввода
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[4] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(200, 80, 600, 205);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      putimage(56, 0, images[2], COPY_PUT);
      rectangle(200, 80, 600, 205), rectangle(255, 115, 582, 135), rectangle(255, 145, 582, 165); // Отрисовка окна меню и полей ввода
      putimage(555, 81, images[14]), putimage(210, 175, images[18]), putimage(340, 175, images[20]);
      outtextxy(210, 90, "Замена"), outtextxy(210, 117, "Что:"), outtextxy(210, 147, "Чем:");
      line(xf, 117, xf, 132); // Рисуем каретку
   }
   if(kbhit()){ // Если нажата клавиша
      int ch = getch(1); //Запоминаем нажатую клавишу
      int pos = (xf - 261) / width;
      int f2 = 0;
      if(ch == KEY_BACKSPACE && pos > 0){ // Если нажата клавиша "Backspace" и каретка не находится в крайней левой позиции строки
         selectedstring.erase(pos - 1, 1);
         length--, xf -= width, f2 = 1;
      }
      else if(ch == static_cast<int>(KEY_LEFT) + static_cast<int>(KEY_SPECIAL) && pos > 0) xf -= width;
      else if(ch == static_cast<int>(KEY_RIGHT) + static_cast<int>(KEY_SPECIAL) && pos < length) xf += width;
      else if(ValidChar(ch) && length < 29){ // Если символ подходит и длина текущей строки не превышает ограничение
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
      outtextxy(260, fstr ? 147 : 117, selectedstring.c_str()); // Выводим строку
      line(xf, fstr ? 147 : 117, xf, fstr ? 162 : 132); // Рисуем каретку
   }
   if(ismouseclick(WM_LBUTTONDOWN)){ // Проверяем, было ли произведено нажатие на ЛКМ
      getmouseclick(WM_LBUTTONDOWN, xm, ym); // Запоминаем координаты места нажатия ЛКМ
      if(xm > buttonbounds[0][0] && xm < buttonbounds[0][2] && ym > buttonbounds[0][1] && ym < buttonbounds[0][3]){ // Если нажат крестик
         flags[4] = 1, f = 0, fstr = 0;
         str1 = "", str2 = "", length = 0, xf = 261; // Очищаем строки, обнуляем их длину и переносим каретку в начало первой строки
         setfillstyle(SOLID_FILL, WHITE);
         bar(200, 80, 600, 205);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
         for(int i = 2; i < 8; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // Возвращаем текст, перекрытый меню
      }
      else if(xm > buttonbounds[3][0] && xm < buttonbounds[3][2] && ym > buttonbounds[3][1] && ym < buttonbounds[3][3]){ // Если нажата в районе поля "Что:"
         fstr = 0;
         setcolor(WHITE), line(xf, 147, xf, 162), setcolor(BLACK); // Стираем каретку
         outtextxy(260, 117, str1.c_str()); // Выводим строку
         length = str1.length(), xf = 261 + length * width;
         line(xf, 117, xf, 132); // Перемещаем каретку
      }
      else if(xm > buttonbounds[4][0] && xm < buttonbounds[4][2] && ym > buttonbounds[4][1] && ym < buttonbounds[4][3]){ // Если нажата в районе поля "Чем:"
         fstr = 1;
         setcolor(WHITE), line(xf, 117, xf, 132), setcolor(BLACK); // Стираем каретку
         outtextxy(260, 147, str2.c_str()); // Выводим строку
         length = str2.length(), xf = 261 + length*width;
         line(xf, 147, xf, 162); // Перемещаем каретку
      }
      else{
         for(int i = 1; i < 3; i++){
            if(xm >= buttonbounds[i][0] && xm <= buttonbounds[i][2] && ym >= buttonbounds[i][1] && ym <= buttonbounds[i][3]){
               if(!str1.empty()){ // Если первая строка не пуста
                  switch(i){
                     case 1: // Если нажата кнопка "Заменить все"
                        replaceall(str1, str2, f, fstr, f_err, length, xf); // Нахождение всех подстрок и их замена
                        break;
                     case 2: // Если нажата кнопка "Заменить"
                        replace(str1, str2, f, fstr, f_err, length, xf); // Нахождение подстроки и её замена
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

void replace(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int &xf){
   bool f1 = false; // Флаг, отвечающий за то, найдено ли хоть одно вхождение подстроки в документе
   int strlength1 = str1.size(), strlength2 = str2.size();
   for(int i = 0; i < max_lines; i++){
      size_t pos = 0;
      while((pos = input[i].find(str1, pos)) != string::npos){
         if(f1 == false && input[i].size() + ((strlength1 > strlength2) ? (strlength1 - strlength2) : (strlength2 - strlength1)) <= max_length){
            f1 = true;
            input[i].erase(pos, strlength1), input[i].insert(pos, str2); // Заменяем
            xk = (input[i].length() + 1) * width, yk = indent + i * line_height, numline = i; // Перемещаем каретку в конец строки, в которой произошла замена
            setfillstyle(SOLID_FILL, WHITE);
            bar(200, 80, 600, 205);
            bar(0, indent + i * line_height, 800, indent + (i + 1) * line_height);
            setfillstyle(SOLID_FILL, LIGHTGRAY);
            for(int j = 2; j < 8 && j != i; j++) outtextxy(width, indent + j * line_height, input[j].c_str()); // Возвращаем текст, перекрытый меню
            outtextxy(width, indent + i * line_height, input[i].c_str()); // Выводим изменённую строку
            break;
         }
         pos += strlength1;
      }
      if(f1) break;
   }
   if(f1){
      flags[4] = 1, f = 0, fstr = 0;
      str1 = "", str2 = "", length = 0, xf = 261; // Очищаем строки, обнуляем их длину и переносим каретку в начало первой строки
   }
   else{
      replacemenu();
      f_err = 1;
      setcolor(COLOR(255, 0, 0));
      rectangle(255, 145, 582, 165), rectangle(255, 115, 582, 135);
      setcolor(BLACK);
   }
} // Нахождение подстроки и её замена

void replaceall(string &str1, string &str2, int &f, int &fstr, int &f_err, int &length, int &xf){
   bool f1 = false; // Флаг, отвечающий за то, было ли произведено хоть одна замена
   int strlength1 = str1.size(), strlength2 = str2.size();
   int last_replaced_string = 0;
   for(int i = 0; i < max_lines; i++){
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
      str1 = "", str2 = "", length = 0, xf = 261; // Очищаем строки, обнуляем их длину и переносим каретку в начало первой строки
      setfillstyle(SOLID_FILL, WHITE);
      bar(0, 31, 800, 579);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      xk = (input[last_replaced_string].length() + 1) * width, yk = indent + last_replaced_string * line_height, numline = last_replaced_string; // Перемещаем каретку в конец строки, в которой произошла последняя замена
      for(int i = 0; i < max_lines; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // Возвращаем текст
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
   flags[2] = 0;
   clearviewport();
   while(flags[2] == 0){
      outtextxy(300, 200, "Текст");
      if(getch()){ // Проверяем, было ли произведено нажатие клавиши с клавиатуры
         flags[2] = 1;
         clearviewport();
         setcolor(BLACK); // Устанавливаем цвет
         line(0, 30, 800, 30); line(0, 580, 800, 580); // Рисование линий
         putimage(0, 0, images[0], COPY_PUT), putimage(56, 0, images[2], COPY_PUT), putimage(136, 0, images[4], COPY_PUT); // Рисуем картинки кнопок
         for(int i = 0; i < max_lines; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // Возвращаем текст, перекрытый меню
         drawtext(); // Вывод последней измененной строки на экран,  количества символов
      }
      delay(5); // Задержка выполнения цикла
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
   int buttonbounds[3][4] = {{635, 181, 680, 205}, {270, 240, 390, 260}, {410, 240, 530, 260}}; // Границы под кнопок
   int xm, ym; // Запоминаем координаты места нажатия ЛКМ
   if(f == 0){
      flags[5] = 0, f = 1;
      setfillstyle(SOLID_FILL, WHITE);
      bar(120, 180, 680, 275);
      setfillstyle(SOLID_FILL, LIGHTGRAY);
      setcolor(BLACK);
      rectangle(120, 180, 680, 275); // Отрисовка окна меню
      putimage(635, 181, images[14]), putimage(270, 240, images[22]), putimage(410, 240, images[24]);
      outtextxy(130, 210, "Вы хотите сохранить изменения в файле output.txt?"); // выводим сообщение
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
         setfillstyle(SOLID_FILL, WHITE);
         bar(120, 180, 680, 275);
         setfillstyle(SOLID_FILL, LIGHTGRAY);
         for(int i = 7; i < 12; i++) outtextxy(width, indent + i * line_height, input[i].c_str()); // Возвращаем текст, перекрытый меню
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
   for(int i = 0; i < 6; i++){
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
            setcolor(WHITE); line(xk, yk, xk, yk + heightk); // Стираем каретку
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
      else if(kbhit() && flags[3] && flags[4] && flags[5]) getch(); // Если клавиша нажата в каком-либо из меню(в которых происходит считывание нажатия клавиш) - считываем её
      delay(5); // Задержка выполнения цикла
   }
} // Поток по вводу символов с клавиатуры и отображению текстовой каретки