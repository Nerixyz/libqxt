
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

// HACK: Trick QSqlResult into treating QxtSqlWrapResult as a friend
#define tst_QSqlQuery QxtSqlWrapResult
#include <QSqlResult>

#define BUILD_QXT_SQLFILE
#include "qxtsqltransaction.h"
#include <QtDebug>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlIndex>

QxtSqlException::QxtSqlException(const QSqlError& err, const QString& query)
: std::runtime_error(qPrintable(err.text())), QSqlError(err), m_query(query)
{
  // initializers only
}

class QxtSqlWrapResult : public QSqlResult
{
public:
  QxtSqlWrapResult(const QSqlDatabase& db, const QSqlDriver* driver) : QSqlResult(driver), wrapped(db)
  { /* initializers only */ }

  virtual QVariant handle() const { return wrapped.result()->handle(); }

protected:
  virtual bool returnOrThrow(bool value)
  {
    setLastError(wrapped.lastError());
    if (!value) {
      throw QxtSqlException(wrapped.lastError(), wrapped.executedQuery());
    }
    return true;
  }

  virtual void setActive(bool a) { if (!a) wrapped.finish(); QSqlResult::setActive(a); }
  virtual void setQuery(const QString& query) { setLastQuery(query); QSqlResult::setQuery(query); }
  virtual void setForwardOnly(bool forward) { wrapped.setForwardOnly(forward); QSqlResult::setForwardOnly(forward); }
  virtual void bindValue(int pos, const QVariant& val, QSql::ParamType type) { QSqlResult::bindValue(pos, val, type); wrapped.bindValue(pos, val, type); }
  virtual void bindValue(const QString& pos, const QVariant& val, QSql::ParamType type) { QSqlResult::bindValue(pos, val, type); wrapped.bindValue(pos, val, type); }
  virtual QVariant data(int i) { return wrapped.value(i); }
  virtual bool isNull(int i) { return wrapped.isNull(i); }
  virtual bool fetch(int i) { bool ok = wrapped.seek(i, false); setAt(wrapped.at()); return ok; }
  virtual bool fetchNext() { bool ok = wrapped.next(); setAt(wrapped.at()); return ok; }
  virtual bool fetchPrevious() { bool ok = wrapped.previous(); setAt(wrapped.at()); return ok; }
  virtual bool fetchFirst() { bool ok = wrapped.first(); setAt(wrapped.at()); return ok; }
  virtual bool fetchLast() { bool ok = wrapped.last(); setAt(wrapped.at()); return ok; }
  virtual int size() { return wrapped.size(); }
  virtual int numRowsAffected() { return wrapped.numRowsAffected(); }
  virtual QSqlRecord record() const { return wrapped.record(); }
  virtual QVariant lastInsertId() const { return wrapped.lastInsertId(); }
  virtual void virtual_hook(int id, void *data) { const_cast<QSqlResult*>(wrapped.result())->virtual_hook(id, data); }
  virtual void setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy policy) { QSqlResult::setNumericalPrecisionPolicy(policy); wrapped.setNumericalPrecisionPolicy(policy); }
  virtual bool nextResult() { QSqlResult::nextResult(); return returnOrThrow(wrapped.nextResult()); }

  virtual bool reset(const QString& sqlquery) {
    setLastQuery(sqlquery);
    returnOrThrow(wrapped.exec(sqlquery));
    setActive(wrapped.isActive());
    setSelect(wrapped.isSelect());
    return true;
  }
  virtual bool exec() {
    returnOrThrow(wrapped.exec());
    setActive(wrapped.isActive());
    setSelect(wrapped.isSelect());
    return true;
  }
  virtual bool prepare(const QString& query) { setLastQuery(query); return returnOrThrow(wrapped.prepare(query)); }
  virtual bool savePrepare(const QString& sqlquery) { setLastQuery(sqlquery); return returnOrThrow(wrapped.prepare(sqlquery)); }
  virtual bool execBatch(bool arrayBind = false) { return returnOrThrow(wrapped.execBatch(arrayBind ? QSqlQuery::ValuesAsColumns : QSqlQuery::ValuesAsRows)); }

  void setLastQuery(const QString& query) {
    lastQuerySQL = query;
    QSqlResult::setQuery(query);
  }

private:
  QSqlQuery wrapped;
  QString lastQuerySQL;
};

class QxtSqlWrapDriver : public QSqlDriver
{
Q_OBJECT
public:
  QxtSqlWrapDriver(const QSqlDatabase& wrapped) : QSqlDriver(wrapped.driver()), db(wrapped), drv(wrapped.driver())
  { /* initializers only */ }

  // Can't open/close a wrapped database
  virtual bool open(const QString&, const QString&, const QString&, const QString&, int, const QString&)
  { throw QxtSqlException(QSqlError("cannot open QxtSqlWrapDriver", "cannot open QxtSqlWrapDriver", QSqlError::ConnectionError), ""); }
  virtual void close() { throw QxtSqlException(QSqlError("cannot close QxtSqlWrapDriver", "cannot close QxtSqlWrapDriver", QSqlError::ConnectionError), ""); }
  virtual bool isOpen() const { return db.isOpen(); }
  virtual QSqlResult* createResult() const { return new QxtSqlWrapResult(db, this); }
  virtual bool hasFeature(QSqlDriver::DriverFeature feature) const { return drv->hasFeature(feature); }
  virtual bool beginTransaction() { return db.transaction(); }
  virtual bool commitTransaction() { return db.commit(); }
  virtual bool rollbackTransaction() { return db.rollback(); }
  virtual QStringList tables(QSql::TableType tableType) const { return db.tables(tableType); }
  virtual QSqlIndex primaryIndex(const QString &tableName) const { return db.primaryIndex(tableName); }
  virtual QSqlRecord record(const QString &tableName) const { return db.record(tableName); }
  virtual QString formatValue(const QSqlField& field, bool trimStrings = false) const { return drv->formatValue(field, trimStrings); }
  virtual QString escapeIdentifier(const QString &identifier, IdentifierType type) const { return drv->escapeIdentifier(identifier, type); }
  virtual QString sqlStatement(StatementType type, const QString &tableName, const QSqlRecord &rec, bool preparedStatement) const
  { return drv->sqlStatement(type, tableName, rec, preparedStatement); }
  virtual QVariant handle() const { return drv->handle(); }
  virtual bool subscribeToNotification(const QString &name) { return drv->subscribeToNotification(name); }
  virtual bool unsubscribeFromNotification(const QString &name) { return drv->unsubscribeFromNotification(name); }
  virtual QStringList subscribedToNotifications() const { return drv->subscribedToNotifications(); }
  virtual bool isIdentifierEscaped(const QString &identifier, IdentifierType type) const { return drv->isIdentifierEscaped(identifier, type); }
  virtual QString stripDelimiters(const QString &identifier, IdentifierType type) const { return drv->stripDelimiters(identifier, type); }
  virtual bool cancelQuery() { return drv->cancelQuery(); }

// These functions can't be wrapped.
// As a result, we have to use QSqlDatabase's APIs instead of the driver's to manage these.
/*
protected:
  virtual void setOpen(bool o);
  virtual void setOpenError(bool e);
  virtual void setLastError(const QSqlError& e);
*/

private:
  QSqlDatabase db;
  QSqlDriver* drv;
};

/*!
\class QxtSqlTransaction

\inmodule QxtSql

\brief The QxtSqlTransaction class provides a scope-bound SQL transaction

QxtSqlTransaction provides an exception-safe way of using SQL transactions.
When a QxtSqlTransaction object is created, a transaction is started. When it
is destroyed, the transaction is rolled back unless it was first committed
with \l {commit()}.

In support of this design, QxtSqlTransaction offers a version of QSqlQuery
that throws exceptions when an error occurs.

Note that some database engines cannot recover from an error during a
transaction, and the transaction must be rolled back before any further
queries can succeed.

\sa QxtSqlException
*/

/*!
Constructs a QxtSqlTransaction on \a {db}.

If the database engine does not support transactions, the constructor will
throw \l {QxtSqlException}.
*/
QxtSqlTransaction::QxtSqlTransaction(QSqlDatabase db) : db(db), committed(false)
{
  driver = new QxtSqlWrapDriver(db);
  if (!db.transaction()) {
    throw QxtSqlException(db.lastError(), "BEGIN");
  }
}

/*!
Destroys the QxtSqlTransaction, rolling back the transaction if it has not been committed.
*/
QxtSqlTransaction::~QxtSqlTransaction()
{
  if (!committed) {
    db.rollback();
  }
  if (driver) {
    delete driver.data();
  }
}

/*!
Returns a newly constructed QSqlQuery that throws exceptions on errors.
*/
QSqlQuery QxtSqlTransaction::query()
{
  return QSqlQuery(driver->createResult());
}

/*!
Commits the transaction.

After calling this function, the destructor will not attempt to rollback.
*/
void QxtSqlTransaction::commit()
{
  if (!db.commit()) {
    throw QxtSqlException(db.lastError(), "COMMIT");
  }
  committed = true;
}

#include "qxtsqltransaction.moc"
