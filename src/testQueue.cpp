template<typename T>
class TestQueue
{
    class const_iterator : public std::iterator<std::forward_iterator_tag, TestQueue>
    {
        const bool END_ITERATOR;
        TestQueue<T>& _queue;
        
    public:
        
        const_iterator(TestQueue<T>& queue, bool end = false) :
            _queue(queue), END_ITERATOR(end)
        {}

        const_iterator(const const_iterator& other) :
            _queue(other._queue), END_ITERATOR(other.END_ITERATOR)
        {}

        bool operator==(const const_iterator& other) const
        {
            if (END_ITERATOR && other.END_ITERATOR) {
                return true;
            }

            if (END_ITERATOR || other.END_ITERATOR) {
                return _queue.done();
            }

            return false;
        }

        bool operator!=(const const_iterator& other) const
        {
            return (*this == other) == false;
        }

        const_iterator& operator++()
        {
            _queue.next();
            return *this;
        }

        const T& operator*()
        {
            return _queue.current_element();
        }
    };

    bool _done;
    std::vector<T> _data;
    const_iterator _begin;
    const_iterator _end;
    std::mutex _mutex;
    std::mutex _cv_mutex;
    std::condition_variable _cv;
    MethodInfo method_info;

public:
    typedef T value_type;

    TestQueue() :
        _done(false),
        _begin(*this),
        _end(*this, true)
    {}

    const_iterator begin() const {return _begin;}
    const_iterator end() const {return _end;}

    friend class const_iterator;
    friend class TestProvider;

    bool done() {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        if (_data.size() != 0) {
            return false;
        }
        else if (_done) {
            return true;
        }

        _cv.wait(cv_lock);
        return _done;
    }

    void next() 
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _data.erase(_data.begin());
    }

    void insert(const T& element)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        _data.push_back(element);
        _cv.notify_one();
    }

    T& current_element() 
    {
        std::unique_lock<std::mutex> cv_lock(_mutex);
        if (_data.size() > 0) {
            return _data[0];
        }
        _cv.wait(cv_lock);
        return _data[0];
    }

    void finish()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _done = true;
        _cv.notify_one();
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _data.size() == 0;
    }

    std::vector<T> toList() 
    {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        while (!_done) {
            _cv.wait(cv_lock);
        }

        return _data;
    }

    void setMethodInfo(MethodInfo& method_info)
    {
        this->method_info = method_info;
    }

    MethodInfo& getMethodInfo()
    {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        while (!_done) {
            _cv.wait(cv_lock);
        }
        
        return method_info;
    }
};