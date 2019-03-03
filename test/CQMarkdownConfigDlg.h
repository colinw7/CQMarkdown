#ifndef CQMarkdownConfigDlg_H
#define CQMarkdownConfigDlg_H

#include <CMarkdown.h>
#include <QDialog>

class CQMarkdownMain;
class QLineEdit;

class CQMarkdownConfigDlg : public QDialog {
  Q_OBJECT

 public:
  CQMarkdownConfigDlg(CQMarkdownMain *main);

 private slots:
  void okSlot();
  bool applySlot();
  void cancelSlot();

 private:
  struct TagEdit {
    QLineEdit* colorEdit;
    QLineEdit* fontEdit;
  };

  using TagEdits = std::map<CMarkdownTagType,TagEdit>;

  TagEdits        tagEdits_;
  CQMarkdownMain* main_ { nullptr };
};

#endif
