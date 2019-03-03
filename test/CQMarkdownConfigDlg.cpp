#include <CQMarkdownConfigDlg.h>
#include <CQMarkdownMain.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

CQMarkdownConfigDlg::
CQMarkdownConfigDlg(CQMarkdownMain *main) :
 main_(main)
{
  QVBoxLayout *layout = new QVBoxLayout(this);

  //---

  QGridLayout *styleLayout = new QGridLayout;

  layout->addLayout(styleLayout);

  auto addTagEdit = [&](QGridLayout *styleLayout, CMarkdownTagType type, int &row) {
    TagEdit &tagEdit = tagEdits_[type];

    QString name = CMarkdown::typeName(type);

    tagEdit.colorEdit = new QLineEdit;
    tagEdit.fontEdit  = new QLineEdit;

    styleLayout->addWidget(new QLabel(name) , row, 0);
    styleLayout->addWidget(tagEdit.colorEdit, row, 1);
    styleLayout->addWidget(tagEdit.fontEdit , row, 2);

    tagEdit.colorEdit->setText(CMarkdown::typeColor(type));
    tagEdit.fontEdit ->setText(CMarkdown::typeFont (type));

    ++row;
  };

  int row = 0;

  addTagEdit(styleLayout, CMarkdownTagType::H1, row);
  addTagEdit(styleLayout, CMarkdownTagType::H2, row);
  addTagEdit(styleLayout, CMarkdownTagType::H3, row);
  addTagEdit(styleLayout, CMarkdownTagType::H4, row);
  addTagEdit(styleLayout, CMarkdownTagType::H5, row);

  //---

  QHBoxLayout *buttonsLayout = new QHBoxLayout;

  QPushButton *okButton     = new QPushButton("OK");
  QPushButton *applyButton  = new QPushButton("Apply");
  QPushButton *cancelButton = new QPushButton("Cancel");

  connect(okButton, SIGNAL(clicked()), this, SLOT(okSlot()));
  connect(applyButton, SIGNAL(clicked()), this, SLOT(applySlot()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelSlot()));

  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(okButton);
  buttonsLayout->addWidget(applyButton);
  buttonsLayout->addWidget(cancelButton);

  layout->addLayout(buttonsLayout);
}

void
CQMarkdownConfigDlg::
okSlot()
{
  if (applySlot())
    cancelSlot();
}

bool
CQMarkdownConfigDlg::
applySlot()
{
  for (const auto &p : tagEdits_) {
    QString color = p.second.colorEdit->text();
    QString font  = p.second.fontEdit ->text();

    CMarkdown::setTypeColor(p.first, color);
    CMarkdown::setTypeFont (p.first, font );
  }

  main_->updateText();

  return true;
}

void
CQMarkdownConfigDlg::
cancelSlot()
{
  close();
}
