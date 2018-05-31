#include <CQMarkdownEdit.h>
#include <CQMarkdown.h>
#include <CMarkdown.h>
#include <CQStrParse.h>

#include <QSyntaxHighlighter>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTimer>

#include <svg/normal_svg.h>
#include <svg/bold_svg.h>
#include <svg/italic_svg.h>
#include <svg/h1_svg.h>
#include <svg/h2_svg.h>
#include <svg/h3_svg.h>
#include <svg/h4_svg.h>
#include <svg/h5_svg.h>
#include <svg/h6_svg.h>
#include <svg/ul_svg.h>
#include <svg/ol_svg.h>
#include <svg/link_svg.h>
#include <svg/image_svg.h>

class CQMarkdownEditSyntaxHighlight : public QSyntaxHighlighter {
 public:
  struct StyleIndices {
    int  start  { 0 };
    int  len    { 0 };
    bool bold   { false };
    bool italic { false };
    bool strike { false };

    StyleIndices(int start1=0, int len1=0) :
     start(start1), len(len1) {
    }
  };

  typedef std::vector<StyleIndices> StyleIndicesList;

 public:
  CQMarkdownEditSyntaxHighlight(CQMarkdownEditText *text) :
   QSyntaxHighlighter(text->document()), text_(text) {
  }

  void highlightBlock(const QString &str) {
    CMarkdownBlock::ATXData atxData;
    CMarkdown::LinkRef      linkRef;
    int                     istart, iend;

    if      (CMarkdownParse::isATXHeader(str, atxData, istart, iend)) {
      QTextCharFormat fmt;

      fmt.setFontWeight(QFont::Bold);

      setFormat(istart, iend - istart + 1, fmt);
    }
    else if (CMarkdownParse::isLinkReference(str, linkRef, istart, iend)) {
      QTextCharFormat fmt;

      fmt.setForeground(QColor(0,0,255));

      setFormat(istart, iend - istart + 1, fmt);
    }
    else if (CMarkdownParse::isRule(str, istart, iend)) {
      QTextCharFormat fmt;

      fmt.setForeground(QColor(128,128,128));

      setFormat(istart, iend - istart + 1, fmt);
    }
    else {
      StyleIndicesList indicesList;

      getStyleIndices(str, indicesList, StyleIndices());

      for (const auto &indices : indicesList) {
        QTextCharFormat fmt;

        if (indices.italic)
          fmt.setFontItalic(true);

        if (indices.bold)
          fmt.setFontWeight(QFont::Bold);

        if (indices.strike)
          fmt.setFontStrikeOut(true);

        setFormat(indices.start, indices.len, fmt);
      }
    }
  }

  void getStyleIndices(const QString &str, StyleIndicesList &indicesList,
                       const StyleIndices &parentIndices) {
    int i   = 0;
    int len = str.length();

    while (i < len) {
      if (str[i] == '*' || str[i] == '_') {
        int istart = i;

        QString str1;
        int     start1;

        int nc = CMarkdownParse::parseSurroundText(str, i, str1, start1);

        if (nc > 0) {
          StyleIndices indices = parentIndices;

          indices.start = istart;
          indices.len   = i - istart + 1;

          if (nc == 1)
            indices.italic = true;
          else
            indices.bold = true;

          indicesList.push_back(indices);

          //---

          StyleIndicesList indicesList1;

          getStyleIndices(str1, indicesList1, indices);

          for (auto &indices1 : indicesList1) {
            indices1.start += start1;

            indicesList.push_back(indices1);
          }
        }
        else
          ++i;
      }
      else if (i < len - 1 && str[i] == '~' && str[i + 1] == '~') {
        int istart = i;

        QString str1;
        int     start1;

        int nc = CMarkdownParse::parseSurroundText(str, i, str1, start1);

        if (nc > 1) {
          StyleIndices indices = parentIndices;

          indices.start = istart;
          indices.len   = i - istart + 1;

          indices.strike = true;

          indicesList.push_back(indices);

          //---

          StyleIndicesList indicesList1;

          getStyleIndices(str1, indicesList1, indices);

          for (auto &indices1 : indicesList1) {
            indices1.start += start1;

            indicesList.push_back(indices1);
          }
        }
        else
          ++i;
      }
      else
        ++i;
    }
  }

 private:
  CQMarkdownEditText *text_ { nullptr };
};

//------

CQMarkdownEdit::
CQMarkdownEdit(CQMarkdown *markdown) :
 QFrame(markdown), markdown_(markdown)
{
  setObjectName("edit");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(0);

  toolbar_ = new CQMarkdownEditToolBar(this);

  layout->addWidget(toolbar_);

  edit_ = new CQMarkdownEditText(this);

  layout->addWidget(edit_);

  connect(edit_, SIGNAL(textChanged()), this, SLOT(updateSlot()));

  connect(edit_, SIGNAL(selectionChanged()), this, SLOT(selectionSlot()));

  timer_ = new QTimer;

  timer_->setSingleShot(true);

  connect(timer_, SIGNAL(timeout()), markdown_, SLOT(updatePreview()));
}

void
CQMarkdownEdit::
setText(const QString &text)
{
  edit_->setText(text);
}

QString
CQMarkdownEdit::
text() const
{
  return edit_->toPlainText();
}

void
CQMarkdownEdit::
updateSlot()
{
  timer_->start(100);
}

void
CQMarkdownEdit::
selectionSlot()
{
  bool selected = edit_->textCursor().hasSelection();

  toolbar_->updateState(selected);
}

void
CQMarkdownEdit::
normalSlot()
{
  QString text = edit_->textCursor().selectedText();
  if (text.isEmpty()) return;

  CQStrParse parse(text);

  parse.skipSpace();

  // get first non-highlight char position
  while (! parse.eof() && (parse.isChar('*') || parse.isChar('_')))
    parse.skipChar();

  int i1 = parse.getPos();

  // get last non-highlight char position
  parse.setPos(parse.getLen() - 1);

  parse.backSkipSpace();

  while (! parse.eof() && (parse.isChar('*') || parse.isChar('_')))
    parse.backSkipChar();

  int i2 = parse.getPos();

#if 0
  int i   = 0;
  int len = text.length();

  while (i < len && text[i].isSpace())
    ++i;

  while (i < len && (text[i] == '*' || text[i] == '_'))
    ++i;

  int i1 = i;

  i = len - 1;

  while (i > i1 && text[i].isSpace())
    --i;

  while (i > i1 && (text[i] == '*' || text[i] == '_'))
    --i;

  int i2 = i;
#endif

  QString text1 = text.mid(i1, i2 - i1 + 1);

  edit_->textCursor().insertText(text1);
}

void
CQMarkdownEdit::
boldSlot()
{
  QString text = edit_->textCursor().selectedText();
  if (text.isEmpty()) return;

  edit_->textCursor().insertText("**" + text + "**");
}

void
CQMarkdownEdit::
italicSlot()
{
  QString text = edit_->textCursor().selectedText();
  if (text.isEmpty()) return;

  edit_->textCursor().insertText("_" + text + "_");
}

void
CQMarkdownEdit::
headerSlot()
{
  QObject *obj = sender();

  int level = toolbar_->headerInd(qobject_cast<QToolButton *>(obj));
  if (level < 0) return;

  //--

  QString text = edit_->textCursor().selectedText();
  if (text.isEmpty()) return;

  int i   = 0;
  int len = text.length();

  while (i < len && text[i].isSpace())
    ++i;

  while (i < len && text[i] == '#')
    ++i;

  while (i < len && text[i].isSpace())
    ++i;

  int i1 = i;

  i = len - 1;

  while (i > i1 && text[i].isSpace())
    --i;

  while (i > i1 && text[i] == '#')
    --i;

  while (i > i1 && text[i].isSpace())
    --i;

  int i2 = i;

  QString text1 = " " + text.mid(i1, i2 - i1 + 1) + " ";

  for (int i = 0; i < level; ++i)
    text1 = "#" + text1 + "#";

  edit_->textCursor().insertText(text1);
}

void
CQMarkdownEdit::
ulSlot()
{
  QString text = edit_->textCursor().selectedText();
  if (text.isEmpty()) return;

  text.replace("\u2029", "\n");

  QStringList lines = text.split("\n");

  for (int i = 0; i < lines.size(); ++i) {
    lines[i] = removeListChars(lines[i]);

    lines[i] = " + " + lines[i];
  }

  edit_->textCursor().insertText(lines.join("\n"));
}

void
CQMarkdownEdit::
olSlot()
{
  QString text = edit_->textCursor().selectedText();
  if (text.isEmpty()) return;

  text.replace("\u2029", "\n");

  QStringList lines = text.split("\n");

  for (int i = 0; i < lines.size(); ++i) {
    lines[i] = removeListChars(lines[i]);

    lines[i] = QString(" %1. ").arg(i + 1) + lines[i];
  }

  edit_->textCursor().insertText(lines.join("\n"));
}

QString
CQMarkdownEdit::
removeListChars(const QString &str) const
{
  CQStrParse parse(str);

  parse.skipSpace();

  if (parse.isChar('+')) {
    parse.skipChar();

    parse.skipSpace();

    return parse.getAt();
  }
  else if (parse.isDigit()) {
    while (parse.isDigit())
      parse.skipChar();

    if (parse.isChar('.')) {
      parse.skipChar();

      parse.skipSpace();

      return parse.getAt();
    }
  }

  return str;
}

void
CQMarkdownEdit::
linkSlot()
{
  QString text = edit_->textCursor().selectedText();
  if (text.isEmpty()) return;

  QString text1 = "[link](" + text + ")";

  edit_->textCursor().insertText(text1);
}

void
CQMarkdownEdit::
imageSlot()
{
  QString text = edit_->textCursor().selectedText();
  if (text.isEmpty()) return;

  QString text1 = "![alt](" + text + " \"desc\")";

  edit_->textCursor().insertText(text1);
}

QSize
CQMarkdownEdit::
sizeHint() const
{
  return QSize(800, 1200);
}

//------

class CQMarkdownToolButton : public QToolButton {
 public:
  CQMarkdownToolButton(CQMarkdownEditToolBar *toolbar, const QString &name,
                       const QString &iconName, const QString &tip, const char *slotName) {
    setObjectName(name);
    setIcon(CQPixmapCacheInst->getIcon(iconName));
    setToolTip(tip);

    QObject::connect(this, SIGNAL(clicked()), toolbar->edit(), slotName);
  }
};

CQMarkdownEditToolBar::
CQMarkdownEditToolBar(CQMarkdownEdit *edit) :
 QFrame(edit), edit_(edit)
{
  setObjectName("toolbar");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(2); layout->setSpacing(2);

  //---

  normalButton_ =
    new CQMarkdownToolButton(this, "normal", "NORMAL", "Normal Text", SLOT(normalSlot()));

  layout->addWidget(normalButton_);

  //---

  boldButton_ =
    new CQMarkdownToolButton(this, "bold", "BOLD", "Bold Text", SLOT(boldSlot()));

  layout->addWidget(boldButton_);

  //---

  italicButton_ =
    new CQMarkdownToolButton(this, "italic", "ITALIC", "Italic Text", SLOT(italicSlot()));

  layout->addWidget(italicButton_);

  //---

  layout->addSpacing(8);

  //---

  hButtons_.resize(6);

  for (int i = 0; i < 6; ++i) {
    hButtons_[i] = new CQMarkdownToolButton(this,
      QString("hbutton%1").arg(i + 1), QString("H%1").arg(i + 1),
      QString("Heading %1").arg(i + 1), SLOT(headerSlot()));

    layout->addWidget(hButtons_[i]);
  }

  //---

  layout->addSpacing(8);

  //---

  ulButton_ =
    new CQMarkdownToolButton(this, "ul", "UL", "Unordered List", SLOT(ulSlot()));

  layout->addWidget(ulButton_);

  //---

  olButton_ =
    new CQMarkdownToolButton(this, "ol", "OL", "Ordered List", SLOT(olSlot()));

  layout->addWidget(olButton_);

  //---

  linkButton_ =
    new CQMarkdownToolButton(this, "link", "LINK", "Link", SLOT(linkSlot()));

  layout->addWidget(linkButton_);

  //---

  imageButton_ =
    new CQMarkdownToolButton(this, "image", "IMAGE", "Image", SLOT(imageSlot()));

  layout->addWidget(imageButton_);

  //---

  layout->addStretch(1);

  updateState(false);
}

void
CQMarkdownEditToolBar::
updateState(bool selected)
{
  normalButton_->setEnabled(selected);
  boldButton_  ->setEnabled(selected);
  italicButton_->setEnabled(selected);

  for (int i = 0; i < 6; ++i)
    hButtons_[i]->setEnabled(selected);
}

int
CQMarkdownEditToolBar::
headerInd(QToolButton *button) const
{
  for (int i = 0; i < 6; ++i)
    if (hButtons_[i] == button)
      return i + 1;

  return -1;
}

//------

CQMarkdownEditText::
CQMarkdownEditText(CQMarkdownEdit *edit) :
 QTextEdit(edit), edit_(edit)
{
  setObjectName("text");

  highlighter_ = new CQMarkdownEditSyntaxHighlight(this);
}

void
CQMarkdownEditText::
keyPressEvent(QKeyEvent *e)
{
  QKeySequence k(e->key() | e->modifiers());

  if      (k == QKeySequence("Ctrl+B"))
    edit_->boldSlot();
  else if (k == QKeySequence("Ctrl+I"))
    edit_->italicSlot();
  else if (k == QKeySequence("Ctrl+Space"))
    edit_->normalSlot();
  else
    QTextEdit::keyPressEvent(e);
}
