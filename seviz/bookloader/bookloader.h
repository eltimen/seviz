#ifndef BOOKLOADER_H
#define BOOKLOADER_H

#include "AbstractModule.h"
#include "bookloaderdialog.h"

class BookLoader : public AbstractModule {
public:
    BookLoader(ModuleManager* engine);
    ~BookLoader();

    virtual QList<Feature> features() override;

    bool importBook(QList<Chapter>& chapters, QDir& dir, QWidget* mainWindow);

    // void load(QDir& dir) override;
    //void save(QDir& dir) override;

private:
    BookLoaderDialog* m_dialog = nullptr;
    ModuleManager* m_engine = nullptr;
};

#endif // BOOKLOADER_H
