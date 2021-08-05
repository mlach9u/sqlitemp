#ifndef __SQLITEMP_H__
#define __SQLITEMP_H__

#include "xsqlitemp.h"

struct sqliteDatabase
{
#if __SQLITEMP_USE_TR1_NAMESPACE__
    typedef std::tr1::shared_ptr<sqliteRowSet> rowset_ptr;
#else
    typedef std::shared_ptr<sqliteRowSet> rowset_ptr;
#endif

public:
    sqliteDatabase()
        : m_SQLite(0), m_nLastError(SQLITE_OK) {}

    sqliteDatabase(const std::string& strFileName, int nFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, const char* zVfs = 0)
        : m_SQLite(0), m_nLastError(SQLITE_OK)
    {
        open(strFileName, nFlags, zVfs);
    }

    virtual ~sqliteDatabase()
    {
        close();
    }

    bool good() const
    {
        return (m_nLastError == SQLITE_OK);
    }
    bool fail() const
    {
        return !good();
    }
    void clear_err()
    {
        m_nLastError = SQLITE_OK;
    }
    void set_err(int nError)
    {
        m_nLastError = nError;
    }

    int get_err()
    {
        if (m_nLastError)
        {
            return m_nLastError;
        }
        else if (m_SQLite)
        {
            return sqlite3_errcode(m_SQLite);
        }
        return 0;
    }

    int get_exterr()
    {
        if (m_SQLite)
        {
            return sqlite3_extended_errcode(m_SQLite);
        }
        return 0;
    }

    void get_errmsg(std::string& strError)
    {
        if (m_nLastError)
        {
            strError = sqlite3_errstr(m_nLastError);
        }
        else if (m_SQLite)
        {
            strError = sqlite3_errmsg(m_SQLite);
        }
    }

    void throw_errmsg()
    {
        if (m_nLastError)
        {
            throw std::runtime_error(sqlite3_errstr(m_nLastError));
        }
        else if (m_SQLite)
        {
            throw std::runtime_error(sqlite3_errmsg(m_SQLite));
        }
    }

    bool open(const std::string& strFileName, int nFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, const char* zVfs = 0)
    {
        if (m_SQLite)
        {
            close();
        }
        m_nLastError = sqlite3_open_v2(strFileName.c_str(), &m_SQLite, nFlags, zVfs);
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

    bool execute(const std::string& strSql)
    {
        SQLiteStmt stmt;
        m_nLastError = sqlite3_prepare_v3(m_SQLite, strSql.c_str(), -1, 0, &stmt, 0);
        if (stmt)
        {
            sqliteExecutor exec(stmt);
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

    rowset_ptr query(const std::string& strSql)
    {
        rowset_ptr pRet;
        SQLiteStmt stmt;
        m_nLastError = sqlite3_prepare_v3(m_SQLite, strSql.c_str(), -1, 0, &stmt, 0);
        if (stmt)
        {
            pRet.reset(new sqliteRowSet(stmt));
            if (!pRet->to_next())
            {
                m_nLastError = pRet->get_result();
                if (m_nLastError == SQLITE_DONE)
                {
                    m_nLastError = SQLITE_OK;
                }
                pRet.reset();
            }
        }
        return pRet;
    }

private:
    SQLite m_SQLite;
    int m_nLastError;
};

#endif
