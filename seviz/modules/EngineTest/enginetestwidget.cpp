#include "enginetestwidget.h"
#include "ui_enginetestwidget.h"
#include "Book.h"
#include "EngineTest.h"

EngineTestWidget::EngineTestWidget(EngineTest& facade) :
    QWidget(nullptr),
    ui(new Ui::EngineTestWidget),
    m_main(facade)
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

    connect(ui->customDataSaveButton, &QPushButton::clicked, [this]() {
        m_main.data = ui->lineEdit->text();
    });
}

EngineTestWidget::~EngineTestWidget()
{
    delete ui;
}

void EngineTestWidget::setText(const QString& str) {
    ui->lineEdit->setText(str);
}
