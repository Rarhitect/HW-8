//
//  main.cpp
//  HW8-4
//
//  Created by Alyaev Roman on 01.04.2021.
//

#include <iostream>
#include <exception>
#include <memory>
#include <mutex>
#include <stack>
#include <stdexcept>
#include <queue>
#include <chrono>
#include <thread>

#include <boost/lockfree/stack.hpp>
#include <boost/lockfree/queue.hpp>

template < typename T >
class Threadsafe_Stack
{
public:

    Threadsafe_Stack() = default;

    Threadsafe_Stack(const Threadsafe_Stack & other)
    {
        std::lock_guard < std::mutex > lock(other.m_mutex);
        m_data = other.m_data;
    }

    Threadsafe_Stack& operator=(const Threadsafe_Stack &) = delete;

public:

    void push(T value)
    {
        std::lock_guard < std::mutex > lock(m_mutex);
        m_data.push(value);
    }

    std::shared_ptr < T > pop()
    {
        std::lock_guard < std::mutex > lock(m_mutex);

        if (m_data.empty())
        {
            throw std::range_error("empty stack");
        }

        const auto result = std::make_shared < T > (m_data.top());
        m_data.pop();

        return result;
    }

    void pop(T & value)
    {
        std::lock_guard < std::mutex > lock(m_mutex);

        if (m_data.empty())
        {
            throw std::range_error("empty stack");
        }

        value = m_data.top();
        m_data.pop();
    }

    bool empty() const
    {
        std::lock_guard < std::mutex > lock(m_mutex);
        return m_data.empty();
    }

private:

    std::stack < T > m_data;
    mutable std::mutex m_mutex;
};

template < typename T >
class Threadsafe_Queue
{
public:

    Threadsafe_Queue() = default;

    Threadsafe_Queue(const Threadsafe_Queue & other)
    {
        std::lock_guard < std::mutex > lock(other.m_mutex);
        m_queue = other.m_queue;
    }

    Threadsafe_Queue & operator=(const Threadsafe_Queue & other)
    {
        std::lock_guard < std::mutex > lock(other.m_mutex);
        m_queue = other.m_queue;
    }

public:

    void push(T value)
    {
        std::lock_guard < std::mutex > lock(m_mutex);
        m_queue.push(value);
        m_condition_variable.notify_one();
    }

    void wait_and_pop(T & value)
    {
        std::unique_lock < std::mutex > lock(m_mutex);
        
        m_condition_variable.wait(lock, [this] {return !m_queue.empty(); });
        value = m_queue.front();
        m_queue.pop();
    }

    std::shared_ptr < T > wait_and_pop()
    {
        std::unique_lock < std::mutex > lock(m_mutex);
        
        m_condition_variable.wait(lock, [this] {return !m_queue.empty(); });
        auto result = std::make_shared < T > (m_queue.front());
        m_queue.pop();
        
        return result;
    }

    bool try_pop(T & value)
    {
        std::lock_guard < std::mutex > lock(m_mutex);
        
        if (m_queue.empty())
        {
            return false;
        }

        value = m_queue.front();
        m_queue.pop();

        return true;
    }

    std::shared_ptr < T > try_pop()
    {
        std::lock_guard < std::mutex > lock(m_mutex);
        
        if (m_queue.empty())
        {
            return std::shared_ptr < T > ();
        }
            
        auto result = std::make_shared < T > (m_queue.front());
        m_queue.pop();
        
        return result;
    }

    bool empty() const
    {
        std::lock_guard < std::mutex > lock(m_mutex);
        return m_queue.empty();
    }

private:

    std::queue < T > m_queue;
    std::condition_variable m_condition_variable;
    mutable std::mutex m_mutex;
};

template <typename T>
void threadsafe_stack_manufacturer(Threadsafe_Stack<T> & object, const std::size_t M);

template <typename T>
void threadsafe_stack_consumerist(Threadsafe_Stack<T> & object, const std::size_t M);

template <typename T>
void boost_stack_manufacturer(boost::lockfree::stack<T> & object, const std::size_t M);

template <typename T>
void boost_stack_consumerist(boost::lockfree::stack<T> & object, const std::size_t M);

template <typename T>
void threadsafe_queue_manufacturer(Threadsafe_Queue<T> & object, const std::size_t M);

template <typename T>
void threadsafe_queue_consumerist(Threadsafe_Queue<T> & object, const std::size_t M);

template <typename T>
void boost_queue_manufacturer(boost::lockfree::queue<T> & object, const std::size_t M);

template <typename T>
void boost_queue_consumerist(boost::lockfree::queue<T> & object, const std::size_t M);

template <typename T>
void time_of_work(Threadsafe_Stack<T> & ts_stack, boost::lockfree::stack<T> & boost_stack, Threadsafe_Queue<T> & ts_queue, boost::lockfree::queue<T> & boost_queue, const std::size_t N, const std::size_t M);

//=========================================================================================================

int main(int argc, const char * argv[])
{
    Threadsafe_Stack< int > ts_stack;
    Threadsafe_Queue< int > ts_queue;
    
    boost::lockfree::stack< int > boost_stack(1);
    boost::lockfree::queue< int > boost_queue(1);
    
    std::size_t N;
    std::size_t M;
    
    std::cout << "Hello, enter the quantity of threads (N) and then quantity of elements (M):" << std::endl;
    std::cin >> N;
    std::cin >> M;
    
    time_of_work(ts_stack, boost_stack, ts_queue, boost_queue, N, M);
    
    return 0;
}

//=========================================================================================================

template <typename T>
void threadsafe_stack_manufacturer(Threadsafe_Stack<T> & object, const std::size_t M)
{
    for (int i = 0; i < M; i++)
    {
        object.push(i);
    }
}

template <typename T>
void threadsafe_stack_consumerist(Threadsafe_Stack<T> & object, const std::size_t M)
{
    for (int i = 0; i < M; i++)
    {
        object.pop(i);
    }
}

template <typename T>
void boost_stack_manufacturer(boost::lockfree::stack<T> & object, const std::size_t M)
{
    for (int i = 0; i < M; i++)
    {
        object.push(i);
    }
}

template <typename T>
void boost_stack_consumerist(boost::lockfree::stack<T> & object, const std::size_t M)
{
    for (int i = 0; i < M; i++)
    {
        object.pop(i);
    }
}

template <typename T>
void threadsafe_queue_manufacturer(Threadsafe_Queue<T> & object, const std::size_t M)
{
    for (int i = 0; i < M; i++)
    {
        object.push(i);
    }
}

template <typename T>
void threadsafe_queue_consumerist(Threadsafe_Queue<T> & object, const std::size_t M)
{
    for (int i = 0; i < M; i++)
    {
        object.try_pop(i);
    }
}

template <typename T>
void boost_queue_manufacturer(boost::lockfree::queue<T> & object, const std::size_t M)
{
    for (int i = 0; i < M; i++)
    {
        object.push(i);
    }
}

template <typename T>
void boost_queue_consumerist(boost::lockfree::queue<T> & object, const std::size_t M)
{
    for (int i = 0; i < M; i++)
    {
        object.pop(i);
    }
}

template <typename T>
void time_of_work(Threadsafe_Stack<T> & ts_stack, boost::lockfree::stack<T> & boost_stack, Threadsafe_Queue<T> & ts_queue, boost::lockfree::queue<T> & boost_queue, const std::size_t N, const std::size_t M)
{
    std::vector < std::thread > ts_stack_threads(2 * N);
    
    for(auto i = 0; i < N; ++i)
    {
        ts_stack_threads[i] = std::thread(threadsafe_stack_manufacturer<T>, std::ref(ts_stack), M);
    }
    for(auto i = N; i < 2 * N; ++i)
    {
        ts_stack_threads[i] = std::thread(threadsafe_stack_consumerist<T>, std::ref(ts_stack), M);
    }
    
    auto begin_1 = std::chrono::system_clock::now();
    std::for_each(ts_stack_threads.begin(), ts_stack_threads.end(), std::mem_fn(&std::thread::join));
    auto end_1 = std::chrono::system_clock::now();
    auto elapsed_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_1 - begin_1);
    std::cout << "Threadsafe stack: " << elapsed_1.count() << " ns" << std::endl;
    
    std::vector < std::thread > boost_stack_threads(2 * N);
    
    for(auto i = 0; i < N; ++i)
    {
        boost_stack_threads[i] = std::thread(boost_stack_manufacturer<T>, std::ref(boost_stack), M);
    }
    for(auto i = N; i < 2 * N; ++i)
    {
        boost_stack_threads[i] = std::thread(boost_stack_consumerist<T>, std::ref(boost_stack), M);
    }
    
    auto begin_2 = std::chrono::system_clock::now();
    std::for_each(boost_stack_threads.begin(), boost_stack_threads.end(), std::mem_fn(&std::thread::join));
    auto end_2 = std::chrono::system_clock::now();
    auto elapsed_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_2 - begin_2);
    std::cout << "Boost lockfree stack: " << elapsed_2.count() << " ns" << std::endl;
    
    std::vector < std::thread > ts_queue_threads(2 * N);
    
    for(auto i = 0; i < N; ++i)
    {
        ts_queue_threads[i] = std::thread(threadsafe_queue_manufacturer<T>, std::ref(ts_queue), M);
    }
    for(auto i = N; i < 2 * N; ++i)
    {
        ts_queue_threads[i] = std::thread(threadsafe_queue_consumerist<T>, std::ref(ts_queue), M);
    }
    
    auto begin_3 = std::chrono::system_clock::now();
    std::for_each(ts_queue_threads.begin(), ts_queue_threads.end(), std::mem_fn(&std::thread::join));
    auto end_3 = std::chrono::system_clock::now();
    auto elapsed_3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_3 - begin_3);
    std::cout << "Threadsafe queue: " << elapsed_3.count() << " ns" << std::endl;
    
    std::vector < std::thread > boost_queue_threads(2 * N);
    
    for(auto i = 0; i < N; ++i)
    {
        boost_queue_threads[i] = std::thread(boost_queue_manufacturer<T>, std::ref(boost_queue), M);
    }
    for(auto i = N; i < 2 * N; ++i)
    {
        boost_queue_threads[i] = std::thread(boost_queue_consumerist<T>, std::ref(boost_queue), M);
    }
    
    auto begin_4 = std::chrono::system_clock::now();
    std::for_each(boost_queue_threads.begin(), boost_queue_threads.end(), std::mem_fn(&std::thread::join));
    auto end_4 = std::chrono::system_clock::now();
    auto elapsed_4 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_4 - begin_4);
    std::cout << "Boost lockfree queue: " << elapsed_4.count() << " ns" << std::endl;
}
