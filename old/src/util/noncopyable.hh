#ifndef NONCOPYABLE_HH
#define NONCOPYABLE_HH


namespace ps {

class NonCopyable
{
public:
    NonCopyable (const NonCopyable &) = delete;
    NonCopyable & operator = (const NonCopyable &) = delete;

protected:
    NonCopyable () = default;
    ~NonCopyable () = default; /// Protected non-virtual destructor
};

}

#endif //NONCOPYABLE_HH
