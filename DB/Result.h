#ifndef DB_RESULT_H
#define DB_RESULT_H

#include <QStringList>
#include <KSharedPtr>

namespace DB
{
class ResultId;
class ResultPtr;
class ConstResultPtr;

// TODO: to be renamed to ListId as per discussion in car
class Result :public KShared
    {
    public:
        class ConstIterator
        {
        public:
            ConstIterator( const Result* result, int pos );
            ConstIterator& operator++();
            DB::ResultId operator*();
            bool operator!=( const ConstIterator& other );

        private:
            const Result* _result;
            int _pos;
        };
        typedef ConstIterator const_iterator;

        /** Never use Result on a stack but rather new it and assign to
         * some (Const)ResultPtr. The fact that the destructor is private will
         * remind you about this ;) */
        Result();

        /** Create a result with a list of file ids. */
        explicit Result( const QList<int>& ids );

        /** Convenience constructor: create a result only one ResultId. */
        explicit Result( const DB::ResultId& );

        void append( const DB::ResultId& );
        void prepend( const DB::ResultId& );

        /** Append the content of another DB::Result */
        void appendAll( const DB::Result& );

        DB::ResultId at(int index) const;
        int size() const;
        bool isEmpty() const;
        int indexOf(const DB::ResultId&) const;
        ConstIterator begin() const;
        ConstIterator end() const;
        void debug();

    private:
        // Noone must delete the Result directly. Only SharedPtr may.
        friend class KSharedPtr<Result>;
        friend class KSharedPtr<const Result>;
        ~Result();  // Don't use Q_FOREACH on DB::Result.

        // No implicit constructors/assignments.
        Result(const DB::Result&);
        DB::Result& operator=(const DB::Result&);

    private:
        QList<int> _items;
    };

class ResultPtr :public KSharedPtr<Result>
{
public:
    ResultPtr( Result* ptr );

private:
    int count() const;  // You certainly meant to call ->size() ?!
};

class ConstResultPtr :public KSharedPtr<const Result>
{
public:
    ConstResultPtr( const Result* ptr );

private:
    int count() const;  // You certainly meant to call ->size() ?!
};


}  // namespace DB


#endif /* DB_RESULT_H */
