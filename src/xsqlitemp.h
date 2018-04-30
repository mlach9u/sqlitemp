#ifndef __XSQLITEMP_H__
#define __XSQLITEMP_H__

#include "inclusion_sqlitemp.h"

typedef sqlite3* _SQLite;
typedef sqlite3_stmt* _SQLiteStmt;

template< typename _Elem >
struct sqliteColumn
{
	typedef typename sqliteColumn< _Elem > _Ty;
	typedef typename basic_string< _Elem > _String;
public:
	sqliteColumn()
		: m_SQLiteStmt(0), m_iColumn(0) {}
	sqliteColumn(_SQLiteStmt SQLiteStmt, int iColumn)
		: m_SQLiteStmt(SQLiteStmt), m_iColumn(iColumn) {}

public:
	_String name() const
	{
		_String strRet;
		strRet.fromutf8(sqlite3_column_name(m_SQLiteStmt, m_iColumn));
		return strRet;
	}

	int type() const
	{ return sqlite3_column_type(m_SQLiteStmt, m_iColumn); }

public:
	const _Ty& operator=(const _Ty& Other)
	{
		if (this != &Other)
		{
			m_SQLiteStmt = Other.m_SQLiteStmt;
			m_iColumn = Other.m_iColumn;
		}
		return *this;
	}

protected:
	_SQLiteStmt m_SQLiteStmt;
	int m_iColumn;
};

template< typename _Elem >
struct sqliteElement : public sqliteColumn< _Elem >
{
	typedef typename sqliteColumn< _Elem > _Base;
	typedef typename sqliteElement< _Elem > _Ty;
	typedef typename _Base::_String _String;
public:
	sqliteElement() {}
	sqliteElement(_SQLiteStmt SQLiteStmt, int iColumn)
		: _Base(SQLiteStmt, iColumn) {}

public:
	int size() const
	{ return sqlite3_column_bytes(m_SQLiteStmt, m_iColumn); }

	const void* as_blob() const
	{ return sqlite3_column_blob(m_SQLiteStmt, m_iColumn); }

	double as_double() const
	{ return sqlite3_column_double(m_SQLiteStmt, m_iColumn); }

	int as_int() const
	{ return sqlite3_column_int(m_SQLiteStmt, m_iColumn); }

	sqlite3_int64 as_int64() const
	{ return sqlite3_column_int64(m_SQLiteStmt, m_iColumn); }

	_String as_string() const
	{
		_String strRet;
		strRet.fromutf8((const char*)(sqlite3_column_text(m_SQLiteStmt, m_iColumn)));
		return strRet;
	}

public:
	const _Ty& operator=(const _Ty& Other)
	{
		if (this != &Other)
			_Base::operator=(Other);
		return *this;
	}
};

template< typename _Elem >
struct sqliteElementConstIterator
{
	typedef typename sqliteElementConstIterator< _Elem > _Ty;
	typedef typename sqliteElement< _Elem > _Element;
public:
	sqliteElementConstIterator()
		: m_SQLiteStmt(0), m_iColumn(0) {}
	sqliteElementConstIterator(_SQLiteStmt SQLiteStmt, int iColumn)
		: m_SQLiteStmt(SQLiteStmt), m_iColumn(iColumn) {}

public:
	bool operator==(const _Ty& Other) const		{ return ((m_SQLiteStmt == Other.m_SQLiteStmt) && (m_iColumn == Other.m_iColumn)); }
	bool operator!=(const _Ty& Other) const		{ return !(*this == Other); }
	const _Ty& operator++(int)					{ m_iColumn++; return *this; }
	const _Ty& operator++()						{ m_iColumn++; return *this; }
	const _Ty& operator=(const _Ty& Other)
	{
		if (this != &Other)
		{
			m_SQLiteStmt = Other.m_SQLiteStmt;
			m_iColumn = Other.m_iColumn;
		}
		return *this;
	}
	const _Element* operator->()				{ m_Element = _Element(m_SQLiteStmt, m_iColumn); return &m_Element; }
	const _Element& operator*()					{ m_Element = _Element(m_SQLiteStmt, m_iColumn); return m_Element; }

protected:
	_Element m_Element;
	_SQLiteStmt m_SQLiteStmt;
	int m_iColumn;
};

template< typename _Elem >
struct sqliteElementIterator : public sqliteElementConstIterator< _Elem >
{
	typedef typename sqliteElementIterator< _Elem > _Ty;
	typedef typename sqliteElementConstIterator< _Elem > _Base;
public:
	sqliteElementIterator() {}
	sqliteElementIterator(_SQLiteStmt SQLiteStmt, int iColumn)
		: _Base(SQLiteStmt, iColumn) {}

public:
	const _Ty& operator=(const _Ty& Other)
	{
		if (this != &Other)
			_Base::operator=(Other);
		return *this;
	}
	_Element* operator->()						{ m_Element = _Element(m_SQLiteStmt, m_iColumn); return &m_Element; }
	_Element& operator*()						{ m_Element = _Element(m_SQLiteStmt, m_iColumn); return m_Element; }
};

template< typename _Elem >
struct sqliteStatement
{
public:
	sqliteStatement(_SQLiteStmt SQLiteStmt) : m_SQLiteStmt(SQLiteStmt) {}
	virtual ~sqliteStatement() { if (m_SQLiteStmt) sqlite3_finalize(m_SQLiteStmt); }

public:
	int step() { return sqlite3_step(m_SQLiteStmt); }
	int reset() { sqlite3_reset(m_SQLiteStmt); }

protected:
	sqliteStatement() {}
	_SQLiteStmt m_SQLiteStmt;
};

template< typename _Elem >
struct sqliteExecutor : protected sqliteStatement< _Elem >
{
	typedef typename sqliteStatement< _Elem > _Base;
public:
	sqliteExecutor(_SQLiteStmt SQLiteStmt) : _Base(SQLiteStmt) {}
	virtual ~sqliteExecutor() {}

public:
	int execute() { return _Base::step(); }
};

template< typename _Elem >
struct sqliteResultSet : protected sqliteStatement< _Elem >
{
	typedef typename sqliteStatement< _Elem > _Base;
	typedef typename sqliteElementConstIterator< _Elem > const_iterator;
	typedef typename sqliteElementIterator< _Elem > iterator;
public:
	sqliteResultSet(_SQLiteStmt SQLiteStmt)
		: _Base(SQLiteStmt), m_nLastResult(SQLITE_ROW) {}
	virtual ~sqliteResultSet() {}

public:
	int get_result() const
	{ return m_nLastResult; }

public:
	bool is_exist_next() const
	{ return (m_nLastResult == SQLITE_ROW); }

	bool to_next()
	{
		if (is_exist_next())
		{
			m_nLastResult = _Base::step();
			return ((m_nLastResult == SQLITE_ROW) || (m_nLastResult == SQLITE_DONE));
		}
		return false;
	}

public:
	int size()
	{ return sqlite3_column_count(m_SQLiteStmt); }

	sqliteElement< _Elem > at(int iColumn)
	{ return sqliteElement< _Elem >(m_SQLiteStmt, iColumn); }

	sqliteElement< _Elem > operator[](int iColumn)
	{ return at(iColumn); }

public:
	const_iterator begin() const	{ return const_iterator(m_SQLiteStmt, 0); }
	const_iterator end() const		{ return const_iterator(m_SQLiteStmt, size()); }
	const_iterator rbegin() const	{ return const_iterator(m_SQLiteStmt, size() - 1); }
	const_iterator rend() const		{ return const_iterator(m_SQLiteStmt, -1); }

public:
	iterator begin()				{ return iterator(m_SQLiteStmt, 0); }
	iterator end()					{ return iterator(m_SQLiteStmt, size()); }
	iterator rbegin()				{ return iterator(m_SQLiteStmt, size() - 1); }
	iterator rend()					{ return iterator(m_SQLiteStmt, -1); }

private:
	int m_nLastResult;
};

#endif
