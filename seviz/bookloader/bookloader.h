#ifndef BOOKLOADER_H
#define BOOKLOADER_H

#include "ISevizPlugin.h"
#include "bookloaderdialog.h"

class BookLoader : public ISevizPlugin {
public:
    BookLoader(ModuleManager* engine);
    ~BookLoader();

    const QString &id() const override;
    int version() const override;
    void init(IEngine* engine) override;
    virtual QList<Feature> features() override;

    bool importBook(QList<Chapter>& chapters, QDir& dir, QWidget* mainWindow);

    void load(QDir* dir) override;
    void save(QDir& dir) override;

private:
    BookLoaderDialog* m_dialog = nullptr;
    ModuleManager* m_engine = nullptr;

public slots:
    void render(const Position &from, const Position &to, DomChapter &dom, const QVector<Feature *> &activeFeatures) override;
};

#endif // BOOKLOADER_H
