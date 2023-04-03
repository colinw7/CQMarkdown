#include <CQMarkdownMain.h>
#include <CQMarkdownConfigDlg.h>
#include <CQMarkdown.h>
#include <CQMarkdownEdit.h>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <iostream>

CQMarkdownMain::
CQMarkdownMain(bool ref) :
 QMainWindow()
{
  setObjectName("main");

  markdown_ = new CQMarkdown(nullptr, ref);

  setCentralWidget(markdown_);

  QMenuBar *menuBar = this->menuBar();

  //----

  QMenu *fileMenu = menuBar->addMenu("&File");

  //--

  QAction *saveMarkdownAction = new QAction("Save Markdown", this);

  connect(saveMarkdownAction, SIGNAL(triggered()), this, SLOT(saveMarkdownSlot()));

  fileMenu->addAction(saveMarkdownAction);

  //--

  QAction *saveMarkdownAsAction = new QAction("Save Markdown as ...", this);

  connect(saveMarkdownAsAction, SIGNAL(triggered()), this, SLOT(saveMarkdownAsSlot()));

  fileMenu->addAction(saveMarkdownAsAction);

  //--

  QAction *saveHtmlAction = new QAction("Save Html as ...", this);

  connect(saveHtmlAction, SIGNAL(triggered()), this, SLOT(saveHtmlSlot()));

  fileMenu->addAction(saveHtmlAction);

  //--

  fileMenu->addSeparator();

  //--

  QAction *exitAction = new QAction("Exit", this);

  connect(exitAction, SIGNAL(triggered()), this, SLOT(exitSlot()));

  fileMenu->addAction(exitAction);

  //----

  QMenu *viewMenu = menuBar->addMenu("&View");

  //--

  QAction *fixedFontAction = new QAction("Fixed Font", this);

  fixedFontAction->setCheckable(true);
  fixedFontAction->setChecked  (false);

  connect(fixedFontAction, SIGNAL(triggered(bool)), this, SLOT(fixedFontSlot(bool)));

  viewMenu->addAction(fixedFontAction);

  //--

  QAction *configAction = new QAction("Config", this);

  connect(configAction, SIGNAL(triggered()), this, SLOT(configSlot()));

  viewMenu->addAction(configAction);
}

void
CQMarkdownMain::
load(const QString &fileName)
{
  markdown_->load(fileName);

  setWindowTitle("MarkDown - " + fileName);
}

void
CQMarkdownMain::
saveMarkdownSlot()
{
  if (! markdown_->fileName().simplified().length()) {
    saveMarkdownAsSlot();
    return;
  }

  bool rc = markdown_->saveMarkdown();

  if (! rc)
    std::cerr << "Failed to save Markdown\n";
}

void
CQMarkdownMain::
saveMarkdownAsSlot()
{
  QString fileName = QFileDialog::getSaveFileName(this,
   "Save Markdown", "", "Markdown (*.md)");

  if (fileName.isNull())
    return;

  bool rc = markdown_->saveMarkdown(fileName);

  if (! rc)
    std::cerr << "Failed to save Markdown\n";

  setWindowTitle("MarkDown - " + fileName);
}

void
CQMarkdownMain::
saveHtmlSlot()
{
  QString fileName = QFileDialog::getSaveFileName(this,
   "Save Html", "", "Html (*.html)");

  bool rc = markdown_->saveHtml(fileName);

  if (! rc)
    std::cerr << "Failed to save Html\n";
}

void
CQMarkdownMain::
exitSlot()
{
  exit(0);
}

void
CQMarkdownMain::
fixedFontSlot(bool b)
{
  QFont font;

  if (b)
    font = QFont("FreeMono", 14);
  else
    font = QFont("FreeSans", 14);

  markdown_->edit()->setFont(font);
}

void
CQMarkdownMain::
configSlot()
{
  if (! configDlg_)
    configDlg_ = new CQMarkdownConfigDlg(this);

  configDlg_->show();
}

void
CQMarkdownMain::
updateText()
{
  markdown_->updatePreview();
}
