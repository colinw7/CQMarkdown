#include <CQMarkdownMain.h>
#include <CQMarkdown.h>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

CQMarkdownMain::
CQMarkdownMain() :
 QMainWindow()
{
  markdown_ = new CQMarkdown;

  setCentralWidget(markdown_);

  QMenuBar *menuBar = this->menuBar();

  QMenu *fileMenu = menuBar->addMenu("&File");

  QAction *saveAction = new QAction("Save", this);
  connect(saveAction, SIGNAL(triggered()), this, SLOT(saveSlot()));

  fileMenu->addAction(saveAction);

  fileMenu->addSeparator();

  QAction *exitAction = new QAction("Exit", this);
  connect(exitAction, SIGNAL(triggered()), this, SLOT(exitSlot()));

  fileMenu->addAction(exitAction);
}

void
CQMarkdownMain::
load(const QString &filename)
{
  markdown_->load(filename);
}

void
CQMarkdownMain::
saveSlot()
{
  markdown_->save();
}

void
CQMarkdownMain::
exitSlot()
{
  exit(0);
}
