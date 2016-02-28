#include "previewimpl.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qgridlayout.h>

#include <qdrag.h>
#include <qevent.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qimagereader.h>
#include <qmatrix.h>
#include <qmimedata.h>
#include <qpixmap.h>

#include "popup.h"
#include "mainwindowimpl.h"
#include "ui_mainwindow.h"
#include "platform.h"
#include "util.h"

QPixmap
ScaleImage(const QImage &image, int width, int height)
{
	QImage simg = image.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	return QPixmap::fromImage(simg);
}

//
// Preview class
//

Preview::Preview(QWidget* parent, Qt::WindowFlags fl)
:QWidget(parent, fl)
{
	resize( 596, 480 );
	setMinimumHeight(100);

	setWindowTitle( tr( "Preview" ) );

	GridLayout = new QGridLayout( this );

	GridLayout->setGeometry( QRect( 0, 0, 596, 480 ) );

	GridLayout->setSpacing( 0 );
	GridLayout->setMargin( 0 );

	SaveButton = new QPushButton( this );
	SaveButton->setText( tr( "Save" ) );

	GridLayout->addWidget( SaveButton, 0, 2 );

	PreviewWidget = new QWidget(this);

	pxlPreview = new QLabel( PreviewWidget );
	pxlPreview->setMinimumSize( QSize( 32, 32 ) );
	pxlPreview->setFrameShape( QLabel::NoFrame );
	pxlPreview->setMargin( 0 );
	pxlPreview->setScaledContents( FALSE );
	pxlPreview->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	pxlPreview->installEventFilter(this);
	pxlPreview->hide();

	//
	// Create erase pixmap
	//
	QPixmap erase;
	if (erase.load(":/erase.xpm", "XPM"))
	{
		PreviewWidget->setBackgroundRole(QPalette::Window);
		QPalette palette = PreviewWidget->palette();
		palette.setBrush(QPalette::Window, QBrush(erase));
		PreviewWidget->setPalette(palette);
		PreviewWidget->setAutoFillBackground(true);
	}
	//
	GridLayout->addWidget( PreviewWidget, 1, 0, 1, 3 );

	// Center
	int pw = (PreviewWidget->width() - pxlPreview->width()) / 2;
	int ph = (PreviewWidget->height() - pxlPreview->height()) / 2;
	pxlPreview->move(pw, ph);

	PreviewButton = new QPushButton( this );
	PreviewButton->setText( tr( "Preview" ) );

	GridLayout->addWidget( PreviewButton, 0, 0 );
	connect(PreviewButton, SIGNAL(clicked()), this, SLOT(PreviewImage()));
	connect(SaveButton, SIGNAL(clicked()), this, SLOT(Save()));
	Splitter = NULL;
	pixPreview = NULL;
	image = NULL;

	dragging = false;
}

Preview::~Preview()
{
}

void
Preview::setOwner(QWidget *owner)
{
	Splitter = dynamic_cast<ImageSplitter *>(owner);
}

void
Preview::ShowImage(QImage *img)
{
	if (img)
	{
		image = img;
		if (Splitter->IsAutoPreview())
			PreviewButton->setEnabled(false);
		else
			PreviewButton->setEnabled(true);
	}
}

void
Preview::scalePixmap(int oldw, int oldh, int &width, int &height)
{
	int hmargin = bound(PreviewWidget->width() / 4, 2, 16);
	int vmargin = bound(PreviewWidget->height() / 4, 2, 16);
	int maxw = PreviewWidget->width() - hmargin;
	int maxh = PreviewWidget->height() - vmargin;
	int neww = maxh * oldw / oldh;
	width = qMin(neww, maxw);
	height = width * oldh / oldw;
}

void
Preview::PreviewImage()
{
	if (!image)
		return;

	//
	//
	//
	bool valid = false;

	int collageSizeX = Splitter->ui->CollageSizeX->text().toLong(&valid);
	if (!valid)
		return;

	int collageSizeY = Splitter->ui->CollageSizeY->text().toLong(&valid);
	if (!valid)
		return;

	int collageIndexX = Splitter->ui->OffsetIndexX->text().toLong(&valid);
	if (!valid)
		return;

	int collageIndexY = Splitter->ui->OffsetIndexY->text().toLong(&valid);
	if (!valid)
		return;
	//
	//
	//
	int collageOffsetTopX = Splitter->ui->CollageOffsetTopX->text().toLong(&valid);
	if (!valid)
		return;

	int collageOffsetTopY = Splitter->ui->CollageOffsetTopY->text().toLong(&valid);
	if (!valid)
		return;

	int collageOffsetBottomX = Splitter->ui->CollageOffsetBottomX->text().toLong(&valid);
	if (!valid)
		return;

	int collageOffsetBottomY = Splitter->ui->CollageOffsetBottomY->text().toLong(&valid);
	if (!valid)
		return;
//
	int imageOffsetTopX = Splitter->ui->ImageOffsetTopX->text().toLong(&valid);
	if (!valid)
		return;

	int imageOffsetTopY = Splitter->ui->ImageOffsetTopY->text().toLong(&valid);
	if (!valid)
		return;

	int imageOffsetBottomX = Splitter->ui->ImageOffsetBottomX->text().toLong(&valid);
	if (!valid)
		return;

	int imageOffsetBottomY = Splitter->ui->ImageOffsetBottomY->text().toLong(&valid);
	if (!valid)
		return;

	double imageRotate = Splitter->ui->ImageRotate->text().toDouble(&valid);
	if (!valid)
		return;

	double imageScale = Splitter->ui->ImageScale->text().toDouble(&valid);
	if (!valid)
		return;

	double shearX = Splitter->ui->ShearX->text().toDouble(&valid);
	if (!valid || qAbs(shearX) > 4.0)
		return;

	double shearY = Splitter->ui->ShearY->text().toDouble(&valid);
	if (!valid || qAbs(shearY) > 4.0)
		return;


	if (imageScale < 0)
	{
		// Negative numbers mean scale down by specified percentage, we will recheck for valid range later
		// -25 = three quarters the size, -50 = half the size, -75 = quarter the size
		imageScale = 100 + imageScale;
	}

	//
	//  Make sure we don't get 'divide by zero' error
	//

	if (collageSizeX < 1)
	{
		collageSizeX = 1;
		Splitter->ui->CollageSizeX->setText("1");
		if (Splitter->IsAutoPreview())
			return;
	}

	if (collageSizeY < 1)
	{
		collageSizeY = 1;
		Splitter->ui->CollageSizeY->setText("1");
		if (Splitter->IsAutoPreview())
			return;
	}

	if (clamp(collageIndexX, 0, collageSizeX - 1))
	{
		Splitter->ui->OffsetIndexX->setText(QString::number(collageIndexX));
		if (Splitter->IsAutoPreview())
			return;
	}

	if (clamp(collageIndexY, 0, collageSizeY - 1))
	{
		Splitter->ui->OffsetIndexY->setText(QString::number(collageIndexY));
		if (Splitter->IsAutoPreview())
			return;
	}

	QImage imgPreview;

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

	int originalWidth = imgPreview.width();
	int originalHeight = imgPreview.height();

	// Make sure image size is divisible by collage size by expanding it if necessary

	if (mod(originalWidth, collageSizeX) != 0)
		originalWidth += collageSizeX - mod(originalWidth, collageSizeX);

	if (mod(originalHeight, collageSizeY) != 0)
		originalHeight += collageSizeY - mod(originalHeight, collageSizeY);

	//
	// Calculate subimage dimensions
	//

	originalWidth -= collageOffsetTopX + collageOffsetBottomX;
	originalHeight -= collageOffsetTopY + collageOffsetBottomY;
	int subOffsetX = (originalWidth / collageSizeX) * collageIndexX + imageOffsetTopX + collageOffsetTopX;
	int subOffsetY = (originalHeight / collageSizeY) * collageIndexY + imageOffsetTopY + collageOffsetTopY;
	int subWidth = (originalWidth / collageSizeX) - imageOffsetTopX - imageOffsetBottomX;
	int subHeight = (originalHeight / collageSizeY) - imageOffsetTopY - imageOffsetBottomY;

	if (subWidth < 1 || subHeight < 1)
	{
		WPopup(tr("Nothing left after crop!"));
		return;
	}

	// Make sure the sub-image without right and bottom offsets doesn't go outside original image

	int subX = subOffsetX + imageOffsetBottomX; // Adjusted X coordinate of top-left of subimage

	if (subWidth + subX > imgPreview.width())
		subWidth = imgPreview.width() - subX;

	int subY = subOffsetY + imageOffsetBottomY; // Adjusted Y coordinate of top-left of subimage

	if (subHeight + subY > imgPreview.height())
		subHeight = imgPreview.height() - subY;

	//
	// Generate subimage
	//

	if (subOffsetX != 0 || subOffsetY != 0 || subWidth != imgPreview.width() || subHeight != imgPreview.height())
		imgPreview = imgPreview.copy(subOffsetX, subOffsetY, subWidth, subHeight);

	// Mirror image if requested

	if (Splitter->ui->mirrorX->isChecked() || Splitter->ui->mirrorY->isChecked())
	{
		imgPreview = imgPreview.mirrored(Splitter->ui->mirrorX->isChecked(), Splitter->ui->mirrorY->isChecked());
	}

	int nh, nw;
	double pscale = imageScale / 100.0;
	if (imageScale != 100.0)
	{
		nw = lrint((double) imgPreview.width() * pscale);
		nh = lrint((double) imgPreview.height() * pscale);
	}
	else
	{
		nw = imgPreview.width();
		nh = imgPreview.height();
	}

	if (nw < 1 || nh < 1)
	{
		WPopup(tr("Scale factor is too small!"));
		return;
	}

	if (pixPreview == NULL)
		pixPreview = new QPixmap();

	if (pixPreview)
	{
		//
		// Last Step: Draw It!
		//

		pxlPreview->show();
		if (imageScale != 100.0)
		{
			*pixPreview = ScaleImage(imgPreview, nw, nh);
		}
		else
		{
			*pixPreview = QPixmap::fromImage(imgPreview);
		}
		//
		int w, h;
		QPixmap tmpPreview;
		scalePixmap(imgPreview.width(), imgPreview.height(), w, h);
		if (w != imgPreview.width() || h != imgPreview.height())
		{
			// Use temporary pixmap, so we don't save scaled pixmap
			tmpPreview = ScaleImage(imgPreview, w, h);
		}
		else
		{
			tmpPreview = QPixmap::fromImage(imgPreview);
		}

		pxlPreview->resize(w, h);
		// Center
		int pw = (PreviewWidget->width() - pxlPreview->width()) / 2;
		int ph = (PreviewWidget->height() - pxlPreview->height()) / 2;
		pxlPreview->move(pw, ph);
		// Update preview
		pxlPreview->setPixmap( tmpPreview );
		SaveButton->setEnabled(true);
		setWindowTitle(tr("Preview - %1 x %2").arg(pixPreview->width()).arg(pixPreview->height()));
	}
}

bool
Preview::eventFilter( QObject *o, QEvent *e )
{
	if (o == pxlPreview)
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
Preview::mousePressEvent(QMouseEvent *e)
{
	if (e->button() & Qt::LeftButton)
	{
		qDebug("Started dragging...\n");
		dragging = true;
	}
	QWidget::mousePressEvent(e);
}

void
Preview::mouseMoveEvent(QMouseEvent *e)
{
	if (dragging)
	{
		QPoint currPos = e->pos();
		if ( ( startPos - currPos ).manhattanLength() > QApplication::startDragDistance() )
			startDrag();
	}
	QWidget::mouseMoveEvent(e);
}

void
Preview::mouseReleaseEvent(QMouseEvent *e)
{
	if (dragging)
	{
		qDebug("Stopped dragging...\n");
		dragging = false;
	}
	QWidget::mouseReleaseEvent(e);
}

void

Preview::startDrag()
{
	if (pixPreview)
	{
		QDrag *drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;
		mimeData->setImageData(pixPreview->toImage());
		drag->setMimeData(mimeData);

		drag->exec(Qt::CopyAction);
	}
}

void
Preview::Save()
{
	PreviewImage();

	if (pixPreview)
	{
		// Format of original image
		QByteArray fmt = QImageReader(Splitter->filename()).format();
		QFileInfo info(Splitter->filename());
		// Path of original image
		QString path = info.path();
		// Base name of original image filename with X and Y offsets appended to it
		QString base = info.baseName();
		base.append("_" + Splitter->ui->OffsetIndexX->text() + "_" + Splitter->ui->OffsetIndexY->text());
		// Extension of original image filename
		QString ext = info.suffix();
		// Full name of new image
		QString newname = MakePath(path, base + "." + ext);
		if (!pixPreview->save(newname, fmt))
		{
			if (QFile::exists(newname))
				QFile::remove(newname);

			// If original plugin doesn't support writing, try jpeg plugin
			//

			if (qstricmp(fmt, "JPEG") != 0)
			{
				newname = MakePath(path, base + ".jpg");
				if (pixPreview->save(newname, "JPEG"))
					return;
				else if (QFile::exists(newname))
					QFile::remove(newname);
			}

			QMessageBox::critical(this, tr("Save"), tr("Unable to save output!"));
		}
	}
}

void
Preview::ClearPreview()
{
	SaveButton->setEnabled(false);

	if (pixPreview)
	{
		delete pixPreview;
		pixPreview = NULL;
	}

	{
//		pxlPreview->clear();
		pxlPreview->hide();
	}

	image = NULL;

	setWindowTitle(tr("Preview"));
}

void
Preview::resizeEvent(QResizeEvent *e)
{
	QSize s = e->size();
	GridLayout->setGeometry(QRect(0, 0, s.width(), s.height()));
	if (pxlPreview->pixmap())
	{
		PreviewImage();
	}
	QWidget::resizeEvent(e);
}
