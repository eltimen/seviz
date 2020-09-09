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
#include <QProgressDialog>

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
    m_widget.clear();
    m_currentSentenceData = nullptr;
    m_currentSentencePos = Position();
    m_storage.clear();

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
                    SentenceData* loadingSentenceData = new SentenceData(m_engine->getBook().getSentence(pos));

                    if (dir.exists("tokens.json")) {
                        QFile file(dir.filePath("tokens.json"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            loadTokensDataCoreNlp(in.readAll(), loadingSentenceData->sentence);
                        }
                    }

                    if (dir.exists("constituency.txt")) {
                        QFile file(dir.filePath("constituency.txt"));
                        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                            QTextStream in(&file);
                            loadingSentenceData->constituency.fromBracedString(in.readAll());
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
                QFile file(sentenceDir.filePath("constituency.txt"));
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out << data->constituency.toBracedString();
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
                QFile file(sentenceDir.filePath("tokens.json"));
                if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&file);
                    out << tokensDataToCoreNlpJson(data->sentence);
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

bool SentenceTree::currentSentenceSelected() const {
    return (bool)m_currentSentenceData;
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
}

void SentenceTree::onRunParser() {
    QJsonDocument doc = execCoreNlp();
    if (!doc.isNull()) {
        // временно до поддержки constituency. в будущем переработать форматы хранения, а также код ниже
        QJsonArray sentences = doc.object().value("sentences").toArray();

        QJsonObject sent = sentences.first().toObject();
        loadTokensDataCoreNlp(QJsonDocument(sent).toJson(), m_currentSentenceData->sentence);
        QString parse = sent.value("parse").toString();
        m_currentSentenceData->constituency.fromBracedString(parse);
        m_currentSentenceData->dependency.fromStanfordCoreNlpJson(QJsonDocument(sent).toJson());
        m_widget.updateTreesView();
    }
}

QJsonDocument SentenceTree::execCoreNlp() {
    if (QProcessEnvironment::systemEnvironment().contains("CORENLP")) {
        QTemporaryFile inputFile;
        if (inputFile.open()) {
            QTextStream out(&inputFile);
            out << m_currentSentenceData->sentence.toStringList().join(' ');
        }
        //inputFile.setAutoRemove(false); // for debug only!

        QString path = QProcessEnvironment::systemEnvironment().value("CORENLP");
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
        //qDebug() << args.join(' ');

        QProcess proc;
        proc.setArguments(args);
        proc.setProgram("java");
        proc.setWorkingDirectory(QDir::tempPath());

        QProgressDialog progress("Выполняется парсинг", "Отмена", 0, 0, &m_widget);
        progress.setWindowTitle("Stanford CoreNLP");
        progress.setWindowModality(Qt::WindowModal);
        connect(&progress, &QProgressDialog::canceled, [&proc]() {
            proc.kill();
         });
        connect(&proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [&progress]() {
            progress.cancel();
        });
        proc.start();
        progress.exec();

        qDebug() << proc.readAllStandardOutput();

        if (proc.exitStatus() == QProcess::NormalExit) {
            QFile outFile(inputFile.fileName() + ".json");
            if (outFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&outFile);
                QByteArray output = in.readAll().toUtf8();
                outFile.remove();
                return QJsonDocument::fromJson(output);
            } else {
                QMessageBox::warning(m_feature.window(), "Ошибка", "Не удалось прочитать вывод StanfordCoreNLP");
            }
        } else {
            QMessageBox mess(QMessageBox::Icon::Warning, "Ошибка запуска парсера", "Статус: " + QVariant::fromValue(proc.error()).toString() +
                                                                                   "Код: " + QString::number(proc.exitCode()));
            mess.setParent(m_feature.window());
            mess.setDetailedText(proc.readAllStandardError());
            mess.exec();
        }
    } else {
        QMessageBox::warning(m_feature.window(), "Ошибка", "Не задан путь к StanfordCoreNLP (переменная окружения CORENLP)");
    }
    return QJsonDocument();
}

void SentenceTree::loadTokensDataCoreNlp(const QString& json, Sentence& sent) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &error);
    if (!doc.isNull()) {
        QJsonValue val = doc.object().value("tokens");
        QJsonArray tokens = val.toArray();
        for (const QJsonValue& val : tokens) {
            int id = val["index"].toInt();
            QString pos = val["pos"].toString();
            try {
                sent[id - 1].setPOS(pos);
                assert(sent[id - 1].id() == id);
            } catch (const std::invalid_argument&) {
            }
        }
    } else {
        throw QString("tokens ") + error.errorString();
    }
}

QString SentenceTree::tokensDataToCoreNlpJson(const Sentence& sent) {
    QString ret = "{\"tokens\": [ \n";
    // Qt don't support trailing commas 
    int i = 0;
    for (const Word& w : sent) {
        if (i)
            ret.append(",");

        ret += QStringLiteral(R"(
		{
          "index": %1,
          "word": "%2",
          "pos": "%3"
        }
		)").arg(QString::number(w.id()))
           .arg(w.text())
           .arg(w.POS());

        ++i;
    }

    ret += "]}";

    return ret;
}



