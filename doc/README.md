# Qt Markdown view and Markdown implementation #

## Qt Interface ##

* Has input text in text widget on left with some basic edit operations on a toolbar
* Has Tabs on right showing processed Html (Using QTextEdit) and Html Plain Text for this code
  and output of an external command ('markdown' by default'). The external command can be
  overriden using the environment variable 'CQMARKDOWN_EXEC'

## Generator

The Markdown to Html generator is written to conform to [CommonMark](http://commonmark.org)
specification. With some minor enhancements.

Not all the specification is implemented or corner cases covered so there will be bugs.

## Enhancements

Support strike throw ~~Strike~~

Support inline links using '\[<text>\](#<name>)' for link source and
'\[<name>\]:#<name>' for link target.
