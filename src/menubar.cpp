#include "menubar.h"

#include <qapplication.h>
#include <qmenu.h>

MenuBar::MenuBar(QWidget * parent) : QMenuBar(parent)
{
	/* create file menu */
	fFile = addMenu(tr("&File"));

	fFile->addAction(tr("&Open"), parent, SLOT(Load()), Qt::CTRL+Qt::Key_O);
	fFile->addAction(tr("&Close"), parent, SLOT(ClearImage()));

	fFile->addSeparator();

	fSave = fFile->addAction(tr("&Save"), parent, SLOT(Save()), QKeySequence::Save);
	fSave->setEnabled(false);

	fFile->addSeparator();

	fFile->addAction(tr("E&xit"), parent, SLOT(Exit()), QKeySequence::Quit);

	fSettings = addMenu(tr("&Settings"));

	fAutoPreview = fSettings->addAction(tr("Automatic preview"), parent, SLOT(AutoPreview()));
	fAutoPreview->setCheckable(true);

	fTools = addMenu(tr("&Tools"));

	fAutoCrop = fTools->addAction(tr("Automatic crop"), parent, SLOT(AutoCrop()));
	fAutoCrop->setEnabled(false);
}

MenuBar::~MenuBar()
{
}

