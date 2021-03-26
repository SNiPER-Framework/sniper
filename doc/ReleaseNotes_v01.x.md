# SNiPER v1.x Release Notes

This is the release notes for the SNiPER project.

## v1.7

- Changes
    - migrate from Python 2 to Python 3

## v1.6

- Fixes
    - Handle the new TPython since ROOT 6.22

- Changes
    - Use "-O2" by default while building

## v1.5

- Fixes
    - Using TFile::Open instead of new TFile

- New feature
    - Now we can stop the application by Ctrl+C
    - JSON support in Sniper

## v1.4

- Fixes
    - finalize() can be invoked twice when there is an error
    - using dict to set property value in Python3 

- Changes
    - stop the initialization when there is any error

- Improvements
    - better CMake supports

- New features
    - Now we can write algorithms in Python. An example: Examples/HelloWorld/python/HelloWorld/HelloPy.py
    - Add a new Package SniperUtil/PyDataStore. We can exchange data between C++ and Python algorithms with PyDataStore.
    - In Examples/HelloWorld, we can find how PyDataStore works.

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
