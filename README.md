## Введение
Данный проект посвящен написанию собственной виртуальной машины, которая симулирует работу процессора. Всего три программы: ассемблер, дизассемблер и машина.

## Assembler
Ассемблер переводит язык ассемблерный в машинный код, далее будет приведён перечень правил написания на нём:

1. Допускаются пустые строки.

2. Допускаются однострочные комментарии с помощью знака ';', приведу пример (будьте аккуратны с именами, начинающимися на inf и nan!):

```
push rax ; push value from register rax
```
3. Имена меток заканчиваются знаком ':', приведу пример:
```
main: <- correct label name
```
4. Список всех команд:

push и pop:
```
push          <- положить что-то в стек

push 1        <- число 
            
push rax      <- число из регистра

push [1]      <- число из 1о ячейки RAM 

push [rax]    <- число из ячейки с номером числа в rax RAM

push [rax+1]  <- число из ячейки с номером числа в rax + 1 RAM
``` 
```
pop           <- взять число из стека (верхнее)

~ аналогично push'у ~
```
команды без аргумента:
```
in            <- ввод числа

out           <- вывод верхнего числа со стека

hlt           <- остановка программы

add           <- сумма двух верхни чисел

sub           <- разность двух верних чисел

mul           <- умножение двух верхних чисел

div           <- частность двух верхних чисел

sqrt          <- корень числа

sin           <- синус числа

cos           <- косинус числа
 
```
команды перехода (их два типа: условные и безусловные):
```
jmp           <- прыжок на метку 

ja            <- прыжок, елси x > y (условный)

jae           <- прыжок, елси x >= y (условный)

jb            <- прыжок, елси x < y (условный)

jbe           <- прыжок, елси x <= y (условный)

je            <- прыжок, елси x = y (условный)

jne           <- прыжок, елси x != y (условный)

call          <- прыжок, в call стек кладется адрес возврата 

ret           <- прыжок по перхнему адресу call стек 
```
существуют два стека: для чисел и для адресов возврата.

## Disassembler
Переводит байт код в файл с командами и аргументами.

## SPU

Исполняет побайтово код. Байт код неравномерный, сначала процессор считывает байт команды, в этом же байте маскируются информация о том, есть ли в аргументе регистр, обращение к оперативке, значение.
~~~
[]                   | []             | [] [] [] [] [] [] [] []
код комманды и флаги | номер регистра | значение аргумента
([] - 1 байт) 
~~~

---
Также хочу обратить на единую систему команд в ассемблере, машине и дизассемблере. Все команды представлены в едином формате. Заметим, что код в этих трёх программах одинаков (ctrl + c, ctrl + v), поэтому было принято решение использовать кодогенерацию с помощью #include.

Как это выглядит? В файле commands.h:
```
DEF_COMMAND(name, num, argc, code)

name          <- название команды

num           <- номер команды

argc          <- наличие аргумента (0 или 1)

code          <- код команды
```



