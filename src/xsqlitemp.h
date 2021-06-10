#ifndef __XSQLITEMP_H__
#define __XSQLITEMP_H__

#include "inclusion_sqlitemp.h"

typedef sqlite3* SQLite;
typedef sqlite3_stmt* SQLiteStmt;

class sqliteColumn
{
public:
    sqliteColumn()
        : m_SQLiteStmt(0), m_iColumn(-1)
    {}
    sqliteColumn(const sqliteColumn& Other)
        : m_SQLiteStmt(Other.m_SQLiteStmt)
        , m_iColumn(Other.m_iColumn)
    {}
#if __SQLITEMP_RVALUE_REFERENCE__
    sqliteColumn(sqliteColumn&& Other)
        : m_SQLiteStmt(Other.m_SQLiteStmt)
        , m_iColumn(Other.m_iColumn)
    {
        Other.m_SQLiteStmt = 0;
        Other.m_iColumn = -1;
    }
#endif
    sqliteColumn(SQLiteStmt stmt, int iColumn)
        : m_SQLiteStmt(stmt), m_iColumn(iColumn)
    {}

    std::string name() const
    {
        return std::string(sqlite3_column_name(m_SQLiteStmt, m_iColumn));
    }

    int type() const
    {
        return sqlite3_column_type(m_SQLiteStmt, m_iColumn);
    }

    const sqliteColumn& operator=(const sqliteColumn& Other)
    {
        if (this != &Other)
        {
            m_SQLiteStmt = Other.m_SQLiteStmt;
            m_iColumn = Other.m_iColumn;
        }
        return *this;
    }

#if __SQLITEMP_RVALUE_REFERENCE__
    const sqliteColumn& operator=(sqliteColumn&& Other)
    {
        swap(Other);
        return *this;
    }
#endif

    void swap(sqliteColumn& Other)
    {
        if (this != &Other)
        {
            std::swap(m_SQLiteStmt, Other.m_SQLiteStmt);
            std::swap(m_iColumn, Other.m_iColumn);
        }
    }

protected:
    SQLiteStmt m_SQLiteStmt;
    int m_iColumn;
};

class sqliteElement : public sqliteColumn
{
public:
    sqliteElement()
    {}
    sqliteElement(const sqliteElement& Other)
        : sqliteColumn(Other)
    {}
#if __SQLITEMP_RVALUE_REFERENCE__
    sqliteElement(sqliteElement&& Other)
        : sqliteColumn(std::move(Other))
    {
    }
#endif
    sqliteElement(SQLiteStmt stmt, int iColumn)
        : sqliteColumn(stmt, iColumn)
    {}

    int size() const
    {
        return sqlite3_column_bytes(m_SQLiteStmt, m_iColumn);
    }

    const void* as_blob() const
    {
        if (type() != SQLITE_BLOB)
            throw std::runtime_error("Invalid column type");
        return sqlite3_column_blob(m_SQLiteStmt, m_iColumn);
    }

    double as_double() const
    {
        if (type() != SQLITE_FLOAT)
            throw std::runtime_error("Invalid column type");
        return sqlite3_column_double(m_SQLiteStmt, m_iColumn);
    }

    int as_int() const
    {
        if (type() != SQLITE_INTEGER)
            throw std::runtime_error("Invalid column type");
        return sqlite3_column_int(m_SQLiteStmt, m_iColumn);
    }

    std::string as_string() const
    {
        if (type() != SQLITE_TEXT)
            throw std::runtime_error("Invalid column type");
        return std::string((const char*)(sqlite3_column_text(m_SQLiteStmt, m_iColumn)));
    }

    const sqliteElement& operator=(const sqliteElement& Other)
    {
        sqliteColumn::operator=(Other);
        return *this;
    }

#if __SQLITEMP_RVALUE_REFERENCE__
    const sqliteElement& operator=(sqliteElement&& Other)
    {
        swap(Other);
        return *this;
    }
#endif

    void swap(sqliteElement& Other)
    {
        sqliteColumn::swap(Other);
    }
};


class sqliteElementConstIterator
{
public:
    sqliteElementConstIterator()
        : m_SQLiteStmt(0), m_iColumn(0) {}
    sqliteElementConstIterator(SQLiteStmt stmt, int iColumn)
        : m_SQLiteStmt(stmt), m_iColumn(iColumn) {}

    bool operator==(const sqliteElementConstIterator& Other) const
    {
        return (m_SQLiteStmt == Other.m_SQLiteStmt)
            && (m_iColumn == Other.m_iColumn);
    }
    bool operator!=(const sqliteElementConstIterator & Other) const
    {
        return !(*this == Other);
    }
    const sqliteElementConstIterator& operator++(int)
    {
        m_Element = sqliteElement(m_SQLiteStmt, ++m_iColumn);
        return *this; 
    }
    const sqliteElementConstIterator& operator++()
    {
        m_Element = sqliteElement(m_SQLiteStmt, ++m_iColumn);
        return *this; 
    }
    const sqliteElementConstIterator& operator=(const sqliteElementConstIterator & Other)
    {
        if (this != &Other)
        {
            m_SQLiteStmt = Other.m_SQLiteStmt;
            m_iColumn = Other.m_iColumn;
        }
        return *this;
    }
    const sqliteElement* operator->() const
    {
        return &m_Element;
    }
    const sqliteElement& operator*() const
    {
        return m_Element;
    }

protected:
    sqliteElement m_Element;
    SQLiteStmt m_SQLiteStmt;
    int m_iColumn;
};


class sqliteElementIterator : public sqliteElementConstIterator
{
public:
    sqliteElementIterator() {}
    sqliteElementIterator(SQLiteStmt stmt, int iColumn)
        : sqliteElementConstIterator(stmt, iColumn) {}

public:
    sqliteElement* operator->()
    {
        return &m_Element;
    }
    sqliteElement& operator*()
    {
        return m_Element;
    }
};


class sqliteColumnSet
{
public:
    typedef sqliteElementConstIterator const_iterator;
    typedef sqliteElementIterator iterator;

    sqliteColumnSet()
        : m_SQLiteStmt(0), m_iColumn(-1)
    {}
    sqliteColumnSet(const sqliteColumnSet& Other)
        : m_SQLiteStmt(Other.m_SQLiteStmt)
        , m_iColumn(Other.m_iColumn)
    {}
#if __SQLITEMP_RVALUE_REFERENCE__
    sqliteColumnSet(sqliteColumnSet&& Other)
        : m_SQLiteStmt(Other.m_SQLiteStmt)
        , m_iColumn(Other.m_iColumn)
    {
        Other.m_SQLiteStmt = 0;
        Other.m_iColumn = -1;
    }
#endif
    sqliteColumnSet(SQLiteStmt stmt, int iStart = 0)
        : m_SQLiteStmt(stmt), m_iColumn(iStart)
    {}

    int size() const
    {
        return sqlite3_column_count(m_SQLiteStmt);
    }

    sqliteElement at(int iColumn) const
    {
        if (!(iColumn < size()))
            throw std::runtime_error("Out of column range");
        return sqliteElement(m_SQLiteStmt, iColumn);
    }

    const_iterator begin() const
    {
        return const_iterator(m_SQLiteStmt, 0);
    }
    const_iterator end() const
    {
        return const_iterator(m_SQLiteStmt, size());
    }
    const_iterator rbegin() const
    {
        return const_iterator(m_SQLiteStmt, size() - 1);
    }
    const_iterator rend() const
    {
        return const_iterator(m_SQLiteStmt, -1);
    }

    iterator begin()
    {
        return iterator(m_SQLiteStmt, 0);
    }
    iterator end()
    {
        return iterator(m_SQLiteStmt, size());
    }
    iterator rbegin()
    {
        return iterator(m_SQLiteStmt, size() - 1);
    }
    iterator rend()
    {
        return iterator(m_SQLiteStmt, -1);
    }

    const sqliteColumnSet& operator=(const sqliteColumnSet & Other)
    {
        if (this != &Other)
        {
            m_SQLiteStmt = Other.m_SQLiteStmt;
            m_iColumn = Other.m_iColumn;
        }
        return *this;
    }

#if __SQLITEMP_RVALUE_REFERENCE__
    const sqliteColumnSet& operator=(sqliteColumnSet&& Other)
    {
        swap(Other);
        return *this;
    }
#endif

    sqliteElement operator[](int iColumn) const
    {
        return at(iColumn);
    }

    sqliteColumnSet& operator >> (sqliteElement& e)
    {
        e = at(m_iColumn++);
        return *this;
    }
    sqliteColumnSet& operator >> (int& n)
    {
        n = at(m_iColumn++).as_int();
        return *this;
    }
    sqliteColumnSet& operator >> (double& d)
    {
        d = at(m_iColumn++).as_double();
        return *this;
    }
    sqliteColumnSet& operator >> (std::string& str)
    {
        str = at(m_iColumn++).as_string();
        return *this;
    }

    void swap(sqliteColumnSet& Other)
    {
        if (this != &Other)
        {
            std::swap(m_SQLiteStmt, Other.m_SQLiteStmt);
            std::swap(m_iColumn, Other.m_iColumn);
        }
    }

private:
    SQLiteStmt m_SQLiteStmt;
    int m_iColumn;
};


class sqliteStatement
{
public:
    sqliteStatement(SQLiteStmt stmt)
        : m_SQLiteStmt(stmt)
    {}
#if __SQLITEMP_RVALUE_REFERENCE__
    sqliteStatement(sqliteStatement&& Other)
        : m_SQLiteStmt(Other.m_SQLiteStmt)
    {
        Other.m_SQLiteStmt = 0;
    }
#endif
    virtual ~sqliteStatement()
    {
        if (m_SQLiteStmt)
        {
            sqlite3_finalize(m_SQLiteStmt);
        }
    }

    int step()
    {
        return sqlite3_step(m_SQLiteStmt);
    }
    int reset()
    {
        sqlite3_reset(m_SQLiteStmt);
    }

    void swap(sqliteStatement& Other)
    {
        if (this != &Other)
        {
            std::swap(m_SQLiteStmt, Other.m_SQLiteStmt);
        }
    }

protected:
    SQLiteStmt m_SQLiteStmt;

private:
    sqliteStatement();
    sqliteStatement(const sqliteStatement&);
};


class sqliteExecutor : protected sqliteStatement
{
public:
    sqliteExecutor(SQLiteStmt stmt)
        : sqliteStatement(stmt)
    {}
    virtual ~sqliteExecutor()
    {}

    int execute()
    {
        return step();
    }
};


class sqliteRowSet : protected sqliteStatement
{
public:
    sqliteRowSet(SQLiteStmt stmt)
        : sqliteStatement(stmt)
        , m_nLastResult(SQLITE_ROW)
    {}
    virtual ~sqliteRowSet()
    {}

    int get_result() const
    {
        return m_nLastResult;
    }

    bool is_exist_next() const
    {
        return (m_nLastResult == SQLITE_ROW);
    }

    bool to_next()
    {
        if (is_exist_next())
        {
            m_nLastResult = step();
            return (m_nLastResult == SQLITE_ROW);
        }
        return false;
    }

    sqliteColumnSet column()
    {
        return sqliteColumnSet(m_SQLiteStmt);
    }

private:
    int m_nLastResult;
};

#endif
