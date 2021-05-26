#ifndef SniperTimer_h
#define SniperTimer_h

#include <sys/time.h>
#include <iostream>
#include <string>

#include   <complex>

class SniperTimer {
    public:
        SniperTimer(const std::string& name);
        ~SniperTimer();
    public:
        bool start();
        bool stop();
        bool pause();
        bool resume();
        bool reset();
    public:
        // retrieve timer name
        const std::string& name() { return m_name; }
        // retrieve elapsed time
        float elapsed( void ) const { return (m_started? 0.0 : m_elapsed);}
        // retrieve mean and rms time
        double mean( void ) const { return m_mean;}
        double rms( void ) const { 
        double var = m_ms - m_mean*m_mean;
        return (var > 0 ? sqrt(var) : 0.0);
        }
        // retrieve number of measurements 
        int number_of_measurements( void ) const { return m_numberOfMeasurements;}

    private:
        std::string m_name;
        struct timeval m_startTime;
        float m_elapsed;
        double m_mean; // mean time
        double m_ms;   // mean squared time
        int m_numberOfMeasurements;
        bool m_started;
        bool m_paused;

};

#endif
