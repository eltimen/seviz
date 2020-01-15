#ifndef EPUBRENDERER_H
#define EPUBRENDERER_H

#include <QObject>
#include <QWebEngineView>

class EpubRenderer : public QObject
{
    Q_OBJECT
public:
    explicit EpubRenderer(QWebEngineView*);

	// open
	// close
	// взять изменения dom?
	
	// C++ не позволяет делать namespace внутри классов. вложенные классы имеют ограничения.
	// [[deprecated("При использовании возможны конфликты с JS/CSS других плагинов.")]] ?
	// запустить js на странице
	// добавить css на страницу

	// геттеры элементов модели по их позиции
	// зарегистрировать обработчик
	// снять регистрацию обработчика
	// сигнал для сбора данных для показа элемента модели

private:
	QWebEngineView *m_view;
	QWebChannel* m_webchannel;
};

#endif // EPUBRENDERER_H
