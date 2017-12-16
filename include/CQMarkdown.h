#ifndef CQMarkdown_H
#define CQMarkdown_H

#include <QSplitter>

class CQMarkdownEdit;
class CQMarkdownPreview;

class CQMarkdown : public QSplitter {
  Q_OBJECT

 public:
  CQMarkdown(QWidget *parent=0, bool ref=false);

  CQMarkdownEdit    *edit   () const { return edit_; }
  CQMarkdownPreview *preview() const { return preview_; }

  bool load(const QString &filename);

  bool saveMarkdown(const QString &filename="");

  bool saveHtml(const QString &filename);

  QString text() const;

 private slots:
  void updatePreview();

 private:
  QString            filename_;
  CQMarkdownEdit    *edit_    { nullptr };
  CQMarkdownPreview *preview_ { nullptr };
};

#endif
