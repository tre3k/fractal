# fractal

## Программа для анализа фрактальной размерности объекта по картине дифракции

![Image alt](https://github.com/tre3k/fractal/blob/master/screenshoot1.jpg "screenshoot main window")
![Image alt](https://github.com/tre3k/fractal/blob/master/screenshoot2.jpg "screenshoot average plot")

Формат текстовго входного файла:
 первые две строчки размер по х и у соотв.
 дальше подряд на каждой новой строчке значения матрицы,
 т.е. если размер матрицы 64х64 то в файле будет 4096 + 2 строчек

После открытия сразу делается ФФТ, а т.к. индикацию процесса я не сделал
то при открытии больших матриц, будет ощущение что программа зависла.
На моём компьютере 2048х2048 открывается примерно за 30сек,
при азимутальном усреднении таже история.

При открытии сразу же ищется центр масс фурье образа и ставится автаматически

Размер матрицы лучше сразу делать кратным степени 2, ну т.е. 64, 128, 256 и.т.д.
впринципе я сделал усечение размера, т.е. если вдруг размер 129х133 то она сделается 128х128,
но на сколько она хорошо будет это делать, я не знаю, т.к. не проверял.


generateCover.py - программка на питоне генерирующая ковёр серпинского
запускать в командной строке как: python generateCover.py > fractal.txt
(заместо fractal.txt можно написать любой файл *.txt)
можно редактировать generateCover.py переменные X, Y размер матрицы, N - кол-во иттераций генерации фрактала
