#include "frameelementseditor.h"
#include "ui_frameelementseditor.h"

#include <QLabel>
#include <QRadioButton>
#include <QComboBox>
#include <QDebug>
#include <QSignalBlocker>
#include <QtGlobal>
#include <algorithm>
#include "framenet.h"
#include "frameelement.h"

FrameElementsEditor::FrameElementsEditor(QWidget* parent, Frame* frame) :
    QDialog(parent),
    ui(new Ui::FrameElementsEditor),
    m_frame(frame)
{
    ui->setupUi(this);

    // инициализация подфреймов
    int feIndex = 0;
    for (const auto& rangeAndFe : m_frame->elements()) {
        if (rangeAndFe.second.isFrame()) {
            m_subFrames.emplace(rangeAndFe.second.name(), rangeAndFe.second.childFrame());
            ++feIndex;
        }
    }

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
        QComboBox* wordFromComboBox = gb->findChild<QComboBox*>("wordFromComboBox");
        QComboBox* wordToComboBox = gb->findChild<QComboBox*>("wordToComboBox");

        if (gb->isChecked() && gb->isEnabled()) {
            std::vector<Word> feContent;
            for (int i = wordFromComboBox->currentData().toInt(); i <= wordToComboBox->currentData().toInt(); ++i) {
                feContent.emplace_back(m_words.at(i));
            }
                                    
            FrameElement fe(gb->title(), feContent);
            if (!fe.isEmpty()) {
               m_frame->setElement(fe);
            }
        }
    }

    for (const auto& subFrame : m_subFrames) {
        m_frame->setElement(FrameElement(subFrame.first, subFrame.second));
    }
    
    this->accept();
}

std::vector<Word> FrameElementsEditor::getPossibleEndsOfWordRangeFrom(int id) {
    std::vector<Word> ret;
    // TODO заменить на бинарный поиск
    auto fromIt = std::find_if(m_wordsPool.begin(), m_wordsPool.end(), [id](const Word& w) {
        return w.id() == id;
    });
    if (fromIt != m_wordsPool.end()) {
        int orderId = id;
        for (auto it = fromIt; it != m_wordsPool.end(); ++it, ++orderId) {
            if (it->id() == orderId) {
                ret.emplace_back(*it);
            } else {
                break;
            }
        }
    }
    return ret;
}

void FrameElementsEditor::takeWordRangeFromPool(int from, int to) {
    qDebug() << QString("take %1 %2").arg(from).arg(to);
    auto fromIt = std::find_if(m_wordsPool.begin(), m_wordsPool.end(), [from](const Word& w) {
        return w.id() == from;
    });
    auto toIt = std::find_if(m_wordsPool.begin(), m_wordsPool.end(), [to](const Word& w) {
        return w.id() == to;
    });

    assert(fromIt != m_wordsPool.end());
    assert(toIt != m_wordsPool.end());

    ++toIt;
    m_wordsPool.erase(fromIt, toIt);
}

void FrameElementsEditor::restoreWordRangeToPool(int from, int to) {
    qDebug() << QString("restore %1 %2").arg(from).arg(to);
    // позиция, перед которой вставлять
    auto upperBoundIt = std::find_if(m_wordsPool.begin(), m_wordsPool.end(), [from](const Word& w) {
        return w.id() > from;
    });
    std::vector<Word> range;
    for (int i = from; i <= to; ++i) {
        range.emplace_back(m_words.at(i));
    }
    m_wordsPool.insert(upperBoundIt, range.begin(), range.end());
}

void FrameElementsEditor::setupWidgets() {
    // слова фрейма сверху окна
    setupWordsWidget();

    // виджеты для редактирования FE (feBox)
    QStringList feNames = m_frame->elementsList();
    int rows = sqrt(feNames.size());
    int column = 0;
    int row = 0;
    for (const QString& feName : feNames) {
        QGroupBox* feBox = new QGroupBox(feName, ui->feContainer);
        feBox->setCheckable(true);
        feBox->setChecked(false);
        feBox->setProperty("FE_name", QVariant(feName));
        QHBoxLayout* layout = new QHBoxLayout(feBox);

        // FE в виде диапазона слов
        QVBoxLayout* verticalLayout = new QVBoxLayout();
        QComboBox* wordFromComboBox = new QComboBox(feBox);
        wordFromComboBox->setObjectName("wordFromComboBox");
        verticalLayout->addWidget(wordFromComboBox);

        QComboBox* wordToComboBox = new QComboBox(feBox);
        wordToComboBox->setObjectName("wordToComboBox");
        verticalLayout->addWidget(wordToComboBox);

        layout->addLayout(verticalLayout);

        // добавление feBox на форму. расстановка квадратом
        ui->feContainerLayout->addWidget(feBox, row, column);
        ++row;
        if (row > rows) {
            row = 0;
            ++column;
        }
        
        // инициализация только что активированного FE
        connect(feBox, &QGroupBox::toggled, [&, wordFromComboBox, wordToComboBox, feBox](bool ch) {
            {
                QSignalBlocker lock(feBox);
                if (ch && (m_frame->getFreeElementsList().empty() || m_wordsPool.empty())) {
                    feBox->setChecked(false);
                    return;
                }
            }

            QSignalBlocker lock1(wordFromComboBox);
            QSignalBlocker lock2(wordToComboBox);
            if (ch) {
                wordFromComboBox->clear();
                wordToComboBox->clear();

                feBox->setEnabled(!m_wordsPool.empty());

                if (m_wordsPool.size() > 0) {
                    for (const Word& w : m_wordsPool) {
                        wordFromComboBox->addItem(w.text(), w.id());
                    }
                    for (const Word& w : getPossibleEndsOfWordRangeFrom(m_wordsPool.begin()->id())) {
                        wordToComboBox->addItem(w.text(), w.id());
                    }
                    m_previousFrom[wordFromComboBox] = m_wordsPool.begin()->id();
                    m_previousTo[wordToComboBox] = m_wordsPool.begin()->id();
                    takeWordRangeFromPool(m_wordsPool.begin()->id(), m_wordsPool.begin()->id());
                } 

            } else {
                // отдаем захваченные FE слова обратно в wordPool
                restoreWordRangeToPool(wordFromComboBox->currentData().toInt(),
                                       wordToComboBox->currentData().toInt());
                wordFromComboBox->clear();
                wordToComboBox->clear();
            }
        });

        connect(wordFromComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int i) {
            if (i < 0) 
                return;

            int fromId = m_previousFrom.at(wordFromComboBox);
            m_previousFrom[wordFromComboBox] = wordFromComboBox->currentData().toInt();

            restoreWordRangeToPool(fromId, wordToComboBox->currentData().toInt());
            QSignalBlocker lock(wordToComboBox);
            wordToComboBox->clear();
            for (const Word& w : getPossibleEndsOfWordRangeFrom(wordFromComboBox->currentData().toInt())) {
                wordToComboBox->addItem(w.text(), w.id());
            }
            m_previousTo[wordToComboBox] = wordToComboBox->currentData().toInt();
            takeWordRangeFromPool(wordFromComboBox->currentData().toInt(),
                                  wordToComboBox->currentData().toInt());
        });

        connect(wordToComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int i) {
            if (i < 0)
                return;

            int toId = m_previousTo.at(wordToComboBox);
            m_previousTo[wordToComboBox] = wordToComboBox->currentData().toInt();

            restoreWordRangeToPool(wordFromComboBox->currentData().toInt(),
                                   toId);
            takeWordRangeFromPool(wordFromComboBox->currentData().toInt(),
                                  wordToComboBox->currentData().toInt());
        });

        // инициализация значением FE
        try {
            FrameElement fe((*m_frame)[feName]);

            feBox->setChecked(false);
            feBox->setChecked(true);
            if (!fe.isEmpty()) {
                if (fe.isWord()) {
                    const Word& first = *fe.words().begin();
                    const Word& last = *(fe.words().end() - 1);

                    wordFromComboBox->setCurrentIndex(wordFromComboBox->findData(QVariant(first.id())));
                    wordToComboBox->setCurrentIndex(wordToComboBox->findData(QVariant(last.id())));
                } else {
                    assert(fe.isFrame());
                    feBox->setEnabled(false);
                    restoreWordRangeToPool(wordFromComboBox->currentData().toInt(),
                                           wordToComboBox->currentData().toInt());
                    wordFromComboBox->clear();
                    wordToComboBox->clear();
                }
            }
        } catch (std::out_of_range&) {
            feBox->setChecked(false);
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
            m_words.emplace(w.id(), w);
            m_wordsPool.emplace_back(w);
        }

        ui->wordsLayout->addWidget(l, row, column);
        ++column;
        if (column > maxColumns) {
            column = 0;
            ++row;
        }
    }
}
