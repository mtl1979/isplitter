#ifndef MENUBAR_H
#define MENUBAR_H

#include <qmenubar.h>
#include <q3popupmenu.h>

class MenuBar : public QMenuBar
{
	Q_OBJECT
public:
	MenuBar(QWidget * parent = NULL);
	~MenuBar();

	Q3PopupMenu * File() const { return fFile; }
	Q3PopupMenu * Settings() const { return fSettings; }
	Q3PopupMenu * Tools() const { return fTools; }

private:

	/* File menu */
	Q3PopupMenu * fFile;
	Q3PopupMenu * fSettings;
	Q3PopupMenu * fTools;
};

#endif


