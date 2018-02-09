//Copyright 2018 Jakob Wyatt
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#include <fstream>
#include <iostream>
#include <ostream>
#include <utility>
#include "../include/bmp.hpp"

std::pair<std::ostream&, std::ostream&> test_suite(std::ostream& ofstream, std::ostream& cout) {
    std::cout << "\nStarting tests...\n";

    bmp::bmp<5, 10> image;

    auto it = image.begin();
    auto end = image.end();

    for(int i = 0; it != end; ++it) {
        (*it).green(i);
        (*it).blue(i);
        (*it).red(i);
        i += 5;
    }

    image.write(ofstream);

    return std::make_pair(std::ref(ofstream), std::ref(cout));
}

int main() {
    std::ofstream file("testiter.bmp", std::ios_base::binary);
    
    test_suite(file, std::cout);

    return 0;
}