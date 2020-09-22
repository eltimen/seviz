#pragma once
#include <QObject>
#include <QSet>
#include <QString>
#include <QDir>
#include "IEngine.h"

class IEngine;

//! Plugin base class
/*! 
  Main class of your plugin must inherit from this class.
*/
Q_DECLARE_INTERFACE(ISevizPlugin, "seviz.ISevizPlugin")
class SEVIZSHARED_EXPORT ISevizPlugin : public QObject
{
    Q_OBJECT

public:
    ISevizPlugin() : QObject(nullptr) {}
    virtual ~ISevizPlugin() {}

	//! Plugin's name
	/*!
	  \warning Return value is reference. Plugin must store the name inside local static variable, global variable or class field.
	*/
    virtual const QString& id() const = 0;
	//! Plugin's version. Default is 0.
    virtual int version() const = 0;
	//! This method will be called during plugin loading. For example, can be used for set up handlers.
    virtual void init(IEngine* engine) = 0;
	//! Return features of the plugin
    virtual QList<Feature> features() = 0;

	//! Called when a new book file is opened. Use for load plugin's data from file, if they exist.
	/*!
	  \param moduleDir pointer to the plugin's own directory inside epub if they exists; nullptr otherwise.
	*/
    virtual void load(QDir* moduleDir) = 0;
	//! Called when a new book file is saved. Use for save plugin's data inside book's .epub file.
    virtual void save(QDir& moduleDir) = 0;

public slots:
    //! Called when engine is rendering book chapter content. Use for add custom CSS styles to text
    virtual void render(const Position& from, const Position& to, DomChapter& dom, const QVector<Feature*>& activeFeatures) = 0;

protected:
    IEngine* m_engine = nullptr;
};
