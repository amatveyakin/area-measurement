#include <QFileDialog>
#include <QImageReader>
#include <QLabel>
#include <QMessageBox>

#include "canvaswidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setAttribute(Qt::WA_QuitOnClose);
  setWindowState(windowState() | Qt::WindowMaximized);
  setWindowTitle(appName());

  QActionGroup* modeActionGroup = new QActionGroup(this);
  setEtalonAction                   = new QAction(QIcon(":/pictures/etalon.png"),                 QString::fromUtf8("Задание эталона"),                    modeActionGroup);
  measureSegmentLengthAction        = new QAction(QIcon(":/pictures/segment_length.png"),         QString::fromUtf8("Измерение длин отрезков"),            modeActionGroup);
  measurePolylineLengthAction       = new QAction(QIcon(":/pictures/polyline_length.png"),        QString::fromUtf8("Измерение длин кривых"),              modeActionGroup);
  measureClosedPolylineLengthAction = new QAction(QIcon(":/pictures/closed_polyline_length.png"), QString::fromUtf8("Измерение длин замкнутых кривых"),    modeActionGroup);
  measureRectangleAreaAction        = new QAction(QIcon(":/pictures/rectangle_area.png"),         QString::fromUtf8("Измерение площадей прямоугольников"), modeActionGroup);
  measurePolygonAreaAction          = new QAction(QIcon(":/pictures/polygon_area.png"),           QString::fromUtf8("Измерение площадей многоугольников"), modeActionGroup);
  foreach (QAction* action, modeActionGroup->actions())
    action->setCheckable(true);
  setEtalonAction->setChecked(true);
  ui->mainToolBar->addActions(modeActionGroup->actions());
  ui->mainToolBar->setIconSize(QSize(32, 32));
  ui->mainToolBar->setContextMenuPolicy(Qt::CustomContextMenu);
  setMeasurementEnabled(false);
  connect(modeActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateMode(QAction*)));

  QList<QByteArray> supportedFormatsList = QImageReader::supportedImageFormats();
  QString supportedFormatsString;
  foreach (const QByteArray& format, supportedFormatsList)
     supportedFormatsString += "*." + QString(format).toLower() + " ";

  QString imageFile = QFileDialog::getOpenFileName (this, QString::fromUtf8("Укажите путь к изображению — ") + appName(),
                                                    QString(), QString::fromUtf8("Все изображения (%1)").arg(supportedFormatsString));
  if (imageFile.isEmpty()) {
    exit(EXIT_SUCCESS);
    return;
  }

  QPixmap* image = new QPixmap;
  if (!image->load(imageFile)) {
    QMessageBox::warning(this, appName(), QString::fromUtf8("Не могу открыть изображение \"%1\".").arg(imageFile));
    delete image;
    exit(EXIT_SUCCESS);
    return;
  }

  QLabel* statusLabel = new QLabel(this);
  QFont labelFont = statusLabel->font();
  int newSize = QFontInfo(labelFont).pointSize() * 1.5;  // In contrast to ``labelFont.pointSize()'' it always works
  labelFont.setPointSize(newSize);
  statusLabel->setFont(labelFont);
  ui->statusBar->addWidget(statusLabel);
  canvasWidget = new CanvasWidget(image, this, ui->containingScrollArea, statusLabel, this);
  ui->containingScrollArea->setBackgroundRole(QPalette::Dark);
  ui->containingScrollArea->setWidget(canvasWidget);
}

MainWindow::~MainWindow()
{
  delete ui;
}


QString MainWindow::appName() const
{
  return "AreaMeasurement";
}


void MainWindow::setMode(Mode newMode)
{
  canvasWidget->setMode(newMode);
}

void MainWindow::setMeasurementEnabled(bool state)
{
  measureSegmentLengthAction       ->setEnabled(state);
  measurePolylineLengthAction      ->setEnabled(state);
  measureClosedPolylineLengthAction->setEnabled(state);
  measureRectangleAreaAction       ->setEnabled(state);
  measurePolygonAreaAction         ->setEnabled(state);
}

void MainWindow::updateMode(QAction* modeAction)
{
  if (modeAction == setEtalonAction)
    return setMode(SET_ETALON);
  if (modeAction == measureSegmentLengthAction)
    return setMode(MEASURE_SEGMENT_LENGTH);
  if (modeAction == measurePolylineLengthAction)
    return setMode(MEASURE_POLYLINE_LENGTH);
  if (modeAction == measureClosedPolylineLengthAction)
    return setMode(MEASURE_CLOSED_POLYLINE_LENGTH);
  if (modeAction == measureRectangleAreaAction)
    return setMode(MEASURE_RECTANGLE_AREA);
  if (modeAction == measurePolygonAreaAction)
    return setMode(MEASURE_POLYGON_AREA);
  abort();
}
