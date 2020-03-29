
# Template classes for SQLite (SQLitemp)

---

## License

This is in the Public Domain.

## Purpose

1. Wrapper classes of SQLite
2. Use SQLite with STL

## Usage

### Open database

#### Use open function

```cpp
sqliteDatabase db;
db.open(filename);
```

#### Use contructor

```cpp
sqliteDatabase db(filename);
```

### Query

* `execute` and `query` functions are separated.
* `execute` is used for `INSERT` or `UPDATE`, etc...
* `query` is used for `SELECT`, etc...

#### Execute query

```cpp
if (db.execute("INSERT INTO sqlitemp_table (name, age) VALUES ('Dylan Kim', 30)"))
    std::cout << "Succeeded" << std::endl;
else
    std::cout << "Failed" << std::endl;
```

#### Select query

```cpp
sqliteDatabase::rowset_ptr prs = db.query("SELECT * FROM sqlitemp_table");
if (prs.get() && db.good())
{
    sqliteColumnSet cs = prs->column();
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
