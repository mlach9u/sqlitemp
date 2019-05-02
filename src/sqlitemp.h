#ifndef __SQLITEMP_H__
#define __SQLITEMP_H__

#include "xsqlitemp.h"

template< typename _Elem, typename _Traits >
std::basic_ostream< _Elem, _Traits >& operator <<(std::basic_ostream< _Elem, _Traits >& _Ostr, const sqliteElement< _Elem >& e)
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
    typedef typename sqliteRowSet< _Elem > _Rowset;
    typedef typename _Rowset::_ColumnSet _Columnset;
    typedef typename _Columnset::_Element _Element;
    typedef typename _Element::_String _String;

    typedef typename sqliteExecutor< _Elem > _Executor;

    typedef typename std::shared_ptr< _Rowset > _Rowset_Ptr;

public:
    sqliteDatabase()
        : m_SQLite(0), m_nLastError(SQLITE_OK) {}

    sqliteDatabase(const _String& strFileName, int nFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, const char* zVfs = 0)
        : m_SQLite(0), m_nLastError(SQLITE_OK)
    {
        open(strFileName, nFlags, zVfs);
    }

    virtual ~sqliteDatabase()
    {
        close();
    }

public:
    bool good() const { return (m_nLastError == SQLITE_OK); }
    bool fail() const { return !good(); }
    void clear_err() { m_nLastError = SQLITE_OK; }
    void set_err(int nError) { m_nLastError = nError; }

    int get_err()
    {
        if (m_nLastError)	return m_nLastError;
        else if (m_SQLite)	return sqlite3_errcode(m_SQLite);
        return 0;
    }

    int get_exterr()
    {
        if (m_SQLite)
            return sqlite3_extended_errcode(m_SQLite);
        return 0;
    }

    void get_errmsg(_String & strError)
    {
        if (m_nLastError)	strError.fromutf8(sqlite3_errstr(m_nLastError));
        else if (m_SQLite)	strError.fromutf8(sqlite3_errmsg(m_SQLite));
    }

    void throw_errmsg()
    {
        if (m_nLastError)	throw std::exception(string().fromutf8(sqlite3_errstr(m_nLastError)).c_str());
        else if (m_SQLite)	throw std::exception(string().fromutf8(sqlite3_errmsg(m_SQLite)).c_str());
    }

public:
    bool open(const _String & strFileName, int nFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, const char* zVfs = 0)
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

private:
    bool _execute(const _String & strSql)
    {
        _SQLiteStmt SQLiteStmt;
        m_nLastError = sqlite3_prepare_v3(m_SQLite, strSql.toutf8().c_str(), -1, 0, &SQLiteStmt, 0);
        if (SQLiteStmt)
        {
            _Executor exec(SQLiteStmt);
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

    _Rowset_Ptr _query(const _String & strSql)
    {
        _Rowset_Ptr pRet;
        _SQLiteStmt SQLiteStmt;
        m_nLastError = sqlite3_prepare_v3(m_SQLite, strSql.toutf8().c_str(), -1, 0, &SQLiteStmt, 0);
        if (SQLiteStmt)
        {
            pRet.reset(new _Rowset(SQLiteStmt));
            if (!pRet->to_next())
            {
                m_nLastError = pRet->get_result();
                pRet.reset();
            }
        }
        return pRet;
    }

public:
    template< typename TYPE >
    bool executeV(const TYPE * lpszFormat, va_list v)
    {
        _String strSql;
        strSql.formatV(lpszFormat, v);
        return _execute(strSql);
    }

    bool execute(const char* lpszFormat, ...)
    {
        va_list v;
        va_start(v, lpszFormat);
        bool bRet = executeV(lpszFormat, v);
        va_end(v);
        return bRet;
    }

    bool execute(const wchar_t* lpszFormat, ...)
    {
        va_list v;
        va_start(v, lpszFormat);
        bool bRet = executeV(lpszFormat, v);
        va_end(v);
        return bRet;
    }

    template< typename TYPE >
    _Rowset_Ptr queryV(const TYPE * lpszFormat, va_list v)
    {
        _String strSql;
        strSql.formatV(lpszFormat, v);
        return _query(strSql);
    }

    _Rowset_Ptr query(const char* lpszFormat, ...)
    {
        va_list v;
        va_start(v, lpszFormat);
        _Rowset_Ptr pRet = queryV(lpszFormat, v);
        va_end(v);
        return pRet;
    }

    _Rowset_Ptr query(const wchar_t* lpszFormat, ...)
    {
        va_list v;
        va_start(v, lpszFormat);
        _Rowset_Ptr pRet = queryV(lpszFormat, v);
        va_end(v);
        return pRet;
    }

private:
    _SQLite m_SQLite;
    int m_nLastError;
};

#endif
