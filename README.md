# signalizaciya
Проект умный дом (охранная система с информированием и управлением по gsm)

# Для сборки понадобится:

1. GSM модуль SIM800L. или SIM900

2. Активная Sim карта с положительным балансом.

3. Литий-ионный аккумулятор 3,7 вольта на 2000-3000 миллиампер.

4. Контроллер заряда аккумуляторов на 3,7 вольта USB-MINI.

5. Пожарный звуковой оповещатель.

6. NPN транзистор подходящий по мощности для пожарного оповещателя.

7. Датчики движения HC-SR501 и концевые выключатели в сумме 5 штук. 

        Рекомендую использовать только концевики или герконовые датчики, 
        так как подобные датчики движения не имеют возможности фильтровать ложные срабатывания, 
        которые могут исходить от GSM модуля или от генератора 
        проезжающего мимо автомобиля!

8. Один маломощный RGB светодиод для индикации.

9. 4 сопротивления: 250, 1К, 2К и 2,4К .

 10. Соединительные провода .

 11. Плата Arduino UNO

<p align="center">
  <img src="http://telegra.ph//file/a3e7ee516a1719314cf44.jpg">
</p>

Перед тем как приступить к сборке прибора, укажи свой номер в скетче, и загрузи этот скетч на плату.


# После загрузки скетча собери все, как показано на этой схеме.

<p align="center">
  <img src="http://telegra.ph//file/050ad8c6c8cc88ecbdda2.jpg">
</p>

# СМС Команды для управления сигнализацией

1. Info = запрос SMS о состоянии сигнализации, и датчиков сигнализации
2. Sms-off = выключить отправку SMS при срабатывании сигнализации
3. Sms-on = включить отправку SMS при срабатывании сигнализации
4. Call-off = выключить исходящий вызов при срабатывании сигнализации
5. Call-on = включить исходящий вызов при срабатывании сигнализации
6. Sig-off = выключить сигнализацию
7. Sig-on = включить сигнализацию
8. Svet-off = выключить реле света
9. Svet-on = включить реле света
 10. Sirena = выключить сирену 
 11. Bal = Проверка баланса
