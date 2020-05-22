#include "frameelementseditor.h"
#include "ui_frameelementseditor.h"

#include <QLabel>
#include <QRadioButton>
#include <QComboBox>
#include <QDebug>
#include "framenet.h"
#include "frameelement.h"

FrameElementsEditor::FrameElementsEditor(QWidget* parent, Frame* frame) :
    QDialog(parent),
    ui(new Ui::FrameElementsEditor),
    m_frame(frame)
{
    ui->setupUi(this);
    setupWidgets();

    connect(ui->saveButton, &QPushButton::clicked, this, &FrameElementsEditor::onSave);
}

FrameElementsEditor::~FrameElementsEditor()
{
    delete ui;
}

void FrameElementsEditor::onSave() {
    m_frame->clearElements();
    for (const QGroupBox* gb : ui->feContainer->findChildren<QGroupBox*>()) {
        QRadioButton* wordRadioButton = gb->findChild<QRadioButton*>("wordRadioButton");
        QComboBox* wordFromComboBox = gb->findChild<QComboBox*>("wordFromComboBox");
        // QComboBox* wordToComboBox = gb->findChild<QComboBox*>("wordFromComboBox");
        QComboBox* subFrameComboBox = gb->findChild<QComboBox*>("subFrameComboBox");

        if (gb->isChecked()) {
            if (wordRadioButton->isChecked()) {
                FrameElement fe(gb->title(), { m_words[wordFromComboBox->currentIndex()] });
                m_frame->setElement(fe);
            } else {
                FrameElement fe(gb->title(), m_subFrames[subFrameComboBox->currentIndex()]);
                m_frame->setElement(fe);
            }
        }
        //
    }

    this->accept();
}

void FrameElementsEditor::setupWidgets() {
    // слова фрейма сверху окна
    setupWordsWidget();
    
    // виджеты для редактирования FE (в дальнейшем - feBox)
    m_subFrames = m_frame->subFrames();
    QStringList feNames = m_frame->elementsList();
    int rows = sqrt(feNames.size());
    int column = 0;
    int row = 0;
    for (const QString& feName : feNames) {
        QGroupBox* feBox = new QGroupBox(feName, ui->feContainer);
        feBox->setCheckable(true);
        QHBoxLayout* layout = new QHBoxLayout(feBox);

        // FE в виде диапазона слов
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        QRadioButton* wordRadioButton = new QRadioButton("Слово:", feBox);
        wordRadioButton->setObjectName("wordRadioButton");
        verticalLayout->addWidget(wordRadioButton);
        QComboBox* wordFromComboBox = new QComboBox(feBox);
        wordFromComboBox->setObjectName("wordFromComboBox");
        verticalLayout->addWidget(wordFromComboBox);
        /*
        QComboBox* wordToComboBox = new QComboBox(feBox);
        wordToComboBox->setObjectName("wordToComboBox");
        verticalLayout->addWidget(wordToComboBox);
        */
        layout->addLayout(verticalLayout);

        // FE в виде вложенного фрейма
        QVBoxLayout* verticalLayout_2 = new QVBoxLayout();
        QRadioButton* subFrameRadioButton = new QRadioButton("Подфрейм:", feBox);
        subFrameRadioButton->setObjectName("subFrameRadioButton");
        verticalLayout_2->addWidget(subFrameRadioButton);
        QComboBox* subFrameComboBox = new QComboBox(feBox);
        subFrameComboBox->setObjectName("subFrameComboBox");
        verticalLayout_2->addWidget(subFrameComboBox);
        layout ->addLayout(verticalLayout_2);

        // добавление feBox на форму. расстановка квадратом
        ui->feContainerLayout->addWidget(feBox, row, column);
        ++row;
        if (row > rows) {
            row = 0;
            ++column;
        }

        connect(wordRadioButton, &QRadioButton::toggled, [=](bool ch) {
            subFrameComboBox->setEnabled(!ch);
        });
        connect(subFrameRadioButton, &QRadioButton::toggled, [=](bool ch) {
            wordFromComboBox->setEnabled(!ch);
            //wordToComboBox->setEnabled(!ch);
        });

        // инициализация комбобоксов выбора диапазона слов или подфрейма
        for (const Word& w : m_words) {
            wordFromComboBox->addItem(w.text(), w.id());
            //wordToComboBox->addItem(w.text(), w.id());
        }
        for (int i = 0; i < m_subFrames.size(); ++i) {
            subFrameComboBox->addItem(m_subFrames[i]->name());
            m_subFrameIndexByFrameName[m_subFrames[i]->name()] = i;
        }

        // инициализация значением FE
        FrameElement fe((*m_frame)[feName]);
        feBox->setChecked(!fe.isEmpty());
        if (!fe.isEmpty()) {
            if (fe.isWord()) {
                const Word& first = fe.words().first();
                const Word& last = fe.words().last();
                wordRadioButton->setChecked(true);
                wordFromComboBox->setCurrentIndex(m_wordIndexById[first.id()]);
                //wordToComboBox->setCurrentIndex(m_wordIndexById[last.id()]);
            } else {
                assert(fe.isFrame());
                subFrameRadioButton->setChecked(true);
                subFrameComboBox->setCurrentIndex(m_subFrameIndexByFrameName.at(fe.childFrame()->name()));
            }
        } else {
            wordRadioButton->setChecked(true);
        }
    }
    
}

void FrameElementsEditor::setupWordsWidget() {
    int column = 0;
    int row = 0;
    int maxColumns = 13;
    for (const Word& w : m_frame->words()) {
        QLabel* l = new QLabel(w.text(), ui->wordsGroupBox);
        if (w == m_frame->lu()) {
            l->setText("<b>" + w.text() + "</b>");
        } else {
            m_words.emplace_back(w);
            m_wordIndexById[w.id()] = (int)m_words.size() - 1;
        }

        ui->wordsLayout->addWidget(l, row, column);
        ++column;
        if (column > maxColumns) {
            column = 0;
            ++row;
        }
    }
}
