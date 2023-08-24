
/****************************************************************************
** Copyright (c) 2006 - 2021, the LibQxt project.
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

#define BUILD_QXT_SQLFILE
#include "qxtsqlfile.h"
//#include "qxtsqltransaction.h"
#include <QIODevice>
#include <QtDebug>
#include <QSqlQuery>

/*!
\class QxtSqlFile

\inmodule QxtSql

\brief The QxtSqlFile class provides a mechanism for SQL scripts

QxtSqlFile is a tool that can read multiple SQL statements out of a string or
QIODevice and execute them in turn.

QxtSqlFile can execute semicolon-separated SQL statements like QSqlQuery
cannot but, in exchange, it is unable to return any rows. It also provides
some extensions to the SQL language to facilitate common scripting tasks.

\sa QxtSqlTransaction

\section1 Extensions

Note that \c{:} is only recognized when followed by whitespace.

\section2 IF

\section3 Syntax

\c IF \a expression [\c FROM \a{join-clause}]\c{:}

\a statements

[\c ELSEIF \a expression [\c FROM \a{join-clause}]\c{:}

\a {statements}]

[\c {ELSE:}

\a {statements}]

\c ENDIF;

\section3 Description

Evaluates a condition, optionally within the scope of a table or join. If
\a expression evaluates to a true value (for at least one row in the \c FROM
clause, if present), then the \a statements will be executed.

If \a expression evaluates to a true value, then each subsequent \c ELSEIF
is similarly evaluated in turn. If no conditions are met, and an \c ELSE
cause is present, then the \a statements in that block are executed.

As many ELSEIF sections may be included as desired.

\section2 BEGIN

\section3 Syntax

{\c BEGIN | \c START} [\c {TRANSACTION} | \c {WORK}];

\section3 Description

This is identical to the standard SQL \c BEGIN statement, but if an error
occurs while the transaction is open, it will be automatically rolled back
before \l exec() returns.

Note that QxtSqlFile does not itself process savepoints, partial rollbacks,
or nested transactions, including explicit transactions opened while using
\c{exec(true)}. Any explicit commit or rollback will end this error-handling
behavior. If you wish to use these features, use \c TRY / \c CATCH to handle
errors yourself instead of relying solely on this automatic rollback.

\section2 TRY / CATCH

\section3 Syntax

\c TRY [\c {TRANSACTION}]\c{:}

\a statements

\c {CATCH:}

\a {catch-statements}

\c ENDTRY;

\section3 Description

Executes \a statements. If an error occurs, it is suppressed and
control flow jumps to \a {catch-statements}.

If the optional \c TRANSACTION keyword is included, a new transaction is
started. If an error occurs during this transaction, then it is rolled
back before executing \a {catch-statements}. \c {TRY TRANSACTION} blocks
may not occur within another transaction, including transactions started
with the \a transaction parameter to \l {exec()}. Attempting to do so is
undefined behavior.

Note that there is no mechanism for inspecting the error.

\section2 Debug

\section3 Syntax

\c DEBUG \a message \c{;}

\section3 Description

Logs \a message to qDebug().

\section2 Comments

\section3 Single-line syntax

\c {--} \a comment

\section3 Block syntax

\c {\unicode{47}*} \a comment \c {*\unicode{47}}

\section3 Description

QxtSqlFile removes most single-line and block comments from the source before
executing the script. However, comments that begin with \c {--!}, \c {--+|,
\c {\unicode{47}*!} or \c {\unicode{47}*+} are treated as hints or pragmas
intended for the SQL server and are not removed.

Note that QxtSqlFile does not require whitespace after \c {--} to indicate a
comment. This consistent with the SQL standard, but it does not match the
behavior of MySQL.
*/
// TODO: consider a "RETURN" extension?
// TODO: consider supporting bind values

/*!
Constructs a QxtSqlFile that will execute statements read from \a source.
*/
QxtSqlFile::QxtSqlFile(QIODevice* source, QSqlDatabase db) : db(db)
{
  parseStatements(QString::fromUtf8(source->readAll()));
}

/*!
Constructs a QxtSqlFile that will execute statements contained in \a source.
*/
QxtSqlFile::QxtSqlFile(const QString& source, QSqlDatabase db) : db(db)
{
  parseStatements(source);
}

/*!
\brief Executes the SQL statements.

Returns \c true if the evaluation is successful.

Returns \c false if an error occurs during execution. \l lastError() and
\l lastStatement() can be used to inspect the error.

If \a transaction is \c true then the statements will be wrapped in a
transaction. If an error occurs during execution, the transaction will be
rolled back. Otherwise, the transaction is committed. The default is \c false
because some common SQL implementations do not support DDL statements inside
of transactions.
*/
bool QxtSqlFile::exec(bool transaction)
{
  Q_UNUSED(transaction);
  if (parseError) {
    return false;
  }
  errorStatement.clear();
  error = QSqlError();
  int pos = 0;
  int numStmts = statements.size();
  bool autoRollback = transaction;
  QSqlQuery query(db);
  if (transaction) {
    query.exec("BEGIN TRANSACTION");
  }

  struct StackEntry {
    int target;
    bool isCatch;
    bool satisfied;
  };
  QList<StackEntry> stack;

  while (pos < numStmts) {
    const Statement& stmt = statements[pos];
    pos++;
    Extension ext = stmt.ext;
    QString sql;
    if (ext == Begin) {
      autoRollback = true;
      ext = NoExtension;
    } else if (ext == EndTransaction) {
      autoRollback = false;
      ext = NoExtension;
    }
    if (ext == Debug) {
      qDebug() << qPrintable(stmt.sql);
      continue;
    } else if (ext == EndIf || ext == EndTry) {
      // No need to validate it matches the stack; this was handled in parsing.
      stack.takeLast();
      continue;
    } else if (ext == Try) {
      stack << StackEntry{ stmt.target, true, false };
    } else if (ext == If || ext == ElseIf || ext == Else) {
      if (ext == If) {
        stack << StackEntry{ stmt.target, false, false };
      } else {
        stack.back().target = stmt.target;
      }
      StackEntry& entry = stack.back();
      if (ext == Else) {
        if (entry.satisfied) {
          pos = entry.target;
        }
        continue;
      } else if (!entry.satisfied) {
        static const char* fromUC = "FROM";
        static const char* fromLC = "from";
        int stmtLen = stmt.sql.size() - 1;
        int fromPos = -1;
        int wherePos = ext == ElseIf ? 7 : 3;
        int fromIdx = 0;
        for (int i = 0; i < stmtLen; i++) {
          QChar ch = stmt.sql[i];
          if (ch.isSpace()) {
            if (fromIdx == 4) {
              fromPos = i - 4;
              break;
            }
            fromIdx = 0;
          } else if (ch == fromUC[fromIdx] || ch == fromLC[fromIdx]) {
            fromIdx++;
          } else {
            fromIdx = -1;
          }
        }
        if (fromPos > -1) {
          QString fromClause = stmt.sql.mid(fromPos + 5);
          QString whereClause = stmt.sql.mid(wherePos, fromPos - wherePos - 1);
          sql = QStringLiteral("SELECT 1 FROM %1 WHERE %2 LIMIT 1").arg(fromClause).arg(whereClause);
        } else {
          sql = "SELECT " + stmt.sql.mid(wherePos);
        }
        entry.satisfied = query.exec(sql) && query.next() && query.value(0).toBool();
      }
      if (!entry.satisfied) {
        pos = entry.target;
      }
    } else if (ext == NoExtension) {
      query.exec(stmt.sql);
    } else {
      continue;
    }
    if (query.lastError().isValid()) {
      errorStatement = stmt.sql;
      error = query.lastError();
      bool caught = false;
      while (stack.size()) {
        StackEntry entry = stack.takeLast();
        if (entry.isCatch) {
          pos = entry.target;
          caught = true;
          break;
        }
      }
      if (!caught) {
        if (autoRollback) {
          query.exec("ROLLBACK");
        }
        return false;
      }
    }
  }
  if (autoRollback) {
    query.exec("COMMIT");
  }
  return true;
}

static void identifyExtension(QxtSqlFile::Statement& stmt)
{
  QString norm = stmt.sql.toUpper().simplified().replace(" :", ":");
  QString first = norm.section(' ', 0, 0);
  stmt.ext = QxtSqlFile::NoExtension;
  bool acceptColon = false, oneWord = false;
  if (first == "IF") {
    stmt.ext = QxtSqlFile::If;
    acceptColon = true;
  } else if (first == "ELSEIF") {
    stmt.ext = QxtSqlFile::ElseIf;
    acceptColon = true;
  } else if (norm == "ELSE:") {
    stmt.ext = QxtSqlFile::Else;
    acceptColon = true;
  } else if (first == "ENDIF") {
    stmt.ext = QxtSqlFile::EndIf;
    oneWord = true;
  } else if (norm == "TRY TRANSACTION:") {
    stmt.ext = QxtSqlFile::TryTransaction;
    acceptColon = true;
  } else if (norm == "TRY:") {
    stmt.ext = QxtSqlFile::Try;
    acceptColon = true;
  } else if (norm == "CATCH:") {
    stmt.ext = QxtSqlFile::Catch;
    acceptColon = true;
  } else if (first == "ENDTRY") {
    stmt.ext = QxtSqlFile::EndTry;
    oneWord = true;
  } else if (first == "BEGIN") {
    stmt.ext = QxtSqlFile::Begin;
  } else if (first == "COMMIT" || first == "ROLLBACK") {
    stmt.ext = QxtSqlFile::EndTransaction;
  } else if (first == "DEBUG") {
    stmt.ext = QxtSqlFile::Debug;
  }
  if (oneWord && first != norm) {
    stmt.ext = QxtSqlFile::ParseError;
  } else if (acceptColon && !norm.endsWith(":")) {
    stmt.ext = QxtSqlFile::ParseError;
  } else if (acceptColon) {
    stmt.sql.chop(1);
  }
}

#define PARSE_ERROR(sql) \
  parseError = true; \
  error = QSqlError(QLatin1Literal("QxtSqlFile parse error"), QLatin1Literal("QxtSqlFile parse error"), QSqlError::StatementError); \
  errorStatement = sql;

/*! \private */
void QxtSqlFile::parseStatements(const QString& source)
{
  parseError = false;
  int copyStart = 0;
  int pos = 0;
  int sourceLen = source.size();

  enum States {
    NORMAL,
    LINE_COMMENT,
    LINE_PRAGMA,
    BLOCK_COMMENT,
    BLOCK_PRAGMA,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
  };
  int state = NORMAL;

  Statement stmt;
  stmt.ext = NoExtension;
  stmt.target = -1;

  while (pos < sourceLen) {
    QChar ch = source[pos];
    if (state == NORMAL) {
      if (ch == '/') {
        QString check = source.mid(pos, 3);
        if (check == "/*+" || check == "/*!") {
          state = BLOCK_PRAGMA;
        } else if (check.left(2) == "/*") {
          state = BLOCK_COMMENT;
          stmt.sql += source.mid(copyStart, pos - copyStart);
        }
      } else if (ch == '-') {
        QString check = source.mid(pos, 3);
        if (check == "--+" || check == "--!") {
          state = LINE_PRAGMA;
        } else if (check.left(2) == "--") {
          state = LINE_COMMENT;
          stmt.sql += source.mid(copyStart, pos - copyStart);
        }
      } else if (ch == '\'') {
        state = SINGLE_QUOTE;
      } else if (ch == '"') {
        state = DOUBLE_QUOTE;
      } else if (ch == ';' || (ch == ':' && pos < sourceLen - 1 && source[pos + 1].isSpace())) {
        if (ch == ':') {
          stmt.sql += source.mid(copyStart, pos - copyStart + 1);
          pos++;
        } else {
          stmt.sql += source.mid(copyStart, pos - copyStart);
        }
        stmt.sql = stmt.sql.trimmed();
        if (!stmt.sql.isEmpty()) {
          identifyExtension(stmt);
          if (stmt.ext == ParseError) {
            PARSE_ERROR(stmt.sql);
            return;
          }
          statements << stmt;
          stmt.sql.clear();
        }
        copyStart = pos + 1;
      }
    } else if (state == LINE_COMMENT || state == LINE_PRAGMA) {
      if (ch == '\n') {
        if (state == LINE_COMMENT) {
          copyStart = pos;
        }
        state = NORMAL;
      }
    } else if (state == BLOCK_COMMENT || state == BLOCK_PRAGMA) {
      if (ch == '*') {
        QString check = source.mid(pos, 2);
        if (check == "*/") {
          if (state == BLOCK_COMMENT) {
            copyStart = pos + 1;
          }
          state = NORMAL;
        }
      }
    } else if (state == SINGLE_QUOTE || state == DOUBLE_QUOTE) {
      if (ch == '\\') {
        ++pos;
      } else if (ch == (state == SINGLE_QUOTE ? '\'' : '"')) {
        state = NORMAL;
      }
    }
    ++pos;
  }
  if (state != LINE_COMMENT && state != BLOCK_COMMENT) {
    stmt.sql += source.mid(copyStart, pos - copyStart);
    stmt.sql = stmt.sql.trimmed();
    if (!stmt.sql.isEmpty()) {
      identifyExtension(stmt);
      if (stmt.ext == ParseError) {
        PARSE_ERROR(stmt.sql);
        return;
      }
      statements << stmt;
    }
  }
  int numStmts = statements.size();
  QList<bool> parseStack;
  for (int i = 0; i < numStmts; i++) {
    Statement& stmt = statements[i];
    Extension ext = stmt.ext;
    bool isIf = ext == If || ext == ElseIf || ext == Else;
    bool isTry = ext == Try || ext == TryTransaction || ext == Catch;
    if (isIf || isTry) {
      if (ext == If || ext == Try || ext == TryTransaction) {
        parseStack << isTry;
      } else if (parseStack.isEmpty()) {
        PARSE_ERROR(stmt.sql);
        return;
      }
      QList<bool> subStack;
      for (int j = i + 1; j < numStmts; j++) {
        Extension other = statements[j].ext;
        bool isIfBound = other == ElseIf || other == Else || other == EndIf;
        bool isTryBound = other == Catch || other == EndTry;
        if (other == If || other == Try || other == TryTransaction) {
          parseStack << (other != If);
        } else if (isIfBound || isTryBound) {
          bool stackTop = subStack.isEmpty() ? isTry : subStack.back();
          if (stackTop != isTryBound) {
            PARSE_ERROR(statements[j].sql);
            return;
          }
          if (other == EndIf || other == EndTry) {
            if (subStack.isEmpty()) {
              stmt.target = j;
              break;
            } else {
              subStack.takeLast();
            }
          } else {
            stmt.target = j;
            break;
          }
        }
      }
      if (stmt.target < 0) {
        PARSE_ERROR(stmt.sql);
        return;
      }
    } else if (ext == EndIf || ext == EndTry) {
      if (parseStack.isEmpty() || parseStack.takeLast() != (ext == EndTry)) {
        PARSE_ERROR(stmt.sql);
        return;
      }
    }
    // NOTE: While it may appear tempting to ask the SQL implementation to
    // validate the syntax of non-extension statements, the validation would
    // be context-sensitive. This means that a statement might not be valid
    // at this stage but might be valid when executed because it refers to
    // an entity that does not yet exist but will be created by the script.
  }
}

QSqlError QxtSqlFile::lastError() const
{
  return error;
}

QString QxtSqlFile::lastErrorStatement() const
{
  return errorStatement;
}
