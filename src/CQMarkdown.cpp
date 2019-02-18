#include <CQMarkdown.h>
#include <CQMarkdownEdit.h>
#include <CQMarkdownPreview.h>
#include <QFile>
#include <QTextStream>

CQMarkdown::
CQMarkdown(QWidget *parent, bool ref) :
 QSplitter(parent)
{
  setObjectName("markdown");

  edit_    = new CQMarkdownEdit(this);
  preview_ = new CQMarkdownPreview(this, ref);

  addWidget(edit_);
  addWidget(preview_);
}

bool
CQMarkdown::
load(const QString &fileName)
{
  fileName_ = fileName;

  QFile file(fileName_);

  if (! file.open(QFile::ReadOnly | QFile::Text))
    return false;

  QTextStream stream(&file);

  edit_->setText(stream.readAll());

  return true;
}

bool
CQMarkdown::
saveMarkdown(const QString &fileName)
{
  QString fileName1 = fileName;

  if (fileName1 == "")
    fileName1 = fileName_;

  if (fileName1 == "")
    return false;

  QFile file(fileName1);

  file.open(QIODevice::WriteOnly);

  if (! file.isOpen())
    return false;

  QTextStream outStream(&file);

  outStream << edit_->text();

  file.close();

  return true;
}

bool
CQMarkdown::
saveHtml(const QString &fileName)
{
  if (fileName == "")
    return false;

  QFile file(fileName);

  file.open(QIODevice::WriteOnly);

  if (! file.isOpen())
    return false;

  QTextStream outStream(&file);

  outStream << preview_->html();

  file.close();

  return true;
}

QString
CQMarkdown::
text() const
{
  return edit_->text();
}

void
CQMarkdown::
updatePreview()
{
  preview_->updateText();
}
