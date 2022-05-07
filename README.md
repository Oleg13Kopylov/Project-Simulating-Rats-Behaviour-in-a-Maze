# Project-Simulating-Rats-Behaviour-in-a-Maze
Весь код в main.cpp

Реализована программная модель на C++, которая воссоздает пищедобывательное поведение грызуна в трёх лабиринтах с помощью дискретных операторов на графе, а также позволяет обрабатывать пути и реальных грызунов, и смоделированных.

Модель использует четыре дискретных оператора: рандома, инверсии, симметрии, кольца (окружности). 

Для определения степени реалистичности модели была введена функция различия, которая зависит от того, насколько сильно различаются средние, медианы и моды длины пути и числá ошибок у реального грызуна и у смоделированного.

Определены оптимальные вероятности применения для каждого из дискретных операторов. Исследование было проведено отдельно для начального, промежуточного и конечного этапов обучения грызуна. Установлено, что поведение смоделированного грызуна близко к поведению реального грызуна на промежуточном и конечном этапах обучения.
Практическая значимость в том, что с помощью программной модели можно ставить виртуальные эксперименты для промежуточного и конечного этапов обучения грызуна в лабиринте – это намного быстрее, удобнее и дешевле.

Произведено сравнение поведения крыс в зависимости от наличия действия кофеина.

Работа выполнена в рамках проектной работы на втором курсе бакалавриата ПМИ ФКН.
