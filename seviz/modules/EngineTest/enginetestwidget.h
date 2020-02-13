#ifndef ENGINETESTWIDGET_H
#define ENGINETESTWIDGET_H

#include <QWidget>

namespace Ui {
class EngineTestWidget;
}

class EngineTestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EngineTestWidget(QWidget *parent = nullptr);
    ~EngineTestWidget();

private:
    Ui::EngineTestWidget *ui;
};

#endif // ENGINETESTWIDGET_H
