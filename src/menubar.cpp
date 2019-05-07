#include "menubar.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmenu.h>

#include <qclipboard.h>

MenuBar::MenuBar(QWidget * parent) : QMenuBar(parent)
{
	/* create file menu */
	fFile = addMenu(tr("&File"));

	fFile->addAction(tr("&Open"), parent, SLOT(Load()), Qt::CTRL+Qt::Key_O);
	fClose = fFile->addAction(tr("&Close"), parent, SLOT(ClearImage()));
	fClose->setEnabled(false);

	fFile->addSeparator();

	fSave = fFile->addAction(tr("&Save"), parent, SLOT(Save()), QKeySequence::Save);
	fSave->setEnabled(false);

	fFile->addSeparator();

	fFile->addAction(tr("E&xit"), parent, SLOT(Exit()), QKeySequence::Quit);

	/* create edit menu */
	fEdit = addMenu(tr("&Edit"));
	connect(fEdit, SIGNAL(aboutToShow()), this, SLOT(editAboutToShow()));

	fPaste = fEdit->addAction(tr("&Paste"), parent, SLOT(PasteImage()), Qt::CTRL + Qt::Key_V);

	/* create settings menu */
	fSettings = addMenu(tr("&Settings"));

	fAutoPreview = fSettings->addAction(tr("Automatic preview"), parent, SLOT(AutoPreview()));
	fAutoPreview->setCheckable(true);

	fSettings->addAction(tr("Set fill color"), parent, SLOT(SetFillColor()));

	/* create tools menu */
	fTools = addMenu(tr("&Tools"));

	fAutoCrop = fTools->addAction(tr("Automatic crop"), parent, SLOT(AutoCrop()));
	fAutoCrop->setEnabled(false);
}

MenuBar::~MenuBar()
{
}

void MenuBar::editAboutToShow()
{
	QClipboard *clipboard = QGuiApplication::clipboard();
	QImage img = clipboard->image();
	if (img.isNull()) {
		fPaste->setDisabled(true);
	} else {
		fPaste->setEnabled(true);
	}
}

