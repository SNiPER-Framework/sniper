/* Copyright (C) 2020
   Institute of High Energy Physics and Shandong University
   This file is part of SNiPER.
 
   SNiPER is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   SNiPER is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public License
   along with SNiPER.  If not, see <http://www.gnu.org/licenses/>. */

#include "SniperKernel/SniperJSON.h"
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

int main(void)
{
    /* 
     * the 1st scope for C++ -> json testing
     */
    try
    {
        //a string C++ object
        std::string aString{"this is a string"};
        //a vector C++ object
        std::vector<bool> aVecBool{true, false};
        //a map C++ object
        std::map<int, float> aMapI2F{
            {2, 0.1},
            {3, -3.6},
            {5, 5.5555e2},
            {7, 5.55E-1},
            {11, 0.66E+1}
        };

        //init the json with C++ objects
        SniperJSON json;
        json.insert("String",  SniperJSON().from(aString));
        json.insert("VecBool", SniperJSON().from(aVecBool));
        json.insert("MapI2F",  SniperJSON().from(aMapI2F));

        //dump the json to a file
        std::ofstream ofs("TestJSON.json");
        SniperJSON::dump(json, ofs);
    }
    catch(std::exception& ex)
    {
        std::cerr << "[scope 1] " << ex.what() << std::endl;
        return 1;
    }

    /* 
     * the 2nd scope for json -> C++ testing
     */
    try {
        //load the json from a file
        std::ifstream ifs("TestJSON.json");
        SniperJSON json = SniperJSON::load(ifs);

        //add some new values to json
        json.insert("Description", SniperJSON().from("This is a demo for SniperJSON"));
        json["VecBool"].push_back(SniperJSON().from(false));
        json["VecBool"].push_back(SniperJSON().from(true));
        json["MapI2F"].insert("13", SniperJSON().from(0.7777));
        json["MapI2F"].insert("17", SniperJSON().from(99.9));

        //access cpp values from json
        auto aString  = json["String"].get<std::string>();
        auto aVecBool = json["VecBool"].get<std::vector<bool>>();
        auto aMapI2F  = json["MapI2F"].get<std::map<int, float>>();

        //check the values
        std::cout << "Get a string: " << aString << std::endl;
        std::cout << "Get a vector: [ ";
        std::for_each(aVecBool.begin(), aVecBool.end(), [](bool var){
                std::cout << var << ' ';
                });
        std::cout << "]" << std::endl;
        std::cout << "Get a map: {" << std::endl;
        //we cannot use auto in lambda until c++14...
        typedef std::pair<int, float> ThePair;
        std::for_each(aMapI2F.begin(), aMapI2F.end(), [](ThePair var){
                std::cout << var.first << ": " << var.second << std::endl;
                });
        std::cout << "}" << std::endl;

        //we can also dumps the json to a string
        std::string jstr = SniperJSON::dumps(json);
        std::cout << "The final json is >>>>>>\n" << jstr << std::endl;
    }
    catch(std::exception& ex) {
        std::cerr << "[scope 2] " << ex.what() << std::endl;
        return 2;
    }

    return 0;
}
