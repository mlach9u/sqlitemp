#ifndef __SQLITEMP_H__
#define __SQLITEMP_H__

#include "xsqlitemp.h"

template< typename _Elem, typename _Traits >
std::basic_ostream< _Elem, _Traits >& operator <<(std::basic_ostream< _Elem, _Traits >& _Ostr, sqliteElement< _Elem >& e)
{
	switch (e.type())
	{
	case SQLITE_INTEGER:	_Ostr << e.as_int();	break;
	case SQLITE_FLOAT:		_Ostr << e.as_double();	break;
	case SQLITE_TEXT:		_Ostr << e.as_string(); break;
	}
	return _Ostr;
}

template< typename _Elem >
struct sqliteDatabase
{
	typedef typename basic_string< _Elem > _String;

public:
	sqliteDatabase()
		: m_SQLite(0), m_nLastError(SQLITE_OK) {}

	sqliteDatabase(const _String& strFileName, int nFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, const char* zVfs = 0)
		: m_SQLite(0), m_nLastError(SQLITE_OK)
	{
		open(strFileName, nFlags, zVfs);
	}

	virtual ~sqliteDatabase()
	{ close(); }

public:
	int get_err()
	{
		if (m_SQLite)
			return sqlite3_errcode(m_SQLite);
		return m_nLastError;
	}

	int get_exterr()
	{
		if (m_SQLite)
			return sqlite3_extended_errcode(m_SQLite);
		return 0;
	}

	void get_errmsg(_String& strError)
	{
		if (m_SQLite)	strError.fromutf8(sqlite3_errmsg(m_SQLite));
		else			strError.fromutf8(sqlite3_errstr(m_nLastError));
	}

	void throw_errmsg()
	{
		if (m_SQLite)	throw std::exception(string().fromutf8(sqlite3_errmsg(m_SQLite)).c_str());
		else			throw std::exception(string().fromutf8(sqlite3_errstr(m_nLastError)).c_str());
	}

public:
	bool open(const _String& strFileName, int nFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, const char* zVfs = 0)
	{
		if (m_SQLite)
			close();
		m_nLastError = sqlite3_open_v2(strFileName.toutf8().c_str(), &m_SQLite, nFlags, zVfs);
		return (m_nLastError == SQLITE_OK);
	}

	void close()
	{
		if (m_SQLite)
		{
			m_nLastError = sqlite3_close_v2(m_SQLite);
			m_SQLite = 0;
		}
	}

	bool execute(const _String& strSql)
	{
		_SQLiteStmt SQLiteStmt;
		m_nLastError = sqlite3_prepare_v3(m_SQLite, strSql.toutf8().c_str(), -1, 0, &SQLiteStmt, 0);
		if (SQLiteStmt)
		{
			sqliteExecutor< _Elem > exec(SQLiteStmt);
			m_nLastError = exec.execute();
			switch (m_nLastError)
			{
			case SQLITE_ERROR:	m_nLastError = get_err();		break;
			case SQLITE_DONE:	m_nLastError = SQLITE_OK;		break;
			case SQLITE_ROW:	m_nLastError = SQLITE_MISUSE;	break;
			}
		}
		return (m_nLastError == SQLITE_OK);
	}

	std::shared_ptr< sqliteResultSet< _Elem > > query(const _String& strSql)
	{
		std::shared_ptr< sqliteResultSet< _Elem > > pRet;
		_SQLiteStmt SQLiteStmt;
		m_nLastError = sqlite3_prepare_v3(m_SQLite, strSql.toutf8().c_str(), -1, 0, &SQLiteStmt, 0);
		if (SQLiteStmt)
		{
			pRet.reset(new sqliteResultSet< _Elem >(SQLiteStmt));
			pRet->to_next();
		}
		return pRet;
	}

private:
	_SQLite m_SQLite;
	int m_nLastError;
};

#endif
