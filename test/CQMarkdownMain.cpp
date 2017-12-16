#include <CQMarkdownMain.h>
#include <CQMarkdown.h>
#include <CQMarkdownEdit.h>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>

CQMarkdownMain::
CQMarkdownMain(bool ref) :
 QMainWindow()
{
  setObjectName("main");

  markdown_ = new CQMarkdown(0, ref);

  setCentralWidget(markdown_);

  QMenuBar *menuBar = this->menuBar();

  //---

  QMenu *fileMenu = menuBar->addMenu("&File");

  QAction *saveMarkdownAction = new QAction("Save Markdown", this);

  connect(saveMarkdownAction, SIGNAL(triggered()), this, SLOT(saveMarkdownSlot()));

  fileMenu->addAction(saveMarkdownAction);

  QAction *saveHtmlAction = new QAction("Save Html", this);

  connect(saveHtmlAction, SIGNAL(triggered()), this, SLOT(saveHtmlSlot()));

  fileMenu->addAction(saveHtmlAction);

  fileMenu->addSeparator();

  QAction *exitAction = new QAction("Exit", this);

  connect(exitAction, SIGNAL(triggered()), this, SLOT(exitSlot()));

  fileMenu->addAction(exitAction);

  //---

  QMenu *viewMenu = menuBar->addMenu("&View");

  QAction *fixedFontAction = new QAction("Fixed Font", this);

  fixedFontAction->setCheckable(true);
  fixedFontAction->setChecked  (false);

  connect(fixedFontAction, SIGNAL(triggered(bool)), this, SLOT(fixedFontSlot(bool)));

  viewMenu->addAction(fixedFontAction);
}

void
CQMarkdownMain::
load(const QString &filename)
{
  markdown_->load(filename);
}

void
CQMarkdownMain::
saveMarkdownSlot()
{
  markdown_->saveMarkdown();
}

void
CQMarkdownMain::
saveHtmlSlot()
{
  QString fileName = QFileDialog::getSaveFileName(this,
   "Save Html", "", "Html (*.html)");

  markdown_->saveHtml(fileName);
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
