#ifndef CQMarkdown_H
#define CQMarkdown_H

#include <QSplitter>

class CQMarkdownEdit;
class CQMarkdownPreview;

class CQMarkdown : public QSplitter {
  Q_OBJECT

 public:
  CQMarkdown(QWidget *parent=nullptr, bool ref=false);

  const QString &fileName() const { return fileName_; }
  void setFileName(const QString &v) { fileName_ = v; }

  CQMarkdownEdit    *edit   () const { return edit_; }
  CQMarkdownPreview *preview() const { return preview_; }

  bool load(const QString &filename);

  bool saveMarkdown(const QString &fileName="");

  bool saveHtml(const QString &fileName);

  QString text() const;

 private slots:
  void updatePreview();

 private:
  QString            fileName_;
  CQMarkdownEdit    *edit_    { nullptr };
  CQMarkdownPreview *preview_ { nullptr };
};

#endif
