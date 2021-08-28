
/****************************************************************************
** Copyright (c) 2006 - 2011, the LibQxt project.
** See the Qxt AUTHORS file for a list of authors and copyright holders.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the LibQxt project nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** <http://libqxt.org>  <foundation@libqxt.org>
*****************************************************************************/

/*!
  \class QxtHtmlTemplate
  \inmodule QxtWeb
  \brief The QxtHtmlTemplate class provides a basic HTML template engine

  QxtHtmlTemplate transforms a provided HTML fragment by replacing special
  \c{<?=} ... \c{?>} variable tags with supplied values.

  To assign a variable, use the \c [] operator. Substitutions are not applied
  recursively; that is, if a variable contains \c{<?= =>} it will not be
  replaced and will appear unmodified in the rendered output.

  For example:
  \code
  QxtHtmlTemplate index;
  if (!index.open(":/index.html")) {
      return 404;
  }
  index["content"] = "Hello, world!";
  return index.render();
  \endcode

  If the \c{index.html} resource contains the following HTML:
  \code
  <html>
    <head>
      <title>Test Page</title>
    </head>
    <body><?= content ?></body>
  </html>
  \endcode
  then the rendered output would be:
  \code
  <html>
    <head>
      <title>Test Page</title>
    </head>
    <body>Hello, world!</body>
  </html>
  \endcode
*/

#include "qxthtmltemplate.h"
#include <QFile>
#include <QStringList>

/*!
  Constructs a new QxtHtmlTemplate.
*/
QxtHtmlTemplate::QxtHtmlTemplate()
: QMap<QString, QString>(), keepIndent(true)
{
  // initializers only
}

/*!
  Returns \c true if surrounding indentation should be preserved in replacements.

  The default value is \c true.

  \sa setIndentPreserved
*/
bool QxtHtmlTemplate::isIndentPreserved() const
{
  return keepIndent;
}

/*!
  Sets if surrounding indentation should be preserved in replacements.

  \sa isIndentPreserved
*/
void QxtHtmlTemplate::setIndentPreserved(bool on)
{
  keepIndent = on;
}

/*!
  Loads HTML from \a d.
*/
void QxtHtmlTemplate::load(const QString& d)
{
  data = d;
}

/*!
  Loads HTML from the file located at \a filename.

  Returns \c true on success. Returns \c false if the file could not be read
  or contained no data.

  The content is interpreted as UTF-8.
*/
bool QxtHtmlTemplate::open(const QString& filename)
{
  QFile f(filename);
  return load(&f);
}

/*!
  Loads HTML from \a source.

  Returns \c true on success. Returns \c false if the device could not be read
  or contained no data.

  The content is interpreted as UTF-8.
*/
bool QxtHtmlTemplate::load(QIODevice* source)
{
  if (!source->isOpen() && !source->open(QIODevice::ReadOnly | QIODevice::Text)) {
    qWarning("QxtHtmlTemplate::open(): unable to read source");
    return false;
  }

  data = QString::fromUtf8(source->readAll());
  while (!data.isEmpty() && data.back().isSpace()) {
    data.chop(1);
  }
  if (data.isEmpty()) {
    qWarning("QxtHtmlTemplate::open(): source is empty");
    return false;
  }
  return true;
}

/*!
  Renders the HTML template.

  Variable names contained in \c{<?=} ... \c{?>} will be replaced with
  the values assigned into the template.
*/
QString QxtHtmlTemplate::render() const
{
  QStringList output;
  int len = data.size();
  QString indent = "\n";
  bool inIndent = true;
  int copyStart = 0;

  for (int i = 0; i < len; i++) {
    QChar ch = data.at(i);
    if (ch == '\n') {
      indent = "\n";
      inIndent = keepIndent;
    } else if (inIndent && ch.isSpace() && ch != '\r') {
      indent += ch;
    } else {
      inIndent = false;
    }

    if (ch == '<') {
      QString peek = data.mid(i, 3);
      if (peek == "<?=") {
        output << data.mid(copyStart, i - copyStart);
        i += 3;
        int varStart = i;
        while (i < len && (data.at(i) != '?' || data.mid(i, 2) != "?>")) {
          ++i;
        }
        if (i >= len) {
          qWarning("QxtHtmlTemplate::render(): unterminated <?=");
          break;
        }
        QString var = data.mid(varStart, i - varStart).trimmed();
        i += 2;
        copyStart = i;
        if (var.isEmpty()) {
          qWarning("QxtHtmlTemplate::render(): empty tag");
          continue;
        } else if (!contains(var)) {
          qWarning("QxtHtmlTemplate::render(): unused variable \"%s\"", qPrintable(var));
          continue;
        } else {
          QString replacement = value(var);
          if (keepIndent && indent.size() > 1) {
            replacement.replace("\n", indent);
          }
          output << replacement;
        }
      }
    }
  }
  if (copyStart < len) {
    output << data.mid(copyStart);
  }

  return output.join("");
}
