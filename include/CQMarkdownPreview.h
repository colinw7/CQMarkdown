#ifndef CQMarkdownPreview_H
#define CQMarkdownPreview_H

#include <CMarkdown.h>
#include <QTabWidget>
#include <vector>
#include <map>

class CQMarkdown;

#ifdef USE_WEB_VIEW
class QWebView;
#endif

class QTextEdit;

class CQMarkdownPreview : public QTabWidget {
 public:
  CQMarkdownPreview(CQMarkdown *markdown, bool ref=false);

  const QString &html() const { return html_; }

  void updateText();

  QSize sizeHint() const;

 private:
  CQMarkdown *markdown_;

#ifdef USE_WEB_VIEW
  QWebView   *markHtmlEdit_ { nullptr };
  QWebView   *refHtmlEdit_  { nullptr };
#else
  QTextEdit  *markHtmlEdit_ { nullptr };
  QTextEdit  *refHtmlEdit_  { nullptr };
#endif

  QTextEdit  *markTextEdit_ { nullptr };
  QTextEdit  *refTextEdit_  { nullptr };
  CMarkdown   mark_;
  QString     html_;
};

#endif
