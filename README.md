# seviz

[![Build status (master)](https://ci.appveyor.com/api/projects/status/yrnfab6uegxch5q6/branch/master?svg=true)](https://ci.appveyor.com/project/eltimen/seviz/branch/master)

# Описание

Программная платформа составления корпуса художественных текстов на английском языке с поддержкой плагинов.

В дистрибутив входят два плагина в качестве примеров: EngineTest и SentenceTree.
SentenceTree предназначен для визуализации и редактирования структуры предложения в виде constituency tree, dependency tree или дерева семантических фреймов (FrameNet).
 
# Сборка

Рекомендуется использовать:
* IDE: `Qt Creator` (или собирать через консоль `qmake`)
* Компилятор: `MSVS 2019`
* ОС: MS Windows 7 и выше
* Внешние библиотеки: Qt 5.13 и выше
 
# Как создать плагин

Плагин представляет собой динамическую библиотеку (dll). При запуске программа сканирует папку `plugins/` внутри рабочей директории и загружает расположенные в ней плагины.
Последовательность действий:
1. Открыть `seviz.pro` в Qt Creator и собрать проект
2. Создать новый shared library подпроект внутри папки `plugins/` (например с названием `MyOwnPlugin`). В появившемся окне поставить галочку `QtWidgets`.
3. Добавить в `seviz.pro` строку `plugins/MyOwnPlugin.depends = seviz_core` (здесь и далее `MyOwnPlugin` заменить на название вашего плагина)
4. Добавить в `MyOwnPlugin.pro` следующие строчки:
	* В начало: 
	```
	include(../../includes.pri)
	Debug:SEVIZ_DESTDIR = ../../core/debug
	Release:SEVIZ_DESTDIR = ../../core/release

	INCLUDEPATH += ../../core/
	LIBS += $$SEVIZ_DESTDIR/seviz.lib
	```

	* В конец:
	```
	copyToDir($$OUT_PWD/$$DESTDIR/$${TARGET}.dll, $$SEVIZ_DESTDIR/plugins/)
	```
	
5. Выполнить сборку проекта (должна завершиться успешно).
6. Сделать класс `MyOwnPlugin` унаследованным от `ISevizPlugin`, реализовать все pure virtual методы (см. примеры плагинов в директории `plugins`). Класс `Feature` представляет кнопку плагина, которая будет расположена на панели инструментов программы.
7. Повторно собрать проект. После запуска на панели инструментов должны появиться кнопки функций вашего плагина.