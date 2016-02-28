#include "popup.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmessagebox.h>

#include <qstring.h>

void WPopup(const QString &msg)
{
	(void) QMessageBox::information(NULL, qApp->translate( "Debug", "Image Splitter" ), msg,
		QMessageBox::Ok | QMessageBox::Default, QMessageBox::NoButton, QMessageBox::NoButton);
}

void WPopup(const char *msg)
{
	WPopup(qApp->translate("Debug", msg));
}
