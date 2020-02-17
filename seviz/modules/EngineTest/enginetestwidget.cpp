#include "enginetestwidget.h"
#include "ui_enginetestwidget.h"
#include "Book.h"

EngineTestWidget::EngineTestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EngineTestWidget)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, [this]() {
        int chapter = ui->chapterspinBox->value();
        int section = ui->sectionBox->value();
        int par = ui->parBox->value();
        int sen = ui->sentBox->value();

        try {
            const Sentence& sent = m_engine->getBook().getSentence(Position(chapter, section, par, sen));
            QString text;
            for (const Word& w : sent) {
                text += w.text() + " | ";
            }
            ui->resultLabel->setText(text);
        } catch (const std::exception& e) {
            ui->resultLabel->setText(e.what());
        }
        
    });
}

EngineTestWidget::~EngineTestWidget()
{
    delete ui;
}
