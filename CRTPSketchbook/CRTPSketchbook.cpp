#include <chrono>
#include <iostream>

class ICounter
{
public:
    virtual ~ICounter() = default;
    ICounter(ICounter const&) = delete;
    ICounter& operator=(ICounter const&) = delete;
    ICounter(ICounter&&) = delete;
    ICounter& operator=(ICounter&&) = delete;

    virtual void increment() = 0;
    [[nodiscard]] virtual int get() const = 0;

protected:
    ICounter() = default;
};

class Counter final : public ICounter
{
public:

    void increment() override
    {
        ++m_value;
    }

    [[nodiscard]] int get() const override
    {
        return m_value;
    }

private:
    int m_value = 0;
};

class DynamicGuardedCounter final : public ICounter
{
public:

    DynamicGuardedCounter(ICounter& counterImpl)
        : m_counter{ counterImpl }
    {}

    void increment() override
    {
        try
        {
            m_counter.increment();
        }
        catch (...)
        {
            std::cout << "An exception was thrown in " __FUNCTION__ << std::endl;
            throw;
        }
    }

    [[nodiscard]] int get() const override
    {
        try
        {
            return m_counter.get();
        }
        catch (...)
        {
            std::cout << "An exception was thrown in " __FUNCTION__ << std::endl;
            throw;
        }
    }

private:
    ICounter& m_counter;
};

template <class CounterImpl>
class StaticGuardedCounter final : public ICounter
{
public:

    StaticGuardedCounter(CounterImpl& counterImpl)
        : m_counter{ counterImpl }
    {}

    void increment() override
    {
        try
        {
            m_counter.increment();
        }
        catch (...)
        {
            std::cout << "An exception was thrown in " __FUNCTION__ << std::endl;
            throw;
        }
    }

    [[nodiscard]] int get() const override
    {
        try
        {
            return m_counter.get();
        }
        catch (...)
        {
            std::cout << "An exception was thrown in " __FUNCTION__ << std::endl;
            throw;
        }
    }

private:
    CounterImpl& m_counter;
};

template <class Counter>
auto benchmarkIncrement(Counter& counter)
{
    using clock = std::chrono::steady_clock;
    auto constexpr numTrials = 10000000;

    auto const start_time = clock::now();
    for (auto i = 0; i < numTrials; ++i)
    {
        counter.increment();
    }
    auto const micros = std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start_time);
    return micros.count();
}

int main()
{
    auto counter1 = Counter{};
    auto dynamicCounter = DynamicGuardedCounter{ counter1 };
    std::cout << "Dynamic Counter: " << benchmarkIncrement(dynamicCounter) << std::endl;
    //std::cout << dynamicCounter.get() << std::endl;

    auto counter2 = Counter{};
    auto staticCounter = StaticGuardedCounter{ counter2 };
    std::cout << "Static Counter: " << benchmarkIncrement(staticCounter) << std::endl;
    //std::cout << staticCounter.get() << std::endl;

    return 0;
}

