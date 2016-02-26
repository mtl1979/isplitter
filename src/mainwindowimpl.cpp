#include "mainwindowimpl.h"

#include <qmainwindow.h>
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
#include "util.h"

#ifdef _DEBUG
#include <wstring.h>
#endif

bool ColorsEqual(const QColor &c1, const QColor &c2, int noise)
{
	if (c1.alpha() != c2.alpha())
		return false;
	int r = qAbs(c1.red() - c2.red());
	int g = qAbs(c1.green() - c2.green());
	int b = qAbs(c1.blue() - c2.blue());
	// Allow a little bit of noise
	int diff = (r + g + b) / 3;
	if (diff > noise)
		return false;
	return true;
}

ImageSplitter::ImageSplitter( QWidget* parent, Qt::WFlags fl)
: QMainWindow(parent, fl)
{
	ui = new Ui_ImageSplitterBase();
	ui->setupUi(this);

	setWindowIcon(QIcon(":/isplitter.png"));

	dragging = false;

	image = NULL;

	menuBar = new MenuBar(this);
	Q_CHECK_PTR(menuBar);
	setMenuBar(menuBar);

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

	connect(ui->noiseminus, SIGNAL(clicked()), this, SLOT(NoiseMinusClicked()));
	connect(ui->noiseplus, SIGNAL(clicked()), this, SLOT(NoisePlusClicked()));
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

	// Mirrored
	connect(ui->mirrorX, SIGNAL(toggled(bool)), this, SLOT(MirrorXtoggled(bool)));
	connect(ui->mirrorY, SIGNAL(toggled(bool)), this, SLOT(MirrorYtoggled(bool)));

	// Tab 3

	// Rotate
	connect(ui->ImageRotate, SIGNAL(textChanged(const QString &)), this, SLOT(ImageRotatechanged(const QString &)));

	// Scale
	connect(ui->ImageScale, SIGNAL(textChanged(const QString &)), this, SLOT(ImageScalechanged(const QString &)));

	// Shear
	connect(ui->ShearX, SIGNAL(textChanged(const QString &)), this, SLOT(ShearXchanged(const QString &)));
	connect(ui->ShearY, SIGNAL(textChanged(const QString &)), this, SLOT(ShearYchanged(const QString &)));

	// Automatic preview
	connect(this, SIGNAL(PreviewChanged()), fPreview, SLOT(PreviewImage()));

	ClearImage();
};

void
ImageSplitter::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasUrls() && event->mimeData()->urls().at(0).isLocalFile())
		event->acceptProposedAction();
	if (event->mimeData()->hasImage())
		event->acceptProposedAction();
	QMainWindow::dragEnterEvent(event);
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
	return QMainWindow::eventFilter(o, e);
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
#ifdef _DEBUG
#  ifdef WIN32
		WString wu(u.toString());
		qDebug("Drop URL: %s", (const char *) wu);
#  else
		wchar_t wu[u.toString().length()+1];
		int len = u.toString().toWCharArray(wu);
		wu[len] = 0;
		qDebug("Drop URL: %S", wu);
#  endif
#endif
		QString file = QDir::convertSeparators(u.toLocalFile());
#ifdef _DEBUG
#  ifdef WIN32
		wu = file;
		qDebug("Drop: %s", (const char *) wu);
#  else
		WString wfile(file);
		qDebug("Drop: %S", (wchar_t *) wfile.getBuffer());
#  endif
#endif
		Load(file);
	}
	else if (event->mimeData()->hasImage())
	{
		ClearImage();
		image = new QImage(qvariant_cast<QImage>(event->mimeData()->imageData()));
		fFilename = QString::null;

		int w, h;
		scaleImage(image, w, h);
		QImage nimg = image->scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui->pxlCollage->setPixmap(QPixmap::fromImage(nimg));

		menuBar->Save()->setEnabled(true);
		menuBar->AutoCrop()->setEnabled(false);
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
	QMainWindow::mousePressEvent(e);
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
	QMainWindow::mouseMoveEvent(e);
}

void
ImageSplitter::mouseReleaseEvent(QMouseEvent *e)
{
	if (dragging)
	{
		qDebug("Stopped dragging...\n");
		dragging = false;
	}
	QMainWindow::mouseReleaseEvent(e);
}

void
ImageSplitter::closeEvent(QCloseEvent *e)
{
	Exit();
	e->accept();
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
		QByteArray temp(256, 0);
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
				menuBar->AutoPreview()->setChecked(a);
				fPreview->PreviewButton->setEnabled(!a);
			}
			else
			{
				menuBar->AutoPreview()->setChecked(false);
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
		QByteArray temp = lastdir.toUtf8();
		qf.write(temp);
		qf.write("\n", 1);
		temp = QString("autopreview = %1").arg(menuBar->AutoPreview()->isChecked() ? "true" : "false").toUtf8();
		qf.write(temp);
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
	QString filename = QFileDialog::getOpenFileName(this, tr("Open image..."), lastdir, tr("Images (*.png;*.bmp;*.xbm;*.xpm;*.pbm;*.pgm;*.ppm;*.jpg;*.jpeg;*.mng;*.gif;*.tiff)"));
	if (!filename.isEmpty())
	{
		Load(filename);
	}
}

void
ImageSplitter::Save()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save image..."), lastdir, tr("Images (*.png;*.bmp;*.xbm;*.xpm;*.pbm;*.pgm;*.ppm;*.jpg;*.jpeg;*.mng;*.gif;*.tiff)"));
	if (!filename.isEmpty())
	{
		Save(filename);
	}
}

void ImageSplitter::AutoCrop()
{
	bool ok;
	QImage imgPreview;

	// Cache variables

	bool lx = ui->CollageOffsetLockX->isChecked();
	bool ly = ui->CollageOffsetLockY->isChecked();

	int left = ui->CollageOffsetTopX->text().toInt();
	int right = ui->CollageOffsetBottomX->text().toInt();
	int top = ui->CollageOffsetTopY->text().toInt();
	int bottom = ui->CollageOffsetBottomY->text().toInt();
	double imageRotate = ui->ImageRotate->text().toDouble();
	double shearX = ui->ShearX->text().toDouble();
	double shearY = ui->ShearY->text().toDouble();

	if (imageRotate == 0.0)
	{
		imgPreview = *image;
	}
	else
	{
		// Rotate original first
		QTransform tf;
		tf.rotate(imageRotate);
		imgPreview = image->transformed(tf, Qt::SmoothTransformation);
	}

	if (shearX != 0.0 || shearY != 0.0)
	{
		QTransform tf;
		tf.shear(shearX, shearY);
		imgPreview = imgPreview.transformed(tf, Qt::SmoothTransformation);
	}

	int width = imgPreview.width() - right;
	int height = imgPreview.height() - bottom;
	// Allowed noise level when comparing if row or column consists of same color, valid range is 0-63, default is 23
	int noise = ui->noiseLevel->text().toInt(&ok);
	if (ok == false)
	{
		noise = 23;
		ui->noiseLevel->setText(QString::number(noise));
	} else if (clamp(noise, 0, 63))
	{
		ui->noiseLevel->setText(QString::number(noise));
	}

	// Left and right

	QColor cl = imgPreview.pixel(left, top);
	QColor cr = imgPreview.pixel(width - 1, top);
	for (int x = left; x < width - 1; x++)
	{
		bool samel = true, samer = true;
		for (int y = top + 1; y < height - 1; y++)
		{
			QColor cl2 = imgPreview.pixel(x, y);
			if (!ColorsEqual(cl, cl2, noise))
			{
				samel = false;
				if (lx)
				{
					samer = false;
					break;
				}
			}
			QColor cr2 = imgPreview.pixel(width - 1, y);
			if (!ColorsEqual(cr, cr2, noise))
			{
				samer = false;
				if (lx)
				{
					samel = false;
					break;
				}
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

	QColor ct = imgPreview.pixel(left, top);
	QColor cb = imgPreview.pixel(left, height - 1);
	for (int y = top; y < height - 1; y++)
	{
		bool samet = true, sameb = true;
		for (int x = left + 1; x < width - 1; x++)
		{
			QColor ct2 = imgPreview.pixel(x, y);
			if (!ColorsEqual(ct, ct2, noise))
			{
				samet = false;
				if (ly)
				{
					sameb = false;
					break;
				}
			}
			QColor cb2 = imgPreview.pixel(x, height - 1);
			if (!ColorsEqual(cb, cb2, noise))
			{
				sameb = false;
				if (ly)
				{
					samet = false;
					break;
				}
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

	ui->CollageOffsetTopX->setText(QString::number(left));
	if (!lx)
		ui->CollageOffsetBottomX->setText(QString::number(right));
	ui->CollageOffsetTopY->setText(QString::number(top));
	if (!ly)
		ui->CollageOffsetBottomY->setText(QString::number(bottom));
}

void
ImageSplitter::AutoPreview()
{
	fPreview->PreviewButton->setEnabled(!IsAutoPreview());
}

bool
ImageSplitter::IsAutoPreview()
{
	return menuBar->AutoPreview()->isChecked();
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
		QImage nimg = image->scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap pixCollage = QPixmap::fromImage(nimg);

		ui->pxlCollage->setPixmap(pixCollage);
		fPreview->ShowImage(image);
		fPreview->show();

		QFileInfo info(fFilename);
		lastdir = info.path();

		setWindowTitle(tr("Image Splitter") + " - " + QDir::convertSeparators(fFilename));

		ui->TabWidget2->setCurrentIndex(ui->TabWidget2->indexOf(ui->tab1));

		menuBar->AutoCrop()->setEnabled(true);

		previewChanged();
	}
	else
		delete img;
}

void
ImageSplitter::Save(const QString &filename)
{
	if (image->save(filename))
	{
		Load(filename);
	}
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

	setWindowTitle( tr( "Image Splitter" ) );

	// reset input boxes
	ui->CollageSizeX->setText("1");
	ui->CollageSizeY->setText("1");
	ui->CollageOffsetTopX->setText("0");
	ui->CollageOffsetTopY->setText("0");
	ui->CollageOffsetBottomX->setText("0");
	ui->CollageOffsetBottomY->setText("0");
	//
	ui->noiseLevel->setText("23");
	//
	ui->OffsetIndexX->setText("0");
	ui->OffsetIndexY->setText("0");
	//
	ui->ImageOffsetTopX->setText("0");
	ui->ImageOffsetTopY->setText("0");
	ui->ImageOffsetBottomX->setText("0");
	ui->ImageOffsetBottomY->setText("0");
	ui->mirrorX->setChecked(false);
	ui->mirrorY->setChecked(false);
	//
	ui->ImageRotate->setText("0");
	ui->ImageScale->setText("100");
	//
	menuBar->Save()->setEnabled(false);
	menuBar->AutoCrop()->setEnabled(false);
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
	QMainWindow::resizeEvent(e);
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
ImageSplitter::NoiseMinusClicked()
{
	int level = ui->noiseLevel->text().toInt();
	if (level > 0)
		ui->noiseLevel->setText(QString::number(--level));
}

void
ImageSplitter::NoisePlusClicked()
{
	int level = ui->noiseLevel->text().toInt();
	if (level < 64)
		ui->noiseLevel->setText(QString::number(++level));
}

void
ImageSplitter::CollageOffsetTopXchanged(const QString &text)
{
	bool ok;
	(void) text.toInt(&ok);
	if (ok)
	{
		if (ui->CollageOffsetLockX->checkState() == Qt::Checked)
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
		if (ui->CollageOffsetLockY->checkState() == Qt::Checked)
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
		if (ui->ImageOffsetLockX->checkState() == Qt::Checked)
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
		if (ui->ImageOffsetLockY->checkState() == Qt::Checked)
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
ImageSplitter::ShearXchanged(const QString &text)
{
	bool ok;
	double d = text.toDouble(&ok);
	if (ok && qAbs(d) <= 4.0)
	{
		previewChanged();
	}
}

void
ImageSplitter::ShearYchanged(const QString &text)
{
	bool ok;
	double d = text.toDouble(&ok);
	if (ok && qAbs(d) <= 4.0)
	{
		previewChanged();
	}
}

void
ImageSplitter::MirrorXtoggled(bool)
{
	previewChanged();
}

void
ImageSplitter::MirrorYtoggled(bool)
{
	previewChanged();
}

void
ImageSplitter::previewChanged()
{
	if (image && menuBar->AutoPreview()->isChecked())
		emit PreviewChanged();
}
