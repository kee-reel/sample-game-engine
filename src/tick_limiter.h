#ifndef TICK_LIMITER_H_
#define TICK_LIMITER_H_
struct TickLimiter
{
public:
    TickLimiter(float seconds) :
        m_start{std::chrono::high_resolution_clock::now()},
        m_limit{seconds}
    {
    }
    
    void operator()()
    {
        auto time_passed = std::chrono::high_resolution_clock::now() - m_start;
        if(m_limit > time_passed)
            std::this_thread::sleep_for(m_limit - time_passed);
        m_prev_time = m_start;
        m_start = std::chrono::high_resolution_clock::now();
        m_dt = (m_start - m_prev_time);
    }

    float dt()
    {
        return m_dt.count();
    }

private:
    std::chrono::duration<float, std::milli> m_limit, m_dt;
    std::chrono::high_resolution_clock::time_point m_start, m_prev_time;
};
#endif
