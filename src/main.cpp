#include <QtWidgets/qapplication.h>
#include <QtWidgets/qfiledialog.h>

#include <qbytearray.h>
#include <qfile.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtranslator.h>

#ifdef _WIN32
#  include <windows.h>
#  include <shlwapi.h>
#  ifdef _DEBUG
#    include "wstring.h"
#  endif
#else
#  include <unistd.h>
#endif

#include "mainwindowimpl.h"
#include "debugimpl.h"
#include "util.h"

QString gAppDir;
#ifdef _WIN32
QString gDataDir;
#endif

#ifndef _WIN32
void
SetWorkingDirectory(const char *app)
{
	const char * wdir = strrchr(app, '/');

	if (wdir)
	{
		char * chd = new char[wdir - app + 1]; // figure out length, and make a new string of that length
		if (chd)
		{
			strncpy(chd, app, wdir - app);
			chd[wdir - app] = 0;
			PRINT("Setting working directory to: %s\n", chd);
			if (chdir(chd) != 0)
				PRINT("Setting working directory failed!\n");
			delete [] chd;
		}
	}
}
#endif

int
main( int argc, char** argv )
{
	QApplication app( argc, argv );
	QTranslator qtr( 0 );
	QTranslator qtr2( 0 );

	// Set our working directory

#ifdef _WIN32
	QString datadir = QString::fromLocal8Bit(qgetenv("APPDATA").constData());
	QDir dir(datadir);
	dir.mkdir("Image Splitter");
	datadir = MakePath(datadir, "Image Splitter");
	gDataDir = datadir;
#ifdef _DEBUG
	WString wdatadir(datadir);
	qDebug("Data directory: %ls", wdatadir.getBuffer());
#endif
	gAppDir = QDir::toNativeSeparators(app.applicationDirPath());
	// Set our working directory
	QDir::setCurrent(datadir);
#endif

	// Load language file
	QString langfile;
#ifndef _WIN32
	langfile = "isplitter.lng";
	// Set our working directory
	SetWorkingDirectory(argv[0]);
	gAppDir = QDir::currentPath();
#else
	langfile = MakePath(gDataDir, "isplitter.lng");
#endif
	QFile lang(langfile);
	QString lfile;
	QString ldir = MakePath(gAppDir, "translations");
	QDir ld(ldir);
	// Try to find directory containing translation files
	if (!ld.exists())
	{
		ld.cdUp();
		// If there is no "translations" sub-directory, check the directory containing the executable
		if (ld.exists("isplitter_en.qm"))
		{
			ldir = gAppDir;
		}
		else
		{
			// ... then try its parent directory, for example if we are running from Visual Studio
			ld.cdUp();
			if (ld.exists("isplitter_en.qm"))
			{
				ldir = QDir::toNativeSeparators(ld.absolutePath());
			}
		}
	}

	if (!lang.exists())
	{
NoTranslation:
		lfile = QFileDialog::getOpenFileName(NULL, app.translate("main", "Open translation file..."), ldir, "isplitter_*.qm");
		// Save selected language's translator filename
		if (!lfile.isEmpty() && lang.open(QIODevice::WriteOnly) )
		{
			QByteArray clang = lfile.toUtf8();
			lang.write(clang);
			lang.close();
		}
	}

	// (Re-)load translator filename
	if ( lang.open(QIODevice::ReadOnly) )
	{
		// file opened successfully
		QByteArray plang = lang.readLine();
		lfile = QString::fromUtf8(plang);
		lang.close();
	}

	// Install translator ;)
	if (!lfile.isEmpty() && QFile::exists(lfile) && qtr.load(lfile))
		app.installTranslator(&qtr);
	else
		goto NoTranslation;

	// Qt's own translator file
	QFileInfo qfi(lfile);
	QString qt_lang = QString::null;
	QString qtdir = QString::fromLocal8Bit(qgetenv("QTDIR").constData());
	langfile = qfi.fileName().replace(QRegExp("isplitter"), "qt");

	if (qtdir != QString::null)
	{
		QString tr_dir = MakePath(qtdir, "translations");
		qt_lang = MakePath(tr_dir, langfile);
		if (!QFile::exists(qt_lang))
			qt_lang = QString::null;
	}

	// Try using same directory as Image Splitter's translations
	if (qt_lang == QString::null)
	{
		qt_lang = MakePath(qfi.absolutePath(), langfile);
	}

	if (QFile::exists(qt_lang) && qtr2.load(qt_lang))
	{
		app.installTranslator( &qtr2 );
	}

	ImageSplitter * window = new ImageSplitter(NULL);
	Q_CHECK_PTR(window);

	window->show();

	if (app.arguments().count() > 1)
		window->Load(app.arguments().at(1));

	return app.exec();
}
