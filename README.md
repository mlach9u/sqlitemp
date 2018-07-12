
# Template classes for SQLite (SQLitemp)

---

## License

This is in the Public Domain.

## Purpose

1. Wrapper classes of SQLite
2. Use SQLite with STL

## Notice

* These classes need `basic_string` class template. If you want detail information, visit [here](https://github.com/mlach9u/col/tree/master/src/stringt).

## Usage

### Open database

#### Use open function

```cpp
sqliteDatabase< char > db;
db.open(filename);
```

#### Use contructor

```cpp
sqliteDatabase< char > db(filename);
```

### Query

* `execute` and `query` functions are separated.
* `execute` is used for `INSERT` or `UPDATE`, etc...
* `query` is used for `SELECT`, etc...

#### Execute query

```cpp
if (db.execute("INSERT INTO sqlitemp_table (name, age) VALUES ('%s', %d)", name, age))
    std::cout << "Succeeded" << std::endl;
else
    std::cout << "Failed" << std::endl;
```

#### Select query

```cpp
sqliteDatabase< char >::_Rowset_Ptr prs = db.query("SELECT * FROM sqlitemp_table");
if (prs.get() && db.good())
{
    sqliteDatabase< char >::_Columnset cs = prs->column();
    int nSize = cs.size();
    for (int i = 0; i < nSize; i++)
        std::cout << cs.at(i).name() << "\t";
    std::cout << std::endl;
    do
    {
        cs = prs->column();
        for (int i = 0; i < nSize; i++)
            std::cout << cs.at(i) << "\t";
        std::cout << std::endl;                
    } while (prs->to_next());
}
```
