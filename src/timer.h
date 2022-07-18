#ifndef TIMER_H_
#define TIMER_H_
class Timer
{
public:
    Timer(std::string &&tag) :
        m_tag{std::move(tag)}
    {
        m_start = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        auto duration = std::chrono::high_resolution_clock::now() - m_start;
        std::cout << m_tag << ": " << duration.count() << std::endl;
    }
private:
    std::string m_tag;
    std::chrono::high_resolution_clock::time_point m_start;
};
#endif
