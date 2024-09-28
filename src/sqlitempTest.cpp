#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <algorithm>

#include "sqlitemp.h"

template<typename _Elem, typename _Traits>
std::basic_ostream<_Elem, _Traits>& operator <<(std::basic_ostream<_Elem, _Traits>& _Ostr, const sqliteElement& e)
{
    switch (e.type())
    {
    case SQLITE_INTEGER:	_Ostr << e.as_int();	break;
    case SQLITE_FLOAT:		_Ostr << e.as_double();	break;
    case SQLITE_TEXT:		_Ostr << e.as_string(); break;
    }
    return _Ostr;
}

typedef sqliteDatabase Database;

enum ColumnIteratorType
{
    citAt,
    citOperator,

    citCount
};

enum ElementIteratorType
{
    eitAt,
    eitOperator,
    eitInputOperator,

    eitCount
};

int main()
{
    setlocale(LC_ALL, "");

    try
    {
        std::string strFileName;
        std::cout << "Input Database file name : ";
        std::getline(std::cin, strFileName);

        Database db;
        if (db.open(strFileName))
        {
            std::cout << "Database file [" << strFileName << "] is opened successfully." << std::endl;
            std::cout << "If you want to exit this program, input 'exit'." << std::endl;

            if (db.execute("create table __sqlitempType (name varchar(32), iteratorType integer)"))
            {
                db.execute("insert into __sqlitempType (name, iteratorType) values ('column', 0)");
                db.execute("insert into __sqlitempType (name, iteratorType) values ('element', 0)");
            }
            db.clear_err();

            std::string strLine, strTempLine;
            do
            {
                std::cout << "Input> ";
                std::getline(std::cin, strLine);

                strTempLine = strLine;
                std::transform(strTempLine.begin(), strTempLine.end(), strTempLine.begin(),
                    [](unsigned char c) { return tolower(c); }
                );

                if (strTempLine != "exit")
                {
                    if (strTempLine.substr(0, 6) == "select")
                    {
                        int iColumnIteratorType = citAt, iElementIteratorType = eitAt;
                        Database::rowset_ptr prsIteratorType = db.query("select * from __sqlitempType");
                        sqliteColumnSet csIteratorType;

                        do
                        {
                            csIteratorType = prsIteratorType->column();
                            if (csIteratorType[0].as_string() == "column")
                                iColumnIteratorType = csIteratorType[1].as_int();
                            else if (csIteratorType[0].as_string() == "element")
                                iElementIteratorType = csIteratorType[1].as_int();
                        } while (prsIteratorType->to_next());

                        Database::rowset_ptr prs = db.query(strLine);

                        if (prs.get() && db.good())
                        {
                            sqliteColumnSet cs = prs->column();
                            int nSize = cs.size();
                            for (int i = 0; i < nSize; i++)
                            {
                                switch (iColumnIteratorType)
                                {
                                case citAt:			std::cout << cs.at(i).name() << "\t";	break;
                                case citOperator:	std::cout << cs[i].name() << "\t";		break;
                                }
                            }
                            std::cout << std::endl;

                            do
                            {
                                cs = prs->column();

                                switch (iElementIteratorType)
                                {
                                case eitAt:
                                    for (int i = 0; i < nSize; i++)
                                        std::cout << cs.at(i) << "\t";
                                    std::cout << std::endl;
                                    break;
                                case eitOperator:
                                    for (int i = 0; i < nSize; i++)
                                        std::cout << cs[i] << "\t";
                                    std::cout << std::endl;
                                    break;
                                case eitInputOperator:
                                {
                                    sqliteElement e;
                                    for (int i = 0; i < nSize; i++)
                                    {
                                        cs >> e;
                                        std::cout << e << "\t";
                                    }
                                    std::cout << std::endl;
                                }
                                break;
                                }
                            } while (prs->to_next());
                        }
                    }
                    else
                    {
                        if (db.execute(strLine))
                            std::cout << "Succeeded" << std::endl;
                    }

                    if (db.fail())
                    {
                        std::string strError;
                        db.get_errmsg(strError);
                        std::cout << strError << std::endl;
                    }
                }
            } while (strTempLine != "exit");
            db.close();
        }
        else
            db.throw_errmsg();
    }
    catch (std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
