#pragma once
#include <QObject>
#include <QAction>
#include <QString>
#include <QDockWidget>

class AbstractModule;

struct Feature {

public:
	Feature() = delete;

	Feature(const QString& id,
		const QIcon& icon,
		const QString& tooltipText,
		const QDockWidget* window,
		AbstractModule* owner,
		bool affectsView = false) :
		m_id(id), m_icon(icon), m_text(tooltipText), m_window(window), m_owner(owner), m_affectsView(affectsView) 
	{}

	~Feature() = default;

	QString m_id;
	QIcon m_icon;
	QString m_text;

	AbstractModule* m_owner;

	const QDockWidget* m_window = nullptr;
	bool m_affectsView = false;
};
