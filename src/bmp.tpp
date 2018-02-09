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
#include <ostream>
#include <memory>

namespace bmp {
    //
    // impl of pixel class
    //
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

    //
    //impl of bmp::row_proxy
    //
    template<std::size_t _width, std::size_t _height>
    bmp<_width, _height>::row_proxy::row_proxy(iterator row)
    : row(row) {}

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::reference bmp<_width, _height>::row_proxy::operator[](size_type column) {
        return *(row + column);
    }

    //
    // impl of bmp class
    //
    template<std::size_t _width, std::size_t _height>
    std::ostream& bmp<_width, _height>::write(std::ostream& ofstream) const {
        //this function writes the char array directly to the underlying stream
        //first we must cast to char* and remove const
        ofstream.write(const_cast<char*>(reinterpret_cast<const char*>(&_data)), sizeof(_data));
        return ofstream;
    }

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::row_proxy bmp<_width, _height>::operator[](size_type row) {
        //we construct row_proxy with an iterator pointing to the start of the row
        //the image is stored bottom to top
        return row_proxy(begin() + _width * (_height - 1 - row));
    }

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator bmp<_width, _height>::begin() {
        return iterator(_data.bitmap_array.begin(), _data.bitmap_array.begin());
    }

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator bmp<_width, _height>::end() {
        return iterator(_data.bitmap_array.end(), _data.bitmap_array.begin());
    }

    //
    // impl of bmp::iterator
    //

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator::reference bmp<_width, _height>::iterator::operator*() const {
        return details::pixel<_width, _height>(*this);
    }

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator::pointer bmp<_width, _height>::iterator::operator->() const {
        return std::make_unique(
            details::pixel<_width, _height>(*this)
        );
    }

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator& bmp<_width, _height>::iterator::operator++() {
        auto index = _iterator - _begin;
        //if we are at the end of the row, move to the next row
        //  else move to the next pixel
        if (details::end_of_row<_width, _height>(index)) {
            _iterator += details::bitmap_array_pad_width<_width, _height>() + 3;
        } else {
            _iterator += 3;
        }
        return *this;
    }

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator bmp<_width, _height>::iterator::operator++(int) {
        auto tmp = iterator(_iterator, _begin);
        ++(*this);
        return tmp;
    }

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator& bmp<_width, _height>::iterator::operator--() {
        auto index = _iterator - _begin;
        //if we are at the start of a row, move to the previous row
        //else move to the previous pixel
        if (details::start_of_row<_width, _height>(index)) {
            _iterator -= details::bitmap_array_pad_width<_width, _height>() - 3;
        } else {
            _iterator -= 3;
        }
        return *this;
    }

    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator bmp<_width, _height>::iterator::operator--(int) {
        auto tmp = iterator(_iterator, _begin);
        --(*this);
        return tmp;
    }

    template<std::size_t _width, std::size_t _height>
    bool bmp<_width, _height>::iterator::operator==(typename bmp<_width, _height>::iterator it) const {
        return it._iterator == this->_iterator;
    }

    template<std::size_t _width, std::size_t _height>
    bool bmp<_width, _height>::iterator::operator!=(typename bmp<_width, _height>::iterator it) const {
        return it._iterator != this->_iterator;
    }

}

#endif