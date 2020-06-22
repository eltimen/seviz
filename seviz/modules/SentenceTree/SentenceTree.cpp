#include "SentenceTree.h"
#include <QMenu>
#include <QIntValidator>
#include <QProcessEnvironment>
#include <QMessageBox>
#include <QProcess>
#include <QTemporaryFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

SentenceTree::SentenceTree(IEngine* engine) :
    AbstractModule(engine, "SentenceTree"),
    m_feature("Деревья предложений", QIcon(":/st/icon.png"), new QDockWidget(), this, true, new QMenu()),
    m_widget(this)
{
    m_feature.window()->setWidget(&m_widget);

    m_feature.menu()->addAction("Запустить Stanford CoreNLP", this, &SentenceTree::execCoreNlp);
    m_feature.menu()->setEnabled(false);
    
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

    m_currentSentenceData = nullptr;

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
                    SentenceData* loadingSentenceData = new SentenceData(m_engine->getBook().getSentence(pos));

                    if (dir.exists("pos.txt")) {
                        QFile file(dir.filePath("pos.txt"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            try {
                                QTextStream in(&file);
                                QString pos;
                                int i = 0;
                                while (in.readLineInto(&pos)) {
                                    loadingSentenceData->sentence[i].setPOS(pos);
                                    ++i;
                                }
                            } catch (const std::invalid_argument& ) {
                                throw QString("invalid pos tag");
                            }
                        }
                    }

                    if (dir.exists("constituency.json")) {
                        QFile file(dir.filePath("constituency.json"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            loadingSentenceData->constituency.fromJson(in.readAll());
                        }
                    }

                    if (dir.exists("dependency.json")) {
                        QFile file(dir.filePath("dependency.json"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            loadingSentenceData->dependency.fromStanfordCoreNlpJson(in.readAll());
                        }
                    }

                    if (dir.exists("framenet.json")) {
                        QFile file(dir.filePath("framenet.json"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            loadingSentenceData->framenet.fromJson(in.readAll(), m_framesModel, m_engine, pos);
                        }
                    }

                    m_storage.emplace(pos, std::unique_ptr<SentenceData>(loadingSentenceData));

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
                QFile file(sentenceDir.filePath("pos.txt"));
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    for (const Word& w : data->sentence) {
                        out << w.POS() << '\n';
                    }
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
    if (m_storage.count(m_currentSentencePos)) {
        m_currentSentenceData = m_storage.at(m_currentSentencePos).get();
    } else {
        const Sentence& sent = m_engine->getBook().getSentence(m_currentSentencePos);
        m_currentSentenceData = new SentenceData(sent);
        m_storage.emplace(m_currentSentencePos, std::unique_ptr<SentenceData>(m_currentSentenceData));
    }
    
    m_widget.showSentence(*m_currentSentenceData);
    m_engine->triggerRerendering(m_currentSentencePos, m_currentSentencePos);
    m_feature.menu()->setEnabled(true);
}

void SentenceTree::execCoreNlp() {
    if (QProcessEnvironment::systemEnvironment().contains("CORENLP")) {
        QString path = QProcessEnvironment::systemEnvironment().value("CORENLP");

        QTemporaryFile inputFile;
        if (inputFile.open()) {
            QTextStream out(&inputFile);
            out << m_currentSentenceData->sentence.toStringList().join(' ');
        }
        //inputFile.setAutoRemove(false); // for debug only!

        QStringList args;
        args << "-cp" << QString("%1%2*").arg(path, QDir::separator());
        args << "-Xmx1024m";
        args << "edu.stanford.nlp.pipeline.StanfordCoreNLP";
        args << "-file" << inputFile.fileName();
        args << "-annotators" << "tokenize,ssplit,pos,lemma,parse";
        args << "-tokenize.whitespace"; // sentence already tokenized
        args << "-ssplit.eolonly";
        args << "-parse.originalDependencies"; 
        args << "-parse.buildgraphs"; 
        args << "-outputFormat" << "json";
        qDebug() << args.join(' ');

        QProcess proc;
        proc.setArguments(args);
        proc.setProgram("java");
        proc.setWorkingDirectory(QDir::tempPath());
        proc.start();
        proc.waitForFinished();
        qDebug() << proc.readAllStandardOutput();
        int exitCode = proc.exitStatus();
        if (!exitCode) {
            QFile outFile(inputFile.fileName() + ".json");
            if (outFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&outFile);
                // временно до поддержки constituency. в будущем переработать форматы хранения, а также код ниже
                QJsonDocument doc = QJsonDocument::fromJson(in.readAll().toUtf8());
                QJsonArray sentences = doc.object().value("sentences").toArray();
                QJsonObject sent = sentences.first().toObject();
                m_currentSentenceData->dependency.fromStanfordCoreNlpJson(QJsonDocument(sent).toJson());
                m_widget.updateTreesView();
            } else {
                QMessageBox::warning(m_feature.window(), "Ошибка", "Не удалось прочитать вывод StanfordCoreNLP");
            }

        } else {
            QMessageBox mess(QMessageBox::Icon::Warning, "Ошибка запуска парсера", "Статус: " + QVariant::fromValue(proc.exitStatus()).toString() +
                                                                                   "Код: " + QString::number(proc.exitCode()));
            mess.setParent(m_feature.window());
            mess.setDetailedText(proc.readAllStandardError());
            mess.exec();
        }

    } else {
        QMessageBox::warning(m_feature.window(), "Ошибка", "Не задан путь к StanfordCoreNLP (переменная окружения CORENLP)");
    }
}



