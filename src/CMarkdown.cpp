#include <CMarkdown.h>
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <set>
#include <iostream>
#include <cassert>

CMarkdown::
CMarkdown()
{
}

void
CMarkdown::
setDebug(bool d)
{
  debug_ = d;
}

QString
CMarkdown::
fileToHtml(const QString &filename)
{
  return fileToFormat(filename, Format::HTML);
}

QString
CMarkdown::
fileToTty(const QString &filename)
{
  return fileToFormat(filename, Format::TTY);
}

QString
CMarkdown::
fileToFormat(const QString &filename, Format format)
{
  QFile file(filename);

  if (! file.open(QFile::ReadOnly | QFile::Text))
    return "";

  QTextStream stream(&file);

  return textToFormat(stream.readAll(), format);
}

QString
CMarkdown::
textToHtml(const QString &str)
{
  return textToFormat(str, Format::HTML);
}

QString
CMarkdown::
textToTty(const QString &str)
{
  return textToFormat(str, Format::TTY);
}

QString
CMarkdown::
textToFormat(const QString &str, Format format)
{
  delete rootBlock_;

  rootBlock_ = new CMarkdownBlock(this);

  //---

  str_ = str;
  pos_ = 0;
  len_ = str_.length();

  QString line;

  while (readLine(line))
    rootBlock_->addLine(line);

  rootBlock_->preProcess();

  return rootBlock_->process(format);
}

void
CMarkdown::
addLink(const LinkRef &link)
{
  if (isDebug())
    std::cerr << "DEBUG: Add Link: [" << link.ref.toStdString() << "]:" <<
                 link.dest.toStdString() << " " <<
                 "'" << link.title.toStdString() << "'\n";

  links_[link.ref.toLower()] = link;
}

bool
CMarkdown::
getLink(const QString &ref, LinkRef &link) const
{
  if (isDebug())
    std::cerr << "DEBUG: Get Link: " << ref.toStdString() << "\n";

  QString lref = ref.toLower();

  auto p = links_.find(lref);

  if (p == links_.end())
    return false;

  link = (*p).second;

  return true;
}

bool
CMarkdown::
readLine(QString &line)
{
  if (pos_ >= len_)
    return false;

  line = "";

  while (pos_ < len_ && str_[pos_] != '\n')
    line += str_[pos_++];

  if (pos_ < len_ && str_[pos_] == '\n')
    ++pos_;

  return true;
}

//------

bool
CMarkdown::
isSingleLineType(CMarkdownTagType type)
{
  const CMarkdownTagData &data = CMarkdown::getTagData(type);

  return data.singleLine;
}

bool
CMarkdown::
isRecurseType(CMarkdownTagType type)
{
  const CMarkdownTagData &data = CMarkdown::getTagData(type);

  return data.recurse;
}

QString
CMarkdown::
typeColor(CMarkdownTagType type)
{
  const CMarkdownTagData &data = CMarkdown::getTagData(type);

  return data.color;
}

void
CMarkdown::
setTypeColor(CMarkdownTagType type, const QString &color)
{
  CMarkdownTagData &data = CMarkdown::getTagData(type);

  data.color = color;
}

QString
CMarkdown::
typeFont(CMarkdownTagType type)
{
  const CMarkdownTagData &data = CMarkdown::getTagData(type);

  return data.font;
}

void
CMarkdown::
setTypeFont(CMarkdownTagType type, const QString &font)
{
  CMarkdownTagData &data = CMarkdown::getTagData(type);

  data.font = font;
}

CMarkdownTagType
CMarkdown::
stringToType(const QString &str)
{
  QString lstr = str.toLower();

  const TagDatas &tagDatas = CMarkdown::getTagDatas();

  for (const auto &p : tagDatas) {
    const CMarkdownTagData &tagData = p.second;

    if (tagData.name == lstr)
      return tagData.type;
  }

  return CMarkdownTagType::NONE;
}

QString
CMarkdown::
typeName(CMarkdownTagType type)
{
  const CMarkdownTagData &data = getTagData(type);

  return data.name;
}

CMarkdownTagData &
CMarkdown::
getTagData(CMarkdownTagType type)
{
  TagDatas &tagDatas = getTagDatas();

  auto p = tagDatas.find(type);
  assert(p != tagDatas.end());

  return (*p).second;
}

CMarkdown::TagDatas &
CMarkdown::
getTagDatas()
{
  static TagDatas tagDatas;

  if (tagDatas.empty()) {
    auto addTagData = [&](CMarkdownTagType type, const QString &typeName,
                          bool singleLine, bool recurse) {
      CMarkdownTagData data(type, typeName);

      data.singleLine = singleLine;
      data.recurse    = recurse;

      tagDatas[type] = data;
    };

    //---

    //         type,                         name,         singleLine, recurse
    addTagData(CMarkdownTagType::DOCUMENT  , "document"  , false     , true );
    addTagData(CMarkdownTagType::P         , "p"         , true      , false);
    addTagData(CMarkdownTagType::BLOCKQUOTE, "blockquote", false     , true );
    addTagData(CMarkdownTagType::H1        , "h1"        , true      , false);
    addTagData(CMarkdownTagType::H2        , "h2"        , true      , false);
    addTagData(CMarkdownTagType::H3        , "h3"        , true      , false);
    addTagData(CMarkdownTagType::H4        , "h4"        , true      , false);
    addTagData(CMarkdownTagType::H5        , "h5"        , true      , false);
    addTagData(CMarkdownTagType::H6        , "h6"        , true      , false);
    addTagData(CMarkdownTagType::UL        , "ul"        , false     , false);
    addTagData(CMarkdownTagType::OL        , "ol"        , false     , false);
    addTagData(CMarkdownTagType::LI        , "li"        , true      , true );
    addTagData(CMarkdownTagType::PRE       , "pre"       , false     , true );
    addTagData(CMarkdownTagType::CODE      , "code"      , false     , false);
    addTagData(CMarkdownTagType::TABLE     , "table"     , false     , true );
    addTagData(CMarkdownTagType::TR        , "tr"        , false     , false);
    addTagData(CMarkdownTagType::TD        , "td"        , false     , false);
    addTagData(CMarkdownTagType::HR        , "hr"        , true      , false);
    addTagData(CMarkdownTagType::EM        , "em"        , false     , false);
    addTagData(CMarkdownTagType::STRONG    , "strong"    , false     , false);
    addTagData(CMarkdownTagType::STRIKE    , "strike"    , false     , false);
    addTagData(CMarkdownTagType::A         , "a"         , false     , false);
  }

  return tagDatas;
}

//------

CMarkdownBlock::
CMarkdownBlock(CMarkdown *markdown) :
 markdown_(markdown), parent_(nullptr), type_(CMarkdownTagType::DOCUMENT)
{
}

CMarkdownBlock::
CMarkdownBlock(CMarkdownBlock *parent, CMarkdownTagType type) :
 markdown_(nullptr), parent_(parent), type_(type)
{
}

CMarkdownBlock::
~CMarkdownBlock()
{
  for (auto &b : blocks_)
    delete b;
}

void
CMarkdownBlock::
addBlock(CMarkdownBlock *block)
{
  blocks_.push_back(block);
}

void
CMarkdownBlock::
addLine(const Line &line)
{
  lines_.push_back(line);
}

void
CMarkdownBlock::
appendLine(const QString &line)
{
  assert(! lines_.empty());

  lines_.back().line += line;
}

void
CMarkdownBlock::
preProcess()
{
  currentLine_ = 0;

  while (currentLine_ < int(lines_.size())) {
    // read line (tabs converted to 4 spaces)
    LineData line1;

    if (! getLine(line1))
      break;

    LinkRef linkRef;
    int     istart, iend;

    if (CMarkdownParse::isLinkReference(line1.line, linkRef, istart, iend)) {
      markdown()->addLink(linkRef);
    }
  }
}

QString
CMarkdownBlock::
process(CMarkdown::Format format)
{
  if (! CMarkdown::isRecurseType(type_) || processed_)
    return processedText_;

  currentLine_  = 0;

  rootBlock_    = this;
  currentBlock_ = rootBlock_;

  processedText_ = processLines(format);

  processed_ = true;

  return processedText_;
}

QString
CMarkdownBlock::
processLines(CMarkdown::Format format)
{
  QString processedText = "";

  int       indent;
  ATXData   atxData;
  LinkRef   linkRef;
  QString   text;
  ListData  list;
  int       istart, iend;

  while (currentLine_ < int(lines_.size())) {
    // read line (tabs converted to 4 spaces)
    LineData line1;

    if (! getLine(line1))
      break;

    if (markdown()->isDebug())
      std::cerr << "DEBUG: Line: '" << line1.line.toStdString() << "'\n";

    //---

    CodeFence fence;

    if      (CMarkdownParse::isBlankLine(line1.line)) {
      endBlock();
    }
    else if (isStartCodeFence(line1.line, fence)) {
      flushBlocks();

      CMarkdownBlock *block = startBlock(CMarkdownTagType::PRE);

      startBlock(CMarkdownTagType::CODE);

      LineData line2;

      while (getLine(line2)) {
        if (isEndCodeFence(line2.line, fence))
          break;

        addBlockLine(line2.line);
      }

      endBlock();
      endBlock();

      processedText += block->toText(format);
    }
    else if (CMarkdownParse::isRule(line1.line, istart, iend)) {
      endBlock();

      CMarkdownBlock *block = startBlock(CMarkdownTagType::HR);

      endBlock();

      processedText += block->toText(format);
    }
    else if (isHtmlLine(line1.line)) {
      flushBlocks();

      processedText += line1.line + "\n";

      LineData line2;

      while (getLine(line2)) {
        if (CMarkdownParse::isBlankLine(line2.line))
          break;

        processedText += line2.line + "\n";
      }

      processedText += "\n";
    }
    else if (CMarkdownParse::isLinkReference(line1.line, linkRef, istart, iend)) {
      endBlock();

      int ind = linkRef.dest.indexOf("#");

      if (ind == 0) {
        QString ref1 = linkRef.dest.mid(1);

        // should match linkRef.ref ?
        if (format == CMarkdown::Format::HTML)
          processedText += QString("<a name=\"%1\"></a>\n").arg(ref1);
        else
          processedText += ref1;
      }

      //markdown()->addLink(linkRef);
    }
    else if (isUnorderedListLine(line1.line, list)) {
      endBlock();

      processedText += processList(CMarkdownTagType::UL, list, format);
    }
    else if (isOrderedListLine(line1.line, list)) {
      endBlock();

      processedText += processList(CMarkdownTagType::OL, list, format);
    }
    else if (CMarkdownParse::isATXHeader(line1.line, atxData, istart, iend)) {
      endBlock();

      CMarkdownBlock *block = startBlock(atxData.type);

      addBlockLine(atxData.text);

      endBlock();

      processedText += block->toText(format);
    }
    else if (isIndentLine(line1.line, indent)) {
      flushBlocks();

      CMarkdownBlock *block = startBlock(CMarkdownTagType::PRE);

      startBlock(CMarkdownTagType::CODE);

      addBlockLine(line1.line.mid(indent));

      LineData line2;

      while (getLine(line2)) {
        if      (isIndentLine(line2.line, indent))
          addBlockLine(line2.line.mid(indent));
        else if (CMarkdownParse::isBlankLine(line2.line))
          addBlockLine(line2.line);
        else {
          ungetLine();
          break;
        }
      }

      endBlock();
      endBlock();

      processedText += block->toText(format);
    }
    else if (isBlockQuote(line1.line, text)) {
      CMarkdownBlock *block = startBlock(CMarkdownTagType::BLOCKQUOTE);

      addBlockLine(text);

      LineData line2;

      QString quote1;

      while (getLine(line2)) {
        if      (isContinuationLine(line2.line)) {
          appendBlockLine(line2.line);
        }
        else if (isBlockQuote(line2.line, quote1)) {
          addBlockLine(quote1);
        }
        else {
          ungetLine();
          break;
        }
      }

      endBlock();

      processedText += block->toText(format);
    }
    else if (isTableLine(line1.line)) {
      CMarkdownBlock *block = startBlock(CMarkdownTagType::TABLE);

      parseTableLine(line1.line);

      LineData line2;

      while (getLine(line2)) {
        if (isTableLine(line2.line))
          parseTableLine(line2.line);
        else {
          ungetLine();
          break;
        }
      }

      endBlock();

      processedText += block->toText(format);
    }
    else {
      endBlock();

      CMarkdownBlock *block = startBlock(CMarkdownTagType::P);

      addBlockLine(line1.line, line1.brk);

      int nl = 0;

      LineData line2;

      while (getLine(line2)) {
        if (CMarkdownParse::isBlankLine(line2.line))
          break;

        CMarkdownTagType type;

        if      (nl == 0 && isSetTextLine(line2.line, type)) {
          endBlock(); // remove block

          int i = 0;

          CMarkdownParse::skipSpace(line1.line, i);

          if (i > 0)
            line1.line = line1.line.mid(i);

          CMarkdownBlock *block = startBlock(type);

          addBlockLine(line1.line);

          endBlock();

          processedText += block->toText(format);

          nl = -1;

          break;
        }
        else if (isFormatLine(line2.line)) {
          ungetLine();
          break;
        }

        addBlockLine(line2.line.mid(line2.indent), line2.brk);

        ++nl;
      }

      if (nl >= 0) {
        endBlock();

        processedText += block->toText(format);
      }
    }
  }

  endBlock();

  return processedText;
}

QString
CMarkdownBlock::
processList(CMarkdownTagType type, const ListData &list, CMarkdown::Format format)
{
  QString res;

  CMarkdownBlock *block = startBlock(type);

  startBlock(CMarkdownTagType::LI);

  addBlockLine(list.text);

  int numBlankLines = 0;

  LineData line2;

  while (getLine(line2)) {
    ListData list1;

    if      (isUnorderedListLine(line2.line, list1)) {
      if (type == CMarkdownTagType::UL) {
        if (numBlankLines > 0) {
          // add empty list item
          endBlock(); // LI

          startBlock(CMarkdownTagType::LI);
        }

        if (list1.indent >= list.indent && list1.c == list.c) {
          // start child list if indent 2 or more greater
          if (list1.indent >= list.indent + 2) {
            endBlock(); // LI

            processList(CMarkdownTagType::UL, list1, format);

            startBlock(CMarkdownTagType::LI);

            numBlankLines = 0;
          }
          else {
            endBlock(); // LI

            startBlock(CMarkdownTagType::LI);

            addBlockLine(list1.text);
          }
        }
        else {
          ungetLine();
          break;
        }

        numBlankLines = 0;
      }
      else {
        if (list1.indent >= list.indent) {
          endBlock(); // LI

          processList(CMarkdownTagType::UL, list1, format);

          startBlock(CMarkdownTagType::LI);

          numBlankLines = 0;
        }
        else {
          ungetLine();
          break;
        }
      }
    }
    else if (isOrderedListLine(line2.line, list1)) {
      if (type == CMarkdownTagType::OL) {
        if (numBlankLines > 0) {
          // add empty list item
          endBlock(); // LI

          startBlock(CMarkdownTagType::LI);
        }

        if (list1.indent >= list.indent && list1.c == list.c) {
          // start child list if indent 2 or more greater
          if (list1.indent >= list.indent + 2) {
            endBlock(); // LI

            processList(CMarkdownTagType::OL, list1, format);

            startBlock(CMarkdownTagType::LI);

            numBlankLines = 0;
          }
          // start new list item
          else {
            endBlock(); // LI

            startBlock(CMarkdownTagType::LI);

            addBlockLine(list1.text);
          }
        }
        else {
          ungetLine();
          break;
        }

        numBlankLines = 0;
      }
      else {
        if (list1.indent >= list.indent) {
          endBlock(); // LI

          processList(CMarkdownTagType::OL, list1, format);

          startBlock(CMarkdownTagType::LI);

          numBlankLines = 0;
        }
        else {
          ungetLine();
          break;
        }
      }
    }
    else if (CMarkdownParse::isBlankLine(line2.line)) {
      ++numBlankLines;

      if (numBlankLines > 1)
        break;
    }
    else if (line2.indent >= list.indent) {
      if (numBlankLines > 0) {
        addBlockLine("");
      }

      if (line2.brk)
        addBlockLine(line2.line.mid(list.indent) + "  ");
      else
        addBlockLine(line2.line.mid(list.indent));

      numBlankLines = 0;
    }
    else {
      ungetLine();
      break;
    }
  }

  endBlock(); // LI
  endBlock(); // UL, OL

  res += block->toText(format);

  return res;
}

bool
CMarkdownBlock::
isContinuationLine(const QString &str) const
{
  int len = str.length();

  if (len == 0 || CMarkdownParse::isASCIIPunct(str[0]))
    return false;

  return true;
}

bool
CMarkdownBlock::
isSetTextLine(const QString &str, CMarkdownTagType &type) const
{
  int i   = 0;
  int len = str.length();

  // up to 3 spaces
  if (CMarkdownParse::skipSpace(str, i) > 3)
    return false;

  // followed by '=' or '-'
  if (i >= len || (str[i] != '=' && str[i] != '-'))
    return false;

  QChar c = str[i];

  (void) CMarkdownParse::skipChar(str, i, c);

  (void) CMarkdownParse::skipSpace(str, i);

  // must be end of line
  if (i < len)
    return false;

  type = (c == '=' ? CMarkdownTagType::H1 : CMarkdownTagType::H2);

  return true;
}

bool
CMarkdownBlock::
isIndentLine(const QString &str, int &n) const
{
  int len = str.length();

  int i = 0;

  n = CMarkdownParse::skipSpace(str, i);

  if (n >= 4 && i != len)
    return true;

  return false;
}

bool
CMarkdownBlock::
isFormatLine(const QString &str) const
{
  int len = str.length();

  int i = 0;

  // skip space and detect indent line
  if (CMarkdownParse::skipSpace(str, i) >= 4 && i != len)
    return true;

  // starts with format char
  if (! isFormatChar(str, i))
    return false;

  // has any format chars
  for ( ; i < len; ++i)
    if (isFormatChar(str, i))
      return true;

  return false;
}

bool
CMarkdownBlock::
isFormatChar(const QString &str, int i) const
{
  if (str[i] == '>') return true; // block quote
  if (str[i] == '+') return true; // unordered list
  if (str[i] == '-') return true; // unordered list
  if (str[i] == '*') return true; // unordered list
  if (str[i] == '#') return true; // header
  if (str[i] == '|') return true; // table

  if (str[i] == '`' || str[i] == '~') { // code fence
    int j = i;

    QChar c = str[i];

    if (CMarkdownParse::skipChar(str, j, c) >= 3)
      return true;
  }

  int len = str.length();

  if (i < len && str[i].isDigit()) {
    int j = i + 1;

    while (j < len && str[j].isDigit())
      ++j;

    if (j < len && (str[j] == '.' || str[j] == ')')) // ordered list
      return true;
  }

  return false;
}

bool
CMarkdownBlock::
isStartCodeFence(const QString &str, CodeFence &fence) const
{
  int len = str.length();

  int i = 0;

  if (CMarkdownParse::skipSpace(str, i) > 3)
    return false;

  if (i >= len)
    return false;

  fence.c = str[i++];

  if (fence.c != '`' && fence.c != '~')
    return false;

  fence.n = CMarkdownParse::skipChar(str, i, fence.c) + 1;

  if (fence.n < 3)
    return false;

  CMarkdownParse::skipSpace(str, i);

  fence.info = "";

  while (i < len) {
    if (str[i] == '`')
      return false;

    fence.info += str[i++];
  }

  fence.info = fence.info.simplified();

  return true;
}

bool
CMarkdownBlock::
isEndCodeFence(const QString &str, const CodeFence &fence) const
{
  int len = str.length();

  int i = 0;

  if (CMarkdownParse::skipSpace(str, i) > 3)
    return false;

  if (i >= len)
    return false;

  if (str[i] != fence.c)
    return false;

  ++i;

  if (CMarkdownParse::skipChar(str, i, fence.c) < fence.n - 1)
    return false;

  return true;
}

bool
CMarkdownBlock::
isHtmlLine(const QString &str) const
{
  using NameSet = std::set<QString>;

  NameSet nameSet;

  if (nameSet.empty()) {
    std::vector<QString> names = {{
      "article", "header", "aside", "hgroup", "blockquote", "hr", "iframe", "img", "body",
      "map", "button", "object", "canvas", "caption", "output", "col", "p", "colgroup",
      "pre", "dd", "progress", "div", "section", "dl", "table", "td", "dt", "tbody",
      "embed", "textarea", "fieldset", "tfoot", "figcaption", "th", "figure", "thead",
      "footer", "tr", "form", "h1", "h2", "h3", "h4", "h5", "h6", "ol", "ul", "li",
      "video", "script", "style" }};

    for (const auto &name : names)
      nameSet.insert(name);
  }

  int len = str.length();

  int i = 0;

  if (CMarkdownParse::skipSpace(str, i) > 3)
    return false;

  if (i >= len)
    return false;

  if (str[i] != '<')
    return false;

  ++i;

  QString name;

  while (i < len && str[i].isLetter())
    name += str[i++];

  QString lname = name.toLower();

  auto p = nameSet.find(lname);

  return (p != nameSet.end());
}

bool
CMarkdownBlock::
isBlockQuote(const QString &str, QString &quote) const
{
  int len = str.length();

  int i = 0;

  if (CMarkdownParse::skipSpace(str, i) >= 4)
    return false;

  if (i >= len || str[i] != '>')
    return false;

  ++i;

  if (i < len && str[i].isSpace())
    quote = str.mid(i + 1);
  else
    quote = str.mid(i);

  return true;
}

bool
CMarkdownBlock::
isUnorderedListLine(const QString &str, ListData &list) const
{
  int len = str.length();

  int i = 0;

  if (CMarkdownParse::skipSpace(str, i) >= 4)
    return false;

  if (i >= len - 1)
    return false;

  if (str[i] != '-' && str[i] != '+' && str[i] != '*')
    return false;

  list.c = str[i];

  ++i;

  if (! str[i].isSpace())
    return false;

  ++i;

  CMarkdownParse::skipSpace(str, i);

  list.indent = i;

  list.text = str.mid(i);

  return true;
}

bool
CMarkdownBlock::
isOrderedListLine(const QString &str, ListData &list) const
{
  int len = str.length();

  int i = 0;

  if (CMarkdownParse::skipSpace(str, i) >= 4)
    return false;

  if (i >= len - 2)
    return false;

  if (i >= len || ! str[i].isDigit())
    return false;

  QString num;

  while (i < len && str[i].isDigit())
    num += str[i++];

  list.n = num.toInt();

  if (i >= len || (str[i] != '.' && str[i] != ')'))
    return false;

  list.c = str[i++];

  if (i >= len || ! str[i].isSpace())
    return false;

  ++i;

  CMarkdownParse::skipSpace(str, i);

  list.indent = i;

  list.text = str.mid(i);

  return true;
}

bool
CMarkdownBlock::
isTableLine(const QString &str) const
{
  int len = str.length();

  int i = 0;

  if (CMarkdownParse::skipSpace(str, i) >= 4)
    return false;

  if (i >= len || str[i] != '|')
    return false;

  return true;
}

void
CMarkdownBlock::
parseTableLine(const QString &str)
{
  int len = str.length();

  int i = 0;

  CMarkdownParse::skipSpace(str, i);
  assert(i < 4);

  assert(i < len && str[i] == '|');

  using Words = std::vector<QString>;

  Words words;

  ++i;

  QString word;

  while (i < len) {
    if (str[i] == '|') {
      ++i;

      words.push_back(word);

      word = "";
    }
    else
      word += str[i++];
  }

  if (words.empty())
    return;

  startBlock(CMarkdownTagType::TR);

  for (const auto &word : words) {
    startBlock(CMarkdownTagType::TD);

    addBlockLine(word);

    endBlock();
  }

  endBlock();
}

QString
CMarkdownBlock::
replaceEmbeddedStyles(const QString &str, bool code, CMarkdown::Format format) const
{
  QString str1;

  int i   = 0;
  int len = str.length();

  while (i < len) {
    // escape
    if      (i < len - 1 && str[i] == '\\' && CMarkdownParse::isASCIIPunct(str[i + 1])) {
      ++i;

      if      (str[i] == '<') {
        str1 += "&lt;"; ++i;
      }
      else if (str[i] == '>') {
        str1 += "&gt;"; ++i;
      }
      else if (str[i] == '"') {
        str1 += "&quot;"; ++i;
      }
      else if (str[i] == '&') {
        str1 += "&amp;"; ++i;
      }
      else
        str1 += str[i++];
    }
    // emphasis
    else if (! code && (str[i] == '*' || str[i] == '_')) {
      QString str2;
      int     start2;

      int nc = CMarkdownParse::parseSurroundText(str, i, str2, start2);

      if (nc > 0) {
        QString str3 = replaceEmbeddedStyles(str2, /*code*/false, format);

        if (nc == 1)
          str1 += emphasisText(str3, format);
        else
          str1 += boldText(str3, format);
      }
      else {
        str1 += str[i++];
      }
    }
    // strike
    else if (! code && (i < len - 1 && str[i] == '~' && str[i + 1] == '~')) {
      QString str2;
      int     start2;

      int nc = CMarkdownParse::parseSurroundText(str, i, str2, start2);

      if (nc > 1) {
        QString str3 = replaceEmbeddedStyles(str2, /*code*/false, format);

        str1 += strikeText(str3, format);
      }
      else {
        str1 += str[i++];
        str1 += str[i++];
      }
    }
    // code
    else if (! code && (str[i] == '`')) {
      QString str2;
      int     start2;

      int nc = CMarkdownParse::parseSurroundText(str, i, str2, start2);

      if (nc > 0) {
        QString str3 = replaceEmbeddedStyles(str2, /*code*/true, format);

        if (format == CMarkdown::Format::HTML)
          str1 += QString("<code>%1</code>").arg(str3);
        else
          str1 += QString("%1").arg(str3);
      }
      else {
        str1 += str[i++];
      }
    }
    // image link
    else if (! code && (i < len - 1 && str[i] == '!' && str[i + 1] == '[')) {
      int i1 = i;

      i += 2;

      QString str2;

      while (i < len && str[i] != ']')
        str2 += str[i++];

      if (i < len && str[i] == ']') {
        ++i;

        QString str3, str4;

        if      (i < len && str[i] == '(') {
          ++i;

          while (i < len && str[i] != ')') {
            if (str[i] == '\"')
              break;

            str3 += str[i++];
          }

          if (i < len && str[i] == '\"') {
            ++i;

            while (i < len && str[i] != ')') {
              if (str[i] == '\"')
                break;

              str4 += str[i++];
            }
          }

          while (i < len && str[i] != ')')
            ++i;

          if (i < len && str[i] == ')') {
            ++i;

            str3 = str3.simplified();

            // TODO: title
            if (format == CMarkdown::Format::HTML) {
              if (str4 != "") {
                if (str2 != "")
                  str1 += QString("<img src=\"%1\" title=\"%2\" alt=\"%3\"/>").
                            arg(imageSrc(str3)).arg(str4).arg(str2);
                else
                  str1 += QString("<img src=\"%1\" title=\"%2\"/>").
                            arg(imageSrc(str3)).arg(str4);
              }
              else {
                if (str2 != "")
                  str1 += QString("<img src=\"%1\" alt=\"%2\"/>").arg(imageSrc(str3)).arg(str2);
                else
                  str1 += QString("<img src=\"%1\"/>").arg(imageSrc(str3));
              }
            }
            else {
              str1 += QString("%1").arg(imageSrc(str3));
            }
          }
          else {
            i = i1;

            str1 += str[i++];
          }
        }
        else if (i < len && str[i] == '[') {
          ++i;

          while (i < len && str[i] != ']')
            str3 += str[i++];

          if (i < len && str[i] == ']') {
            ++i;

            if (format == CMarkdown::Format::HTML) {
              if (str2 != "")
                str1 += QString("<img src=\"%1\" alt=\"%2\"/>").arg(imageSrc(str3)).arg(str2);
              else
                str1 += QString("<img src=\"%1\"/>").arg(imageSrc(str3));
            }
            else {
              str1 += QString("%1").arg(imageSrc(str3));
            }
          }
          else {
            i = i1;

            str1 += str[i++];
          }
        }
        else {
          LinkRef ref;

          if (markdown()->getLink(str2, ref)) {
            if (format == CMarkdown::Format::HTML) {
              if (ref.title != "") {
                if (ref.ref != "")
                  str1 += QString("<img src=\"%1\" alt=\"%2\" title=\"%3\"/>").
                           arg(imageSrc(ref.dest)).arg(ref.ref).arg(ref.title);
                else
                  str1 += QString("<img src=\"%1\" title=\"%3\"/>").
                           arg(imageSrc(ref.dest)).arg(ref.title);
              }
              else {
                if (ref.ref != "")
                  str1 += QString("<img src=\"%1\" alt=\"%2\"/>").
                           arg(imageSrc(ref.dest)).arg(ref.ref);
                else
                  str1 += QString("<img src=\"%1\"/>").
                           arg(imageSrc(ref.dest));
              }
            }
            else {
              str1 += QString("%1").arg(imageSrc(ref.dest));
            }
          }
          else {
            i = i1;

            str1 += str[i++];
          }
        }
      }
      else {
        i = i1;

        str1 += str[i++];
      }
    }
    // link
    else if (! code && str[i] == '[') {
      int i1 = i;

      ++i;

      // link text
      QString str2;

      while (i < len && str[i] != ']')
        str2 += str[i++];

      if (i < len && str[i] == ']') {
        ++i;

        QString str3;

        // '(' href "title" ')'
        if      (i < len && str[i] == '(') {
          ++i;

          while (i < len && str[i] != ')')
            str3 += str[i++];

          if (i < len && str[i] == ')') {
            ++i;

            // split into href and title
            QString href, title;

            splitLinkRef(str3, href, title);

            str1 += anchorText(href, title, str2, format);
          }
          else {
            i = i1;

            str1 += str[i++];
          }
        }
        // '[' href ']'
        else if (i < len && str[i] == '[') {
          ++i;

          while (i < len && str[i] != ']')
            str3 += str[i++];

          if (i < len && str[i] == ']') {
            ++i;

            LinkRef ref;

            if (markdown()->getLink(str3, ref))
              str1 += anchorText(ref.dest, ref.title, str2, format);
            else
              str1 += anchorText(str3, "", str2, format);
          }
          else {
            i = i1;

            str1 += str[i++];
          }
        }
        // no href so lookup
        else {
          LinkRef ref;

          if (markdown()->getLink(str2, ref))
            str1 += anchorText(ref.dest, ref.title, ref.ref, format);
          else {
            i = i1;

            str1 += str[i++];
          }
        }
      }
      else {
        i = i1;

        str1 += str[i++];
      }
    }
    // TODO: auto links

    // escape special chars
    else if (str[i] == '<') {
      QString ref;

      if (isAutoLink(str, i, ref)) {
        QString ref1 = replaceEmbeddedStyles(ref, /*code*/false, format);

        str1 += anchorText(ref, "", ref1, format);
      }
      else {
        str1 += "&lt;"; ++i;
      }
    }
    else if (str[i] == '>') {
      str1 += "&gt;"; ++i;
    }
    else if (str[i] == '"') {
      str1 += "&quot;"; ++i;
    }
    else if (str[i] == '&') {
      str1 += "&amp;"; ++i;
    }
    else
      str1 += str[i++];
  }

  return str1;
}

QString
CMarkdownBlock::
imageSrc(const QString &filename) const
{
  return QUrl::fromLocalFile(filename).toEncoded();
  //return QString("file:%1").arg(filename);
}

void
CMarkdownBlock::
splitLinkRef(const QString &str, QString &href, QString &title) const
{
  int i   = 0;
  int len = str.size();

  href = "";

  while (i < len && ! str[i].isSpace()) {
    href += str[i++];
  }

  CMarkdownParse::skipSpace(str, i);

  // check for title
  title = "";

  if (i < len && (str[i] == '"' || str[i] == '\'')) {
    QChar c = str[i++];

    while (i < len && str[i] != c)
      title += str[i++];

    if (str[i] == c) {
      ++i;
    }
  }

  // TODO: error handling
}

QString
CMarkdownBlock::
replaceHtmlChars(const QString &str) const
{
  QString str1;

  int i   = 0;
  int len = str.length();

  while (i < len) {
    if      (str[i] == '<') {
      str1 += "&lt;"; ++i;
    }
    else if (str[i] == '>') {
      str1 += "&gt;"; ++i;
    }
    else if (str[i] == '"') {
      str1 += "&quot;"; ++i;
    }
    else if (str[i] == '&') {
      str1 += "&amp;"; ++i;
    }
    else
      str1 += str[i++];
  }

  return str1;
}

bool
CMarkdownBlock::
isAutoLink(const QString &str, int &i, QString &ref) const
{
  int i1 = i;

  int len = str.length();

  if (i1 >= len || str[i1] != '<')
    return false;

  CMarkdownParse::skipSpace(str, i1);

  ++i1;

  QString scheme;

  while (i1 < len && ! str[i1].isSpace() && str[i1] != ':') {
    if (! str[i1].isLetter())
      return false;

    scheme += str[i1++];
  }

  CMarkdownParse::skipSpace(str, i1);

  if (i1 >= len || str[i1] != ':')
    return false;

  ++i1;

  CMarkdownParse::skipSpace(str, i1);

  QString ref1;

  while (i1 < len && str[i1] != '>')
    ref1 += str[i1++];

  if (i1 >= len || str[i1] != '>')
    return false;

  ++i1;

  ref = QString("%1:%2").arg(scheme).arg(ref1);

  //ref = str.mid(i + 1, i1 - i - 2);

  i = i1;

  return true;
}

bool
CMarkdownBlock::
getLine(LineData &line)
{
  line.brk = false;

  if (currentLine_ >= int(lines_.size()))
    return false;

  QString str = lines_[currentLine_++].line;

  QString spaces;
  int     ns = 0;

  int len = str.size();

  int i = 0;

  line.line  = "";
  line.blank = true;

  while (i < len && str[i] != '\n') {
    if (str[i].isSpace()) {
      // expand tabs
      if (str[i] == '\t')
        spaces += "    ";
      else
        spaces += " ";

      ++ns;
      ++i;
    }
    else {
      line.line += spaces;

      spaces = "";
      ns     = 0;

      line.blank = false;

      line.line += str[i++];
    }
  }

  if (ns >= 2)
    line.brk = true;

  line.indent = 0;

  CMarkdownParse::skipSpace(line.line, line.indent);

  return true;
}

void
CMarkdownBlock::
ungetLine()
{
  assert(currentLine_ > 0);

  --currentLine_;
}

CMarkdownBlock *
CMarkdownBlock::
startBlock(CMarkdownTagType type)
{
  CMarkdownBlock *block = new CMarkdownBlock(currentBlock_, type);

  currentBlock_->addBlock(block);

  currentBlock_ = block;

  if (markdown()->isDebug())
    std::cerr << "DEBUG: startBlock " << CMarkdown::typeName(type).toStdString() << "\n";

  return block;
}

void
CMarkdownBlock::
addBlockLine(const QString &line, bool brk)
{
  if (markdown()->isDebug())
    std::cerr << "DEBUG: add: " << line.toStdString() << "\n";

  currentBlock_->addLine(Line(line, brk));
}

void
CMarkdownBlock::
appendBlockLine(const QString &line)
{
  if (markdown()->isDebug())
    std::cerr << "DEBUG: append: " << line.toStdString() << "\n";

  currentBlock_->appendLine(line);
}

void
CMarkdownBlock::
flushBlocks()
{
  while (currentBlock_ != rootBlock_)
    endBlock();
}

CMarkdownBlock *
CMarkdownBlock::
endBlock()
{
  if (currentBlock_ == rootBlock_)
    return nullptr;

  if (markdown()->isDebug())
    std::cerr << "DEBUG: endBlock " <<
      CMarkdown::typeName(currentBlock_->blockType()).toStdString() << "\n";

  currentBlock_ = currentBlock_->parent();

  return currentBlock_;
}

void
CMarkdownBlock::
print(int depth) const
{
  QString tag = CMarkdown::typeName(type_);

  for (int i = 0; i < depth; ++i)
    std::cout << "  ";

  std::cout << QString("-> %1").arg(tag).toStdString() << "\n";

  for (auto &l : lines_) {
    for (int i = 0; i < depth; ++i)
      std::cout << "  ";

    std::cout << "  \"" << l.line.toStdString() << "\"\n";
  }

  for (auto &b : blocks_)
    b->print(depth + 1);
}

QString
CMarkdownBlock::
toText(CMarkdown::Format format) const
{
  CMarkdownBlock *th = const_cast<CMarkdownBlock *>(this);

  th->process(format);

  //---

  QString text;

  bool single = CMarkdown::isSingleLineType(type_);

  bool empty = false;

  if (single) {
    if (! processed_)
      empty = (lines_.empty() && blocks_.empty());
    else
      empty = blocks_.empty();
  }

  if (empty) {
    text += fullTag(type_, format) + "\n";
  }
  else {
    text += startTag(type_, format);

    if (! single)
      text += "\n";

    if (! processed_) {
      int  nl  = 0;
      bool brk = false;

      QString line1;

      for (auto &line : lines_) {
        if (nl > 0) {
          if (brk)
            line1 += "\t";
          else
            line1 += "\n";
        }

        line1 += line.line;

        brk = line.brk;

        ++nl;
      }

      if (type_ !=  CMarkdownTagType::CODE)
        line1 = replaceEmbeddedStyles(line1, /*code*/false, format);

      QString line2;

      for (int i = 0; i < line1.size(); ++i) {
        if (line1[i] == '\t') {
          if (format == CMarkdown::Format::HTML)
            line2 += "<br>\n";
          else
            line2 += "\n";
        }
        else
          line2 += line1[i];
      }

      text += line2;
    }

    int nb = 0;

    for (auto &b : blocks_) {
      text += b->toText(format);

      ++nb;
    }

    text += endTag(type_, format) + "\n";
  }

  return text;
}

QString
CMarkdownBlock::
anchorText(const QString &ref, const QString &title, const QString &str,
           CMarkdown::Format format) const
{
  QString text;

  if (format == CMarkdown::Format::HTML) {
    text = QString("<a href=\"%1\"").arg(ref);

    if (title != "")
      text += QString(" title=\"%1\"").arg(title);

    text += htmlStyle(CMarkdownTagType::A);

    text += QString(">%1</a>").arg(str);
  }
  else {
    text = QString("%1%2%3").
      arg(ttyStartStyle(CMarkdownTagType::A)).arg(str).arg(ttyEndStyle(CMarkdownTagType::A));
  }

  return text;
}

QString
CMarkdownBlock::
emphasisText(const QString &str, CMarkdown::Format format) const
{
  QString text;

  if (format == CMarkdown::Format::HTML)
    text = QString("<em%1>%2</em>").arg(htmlStyle(CMarkdownTagType::EM)).arg(str);
  else
    text = QString("[3m%1%2[0m").arg(ttyStartStyle(CMarkdownTagType::EM)).arg(str);

  return text;
}

QString
CMarkdownBlock::
boldText(const QString &str, CMarkdown::Format format) const
{
  QString text;

  if (format == CMarkdown::Format::HTML)
    text = QString("<strong%1>%2</strong>").arg(htmlStyle(CMarkdownTagType::STRONG)).arg(str);
  else
    text = QString("[1m%1%2[0m").arg(ttyStartStyle(CMarkdownTagType::STRONG)).arg(str);

  return text;
}

QString
CMarkdownBlock::
strikeText(const QString &str, CMarkdown::Format format) const
{
  QString text;

  if (format == CMarkdown::Format::HTML)
    text = QString("<strike%1>%2</strike>").arg(htmlStyle(CMarkdownTagType::STRIKE)).arg(str);
  else
    text += QString("[9m%1%2[0m").arg(ttyStartStyle(CMarkdownTagType::STRIKE)).arg(str);

  return text;
}

QString
CMarkdownBlock::
startTag(CMarkdownTagType type, CMarkdown::Format format) const
{
  QString text;

  if (format == CMarkdown::Format::HTML) {
    text = QString("<%1").arg(CMarkdown::typeName(type));

    text += htmlStyle(type);

    text += ">";

    return text;
  }
  else {
    return ttyStartStyle(type);
  }
}

QString
CMarkdownBlock::
endTag(CMarkdownTagType type, CMarkdown::Format format) const
{
  if (format == CMarkdown::Format::HTML)
    return QString("</%1>").arg(CMarkdown::typeName(type));
  else
    return ttyEndStyle(type);
}

QString
CMarkdownBlock::
fullTag(CMarkdownTagType type, CMarkdown::Format format) const
{
  if (format == CMarkdown::Format::HTML)
    return QString("<%1/>").arg(CMarkdown::typeName(type));
  else
    return "";
}

QString
CMarkdownBlock::
htmlStyle(CMarkdownTagType type) const
{
  QString color = CMarkdown::typeColor(type);
  QString font  = CMarkdown::typeFont (type);

  if (color == "" && font == "")
    return "";

  QString text = " style=\"";

  if (color != "")
    text += QString("color:%1;").arg(color);

  if (font != "") {
    QStringList fontParts = font.split(":");

    if      (fontParts.size() == 1)
      text += QString("font-family:%1").arg(fontParts[0]);
    else if (fontParts.size() == 2)
      text += QString("font-family:%1;font-size:%2;").arg(fontParts[0]).arg(fontParts[1]);
    else if (fontParts.size() == 3)
      text += QString("font-family:%1;font-size:%2;font-style:%3").
                arg(fontParts[0]).arg(fontParts[1]).arg(fontParts[2]);
  }

  text += "\"";

  return text;
}

QString
CMarkdownBlock::
ttyStartStyle(CMarkdownTagType type) const
{
  QString color = CMarkdown::typeColor(type);

  if      (color == "black"  ) return "[30m";
  else if (color == "red"    ) return "[31m";
  else if (color == "green"  ) return "[32m";
  else if (color == "yellow" ) return "[33m";
  else if (color == "blue"   ) return "[34m";
  else if (color == "magenta") return "[35m";
  else if (color == "cyan"   ) return "[36m";
  else if (color == "white"  ) return "[37m";

  return "";
}

QString
CMarkdownBlock::
ttyEndStyle(CMarkdownTagType type) const
{
  QString color = CMarkdown::typeColor(type);

  if (color != "")
    return "[0m";

  return "";
}

//------

// get ATX header type, text range and inside text
bool
CMarkdownParse::
isATXHeader(const QString &str, CMarkdownBlock::ATXData &atxData, int &istart, int &iend)
{
  int len = str.length();

  int i = 0;

  // up to 3 spaces
  if (skipSpace(str, i) > 3)
    return false;

  istart = i;

  // followed by hash (max of 6 hashes)
  int nh = skipChar(str, i, '#');

  if (nh < 1 || nh > 6)
    return false;

  // followed by a space or end of line
  if (i < len && ! str[i].isSpace())
    return false;

  // get header type
  if      (nh == 1) atxData.type = CMarkdownTagType::H1;
  else if (nh == 2) atxData.type = CMarkdownTagType::H2;
  else if (nh == 3) atxData.type = CMarkdownTagType::H3;
  else if (nh == 4) atxData.type = CMarkdownTagType::H4;
  else if (nh == 5) atxData.type = CMarkdownTagType::H5;
  else if (nh == 6) atxData.type = CMarkdownTagType::H6;

  // skip spaces after '#'
  CMarkdownParse::skipSpace(str, i);

  // get remaining text
  atxData.text = str.mid(i);

  iend = i;

  // remove trailing '#'
  if (atxData.text != "") {
    if (atxData.text[0] == '#')
      atxData.text = " " + atxData.text;

    int len1 = atxData.text.length();

    int i1 = len1 - 1;

    backSkipSpace(atxData.text, i1);

    iend += i1;

    backSkipChar(atxData.text, i1, '#');

    if (i1 >= 0 && atxData.text[i1].isSpace()) {
      backSkipSpace(atxData.text, i1);

      atxData.text = atxData.text.mid(0, i1 + 1);
    }
  }

  return true;
}

// get link reference details from string with text range
bool
CMarkdownParse::
isLinkReference(const QString &str, CMarkdown::LinkRef &link, int &istart, int &iend)
{
  int len = str.length();

  int i = 0;

  if (skipSpace(str, i) > 3)
    return false;

  // '[' link reference ']'
  if (i >= len || str[i] != '[')
    return false;

  istart = i;

  ++i;

  link.ref = "";

  while (i < len && str[i] != ']') {
    link.ref += str[i++];
  }

  if (i >= len)
    return false;

  ++i;

  if (i >= len || str[i] != ':')
    return false;

  ++i;

  skipSpace(str, i);

  //---

  // link destination

  // TODO: <> enclosed link
  // TODO: allow line ending ?

  link.dest = "";

  while (i < len && ! str[i].isSpace()) {
    link.dest += str[i++];
  }

  iend = i;

  skipSpace(str, i);

  //---

  // link title

  link.title = "";

  if (i < len && (str[i] == '"' || str[i] == '\'')) {
    QChar c = str[i++];

    while (i < len && str[i] != c)
      link.title += str[i++];

    if (i >= len || str[i] != c)
      return false;

    ++i;

    iend = i;
  }

  //---

  skipSpace(str, i);

  if (i < len)
    return false;

  return true;
}

bool
CMarkdownParse::
isRule(const QString &str, int &istart, int &iend)
{
  int len = str.length();

  int i = 0;

  // max of three spaces indent
  if (skipSpace(str, i) >= 4)
    return false;

  // check for rule characters
  if (i >= len)
    return false;

  QChar c = str[i];

  if (c != '-' && c != '*' && c != '_')
    return false;

  istart = i;

  ++i;

  // check for three or more matching characters (with spaces between)
  int nc = 1;

  while (i < len) {
    skipSpace(str, i);

    if (i >= len || str[i] != c)
      return false;

    ++i; ++nc;
  }

  iend = i;

  return (nc >= 3);
}

// check for char surrounded text
int
CMarkdownParse::
parseSurroundText(const QString &str, int &i, QString &str1, int &start1)
{
  return parseSurroundText(str, i, str[i], str1, start1);
}

// check for char surrounded text
int
CMarkdownParse::
parseSurroundText(const QString &str, int &i, const QChar &c, QString &str1, int &start1)
{
  int len = str.length();

  if (i >= len || str[i] != c)
    return 0;

  int  i1    = i;
  bool found = false;

  // count number of start characters
  int nc = 1;

  ++i;

  while (i < len && str[i] == c) {
    ++nc; ++i;
  }

  // search for matching number of end characters
  while (i < len) {
    if      (i < len - 1 && str[i] == '\\' && CMarkdownParse::isASCIIPunct(str[i + 1])) {
      ++i;

      str1 += str[i++];
    }
    else if (str[i] == c) {
      ++i;

      int nc1 = 1;

      while (i < len && nc1 < nc && str[i] == c) {
        ++nc1; ++i;
      }

      if (nc1 == nc) {
        found = true;
        break;
      }

      if (str1 == "")
        start1 = i;

      for (int j = 0; j < nc1; ++j)
        str1 += c;
    }
    else {
      if (str1 == "")
        start1 = i;

      str1 += str[i++];
    }
  }

  if (! found) {
    i = i1;
    return 0;
  }

  return nc;
}

bool
CMarkdownParse::
isASCIIPunct(const QChar &c)
{
  static QString chars("!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");

  return (chars.indexOf(c) >= 0);
}

bool
CMarkdownParse::
isBlankLine(const QString &str)
{
  // An empty line, or a line containing only spaces or tabs, is a blank line.
  int i = 0;

  if (skipSpace(str, i) != str.length())
    return false;

  return true;
}

int
CMarkdownParse::
skipSpace(const QString &str, int &i)
{
  int len = str.length();

  int n = 0;

  while (i < len && str[i].isSpace()) {
    ++i; ++n;
  }

  return n;
}

int
CMarkdownParse::
backSkipSpace(const QString &str, int &i)
{
  int n = 0;

  while (i >= 0 && str[i].isSpace()) {
    --i; ++n;
  }

  return n;
}

int
CMarkdownParse::
skipChar(const QString &str, int &i, const QChar &c)
{
  int len = str.length();

  int n = 0;

  while (i < len && str[i] == c) {
    ++i; ++n;
  }

  return n;
}

int
CMarkdownParse::
backSkipChar(const QString &str, int &i, const QChar &c)
{
  int n = 0;

  while (i >= 0 && str[i] == c) {
    --i; ++n;
  }

  return n;
}
