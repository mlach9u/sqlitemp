#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <algorithm>

#include "sqlitemp.h"

typedef sqliteDatabase< char > _Database;

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
    eitIterator,
    eitConstIterator,
    eitInputOperator,

    eitCount
};

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    try
    {
        string strFileName;
        std::cout << "Input Database file name : ";
        std::getline(std::cin, strFileName);

        _Database db;
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

            string strLine, strTempLine;
            do
            {
                std::cout << "Input> ";
                std::getline(std::cin, strLine);

                strTempLine = strLine;
                strTempLine.tolower();

                if (strTempLine != "exit")
                {
                    if (strTempLine.substr(0, 6) == "select")
                    {
                        int iColumnIteratorType = citAt, iElementIteratorType = eitAt;
                        _Database::_Rowset_Ptr prsIteratorType = db.query("select * from __sqlitempType");
                        _Database::_Columnset csIteratorType;

                        do
                        {
                            csIteratorType = prsIteratorType->column();
                            if (csIteratorType[0].as_string() == "column")
                                iColumnIteratorType = csIteratorType[1].as_int();
                            else if (csIteratorType[0].as_string() == "element")
                                iElementIteratorType = csIteratorType[1].as_int();
                        } while (prsIteratorType->to_next());

                        _Database::_Rowset_Ptr prs = db.query(strLine);

                        if (prs.get() && db.good())
                        {
                            _Database::_Columnset cs = prs->column();
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
                                case eitIterator:
                                {
                                    _Database::_Columnset::iterator it;
                                    for (it = cs.begin(); it != cs.end(); ++it)
                                        std::cout << *it << "\t";
                                    std::cout << std::endl;
                                }
                                break;
                                case eitConstIterator:
                                {
                                    _Database::_Columnset::const_iterator cit;
                                    for (cit = cs.begin(); cit != cs.end(); ++cit)
                                        std::cout << *cit << "\t";
                                    std::cout << std::endl;
                                }
                                break;
                                case eitInputOperator:
                                {
                                    _Database::_Element e;
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
                        string strError;
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
