# Change Log

This is the ChangeLog for the SNiPER project.

## v1.3

- Changes
    - The default behaviour of Task::stop()/TaskWatchDog::stop() is stopping the process promptly, which is different than before (peacefully).

- New features
    - Stop SNiPER via Task::stop()/TaskWatchDog::stop() in multiple ways with a parameter.

## v1.2

- Changes
    - ParaPack is seperated as a standalone project, MT.SNiPER. Please see https://github.com/SNiPER-Framework/mt.sniper
    - Remove some python bindings which are not thread safe.

- Improvements
    - Optimization of SniperLog. Now we can print colorful logs, add timestamps for each line, and set a log file instead of cout.

- New features
    - Add CMake build support. This is contributed by Vladimir Belov <belov@itep.ru>.

## v1.1

- Changes
    - Instead of a bool value, an integer is returned from Incident::fire(). The new return value represent the number of handlers that be triggered by the incident.

## v1.0

- Initial release, see README
