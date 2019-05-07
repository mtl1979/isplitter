#ifndef IMAGESPLITTER_H
#define IMAGESPLITTER_H

#include <QtWidgets/QMainWindow>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QPixmap>

class MenuBar;
class QImage;
class QString;
class Preview;
class QPoint;
class Ui_ImageSplitterBase;

class ImageSplitter : public QMainWindow
{
	Q_OBJECT
public:
	ImageSplitter( QWidget* parent = 0, Qt::WindowFlags fl = 0);
	~ImageSplitter();
	QImage *getImage() {return image;}
	QString filename() {return fFilename;}
	const QColor &fillColor() { return fFillColor; }
	void Load(const QString &filename);
	void Save(const QString &filename);
	bool IsAutoPreview();

	Ui_ImageSplitterBase * ui;

signals:
	void PreviewChanged();

protected slots:
	void Load();
	void Save();
	void ClearImage();
	void Exit();

	void AutoCrop();
	void SetFillColor();
	void AutoPreview();

	void PasteImage();

	// Tab 1
	void CollageSizeXminusClicked();
	void CollageSizeXplusClicked();
	void CollageSizeYminusClicked();
	void CollageSizeYplusClicked();

	void CollageOffsetTopXminusClicked();
	void CollageOffsetTopXplusClicked();
	void CollageOffsetTopYminusClicked();
	void CollageOffsetTopYplusClicked();

	void CollageOffsetBottomXminusClicked();
	void CollageOffsetBottomXplusClicked();
	void CollageOffsetBottomYminusClicked();
	void CollageOffsetBottomYplusClicked();

	void CollageOffsetLockXchanged(int);
	void CollageOffsetLockYchanged(int);

	void CollageSizeXchanged(const QString &);
	void CollageSizeYchanged(const QString &);
	void CollageOffsetTopXchanged(const QString &);
	void CollageOffsetTopYchanged(const QString &);
	void CollageOffsetBottomXchanged(const QString &);
	void CollageOffsetBottomYchanged(const QString &);

	void NoiseMinusClicked();
	void NoisePlusClicked();

	// Tab 2
	void OffsetIndexXminusClicked();
	void OffsetIndexXplusClicked();
	void OffsetIndexYminusClicked();
	void OffsetIndexYplusClicked();

	void ImageOffsetTopXminusClicked();
	void ImageOffsetTopXplusClicked();
	void ImageOffsetTopYminusClicked();
	void ImageOffsetTopYplusClicked();

	void ImageOffsetBottomXminusClicked();
	void ImageOffsetBottomXplusClicked();
	void ImageOffsetBottomYminusClicked();
	void ImageOffsetBottomYplusClicked();

	void ImageOffsetLockXchanged(int);
	void ImageOffsetLockYchanged(int);

	void OffsetIndexXchanged(const QString &);
	void OffsetIndexYchanged(const QString &);
	void ImageOffsetTopXchanged(const QString &);
	void ImageOffsetTopYchanged(const QString &);
	void ImageOffsetBottomXchanged(const QString &);
	void ImageOffsetBottomYchanged(const QString &);

	void MirrorXtoggled(bool);
	void MirrorYtoggled(bool);

	// Tab 3
	void ImageRotatechanged(const QString &);
	void ImageScalechanged(const QString &);
	void ShearXchanged(const QString &);
	void ShearYchanged(const QString &);

protected:
	void resizeEvent(QResizeEvent *e);
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void closeEvent(QCloseEvent *e);
	bool eventFilter(QObject *o, QEvent *e);
	void startDrag();

	void AddImage(const QImage &);

	void LoadSettings();
	void SaveSettings();

private:
	QImage *image;
	QString fFilename;
	QString lastdir;
	QColor fFillColor;
	MenuBar * menuBar;

	Preview * fPreview;

	bool dragging;
	QPoint startPos;

	void scalePixmap(const QPixmap * image, int &width, int &height);
	void scaleImage(const QImage * image, int &width, int &height);

	void previewChanged();
};
#endif
