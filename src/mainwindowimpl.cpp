#include "mainwindowimpl.h"

#include <q3mainwindow.h>
#include <qimage.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qfile.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qbytearray.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qurl.h>

#include "ui_mainwindow.h"
#include "previewimpl.h"
#include "menubar.h"
#include "platform.h"

#ifdef _DEBUG
#include <wstring.h>
#endif

bool ColorsEqual(const QColor &c1, const QColor &c2)
{
	if (c1.alpha() != c2.alpha())
		return false;
	int r = QABS(c1.red() - c2.red());
	int g = QABS(c1.green() - c2.green());
	int b = QABS(c1.blue() - c2.blue());
	// Allow a little bit of noise
	int diff = (r + g + b) / 3;
	if (diff > 23)
		return false;
	return true;
}

ImageSplitter::ImageSplitter( QWidget* parent, const char* name, Qt::WFlags fl)
: Q3MainWindow(parent, name, fl | Qt::WMouseNoMask)
{
	ui = new Ui_ImageSplitterBase();
	ui->setupUi(this);

	setWindowIcon(QIcon(":/isplitter.png"));

	dragging = false;

	image = NULL;

	menuBar = new MenuBar(this);
	Q_CHECK_PTR(menuBar);

	fPreview = new Preview(NULL);
	Q_CHECK_PTR(fPreview);
	fPreview->setOwner(this);

	LoadSettings();

	setAcceptDrops(true);
	ui->pxlCollage->installEventFilter(this);

	// Tab 1

	// Collage Size
	connect(ui->CollageSizeXminus, SIGNAL(clicked()), this, SLOT(CollageSizeXminusClicked()));
	connect(ui->CollageSizeX, SIGNAL(textChanged(const QString &)), this, SLOT(CollageSizeXchanged(const QString &)));
	connect(ui->CollageSizeXplus, SIGNAL(clicked()), this, SLOT(CollageSizeXplusClicked()));

	connect(ui->CollageSizeYminus, SIGNAL(clicked()), this, SLOT(CollageSizeYminusClicked()));
	connect(ui->CollageSizeY, SIGNAL(textChanged(const QString &)), this, SLOT(CollageSizeYchanged(const QString &)));
	connect(ui->CollageSizeYplus, SIGNAL(clicked()), this, SLOT(CollageSizeYplusClicked()));

	// Collage Offset
	connect(ui->CollageOffsetTopXminus, SIGNAL(clicked()), this, SLOT(CollageOffsetTopXminusClicked()));
	connect(ui->CollageOffsetTopX, SIGNAL(textChanged(const QString &)), this, SLOT(CollageOffsetTopXchanged(const QString &)));
	connect(ui->CollageOffsetTopXplus, SIGNAL(clicked()), this, SLOT(CollageOffsetTopXplusClicked()));

	connect(ui->CollageOffsetTopYminus, SIGNAL(clicked()), this, SLOT(CollageOffsetTopYminusClicked()));
	connect(ui->CollageOffsetTopY, SIGNAL(textChanged(const QString &)), this, SLOT(CollageOffsetTopYchanged(const QString &)));
	connect(ui->CollageOffsetTopYplus, SIGNAL(clicked()), this, SLOT(CollageOffsetTopYplusClicked()));

	connect(ui->CollageOffsetBottomXminus, SIGNAL(clicked()), this, SLOT(CollageOffsetBottomXminusClicked()));
	connect(ui->CollageOffsetBottomX, SIGNAL(textChanged(const QString &)), this, SLOT(CollageOffsetBottomXchanged(const QString &)));
	connect(ui->CollageOffsetBottomXplus, SIGNAL(clicked()), this, SLOT(CollageOffsetBottomXplusClicked()));

	connect(ui->CollageOffsetBottomYminus, SIGNAL(clicked()), this, SLOT(CollageOffsetBottomYminusClicked()));
	connect(ui->CollageOffsetBottomY, SIGNAL(textChanged(const QString &)), this, SLOT(CollageOffsetBottomYchanged(const QString &)));
	connect(ui->CollageOffsetBottomYplus, SIGNAL(clicked()), this, SLOT(CollageOffsetBottomYplusClicked()));

	connect(ui->CollageOffsetLockX, SIGNAL(stateChanged(int)), this, SLOT(CollageOffsetLockXchanged(int)));
	connect(ui->CollageOffsetLockY, SIGNAL(stateChanged(int)), this, SLOT(CollageOffsetLockYchanged(int)));

	connect(ui->CollageOffsetTopX, SIGNAL(textChanged(const QString &)), this, SLOT(CollageOffsetTopXchanged(const QString &)));
	connect(ui->CollageOffsetTopY, SIGNAL(textChanged(const QString &)), this, SLOT(CollageOffsetTopYchanged(const QString &)));

	// Tab 2

	// Offset Index
	connect(ui->OffsetIndexXminus, SIGNAL(clicked()), this, SLOT(OffsetIndexXminusClicked()));
	connect(ui->OffsetIndexX, SIGNAL(textChanged(const QString &)), this, SLOT(OffsetIndexXchanged(const QString &)));
	connect(ui->OffsetIndexXplus, SIGNAL(clicked()), this, SLOT(OffsetIndexXplusClicked()));

	connect(ui->OffsetIndexYminus, SIGNAL(clicked()), this, SLOT(OffsetIndexYminusClicked()));
	connect(ui->OffsetIndexY, SIGNAL(textChanged(const QString &)), this, SLOT(OffsetIndexYchanged(const QString &)));
	connect(ui->OffsetIndexYplus, SIGNAL(clicked()), this, SLOT(OffsetIndexYplusClicked()));

	// Image Offset
	connect(ui->ImageOffsetTopXminus, SIGNAL(clicked()), this, SLOT(ImageOffsetTopXminusClicked()));
	connect(ui->ImageOffsetTopX, SIGNAL(textChanged(const QString &)), this, SLOT(ImageOffsetTopXchanged(const QString &)));
	connect(ui->ImageOffsetTopXplus, SIGNAL(clicked()), this, SLOT(ImageOffsetTopXplusClicked()));

	connect(ui->ImageOffsetTopYminus, SIGNAL(clicked()), this, SLOT(ImageOffsetTopYminusClicked()));
	connect(ui->ImageOffsetTopY, SIGNAL(textChanged(const QString &)), this, SLOT(ImageOffsetTopYchanged(const QString &)));
	connect(ui->ImageOffsetTopYplus, SIGNAL(clicked()), this, SLOT(ImageOffsetTopYplusClicked()));

	connect(ui->ImageOffsetBottomXminus, SIGNAL(clicked()), this, SLOT(ImageOffsetBottomXminusClicked()));
	connect(ui->ImageOffsetBottomX, SIGNAL(textChanged(const QString &)), this, SLOT(ImageOffsetBottomXchanged(const QString &)));
	connect(ui->ImageOffsetBottomXplus, SIGNAL(clicked()), this, SLOT(ImageOffsetBottomXplusClicked()));

	connect(ui->ImageOffsetBottomYminus, SIGNAL(clicked()), this, SLOT(ImageOffsetBottomYminusClicked()));
	connect(ui->ImageOffsetBottomY, SIGNAL(textChanged(const QString &)), this, SLOT(ImageOffsetBottomYchanged(const QString &)));
	connect(ui->ImageOffsetBottomYplus, SIGNAL(clicked()), this, SLOT(ImageOffsetBottomYplusClicked()));

	connect(ui->ImageOffsetLockX, SIGNAL(stateChanged(int)), this, SLOT(ImageOffsetLockXchanged(int)));
	connect(ui->ImageOffsetLockY, SIGNAL(stateChanged(int)), this, SLOT(ImageOffsetLockYchanged(int)));

	// Tab 3

	// Rotate
	connect(ui->ImageRotate, SIGNAL(textChanged(const QString &)), this, SLOT(ImageRotatechanged(const QString &)));

	// Scale
	connect(ui->ImageScale, SIGNAL(textChanged(const QString &)), this, SLOT(ImageScalechanged(const QString &)));

	// Automatic preview
	connect(this, SIGNAL(PreviewChanged()), fPreview, SLOT(PreviewImage()));

	ClearImage();
};

void
ImageSplitter::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
         event->acceptProposedAction();
	Q3MainWindow::dragEnterEvent(event);
}

bool
ImageSplitter::eventFilter( QObject *o, QEvent *e )
{
	if (o == ui->pxlCollage)
	{
		switch(e->type())
		{
		case QEvent::MouseButtonPress:
			mousePressEvent((QMouseEvent *) e);
			return true;
		case QEvent::MouseMove:
			mouseMoveEvent((QMouseEvent *) e);
			return true;
		case QEvent::MouseButtonRelease:
			mouseReleaseEvent((QMouseEvent *) e);
			return true;
		case QEvent::Drop:
			dropEvent((QDropEvent *) e);
			return true;
		}
	}
	return false;
}

void
ImageSplitter::dropEvent(QDropEvent* event)
{
	if (event->source() == this)
	{
		event->ignore();
		return;
	}

	if (event->mimeData()->hasUrls())
	{
		QList<QUrl> urls = event->mimeData()->urls();
		QUrl u = urls.takeFirst();
#ifdef WIN32
		qDebug("Drop URL: %s", u.toString().toLocal8Bit());
#else
		wchar_t wu[u.toString().length()+1];
		int len = u.toString().toWCharArray(wu);
		wu[len] = 0;
		qDebug("Drop URL: %S", wu);
#endif
		QString file = u.toLocalFile();
#ifdef WIN32
		qDebug("Drop: %s", file.toLocal8Bit());
#else
		wchar_t wfile[file.length()+1];
		len = file.toWCharArray(wfile);
		wfile[len] = 0;
		qDebug("Drop: %S", wfile);
#endif
		Load(file);
	}
}

void
ImageSplitter::mousePressEvent(QMouseEvent *e)
{
	if (e->button() & Qt::LeftButton)
	{
		qDebug("Started dragging...\n");
		dragging = true;
	}
	Q3MainWindow::mousePressEvent(e);
}

void
ImageSplitter::mouseMoveEvent(QMouseEvent *e)
{
	if (dragging)
	{
		QPoint currPos = e->pos();
		if ( ( startPos - currPos ).manhattanLength() > QApplication::startDragDistance() )
			startDrag();
	}
	Q3MainWindow::mouseMoveEvent(e);
}

void
ImageSplitter::mouseReleaseEvent(QMouseEvent *e)
{
	if (dragging)
	{
		qDebug("Stopped dragging...\n");
		dragging = false;
	}
	Q3MainWindow::mouseReleaseEvent(e);
}

void
ImageSplitter::startDrag()
{
	if (fFilename != QString::null)
	{
		QList<QUrl> list;
		QUrl url = QUrl::fromLocalFile(fFilename);
		list.append(url);
		QDrag *drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;
		mimeData->setUrls(list);
		drag->setMimeData(mimeData);

                drag->exec(Qt::CopyAction);
	}
}

void
ImageSplitter::LoadSettings()
{
	QString filename = QString::null;
	lastdir = QString::null;
	QFile qf("isplitter.ini");
	if (qf.open(QIODevice::ReadOnly))
	{
		QByteArray temp(256);
		if (qf.readLine(temp.data(), 255) > 0)
		{
			lastdir = QString::fromUtf8(temp);
			lastdir.replace("\n", "");
		}
		if (qf.readLine(temp.data(), 255) > 0)
		{
			QString autopreview = QString::fromUtf8(temp);
			if (autopreview.startsWith("autopreview = "))
			{
				bool a = autopreview.mid(14, 4) == "true";
				menuBar->Settings()->setItemChecked(0, a);
			    fPreview->PreviewButton->setEnabled(!a);
			}
			else
			{
				menuBar->Settings()->setItemChecked(0, false);
				fPreview->PreviewButton->setEnabled(true);
			}
		}
		qf.close();
	}
}

void
ImageSplitter::SaveSettings()
{
	QFile qf("isplitter.ini");
	if (qf.open(QIODevice::WriteOnly))
	{
		QByteArray temp = lastdir.utf8();
		qf.writeBlock(temp, temp.length());
		qf.writeBlock("\n", 1);
		temp = QString("autopreview = %1").arg(menuBar->Settings()->isItemChecked(0) ? "true" : "false").utf8();
		qf.writeBlock(temp, temp.length());
		qf.close();
	}
}

ImageSplitter::~ImageSplitter()
{
	SaveSettings();
}

void
ImageSplitter::Load()
{
#ifdef _DEBUG
	WString wlastdir(lastdir);
#endif
	QString filename = QFileDialog::getOpenFileName(this, tr("Open image..."), lastdir, "*.png;*.bmp;*.xbm;*.xpm;*.pbm;*.pgm;*.ppm;*.jpg;*.jpeg;*.mng;*.gif;*.tiff");
	if (!filename.isEmpty())
	{
		Load(filename);
	}
}

void ImageSplitter::AutoCrop()
{
	// Cache variables
	bool lx = ui->CollageOffsetLockX->isChecked();
	bool ly = ui->CollageOffsetLockY->isChecked();

	int left = ui->CollageOffsetTopX->text().toInt();
	int right = ui->CollageOffsetBottomX->text().toInt();
	int top = ui->CollageOffsetTopY->text().toInt();
	int bottom = ui->CollageOffsetBottomY->text().toInt();
	int width = image->width() - right;
	int height = image->height() - bottom;
	// Left and right
	QColor cl = image->pixel(left, top);
	QColor cr = image->pixel(width - 1, top);
	for (int x = left; x < width - 1; x++)
	{
		bool samel = true, samer = true;
		for (int y = top + 1; y < height - 1; y++)
		{
			QColor cl2 = image->pixel(x, y);
			if (!ColorsEqual(cl, cl2))
			{
				samel = false;
				if (lx)
					break;
			}
			QColor cr2 = image->pixel(width - 1, y);
			if (!ColorsEqual(cr, cr2))
			{
				samer = false;
				if (lx)
					break;
			}
			if (!samel && !samer)
			{
				break;
			}
		}
		if (samel)
		{
			left++;
		}
		if (samer)
		{
			right++;
			width--;
		}
		else if (!samel)
		{
			break;
		}
	}
	// Top and bottom
	QColor ct = image->pixel(left, top);
	QColor cb = image->pixel(left, height - 1);
	for (int y = top; y < height - 1; y++)
	{
		bool samet = true, sameb = true;
		for (int x = left + 1; x < width - 1; x++)
		{
			QColor ct2 = image->pixel(x, y);
			if (!ColorsEqual(ct, ct2))
			{
				samet = false;
				if (ly)
					break;
			}
			QColor cb2 = image->pixel(x, height - 1);
			if (!ColorsEqual(cb, cb2))
			{
				sameb = false;
				if (lx)
					break;
			}
			if (!samet && !sameb)
			{
				break;
			}
		}
		if (samet)
		{
			top++;
		}
		if (sameb)
		{
			bottom++;
			height--;
		}
		else if (!samet)
		{
			break;
		}
	}
	// Save new values
	ui->CollageOffsetTopX->setText(QString::number(lx ? QMIN(left, right) : left));
	if (!lx)
		ui->CollageOffsetBottomX->setText(QString::number(right));
	ui->CollageOffsetTopY->setText(QString::number(ly ? QMIN(top, bottom) : top));
	if (!ly)
		ui->CollageOffsetBottomY->setText(QString::number(bottom));
}

void
ImageSplitter::AutoPreview()
{
	menuBar->Settings()->setItemChecked(0, !IsAutoPreview());
	fPreview->PreviewButton->setEnabled(!IsAutoPreview());
}

bool
ImageSplitter::IsAutoPreview()
{
	return menuBar->Settings()->isItemChecked(0);
}

void
ImageSplitter::scalePixmap(const QPixmap * image, int &width, int &height)
{
	int oldw = image->width();
	int oldh = image->height();
	double ratio = (double) oldw / (double) oldh;
	double neww = ui->pxlCollage->height() * ratio;
	if (neww > ui->pxlCollage->width())
	{
		width = ui->pxlCollage->width();
		double dh = ui->pxlCollage->width() / ratio;
		height = lrint(dh);
	}
	else
	{
		width = lrint(neww);
		height = ui->pxlCollage->height();
	}
}

void
ImageSplitter::scaleImage(const QImage * image, int &width, int &height)
{
	int oldw = image->width();
	int oldh = image->height();
	double ratio = (double) oldw / (double) oldh;
	double neww = ui->pxlCollage->height() * ratio;
	if (neww > ui->pxlCollage->width())
	{
		width = ui->pxlCollage->width();
		double dh = ui->pxlCollage->width() / ratio;
		height = lrint(dh);
	}
	else
	{
		width = lrint(neww);
		height = ui->pxlCollage->height();
	}
}

void
ImageSplitter::Load(const QString &filename)
{
	QImage * img = new QImage();
	if (img->load(filename))
	{
		ClearImage();

		image = img;
		fFilename = filename;

		int w, h;
		scaleImage(image, w, h);
		QImage nimg = image->smoothScale(w, h);
		QPixmap pixCollage;
		pixCollage.convertFromImage(nimg);

		ui->pxlCollage->setPixmap(pixCollage);
		fPreview->ShowImage(image);
		fPreview->show();

		QFileInfo info(fFilename);
		lastdir = info.dirPath();

		setCaption( tr("Image Splitter") + " - " + QDir::convertSeparators(fFilename) );

		ui->TabWidget2->showPage(ui->tab1);

		menuBar->Tools()->setItemEnabled(10, true);

		previewChanged();
	}
	else
		delete img;
}

void
ImageSplitter::ClearImage()
{
	if (image)
	{
		delete image;
		image = NULL;
	}

	ui->pxlCollage->clear();

    setCaption( tr( "Image Splitter" ) );

	// reset input boxes
	ui->CollageSizeX->setText("1");
	ui->CollageSizeY->setText("1");
	ui->CollageOffsetTopX->setText("0");
	ui->CollageOffsetTopY->setText("0");
	ui->CollageOffsetBottomX->setText("0");
	ui->CollageOffsetBottomY->setText("0");
	//
	ui->OffsetIndexX->setText("0");
	ui->OffsetIndexY->setText("0");
	//
	ui->ImageOffsetTopX->setText("0");
	ui->ImageOffsetTopY->setText("0");
	ui->ImageOffsetBottomX->setText("0");
	ui->ImageOffsetBottomY->setText("0");
	//
	ui->ImageRotate->setText("0");
	ui->ImageScale->setText("100");
	//
	menuBar->Tools()->setItemEnabled(10, false);
	//
	fPreview->ClearPreview();
	fPreview->hide();
}

void
ImageSplitter::Exit()
{
	SaveSettings();
	QApplication::exit(0);
}

void
ImageSplitter::resizeEvent(QResizeEvent *e)
{
	Q3MainWindow::resizeEvent(e);
}

void
ImageSplitter::CollageSizeXminusClicked()
{
	int val = ui->CollageSizeX->text().toInt();
	if (val > 1)
		ui->CollageSizeX->setText(QString::number(--val));
}

void
ImageSplitter::CollageSizeXplusClicked()
{
	int val = ui->CollageSizeX->text().toInt();
	ui->CollageSizeX->setText(QString::number(++val));
}

void
ImageSplitter::CollageSizeYminusClicked()
{
	int val = ui->CollageSizeY->text().toInt();
	if (val > 1)
		ui->CollageSizeY->setText(QString::number(--val));
}

void
ImageSplitter::CollageSizeYplusClicked()
{
	int val = ui->CollageSizeY->text().toInt();
	ui->CollageSizeY->setText(QString::number(++val));
}

void
ImageSplitter::CollageOffsetTopXminusClicked()
{
	int val = ui->CollageOffsetTopX->text().toInt();
	ui->CollageOffsetTopX->setText(QString::number(--val));
}

void
ImageSplitter::CollageOffsetTopXplusClicked()
{
	int val = ui->CollageOffsetTopX->text().toInt();
	ui->CollageOffsetTopX->setText(QString::number(++val));
}


void
ImageSplitter::CollageOffsetTopYminusClicked()
{
	int val = ui->CollageOffsetTopY->text().toInt();
	ui->CollageOffsetTopY->setText(QString::number(--val));
}

void
ImageSplitter::CollageOffsetTopYplusClicked()
{
	int val = ui->CollageOffsetTopY->text().toInt();
	ui->CollageOffsetTopY->setText(QString::number(++val));
}

void
ImageSplitter::CollageOffsetBottomXminusClicked()
{
	int val = ui->CollageOffsetBottomX->text().toInt();
	ui->CollageOffsetBottomX->setText(QString::number(--val));
}

void
ImageSplitter::CollageOffsetBottomXplusClicked()
{
	int val = ui->CollageOffsetBottomX->text().toInt();
	ui->CollageOffsetBottomX->setText(QString::number(++val));
}

void
ImageSplitter::CollageOffsetBottomYminusClicked()
{
	int val = ui->CollageOffsetBottomY->text().toInt();
	ui->CollageOffsetBottomY->setText(QString::number(--val));
}

void
ImageSplitter::CollageOffsetBottomYplusClicked()
{
	int val = ui->CollageOffsetBottomY->text().toInt();
	ui->CollageOffsetBottomY->setText(QString::number(++val));
}

void
ImageSplitter::CollageOffsetLockXchanged(int state)
{
	if (state == Qt::Checked)
	{
		ui->CollageOffsetBottomX->setEnabled(false);
		ui->CollageOffsetBottomXminus->setEnabled(false);
		ui->CollageOffsetBottomXplus->setEnabled(false);
		ui->CollageOffsetBottomX->setText(ui->CollageOffsetTopX->text());
	}
	else
	{
		ui->CollageOffsetBottomX->setEnabled(true);
		ui->CollageOffsetBottomXminus->setEnabled(true);
		ui->CollageOffsetBottomXplus->setEnabled(true);
	}
}

void
ImageSplitter::CollageOffsetLockYchanged(int state)
{
	if (state == Qt::Checked)
	{
		ui->CollageOffsetBottomY->setEnabled(false);
		ui->CollageOffsetBottomYminus->setEnabled(false);
		ui->CollageOffsetBottomYplus->setEnabled(false);
		ui->CollageOffsetBottomY->setText(ui->CollageOffsetTopY->text());
	}
	else
	{
		ui->CollageOffsetBottomY->setEnabled(true);
		ui->CollageOffsetBottomYminus->setEnabled(true);
		ui->CollageOffsetBottomYplus->setEnabled(true);
	}
}

void
ImageSplitter::CollageSizeXchanged(const QString &text)
{
	bool ok;
	int i = text.toInt(&ok);
	if (i > 0)
		previewChanged();
}

void
ImageSplitter::CollageSizeYchanged(const QString &text)
{
	bool ok;
	int i = text.toInt(&ok);
	if (i > 0)
		previewChanged();
}

void
ImageSplitter::CollageOffsetTopXchanged(const QString &text)
{
	bool ok;
	(void) text.toInt(&ok);
	if (ok)
	{
		if (ui->CollageOffsetLockX->state() == Qt::Checked)
			ui->CollageOffsetBottomX->setText(text);
		else
			previewChanged();
	}
}

void
ImageSplitter::CollageOffsetTopYchanged(const QString &text)
{
	bool ok;
	(void) text.toInt(&ok);
	if (ok)
	{
		if (ui->CollageOffsetLockY->state() == Qt::Checked)
			ui->CollageOffsetBottomY->setText(text);
		else
			previewChanged();
	}
}

void
ImageSplitter::CollageOffsetBottomXchanged(const QString &text)
{
	bool ok;
	(void) text.toInt(&ok);
	if (ok)
	{
		previewChanged();
	}
}

void
ImageSplitter::CollageOffsetBottomYchanged(const QString &text)
{
	bool ok;
	(void)text.toInt(&ok);
	if (ok)
	{
		previewChanged();
	}
}

void
ImageSplitter::OffsetIndexXminusClicked()
{
	int val = ui->OffsetIndexX->text().toInt();
	if (val > 0)
		ui->OffsetIndexX->setText(QString::number(--val));
}

void
ImageSplitter::OffsetIndexXplusClicked()
{
	int val = ui->OffsetIndexX->text().toInt();
	int size = ui->CollageSizeX->text().toInt();
	if (val < size - 1)
		ui->OffsetIndexX->setText(QString::number(++val));
}

void
ImageSplitter::OffsetIndexYminusClicked()
{
	int val = ui->OffsetIndexY->text().toInt();
	if (val > 0)
		ui->OffsetIndexY->setText(QString::number(--val));
}

void
ImageSplitter::OffsetIndexYplusClicked()
{
	int val = ui->OffsetIndexY->text().toInt();
	int size = ui->CollageSizeY->text().toInt();
	if (val < size - 1)
		ui->OffsetIndexY->setText(QString::number(++val));
}

void
ImageSplitter::ImageOffsetTopXminusClicked()
{
	int val = ui->ImageOffsetTopX->text().toInt();
	ui->ImageOffsetTopX->setText(QString::number(--val));
}

void
ImageSplitter::ImageOffsetTopXplusClicked()
{
	int val = ui->ImageOffsetTopX->text().toInt();
	ui->ImageOffsetTopX->setText(QString::number(++val));
}


void
ImageSplitter::ImageOffsetTopYminusClicked()
{
	int val = ui->ImageOffsetTopY->text().toInt();
	ui->ImageOffsetTopY->setText(QString::number(--val));
}

void
ImageSplitter::ImageOffsetTopYplusClicked()
{
	int val = ui->ImageOffsetTopY->text().toInt();
	ui->ImageOffsetTopY->setText(QString::number(++val));
}

void
ImageSplitter::ImageOffsetBottomXminusClicked()
{
	int val = ui->ImageOffsetBottomX->text().toInt();
	ui->ImageOffsetBottomX->setText(QString::number(--val));
}

void
ImageSplitter::ImageOffsetBottomXplusClicked()
{
	int val = ui->ImageOffsetBottomX->text().toInt();
	ui->ImageOffsetBottomX->setText(QString::number(++val));
}

void
ImageSplitter::ImageOffsetBottomYminusClicked()
{
	int val = ui->ImageOffsetBottomY->text().toInt();
	ui->ImageOffsetBottomY->setText(QString::number(--val));
}

void
ImageSplitter::ImageOffsetBottomYplusClicked()
{
	int val = ui->ImageOffsetBottomY->text().toInt();
	ui->ImageOffsetBottomY->setText(QString::number(++val));
}

void
ImageSplitter::ImageOffsetLockXchanged(int state)
{
	if (state == Qt::Checked)
	{
		ui->ImageOffsetBottomX->setEnabled(false);
		ui->ImageOffsetBottomXminus->setEnabled(false);
		ui->ImageOffsetBottomXplus->setEnabled(false);
		ui->ImageOffsetBottomX->setText(ui->ImageOffsetTopX->text());
	}
	else
	{
		ui->ImageOffsetBottomX->setEnabled(true);
		ui->ImageOffsetBottomXminus->setEnabled(true);
		ui->ImageOffsetBottomXplus->setEnabled(true);
	}
}

void
ImageSplitter::ImageOffsetLockYchanged(int state)
{
	if (state == Qt::Checked)
	{
		ui->ImageOffsetBottomY->setEnabled(false);
		ui->ImageOffsetBottomYminus->setEnabled(false);
		ui->ImageOffsetBottomYplus->setEnabled(false);
		ui->ImageOffsetBottomY->setText(ui->ImageOffsetTopY->text());
	}
	else
	{
		ui->ImageOffsetBottomY->setEnabled(true);
		ui->ImageOffsetBottomYminus->setEnabled(true);
		ui->ImageOffsetBottomYplus->setEnabled(true);
	}
}

void
ImageSplitter::OffsetIndexXchanged(const QString &text)
{
	bool ok;
	int i = text.toInt(&ok);
	if (ok && i >= 0)
	{
		int xmax = ui->CollageSizeX->text().toInt(&ok);
		if (ok && i < xmax)
		{
			previewChanged();
		}
	}
}

void
ImageSplitter::OffsetIndexYchanged(const QString &text)
{
	bool ok;
	int i = text.toInt(&ok);
	if (ok && i >= 0)
	{
		int xmax = ui->CollageSizeY->text().toInt(&ok);
		if (ok && i < xmax)
		{
			previewChanged();
		}
	}
}

void
ImageSplitter::ImageOffsetTopXchanged(const QString &text)
{
	bool ok;
	(void) text.toInt(&ok);
	if (ok)
	{
		if (ui->ImageOffsetLockX->state() == Qt::Checked)
			ui->ImageOffsetBottomX->setText(text);
		else
			previewChanged();
	}
}

void
ImageSplitter::ImageOffsetTopYchanged(const QString &text)
{
	bool ok;
	(void) text.toInt(&ok);
	if (ok)
	{
		if (ui->ImageOffsetLockY->state() == Qt::Checked)
			ui->ImageOffsetBottomY->setText(text);
		else
			previewChanged();
	}
}

void
ImageSplitter::ImageOffsetBottomXchanged(const QString &text)
{
	bool ok;
	(void) text.toInt(&ok);
	if (ok)
	{
		previewChanged();
	}
}

void
ImageSplitter::ImageOffsetBottomYchanged(const QString &text)
{
	bool ok;
	(void) text.toInt(&ok);
	if (ok)
	{
		previewChanged();
	}
}

void
ImageSplitter::ImageRotatechanged(const QString &text)
{
	bool ok;
	double d = text.toDouble(&ok);
	if (ok && (d > -360.0 && d < 360.0))
	{
		previewChanged();
	}
}

void
ImageSplitter::ImageScalechanged(const QString &text)
{
	bool ok;
	double d = text.toDouble(&ok);
	if (ok && (d > -100.0 && d != 0.0))
	{
		previewChanged();
	}
}

void
ImageSplitter::previewChanged()
{
	if (image && menuBar->Settings()->isItemChecked(0))
		emit PreviewChanged();
}