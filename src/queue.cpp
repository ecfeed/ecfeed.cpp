#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class TestProducer;

template<typename T>
class TestQueue{
    class const_iterator{
        const bool END_ITERATOR;
        TestQueue<T>& _queue;
        
    public:
        const_iterator(TestQueue<T>& queue, bool end = false) : 
            _queue(queue), END_ITERATOR(end)
        {}

        const_iterator(const const_iterator& other) : 
            _queue(other._queue), END_ITERATOR(other.END_ITERATOR)
        {}

        bool operator==(const const_iterator& other) const{
            if(END_ITERATOR && other.END_ITERATOR){
                return true;
            }

            if(END_ITERATOR || other.END_ITERATOR){
                return _queue.done();
            }
            return false;
        }

        bool operator!=(const const_iterator& other) const{
            return (*this == other) == false;
        }

        const_iterator& operator++(){
            _queue.next();
            return *this;
        }

        const T& operator*(){
            return _queue.current_element();
        }
    };
                // std::cout << "line: " << __LINE__ << std::endl;

    bool _done;
    std::vector<T> _data;
    const_iterator _begin;
    const_iterator _end;
    std::mutex _mutex;
    std::mutex _cv_mutex;
    std::condition_variable _cv;

public:
    TestQueue() :
        _done(false),
        _begin(*this),
        _end(*this, true)
    {}

    const_iterator begin() const {return _begin;}
    const_iterator end() const {return _end;}

    friend class const_iterator;
    friend class TestProducer;

private:
    bool done() {
        std::unique_lock<std::mutex> cv_lock(_mutex);

        if(_data.size() != 0){
            return false;
        }
        else if(_done){
            return true;
        }

        _cv.wait(cv_lock);
        return _done;
    }

    void next() {
        std::lock_guard<std::mutex> lock(_mutex);
        _data.erase(_data.begin());
    }

    void insert(T element){
        std::lock_guard<std::mutex> lock(_mutex);
        _data.push_back(element);
        _cv.notify_one();
    }

    T& current_element() {
        std::unique_lock<std::mutex> cv_lock(_mutex);
        if(_data.size() > 0){
            return _data[0];
        }
        _cv.wait(cv_lock);
        return _data[0];
    }
    void finish(){
        std::lock_guard<std::mutex> lock(_mutex);
        _done = true;
        _cv.notify_one();
    }

    bool empty(){
        std::lock_guard<std::mutex> lock(_mutex);
        return _data.size() == 0;
    }

};

class TestProducer{
    TestQueue<int>& _queue;
    std::thread _producing_thread;

public:
    TestProducer(TestQueue<int>& q) : _queue(q)
    {
        _producing_thread = std::thread([this]{
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            _queue.insert(3);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            _queue.insert(4);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            _queue.insert(2);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            _queue.insert(8);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            _queue.finish();
        });
    }

    ~TestProducer(){
        if(_producing_thread.joinable()){
            _producing_thread.join();
        }
    }

};

int main(int argc, char** argv){
    TestQueue<int> q;
    TestProducer p(q);

    for(auto x : q){
        std::cout << x << std::endl;
    }

    TestQueue<int> q1;
    TestProducer p1(q1);

    for(auto it = q1.begin(); it != q1.end(); ++it){
        std::cout << *it << std::endl;
    }

    return 0;
}