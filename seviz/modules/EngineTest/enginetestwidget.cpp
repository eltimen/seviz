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

    connect(ui->getInfoButton, &QPushButton::clicked, [this]() {
        try {
            QPair<Position, Position> poses = m_engine->selectedTextPos();
            ui->selectedLabel->setText(QStringLiteral("par %1 sent %2 word %3 \n par %4 sent %5 word %6").arg(
                QString::number(poses.first.paragraphId()), QString::number(poses.first.sentenceId()), QString::number(poses.first.wordId()),
                QString::number(poses.second.paragraphId()), QString::number(poses.second.sentenceId()), QString::number(poses.second.wordId()))
            );
        } catch (EmptySelectionException&) {
            ui->selectedLabel->setText("Не выделено");
        }
    });

}

EngineTestWidget::~EngineTestWidget()
{
    delete ui;
}

void EngineTestWidget::setText(const QString& str) {
    ui->lineEdit->setText(str);
}
