#include "SentenceTree.h"
#include <QIntValidator>

SentenceTree::SentenceTree(IEngine* engine) :
    AbstractModule(engine, "SentenceTree"),
    m_feature("Деревья предложений", QIcon(":/st/icon.png"), new QDockWidget(), this, true),
    m_widget(this)
{
    m_feature.window()->setWidget(&m_widget);
    
    m_engine->registerHandler(EventType::MOUSE_OVER, ElementType::SENTENCE, CTRL, m_feature, SLOTPOS(onSentenceChanged));
    m_engine->registerHotkey(QKeySequence("Ctrl+F"), m_feature, [this]() {
        m_widget.onFrameInsert(m_engine);
    });
}

SentenceTree::~SentenceTree() {
    // TODO
}

QList<Feature> SentenceTree::features() {
    return { 
        m_feature
    };
}

void SentenceTree::render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) {
    if (m_currentSentenceData) {
        dom.addStyle(m_currentSentencePos.firstWord(), "border-left: 1px solid black;");
        Position pos = m_currentSentencePos.firstWord();
        while (pos.hasNextWord()) {
            pos = pos.nextWord();
        }
        dom.addStyle(pos, "border-right: 1px solid black;");

        dom.addStyle(m_currentSentencePos, "font-weight: bold;");
    }
}

void SentenceTree::load(QDir* moduleDir) {
    if (!moduleDir)
        return;

    QDir dir = *moduleDir;

    for (const auto& chapterDir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        bool ok = false;
        int chId = chapterDir.fileName().toInt(&ok);
        if (!ok) continue;
        dir.cd(chapterDir.fileName());

        for (const auto& sectionDir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            bool ok = false;
            int sId = sectionDir.fileName().toInt(&ok);
            if (!ok) continue;
            dir.cd(sectionDir.fileName());

            for (const auto& parDir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                bool ok = false;
                int parId = parDir.fileName().toInt(&ok);
                if (!ok) continue;
                dir.cd(parDir.fileName());

                for (const auto& sentDir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                    bool ok = false;
                    int sentId = sentDir.fileName().toInt(&ok);
                    if (!ok) continue;
                    dir.cd(sentDir.fileName());

                    Position pos(chId, sId, parId, sentId);
                    m_currentSentencePos = pos;
                    m_currentSentence = m_engine->getBook().getSentence(m_currentSentencePos);
                    m_currentSentenceData = new SentenceData(m_currentSentence);

                    if (dir.exists("constituency.json")) {
                        QFile file(dir.filePath("constituency.json"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            m_currentSentenceData->constituency.fromJson(in.readAll());
                        }
                    }


                    if (dir.exists("dependency.json")) {
                        QFile file(dir.filePath("dependency.json"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            m_currentSentenceData->dependency.fromStanfordCoreNlpJson(in.readAll());
                        }
                    }

                    if (dir.exists("framenet.json")) {
                        QFile file(dir.filePath("framenet.json"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            m_currentSentenceData->framenet.fromJson(in.readAll(), m_framesModel, m_engine, m_currentSentencePos);
                        }
                    }

                    m_storage.emplace(pos, std::unique_ptr<SentenceData>(m_currentSentenceData));

                    dir.cdUp();
                }
                dir.cdUp();
            }
            dir.cdUp();
        }
        dir.cdUp();
    }
}

void SentenceTree::save(QDir& moduleDir) {
    qDebug() << moduleDir.path();
    for (const auto& positionAndTrees : m_storage) {
        Position pos = positionAndTrees.first;
        const auto& data = positionAndTrees.second;
        QString sentencePath = QString("%1/%2/%3/%4")
                               .arg(QString::number(pos.chapterId()))
                               .arg(QString::number(pos.sectionId()))
                               .arg(QString::number(pos.paragraphId()))
                               .arg(QString::number(pos.sentenceId()));
        if (moduleDir.mkpath(sentencePath)) {
            QDir sentenceDir(moduleDir.path() + "/" + sentencePath);

            {
                QFile file(sentenceDir.filePath("constituency.json"));
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out << data->constituency.toJson();
                }
            }

            {
                QFile file(sentenceDir.filePath("dependency.json"));
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out << data->dependency.toStanfordCoreNlpJson();
                }
            }

            {
                QFile file(sentenceDir.filePath("framenet.json"));
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out << data->framenet.toJson();
                }
            }
        }
    }
}

SentenceData& SentenceTree::currentSentenceData() {
    return *m_currentSentenceData;
}

const Position& SentenceTree::currentSentencePos() const {
    return m_currentSentencePos;
}

const FrameNetModel& SentenceTree::framesModel() {
    return m_framesModel;
}

void SentenceTree::onSentenceChanged(const Position& pos) {
    // TODO проверка на наличие несохраненных изменений

    m_currentSentencePos = pos.trimmedTo(ElementType::SENTENCE);
    m_currentSentence = m_engine->getBook().getSentence(m_currentSentencePos);
    if (m_storage.count(m_currentSentencePos)) {
        m_currentSentenceData = m_storage.at(m_currentSentencePos).get();
    } else {
        m_currentSentenceData = new SentenceData(m_currentSentence);
        m_storage.emplace(m_currentSentencePos, std::unique_ptr<SentenceData>(m_currentSentenceData));
    }

    m_widget.showSentence(m_currentSentence, *m_currentSentenceData);
    m_engine->triggerRerendering(m_currentSentencePos, m_currentSentencePos);
}



