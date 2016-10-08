#ifndef MENUBAR_H
#define MENUBAR_H

#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qmenu.h>

class MenuBar : public QMenuBar
{
	Q_OBJECT
public:
	MenuBar(QWidget * parent = NULL);
	~MenuBar();

	QMenu * File() const {
		return fFile;
	}

	QMenu * Settings() const {
		return fSettings;
	}

	QMenu * Tools() const {
		return fTools;
	}

	QAction * Close() const {
		return fClose;
	}

	QAction * Save() const {
		return fSave;
	}

	QAction * AutoPreview() const {
		return fAutoPreview;
	}

	QAction * AutoCrop() const {
		return fAutoCrop;
	}

private:

	/* File menu */
	QMenu * fFile;
	QMenu * fSettings;
	QMenu * fTools;

	QAction * fClose;
	QAction * fSave;
	QAction * fAutoPreview;
	QAction * fAutoCrop;
};

#endif


