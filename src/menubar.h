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

	QMenu * Edit() const {
		return fEdit;
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

	QAction * Paste() const {
		return fPaste;
	}

	QAction * AutoPreview() const {
		return fAutoPreview;
	}

	QAction * AutoCrop() const {
		return fAutoCrop;
	}


protected slots:
	void editAboutToShow();

private:

	/* File menu */
	QMenu * fFile;
	/* Edit menu */
	QMenu * fEdit;
	/* Settings menu */
	QMenu * fSettings;
	/* Tools menu */
	QMenu * fTools;

	QAction * fClose;
	QAction * fSave;
	QAction * fPaste;
	QAction * fAutoPreview;
	QAction * fAutoCrop;

};

#endif


