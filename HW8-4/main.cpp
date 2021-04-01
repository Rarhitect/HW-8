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

int main(int argc, const char * argv[])
{
    
    
    return 0;
}
