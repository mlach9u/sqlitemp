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
	{ return sqlite3_column_bytes(this->m_SQLiteStmt, this->m_iColumn); }

	const void* as_blob() const
	{
		if (_Base::type() != SQLITE_BLOB)
			throw std::exception("Invalid column type");
		return sqlite3_column_blob(this->m_SQLiteStmt, this->m_iColumn);
	}

	double as_double() const
	{
		if (_Base::type() != SQLITE_FLOAT)
			throw std::exception("Invalid column type");
		return sqlite3_column_double(this->m_SQLiteStmt, this->m_iColumn);
	}

	int as_int() const
	{
		if (_Base::type() != SQLITE_INTEGER)
			throw std::exception("Invalid column type");
		return sqlite3_column_int(this->m_SQLiteStmt, this->m_iColumn);
	}

	_String as_string() const
	{
		if (_Base::type() != SQLITE_TEXT)
			throw std::exception("Invalid column type");
		_String strRet;
		strRet.fromutf8((const char*)(sqlite3_column_text(this->m_SQLiteStmt, this->m_iColumn)));
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
	typedef typename _Base::_Element _Element;
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
	_Element* operator->()						{ this->m_Element = _Element(this->m_SQLiteStmt, this->m_iColumn); return &this->m_Element; }
	_Element& operator*()						{ this->m_Element = _Element(this->m_SQLiteStmt, this->m_iColumn); return this->m_Element; }
};

template< typename _Elem >
struct sqliteColumnSet
{
	typedef typename sqliteColumnSet< _Elem > _Ty;
	typedef typename sqliteElementConstIterator< _Elem > const_iterator;
	typedef typename sqliteElementIterator< _Elem > iterator;
	typedef typename const_iterator::_Element _Element;
	typedef typename _Element::_String _String;
public:
	sqliteColumnSet() : m_SQLiteStmt(0), m_iColumn(0) {}
	sqliteColumnSet(_SQLiteStmt SQLiteStmt) : m_SQLiteStmt(SQLiteStmt), m_iColumn(0) {}
	~sqliteColumnSet() {}

public:
	int size() const
	{
		return sqlite3_column_count(m_SQLiteStmt);
	}

	const _Element at(int iColumn) const
	{
		if (!(iColumn < size()))
			throw std::exception("Out of column range");
		return _Element(m_SQLiteStmt, iColumn);
	}

public:
	const_iterator begin() const { return const_iterator(m_SQLiteStmt, 0); }
	const_iterator end() const { return const_iterator(m_SQLiteStmt, size()); }
	const_iterator rbegin() const { return const_iterator(m_SQLiteStmt, size() - 1); }
	const_iterator rend() const { return const_iterator(m_SQLiteStmt, -1); }

public:
	iterator begin() { return iterator(m_SQLiteStmt, 0); }
	iterator end() { return iterator(m_SQLiteStmt, size()); }
	iterator rbegin() { return iterator(m_SQLiteStmt, size() - 1); }
	iterator rend() { return iterator(m_SQLiteStmt, -1); }

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

	const _Element operator[](int iColumn) const
	{
		return at(iColumn);
	}

	_Ty& operator >> (_Element& e) { e = at(m_iColumn++); return *this; }
	_Ty& operator >> (int& n) { n = at(m_iColumn++).as_int(); return *this; }
	_Ty& operator >> (double& d) { d = at(m_iColumn++).as_double(); return *this; }
	_Ty& operator >> (_String& str) { str = at(m_iColumn++).as_string(); return *this; }

private:
	_SQLiteStmt m_SQLiteStmt;
	int m_iColumn;
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
struct sqliteRowSet : protected sqliteStatement< _Elem >
{
	typedef typename sqliteRowSet< _Elem > _Ty;
	typedef typename sqliteStatement< _Elem > _Base;
	typedef typename sqliteColumnSet< _Elem > _ColumnSet;
public:
	sqliteRowSet(_SQLiteStmt SQLiteStmt)
		: _Base(SQLiteStmt), m_nLastResult(SQLITE_ROW) {}
	virtual ~sqliteRowSet() {}

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
			return (m_nLastResult == SQLITE_ROW);
		}
		return false;
	}

	_ColumnSet column()
	{ return _ColumnSet(this->m_SQLiteStmt); }

private:
	int m_nLastResult;
};

#endif
