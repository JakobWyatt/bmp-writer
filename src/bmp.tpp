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

#ifndef GUARD_BMP_TPP
#define GUARD_BMP_TPP

#include <cstdint>
#include <cstddef>

namespace bmp {

    template<std::size_t _width, std::size_t _height>
    details::pixel<_width, _height>::pixel(typename bmp<_width, _height>::iterator pixel_location)
    : pixel_location(pixel_location) {}

    //the image is stored as BGR byte order
    //to access the red pixel, we need to increment by 2
    template<std::size_t _width, std::size_t _height>
    void details::pixel<_width, _height>::red(std::uint8_t val) {
        *(pixel_location._iterator + 2) = val;
    }

    template<std::size_t _width, std::size_t _height>
    std::uint8_t details::pixel<_width, _height>::red() const {
        return *(pixel_location._iterator + 2);
    }

    //to access the green pixel, we need to increment by 1
    template<std::size_t _width, std::size_t _height>
    void details::pixel<_width, _height>::green(std::uint8_t val) {
        *(pixel_location._iterator + 1) = val;
    }

    template<std::size_t _width, std::size_t _height>
    std::uint8_t details::pixel<_width, _height>::green() const {
        return *(pixel_location._iterator + 1);
    }

    //no incrementation is needed for the blue pixel
    template<std::size_t _width, std::size_t _height>
    void details::pixel<_width, _height>::blue(std::uint8_t val) {
        *pixel_location._iterator = val;
    }

    template<std::size_t _width, std::size_t _height>
    std::uint8_t details::pixel<_width, _height>::blue() const {
        return *pixel_location._iterator;
    }

}

#endif