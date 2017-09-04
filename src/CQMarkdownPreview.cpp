#include <CQMarkdownPreview.h>
#include <CQMarkdown.h>
#include <CCommand.h>
#ifdef USE_WEB_VIEW
#include <QWebView>
#endif
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
#ifdef USE_WEB_VIEW
  addTab(markHtmlEdit_ = new QWebView , "Mark-HTML");
  addTab(refHtmlEdit_  = new QWebView , "Ref-HTML" );
#else
  addTab(markHtmlEdit_ = new QTextEdit, "Mark-HTML");
  addTab(refHtmlEdit_  = new QTextEdit, "Ref-HTML" );

  markHtmlEdit_->setReadOnly(true);
  refHtmlEdit_ ->setReadOnly(true);
#endif

  addTab(markTextEdit_ = new QTextEdit, "Mark-Text");
  addTab(refTextEdit_  = new QTextEdit, "Ref-Text" );

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

#ifdef USE_WEB_VIEW
  refHtmlEdit_->setHtml(refHtml);
#else
  refHtmlEdit_->setHtml(refHtml);
#endif
  refTextEdit_->setPlainText(refHtml);

  html_ = mark_.processText(str);

#ifdef USE_WEB_VIEW
  markHtmlEdit_->setHtml(html_);
#else
  markHtmlEdit_->setHtml(html_);
#endif
  markTextEdit_->setPlainText(html_);
}

QSize
CQMarkdownPreview::
sizeHint() const
{
  return QSize(500, 600);
}
