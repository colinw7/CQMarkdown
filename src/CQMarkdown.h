#ifndef CQMarkdown_H
#define CQMarkdown_H

#include <QSplitter>

class CQMarkdownEdit;
class CQMarkdownPreview;

class CQMarkdown : public QSplitter {
  Q_OBJECT

 public:
  CQMarkdown(QWidget *parent=0);

  bool load(const QString &filename);
  bool save();

  QString text() const;

 private slots:
  void updatePreview();

 private:
  QString            filename_;
  CQMarkdownEdit    *edit_;
  CQMarkdownPreview *preview_;
};

#endif
