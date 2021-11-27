//Cicada3301
#include <SoftwareSerial.h>
#include <SPI.h>
const int rx_prog = 2;                                          //программный rx для подключения SIM900
const int tx_prog = 3;                                          //программный tx для подключения SIM900
SoftwareSerial SIM800(rx_prog, tx_prog);                        //программные RX, TX для связи с модулем SIM900
const int led_2 = 9;                                            //светодиод для индикации сигнализации красный
const int led_i = 8;                                            //светодиод для индикации сигнализации зеленый
int sounds[] = {262,294,330,349,392,440,494};                   //Мелодия сирены
int BEATTIME = 200;                                             //Иайминг сирены
const int a1 = 15;                                              //линия для подключения датчика двери
const int a2 = 16;                                              //линия для подключения датчика 
const int a3 = 17;                                              //линия для подключения датчика
const int a4 = 18;                                              //линия для подключения датчика
const int a5 = 19;                                              //линия для подключения датчика двери
const int led_4 = 4;                                            //подключение индикации сигнализации
const int speaker = 10;                                         //подключение сирены
String phon = "+380xxxxxxxxx";                                  //номер на который будет выполняться звонок
String phones = "+380xxxxxxxxx, +380xxxxxxxxx, +380xxxxxxxxx";  //несколько номеров администраторов, для управления по СМС
String balance = "*111#";                                       //номер для проверки баланса Sim карты GSM сигнализации
void numbr_sms(){
  SIM800.println("AT+CMGS=\"+380xxxxxxxxx\"");                  //1 номер для получения СМС, вписывается в международном формате начиная с +
  delay(3000);
}
long led_i_time = 30000;
long spiker_time = 40000;                                       //(20 секунд)время работы сирены, после отключения датчиков
long ring_time = 20000;                                         //(20 секунд)пауза между звонками при срабатывании сигнализации
int batery_limit = 50;                                          //значение заряда батареи в процентах при котором отправится СМС о разряженной батареи (оптимально от 30% до 60%)
int ring_x = 3;                                                 //пауза 3 минуты между сериями звонков, при постоянно сработаном концевике (от 1 до 32000 минут)
                                
//Переменые для хранения значений
int val, send_mesag_state_Batery = 0, send_mesag_Balanse = 0, led_2_OFF, led_2_ON, led_3_OFF, led_3_ON;                       
int hhh = 0, hhh2 = 0, stat_a1 = 0, stat_a2  = 0, stat_a3 = 0, stat_a4 = 0, stat_a5 = 0;                           
int ring = 0, ring_st = 1, ring_fl = 0, ring_fl2 = 0, send_mesag = 0, sms_st = 0, blinker = 1;           
String signalingS = "", signaling = "", str_a1 = "", str_a2 = "", str_a3 = "", str_a4 = "", str_a5 = ""; 
String signaling_OK = "Signaling OK";                                                                    
int k=0, v=0, msgphone = 0, state_Batery = 100;                                                          
int sirena = 0;                                                                                          
int sv = 1;                                                                                              

                                                       

unsigned long timing10, timing11, timing12, timing13, timing14, timing30; 
String _response = "";                                   // Переменная для хранения ответа модуля
long lastUpdate = millis();                              // Время последнего обновления
long updatePeriod = 60000;                               // Проверять каждую минуту
String waitResponse() {                                  // Функция ожидания ответа и возврата полученного результата
String _resp = "";                                       // Переменная для хранения результата
long _timeout = millis() + 10000;                        // Переменная для отслеживания тайм аута (10 секунд)
while (!SIM800.available() && millis() < _timeout)  {};  // Ждем ответа 10 секунд, если пришел ответ или наступил тайм аут, то...
if (SIM800.available()) {                                // Если есть, что считывать...
_resp = SIM800.readString();                             // считываем и запоминаем
}else {                                                  // Если пришел тайм аут, то...
Serial.println("Timeout...");                            // оповещаем об этом и...
}return _resp;                                           // возвращаем результат. Пусто, если проблема
}
String sendATCommand(String cmd, bool waiting) {
String _resp = "";                                       // Переменная для хранения результата
Serial.println(cmd);                                     // Дублируем команду в монитор порта
SIM800.println(cmd);                                     // Отправляем команду модулю
if (waiting) {                                           // Если необходимо дождаться ответа...
_resp = waitResponse();                                  // ... ждем, когда будет передан ответ
// Если Echo Mode выключен (ATE0), то эти 3 строки можно за комментировать
if (_resp.startsWith(cmd)) {                             // Убираем из ответа дублирующуюся команду
_resp = _resp.substring(_resp.indexOf("\r", cmd.length()) + 2);}
Serial.println(_resp);                                   // Дублируем ответ в монитор порта
}return _resp;                                           // Возвращаем результат. Пусто, если проблема
}
void parseSMS(String msg) {                              // Парсим SMS
String msgheader  = "";
String msgbody    = "";
String msgphone   = "";
msg = msg.substring(msg.indexOf("+CMGR: "));              //удаляем АТ команду
msgheader = msg.substring(0, msg.indexOf("\r"));          // выдергиваем телефон от начала до перехода на следующую строку
msgbody = msg.substring(msgheader.length() + 2);          //сохраняем в строку msgbody
msgbody = msgbody.substring(0, msgbody.lastIndexOf("OK")); // выдергиваем текст SMS от номера до ОК
msgbody.trim();                                           // удаляем пробелы в начале и в конце
int firstIndex = msgheader.indexOf("\",\"") + 3;
int secondIndex = msgheader.indexOf("\",\"", firstIndex);
msgphone = msgheader.substring(firstIndex, secondIndex);
Serial.println("Phone: " + msgphone);                     // Выводим номер телефона
Serial.println("Message: " + msgbody);                    // Выводим текст SMS
if (msgphone.length() > 6 && phones.indexOf(msgphone) > -1) { // Если телефон в белом списке, то...
//<<<<<<<<<<<<<<<<<<< SMS команды >>>>>>>>>>>>>>>>>>>>>>
if(msgbody == "Info"){ Serial.println("SMS = Info"); delay(300);          k=3;}  //запрос SMS о состоянии сигнализации, и датчиков сигнализации
if(msgbody == "Sms-off"){ Serial.println("SMS = Sms-off"); delay(300);   sms_st = 0;}  //выключить отправку SMS при срабатывании сигнализации
if(msgbody == "Sms-on"){ Serial.println("SMS = Sms-on"); delay(300);   sms_st = 1;}  //включить отправку SMS при срабатывании сигнализации
if(msgbody == "Call-off"){Serial.println("SMS = Call-off");delay(300);   ring_st = 0;} //выключить исходящий вызов при срабатывании сигнализации
if(msgbody == "Call-on"){Serial.println("SMS = Call-on");delay(300);ring_st = 1;ring = 0;} //включить исходящий вызов при срабатывании сигнализации
if(msgbody == "Sig-off"){ Serial.println("SMS = Sig-off"); delay(300);hhh = 0;      }  //выключить сигнализацию
if(msgbody == "Sig-on"){ Serial.println("SMS = Sig-on"); delay(300);hhh = 1;      }  //включить сигнализацию
if(msgbody == "Svet-off"){ Serial.println("SMS = Svet-off"); delay(300); sv = 1;}    //выключить реле света
if(msgbody == "Svet-on"){ Serial.println("SMS = Svet-on"); delay(300); sv = 0;}      //включить реле света
if(msgbody == "Sirena"){ Serial.println("SMS = Sirena"); delay(300); sirena = 1;}    //выключить сирену 
if(msgbody == "Bal"){  Serial.println("SMS = Bal");                              //проверка баланса SIM карты
  SIM800.println("AT+CUSD=1,\"" + balance + "\"");}         //Проверка баланса
}else {
Serial.println("Unknown phonenumber");      //Выводит при получении смс от посторонего номера
}}
void text_sms(){    //Создание смс отчета при срабатывании сигнализации
SIM800.println("Security Cicada");delay(300);
if (state_Batery == 10){state_Batery = 100;}
SIM800.print("Battery status = "); delay(300); SIM800.print(state_Batery); delay(300); SIM800.println("%");delay(300);
SIM800.println(signaling);delay(300);
SIM800.println(signalingS);delay(300);
SIM800.println(str_a1);delay(300);
SIM800.println(str_a2);delay(300);
SIM800.println(str_a3);delay(300);
SIM800.println(str_a4);delay(300);
SIM800.println(str_a5);delay(3000);
SIM800.print("Ring = "); delay(300); SIM800.println(ring_st); delay(300);
SIM800.print("SMS = "); delay(300); SIM800.println(sms_st);
}
void go_sms(){
    delay(300);
    SIM800.print((char)26); //команда для отправки SMS
    delay(300);
}
void setup() {  //>>>>>>>>>>>>>>>>> SETUP <<<<<<<<<<<<<<<<<<<
Serial.begin(9600);                               // Скорость обмена данными с компьютером
SIM800.begin(9600);                               // Скорость обмена данными с модемом

sendATCommand("AT", true);                        // Отправили AT для настройки скорости обмена данными
sendATCommand("AT+CMGDA=\"DEL ALL\"", true);      // Удаляем все SMS, чтобы не забивать память
sendATCommand("AT+CMGF=1;&W", true);              // Включаем текстовый режима SMS (Text mode) и сразу сохраняем значение (AT&W)!
sendATCommand("AT+CSCB=1;&W", true);              // Включаем приём специальных сообщений и сразу сохраняем значение (AT&W)!
lastUpdate = millis();                            // Обнуляем таймер

//Иницилизация пинов 
analogReference(INTERNAL);
pinMode(a1, INPUT_PULLUP);
pinMode(a2, INPUT_PULLUP);
pinMode(a3, INPUT_PULLUP);
pinMode(a4, INPUT_PULLUP);
pinMode(a5, INPUT_PULLUP);
pinMode(11, OUTPUT);
pinMode(speaker, OUTPUT);
pinMode(led_2, OUTPUT);
pinMode(led_i, OUTPUT);
pinMode(5, OUTPUT);

}
bool hasmsg = false;                              // Флаг наличия сообщений к удалению
void loop() { //>>>>>>>>>>>>>>>>>> LOOP <<<<<<<<<<<<<<<<<<
/* Следующие 11 строк после этого комментария служат для отладки и управления по UART.
То есть если в UART отправить 3 то должен прийти SMS отчет, а если отправить 4,
то выполнится проверка баланса сим карты, и так далее.
 */
if (Serial.available() > 0) { 
char val = Serial.read();
if(val == '3'){k = 3;}         //запрос SMS отчета
if(val == '4'){  delay(1000); SIM800.println("AT+CUSD=1,\"" + balance + "\"");}//проверка баланса SIM карты
if(val == '5'){hhh = 1;}       //включить сигнализацию
if(val == '6'){hhh = 0;}       //выключить сигнализацию
if(val == '7'){ring_st = 1; ring = 0;}   //включить исходящий вызов при срабатывании сигнализации
if(val == '8'){ring_st = 0;}   //выключить исходящий вызов при срабатывании сигнализации
if(val == '9'){sms_st = 1;}    //включить отправку SMS при срабатывании сигнализации
if(val == '@'){sms_st = 0;}    //выключить отправку SMS при срабатывании сигнализации
if(val == '*'){sv = 1;} // выключить реле света
if(val == '-'){sv = 0;} // включить реле света
if(val == '$'){sirena = 1;} // выключить сработавшию сирену
}
if (lastUpdate + updatePeriod < millis() ) {                // Пора проверить наличие новых сообщений
do {
_response = sendATCommand("AT+CMGL=\"REC UNREAD\",1", true); // Отправляем запрос чтения непрочитанных сообщений
if (_response.indexOf("+CMGL: ") > -1) {                    // Если есть хоть одно, получаем его индекс
  //Serial.println ("POLUCHENO 1 SMS");
int msgIndex = _response.substring(_response.indexOf("+CMGL: ") + 7, _response.indexOf("\"REC UNREAD\"", _response.indexOf("+CMGL: ")) - 1).toInt();
char i = 0;                                             // Объявляем счетчик попыток
do {
i++;                                                    // Увеличиваем счетчик
_response = sendATCommand("AT+CMGR=" + (String)msgIndex + ",1", true);  // Пробуем получить текст SMS по индексу
_response.trim();                                       // Убираем пробелы в начале/конце
if (_response.endsWith("OK")) {                         // Если ответ заканчивается на "ОК"
  if (!hasmsg) hasmsg = true;                           // Ставим флаг наличия сообщений для удаления
  sendATCommand("AT+CMGR=" + (String)msgIndex, true);   // Делаем сообщение прочитанным
  sendATCommand("\n", true);                            // Перестраховка - вывод новой строки
  parseSMS(_response);                                  // Отправляем текст сообщения на обработку
  break;                                                // Выход из do{}
}else {                                                 // Если сообщение не заканчивается на OK
  Serial.println ("Error answer");                      // Какая-то ошибка
  sendATCommand("\n", true);                            // Отправляем новую строку и повторяем попытку
}} while (i < 10);
break;}
else {
lastUpdate = millis();                                  // Обнуляем таймер
if (hasmsg) {
sendATCommand("AT+CMGDA=\"DEL READ\"", true);           // Удаляем все прочитанные сообщения
hasmsg = false;
}break;}} while (1);}
if (SIM800.available())   {                         // Если модем, что-то отправил...
_response = waitResponse();                         // Получаем ответ от модема для анализа
_response.trim();                                   // Убираем лишние пробелы в начале и конце
  Serial.println(_response);                          // Если нужно выводим в монитор порта
if (_response.indexOf("+CMTI:")>-1) {               // Пришло сообщение об отправке SMS
lastUpdate = millis() -  updatePeriod;              // Теперь нет необходимости обрабатывать SMS здесь, достаточно просто
}
if (_response.indexOf("+CBC:")>-1) {                //Если пришел ответ о состоянии батареи
state_Batery = _response.substring(17, 19).toInt(); //Выдергиваем часть строки с данными о состоянии батареи, переводим в integer
/*Закомментированые строки с функциями Serial.print или Serial.println служат для более удобной отладки по UART.
Чтобы наблюдать все процессы в мониторе порта, необходимо раскомментировать все подобные строки находящиеся во всем void loop()*/
//Serial.print("Batery = "); Serial.println(state_Batery);
//Serial.print("Set = "); Serial.println(v);
}
//команды для проверки баланса
if (_response.indexOf("+CUSD:")>-1) {               //Если пришел ответ о состоянии баланса
  //Serial.println(">>Balanse!<<");
send_mesag_Balanse++;
}}
if (sirena == 1){  // Отключение сработавшей сирены смс командой
  if (millis() - timing12 > spiker_time){timing12 = millis(); noTone(speaker);}}

if (sv == 1){digitalWrite(5, HIGH); } // Включение реле света смс командой
if (sv == 0){digitalWrite(5, LOW); }  // Выключение реле света смс командой
if (k == 3){delay(300);               //Отправка смс отчета по состоянию датчиков по смс команде 
   Serial.println("<<Send SMS (info)>>"); 
numbr_sms(); text_sms(); go_sms();k=0;} 
//опрос датчиков
 stat_a1 = digitalRead(a1);
if (stat_a1 == LOW && signalingS == signaling_OK) { str_a1 = "Door  OK!";} 
if (stat_a1 == HIGH) {str_a1 = "Door OPEN!";}
 stat_a2 = digitalRead(a2);
if (stat_a2 == LOW && signalingS == signaling_OK) { str_a2 = "Windows zala OK!";}
if (stat_a2 == HIGH) {str_a2 = "Windows zala is OPEN!";}
 stat_a3 = digitalRead(a3);
if (stat_a3 == LOW && signalingS == signaling_OK) { str_a3 = "Windows litl room  OK!";}
if (stat_a3 == HIGH) {str_a3 = "Windows litl room OPEN!";}
 stat_a4 = digitalRead(a4);
if (stat_a4 == LOW && signalingS == signaling_OK) { str_a4 = "Gas OK!";}
if (stat_a4 == HIGH) {str_a4 = "Gas WORKED!";}
 stat_a5 = digitalRead(a5);
if (stat_a5 == LOW && signalingS == signaling_OK) { str_a5 = "Water OK!";}
if (stat_a5 == HIGH) {str_a5 = "Water WORKED!";}
//Сигнализация выключена
if(hhh == 0){ 
  send_mesag = 0; ring = 0; ring_fl = 0;
  digitalWrite(11, HIGH);
  signaling = "Signaling OFF";  noTone(speaker);}
//Сигнализация включена 
if(hhh == 1){signaling = "Signaling ON"; 
if (millis() - timing12 > led_i_time){timing12 = millis(); digitalWrite(8, LOW);}}
// если датчик сработает
if (ring == 0 && led_2_ON == 200 && ring_fl == 0) 
{ signalingS = "POPYTKA VZLOMA";  sms_st = 1;
if (ring_st == 1){ring ++; }
if(sms_st == 1){send_mesag++; }
timing10 = millis(); 
  //Serial.println("POPYTKA VZLOMA");
  } else { signalingS = signaling_OK; 
  if (millis() - timing12 > spiker_time){timing12 = millis(); tone(speaker, 700); digitalWrite(6, HIGH);}
   }
// 3 звонка при срабатывании датчиков
  if (ring > 0 && millis() - timing10 > ring_time && ring_st == 1 && ring_fl == 0){ 
      timing10 = millis(); ring++;
     // String ring = String(ring-1);
    //Serial.print("<< Ring " + ring + " >>");
   SIM800.println("ATD" + phon + ";");
  }
if (ring > 3) {ring = 0; send_mesag = 0; ring_fl = 1;} 
//опрос датчиков
if(stat_a1 == LOW && stat_a2 == LOW && stat_a3 == LOW && stat_a4 == LOW && stat_a5 == LOW)
{blinker = 1;}else{blinker = 0;}
//блинк для индикации сигнализации
if(hhh == 0 && blinker == 1){led_2_ON = 1000; led_2_OFF = 1000;}
if(hhh == 1 && blinker == 1){led_2_ON = 3000; led_2_OFF = 100;}
//опрос датчиков
if(stat_a1 == HIGH || stat_a2 == HIGH || stat_a3 == HIGH || stat_a4 == HIGH || stat_a5 == HIGH)
{led_2_ON = 200; led_2_OFF = 100; 
if(hhh == 1){tone(speaker,sounds,BEATTIME);}
}else {
if (millis() - timing12 > spiker_time){timing12 = millis(); tone(speaker, sounds,BEATTIME);}
}
if (millis() - timing13 > led_2_ON){
timing13 = millis();timing14 = millis(); 
digitalWrite(led_2, HIGH);
}
if (millis() - timing14 > led_2_OFF){digitalWrite(led_2, LOW); }
  if (send_mesag == 1 && sms_st == 1){
  // Serial.println("<<<Soobshenie Otpravleno (Pri vzlome)>>>");
numbr_sms(); text_sms(); go_sms(); send_mesag++;
 }
 if (send_mesag > 2) {send_mesag = 2;}
 
 if (send_mesag_Balanse == 1){
   //Serial.println("<<Send SMS (Balanse)>>");
numbr_sms();  SIM800.println(_response); go_sms();
send_mesag_Balanse = 0; delay(1000);
sendATCommand("AT+CMGDA=\"DEL ALL\"", true);               // Удаляем все SMS, чтобы не забивать память
 }
 if (send_mesag_Balanse >= 2) {send_mesag_Balanse = 0;}
//Сообщение о разряженной батареи
if(state_Batery > 11 && state_Batery < batery_limit){send_mesag_state_Batery++;}
 if (send_mesag_state_Batery == 1){
   //Serial.println("<<Send SMS (Batery < 50%)>>");
numbr_sms(); 
SIM800.print("Set = "); SIM800.print(v); SIM800.println(" volt");
delay(300); 
SIM800.print("Battery status = "); SIM800.print(state_Batery); SIM800.println("%");
go_sms();
 }
 if (send_mesag_state_Batery > 2) {send_mesag_state_Batery = 2;}
 if(state_Batery > 80){send_mesag_state_Batery = 0;}
 }
