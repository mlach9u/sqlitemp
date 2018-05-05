#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <algorithm>

#include "sqlitemp.h"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");

	try
	{
		string strFileName;
		std::cout << "Input Database file name : ";
		std::getline(std::cin, strFileName);

		sqliteDatabase< char > db;
		if (db.open(strFileName))
		{
			std::cout << "Database file [" << strFileName << "] is opened successfully." << std::endl;
			std::cout << "If you want to exit this program, input 'exit'." << std::endl;

			string strLine;
			do
			{
				std::cout << "Input> ";
				std::getline(std::cin, strLine);

				strLine.tolower();

				if (strLine != "exit")
				{
					if (strLine.substr(0, 6) == "select")
					{
						std::shared_ptr< sqliteRowSet< char > > prs = db.query(strLine);
						sqliteColumnSet< char > cs = prs->column();

						int nSize = cs.size();
						for (int i = 0; i < nSize; i++)
							std::cout << cs.at(i).name() << "\t";
						std::cout << std::endl;

						do
						{
							for (int i = 0; i < nSize; i++)
								std::cout << cs.at(i) << "\t";
							std::cout << std::endl;

							sqliteColumnSet< char >::iterator it;
							for (it = cs.begin(); it != cs.end(); ++it)
								std::cout << *it << "\t";
							std::cout << std::endl;

							sqliteColumnSet< char >::const_iterator cit;
							for (cit = cs.begin(); cit != cs.end(); ++cit)
								std::cout << *cit << "\t";
							std::cout << std::endl;
						} while (prs->to_next());
					}
					else
					{
						if (db.execute(strLine))
							std::cout << "Succeeded" << std::endl;
						else
						{
							string strError;
							db.get_errmsg(strError);
							std::cout << strError << std::endl;
						}
					}
				}
			} while (strLine != "exit");
			db.close();
		}
		else
			db.throw_errmsg();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return 0;
}
