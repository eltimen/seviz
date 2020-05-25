#include "stwindow.h"
#include "ui_stwindow.h"

#include <QWebEngineSettings>
#include <QWebChannel>
#include <QMessageBox>
#include <QDebug>
#include <QInputDialog>
#include "SentenceTree.h"
#include "constituency.h"
#include "dependency.h"
#include "framenet.h"
#include "choosepalettedialog.h"
#include "frameelementseditor.h"

STWindow::STWindow(SentenceTree* core) :
    QWidget(nullptr),
    ui(new Ui::STWindow),
    m_webchannel(new QWebChannel(this)),
    m_core(core)
{
    ui->setupUi(this);

    m_webchannel->registerObject("core", this);

    ui->dependencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->dependencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);  
    ui->dependencyView->setContextMenuPolicy(Qt::NoContextMenu);
    ui->dependencyView->page()->setWebChannel(m_webchannel);
    ui->dependencyView->setUrl(QUrl("file:///SentenceTree_resources/dep.html"));

    ui->constituencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->constituencyView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true); 
    ui->constituencyView->setContextMenuPolicy(Qt::NoContextMenu);
    ui->constituencyView->page()->setWebChannel(m_webchannel);
    ui->constituencyView->setUrl(QUrl("file:///SentenceTree_resources/constituency.html"));

    ui->framenetView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    ui->framenetView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    ui->framenetView->setContextMenuPolicy(Qt::NoContextMenu);
    ui->framenetView->page()->setWebChannel(m_webchannel);
    ui->framenetView->setUrl(QUrl("file:///SentenceTree_resources/frame.html"));
}

STWindow::~STWindow()
{
    m_webchannel->deregisterObject(this);
    delete ui;
}

void STWindow::showSentence(const Sentence& sent, const SentenceData& data) {
    m_sentenceText.clear();
    for (const Word& w : sent) {
        m_sentenceText.append(w.text() + " ");
    }
    ui->idLabel->setText(QStringLiteral("ID: %1").arg(sent.id()));

    renderConstituency(data.constituency);
    renderDependencies(data.dependency); 
    renderFrameNet(data.framenet); 

}

void STWindow::onFrameInsert(IEngine* engine) {
    try {
        QPair<Position, Position> frameBorders = engine->selectedTextPos();
        m_core->onSentenceChanged(frameBorders.first);
        Position& from = frameBorders.first;
        Position& to = frameBorders.second;

        if (from.chapterId() == to.chapterId() &&
            from.sectionId() == to.sectionId() &&
            from.paragraphId() == to.paragraphId() &&
            from.sentenceId() == to.sentenceId() &&
            from.wordId() > 0 && to.wordId() > 0) {

            // извлечение списка слов, которые будут внутри фрейма
            std::vector<Word> frameWords = getWordsInsideFrameRange(engine, from, to);
            
            if (frameWords.size() > 0) {
                FrameTree& tree = m_core->currentSentenceData().framenet;
                FrameInsertionData insertPos;
                WordRange range(frameWords[0].id(), frameWords[frameWords.size() - 1].id());
                if (tree.canInsertFrameWithRange(range, &insertPos)) {
                    std::vector<std::pair<Word, QString>> possibleFrames;
                    QStringList paletteButtons = generateFrameChoosingPalette(frameWords, possibleFrames, insertPos);
                    if (paletteButtons.isEmpty()) {
                        QMessageBox::warning(this, "Вставка фрейма", "В данном отрезке не найдено LU, выделенных в FrameNet");
                        return;
                    }

                    QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, paletteButtons, 20));
                    chooser->setWindowTitle("Выберите фрейм и LU");
                    if (chooser->exec()) {
                        int index = chooser->getChoosedIndex();
                        Frame* f = new Frame(possibleFrames[index].second, possibleFrames[index].first, range, frameWords, m_core->framesModel());
                        std::map<QString, QString> feForSubframes = askFrameElementsForSubframes(insertPos, f);
                        QString parentFE = askParentFrameElementForNewFrame(insertPos, f);
                                                
                        // TODO проверить, что FE указаны
                        tree.insertFrame(f, parentFE, feForSubframes);
                        // TODO добавить CSS border границ фрейма и выделить черным LU
                        renderFrameNet(tree);

                    }
                } else {
                    QMessageBox::warning(this, "Ошибка", "Дерево фреймов не может содержать пересекающихся узлов");
                }
                
            } else {
                throw EmptySelectionException();
            }
        } else {
            throw EmptySelectionException();
        }
    } catch (EmptySelectionException&) {
        QMessageBox::warning(this, "Ошибка", "Для добавления фрейма нужно выделить диапазон слов ВНУТРИ ОДНОГО предложения");
    }
   
}// ------------------ constituency tree event handlers ---------------------
void STWindow::onConstituencyCreateNode(int from, int to) {
    ConstituencyTree& tree = m_core->currentSentenceData().constituency;

    NodeInsertPosition pos;
    if (tree.canInsertNodeWithRange(from, to, &pos)) {
        QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, ConstituencyLabelStr, 4));
        chooser->setWindowTitle("Выберите тип узла");
        if (chooser->exec()) {
            ConstituencyLabel label = chooser->getChoosedAsEnum<ConstituencyLabel>();
            tree.insert(std::make_pair(from, to), label, &pos);
            renderConstituency(tree);
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Дерево составляющих не может содержать пересекающиеся узлы");
    }

    qDebug() << tree.toBracedString();
}

void STWindow::onConstituencyChangeNodeType(int id) {
    ConstituencyTree& tree = m_core->currentSentenceData().constituency;

    if (tree.canChangeOrDeleteNode(id)) {
        QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, ConstituencyLabelStr, 4));
        chooser->setWindowTitle("Выберите тип узла");
        if (chooser->exec()) {
            ConstituencyLabel rel = static_cast<ConstituencyLabel>(chooser->getChoosedIndex());
            tree.change(id, rel);
            renderConstituency(tree);
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Нельзя менять тип узла корня или элементов предложения");
    }
}

void STWindow::onConstituencyDeleteNode(int id) {
    ConstituencyTree& tree = m_core->currentSentenceData().constituency;

    if (tree.canChangeOrDeleteNode(id)) {
         tree.remove(id);
         renderConstituency(tree);
    } else {
        QMessageBox::warning(this, "Ошибка", "Нельзя удалить корень или элемент предложения");
    }
}

// ------------------ dependency tree event handlers ---------------------
void STWindow::onDepCreateEdge(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;
    if (tree.canInsertRelation(from, to)) {
        QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, DependencyRelationStr));
        if (chooser->exec()) {
            DependencyRelation rel = static_cast<DependencyRelation>(chooser->getChoosedIndex());
            tree.insert(from, to, rel);
            renderDependencies(tree);
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Дерево зависимостей не может содержать циклов или параллельных связей");
    }
}

void STWindow::onDepRemoveEdge(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;
    tree.remove(from, to);
    renderDependencies(tree);
}

void STWindow::onDepChangeEdgeType(int from, int to) {
    DependencyTree& tree = m_core->currentSentenceData().dependency;

    QScopedPointer<ChoosePaletteDialog> chooser(new ChoosePaletteDialog(this, DependencyRelationStr));
    if (chooser->exec()) {
        DependencyRelation rel = static_cast<DependencyRelation>(chooser->getChoosedIndex());
        tree.change(from, to, rel);
        // TODO может лучше перерендеривать на уровне JS? например, через forceRedraw() или просто скопипастить код из renderDependencies()
        renderDependencies(tree);
    }
}

// ------------------ Berkeley FrameNet tree event handlers ---------------------

void STWindow::onFrameEdit(int id) {
    FrameTree& tree = m_core->currentSentenceData().framenet;
    Frame* f = tree.findByTreeId(id);
    QScopedPointer<FrameElementsEditor> chooser(new FrameElementsEditor(this, f));
    if (chooser->exec()) {

        renderFrameNet(tree);
    }
}

void STWindow::onFrameRemove(int id) {
    FrameTree& tree = m_core->currentSentenceData().framenet;
    tree.remove(id);
    renderFrameNet(tree);
}

// --------------------- render methods ----------------------------
void STWindow::renderConstituency(const ConstituencyTree& tree) {
    QString data = tree.toTreantJson();
    ui->constituencyView->page()->runJavaScript("var constituency = " + data + "; render(constituency);");
}
void STWindow::renderDependencies(const DependencyTree& tree) {
    QString docData = tree.toBratJson();
    ui->dependencyView->page()->runJavaScript("docData=" + docData + "; render();");
}

void STWindow::renderFrameNet(const FrameTree& tree) {
    QString data = tree.toTreantJson();
    ui->framenetView->page()->runJavaScript("config=" + data + "; render(config);");

}

std::vector<Word> STWindow::getWordsInsideFrameRange(IEngine* engine, const Position& from, const Position& to) {
    std::vector<Word> ret;
    Position current = from;
    while (current < to || current == to) {
        Word w = engine->getBook().getWord(current);
        if (!w.isPunct()) {
            ret.emplace_back(engine->getBook().getWord(current));
        }
        if (current.hasNextWord()) {
            current = current.nextWord();
        } else {
            break;
        }
    }
    return ret;
}

QStringList STWindow::generateFrameChoosingPalette(const std::vector<Word>& frameWords, std::vector<std::pair<Word, QString>>& possibleFrames, FrameInsertionData& insertionContext) {
    QStringList paletteButtons;
    for (const Word& w : frameWords) {
        bool insideChildFrame = false;
        for (Frame* f : insertionContext.lowFrames) {
            if (f->range().contains(w.id())) {
                insideChildFrame = true;
                break;
            }
        }
        if (!insideChildFrame) {
            QStringList framesForWord = m_core->framesModel().frameNamesForLexicalUnit(w.text());
            for (QString& frame : framesForWord) {
                possibleFrames.emplace_back(std::make_pair(w, frame));
                paletteButtons << QString("%1 (\"%2\")").arg(frame, w.text());
            }
        }
    }

    return paletteButtons;
}

QString STWindow::askParentFrameElementForNewFrame(const FrameInsertionData& insertionData, Frame* newFrame) {
    QString ret;
    bool ok = false;
    if (insertionData.highFrame) {
        ret = QInputDialog::getItem(this,
                                    "Вставка фрейма",
                                    QString("Выберите элемент фрейма %1 для добавления в него %2: ")
                                        .arg(insertionData.highFrame->name())
                                        .arg(newFrame->name()),
                                    insertionData.highFrame->getFreeElementsList(),
                                    0,
                                    false,
                                    &ok);
    }
    return ret;
}

std::map<QString, QString> STWindow::askFrameElementsForSubframes(const FrameInsertionData& insertionData, Frame* newFrame) {
    std::map<QString, QString> ret;
    QString subframeFE;
    bool ok = false;
    assert(insertionData.lowFrames.size() == insertionData.lowFramesAsHighFrameElements.size());
    for (int i = 0; i < insertionData.lowFrames.size(); ++i) {
        QString name = insertionData.lowFrames[i]->name();
        subframeFE = QInputDialog::getItem(this,
                                           "Вставка фрейма",
                                           QString("Выберите FE для вставки дочернего фрейма %1: ").arg(name),
                                           newFrame->getFreeElementsList(), // TODO исключая занятые ранее в этом цикле
                                           0,
                                           false,
                                           &ok);
        if (ok) {
            ret.emplace(subframeFE, insertionData.feNamesOfChildFramesInsideFutureParent[i]);
        }
    }
    return ret;
}

