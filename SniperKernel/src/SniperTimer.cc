#include "SniperKernel/SniperTimer.h"

#define SecsInDay 86400

SniperTimer::SniperTimer(const std::string& name)
    : m_name(name)
    , m_elapsed(0.0)
    , m_mean(0.0)
    , m_ms(0.0)
    , m_numberOfMeasurements(0)
    , m_started(false)
    , m_paused(false) 
{
}

SniperTimer::~SniperTimer()
{
}

bool SniperTimer::start( void ) {
    struct timeval tv;

    m_started = true;
    m_paused = false;
    m_elapsed = 0.0;
    // TODO: if already started, the start time will be over-written
    gettimeofday(&tv, 0);
    m_startTime = tv;
    return true;
}
bool SniperTimer::stop ( void ) {
    struct timeval tv;

    if (!m_started) return false;

    gettimeofday(&tv, 0);
    m_started = false;

    if (!m_paused) {
        float mtime;
        int secs, usecs;

        if (tv.tv_sec >= m_startTime.tv_sec) {
            secs = tv.tv_sec - m_startTime.tv_sec;
        } else {
            secs = tv.tv_sec - m_startTime.tv_sec + SecsInDay;
        }
        usecs = tv.tv_usec - m_startTime.tv_usec;
        mtime = static_cast<float>(secs)*1000. + static_cast<float>(usecs)/1000.;

        // elapsed time
        m_elapsed += mtime;

    }

    // statistics - mean, rms
    double denom = static_cast <double> (++m_numberOfMeasurements);
    double d = m_elapsed - m_mean;
    m_mean += d / denom;
    double dd = m_elapsed*m_elapsed - m_ms;
    m_ms += dd / denom;

    return true;
}
bool SniperTimer::pause()
{
    struct timeval tv;

    // only pause if started
    // a pause while paused is ignored 
    if (m_paused || !m_started) return false;
    m_paused = true;
    gettimeofday(&tv, 0);

    float mtime;
    int secs, usecs;

    if (tv.tv_sec >= m_startTime.tv_sec) {
        secs = tv.tv_sec - m_startTime.tv_sec;
    } else {
        secs = tv.tv_sec - m_startTime.tv_sec + SecsInDay;
    }
    usecs = tv.tv_usec - m_startTime.tv_usec;
    mtime = static_cast<float>(secs)*1000. + static_cast<float>(usecs)/1000.;
    m_elapsed += mtime; // time so far

    return true;
}
bool SniperTimer::resume()
{
    struct timeval tv;

    if (!m_started) { 
        start(); // resume acts as start if not started
    } else if (m_paused) {
        m_paused = false;
        gettimeofday(&tv, 0);
        m_startTime = tv;
    }
    return true;
}
bool SniperTimer::reset () { 
    if (!m_started) {
        m_elapsed = 0.0; 
    }
    return true;
}
