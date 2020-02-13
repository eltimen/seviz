#include "enginetestwidget.h"
#include "ui_enginetestwidget.h"

EngineTestWidget::EngineTestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EngineTestWidget)
{
    ui->setupUi(this);
}

EngineTestWidget::~EngineTestWidget()
{
    delete ui;
}
