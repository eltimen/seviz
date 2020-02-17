#ifndef ENGINETESTWIDGET_H
#define ENGINETESTWIDGET_H

#include <QWidget>
#include "ModuleManager.h"

namespace Ui {
class EngineTestWidget;
}

class EngineTestWidget : public QWidget
{
    friend class EngineTest;

    Q_OBJECT

public:
    explicit EngineTestWidget(QWidget *parent = nullptr);
    ~EngineTestWidget();

private:
    Ui::EngineTestWidget *ui;
    ModuleManager* m_engine = nullptr;
};

#endif // ENGINETESTWIDGET_H
