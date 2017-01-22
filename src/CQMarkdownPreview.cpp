#include <CQMarkdownPreview.h>
#include <CQMarkdown.h>
#include <CCommand.h>
#include <QTextEdit>

namespace {

QString runMarkdown(const QString &str) {
  char *env = getenv("CQMARKDOWN_EXEC");

  CCommand cmd(env ? env : "markdown");

  cmd.addStringSrc(str.toStdString());

  std::string res;

  cmd.addStringDest(res);

  cmd.start();

  cmd.wait();

  return QString(res.c_str());
}

}

//---

CQMarkdownPreview::
CQMarkdownPreview(CQMarkdown *markdown) :
 QTabWidget(markdown), markdown_(markdown)
{
  addTab(markHtmlEdit_ = new QTextEdit, "Mark-HTML");
  addTab(refHtmlEdit_  = new QTextEdit, "Ref-HTML");

  addTab(markTextEdit_ = new QTextEdit, "Mark-Text");
  addTab(refTextEdit_  = new QTextEdit, "Ref-Text");

  markHtmlEdit_->setReadOnly(true);
  refHtmlEdit_ ->setReadOnly(true);
  markTextEdit_->setReadOnly(true);
  refTextEdit_ ->setReadOnly(true);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void
CQMarkdownPreview::
updateText()
{
  QString str = markdown_->text();

  QString refHtml = runMarkdown(str);

  refHtmlEdit_->setHtml(refHtml);
  refTextEdit_->setPlainText(refHtml);

  html_ = mark_.processText(str);

  markHtmlEdit_->setHtml(html_);
  markTextEdit_->setPlainText(html_);
}

QSize
CQMarkdownPreview::
sizeHint() const
{
  return QSize(500, 600);
}
