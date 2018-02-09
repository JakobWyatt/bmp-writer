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

#ifndef GUARD_BMP_HPP
#define GUARD_BMP_HPP

#include <array>
#include <ostream>
#include <cstdint>
#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>

namespace bmp {
    //interface class for our bitmap POD struct
    template <std::size_t _width, std::size_t _height> class bmp;

    //this namespace is not intended to be accessed by library users
    namespace details {
        //
        // helper functions
        //

        template<std::size_t _width, std::size_t _height> std::size_t constexpr data_bytes_per_row() {
            //3 bytes per pixel
            return _width * 3;
        }

        //calculates the size in bytes of a padded bitmap row
        template<std::size_t _width, std::size_t _height> std::size_t constexpr bitmap_row_size() {
            //our bitmap array is stored row wise,
            //with 3 bytes per pixel
            //each row is padded to a multiple of 4 bytes
            return ((data_bytes_per_row<_width, _height>()   //find the number of data-storing bytes per row
                    + 3) / 4)                               //use int truncation to find the number of multiples of 4 we have
                    * 4;                                    //multiply by 4 to find the number of bytes in a row
        }

        //calculates the size, in bytes, of a padded bitmap array
        template<std::size_t _width, std::size_t _height> std::size_t constexpr bitmap_array_size() {
            return bitmap_row_size<_width, _height>() * _height;
        }

        template<std::size_t _width, std::size_t _height> std::size_t constexpr bitmap_array_pad_width() {
            return bitmap_row_size<_width, _height>() - (_width * 3);     //subtract by the number of data-storing bytes to find the pad width
        }
        
        //determines if a 0-indexed element of a padded byte array
        //  points to the last pixel in a row,
        //  given that it points to a valid pixel
        template<std::size_t _width, std::size_t _height> bool end_of_row(std::size_t elem) {
            return 
                (elem % bitmap_row_size<_width, _height>()) //reduce the problem to a single row
                / 3                                         //find the 0-indexed pixel value from the start of the row
             == (_width - 1);                            //check if this is the last pixel in the row
        }

        //determines if a 0-indexed element of a padded byte array
        //  points to the first pixel in a row,
        //  given that it points to a valid pixel
        template<std::size_t _width, std::size_t _height> bool start_of_row(std::size_t elem) {
            return (elem % bitmap_row_size<_width, _height>()) == 0;
        }

        //
        // POD file structs
        //

        //this POD struct defines info about our bitmap array
        #pragma pack(1)
        template<std::size_t _width, std::size_t _height> struct bitmapinfoheader {
            //the size of this header
            std::uint32_t header_size = sizeof(bitmapinfoheader);
            //width and height of our bitmap, in pixels
            std::int32_t width = _width;
            std::int32_t height = _height;
            //number of colour planes, must be 1
            std::uint16_t colour_planes = 1;
            //standard RGB with 256 intensity values per colour
            //check https://msdn.microsoft.com/en-us/library/windows/desktop/dd183376(v=vs.85).aspx
            //  for alternate values
            std::uint16_t bits_per_pixel = 24;
            //non-compressed bitmap (BI_RGB)
            //check link above for alternate values
            std::uint32_t compression_type = 0;
            //number of bytes in the bitmap
            //may be set to 0 if the bitmap's compression_type = 0
            std::uint32_t image_size = 0;
            //the pixel density of the image
            //standard of 72 ppi * 39.37 inches per meter = 2834.64
            //  which is approx 2835 pixels per meter
            std::int32_t horizontal_pixels_per_meter = 2835;
            std::int32_t vertical_pixels_per_meter = 2835;
            //number of colour indexes in the colour table that are actually used by the bitmap
            //if this value if 0, the bitmap is assumed to use the maximum number of colours
            //  given by bits_per_pixel and compression_type
            std::uint32_t colour_indexes_used = 0;
            //number of colour indexes required to display the bitmap
            //if this value is 0, it is assumed that all colours are required
            std::uint32_t colour_indexes_required = 0;
        };
        
        //this POD struct describes info about our bmp file
        #pragma pack(1)
        template<std::size_t _width, std::size_t _height> struct bitmapfileheader {
            //first two bytes in the header are the ascii encoded characters "BM"
            //the windows standard uses a WORD, (unsigned 16-bit int), for these bytes
            char file_type[2] = {'B', 'M'};
            //file size in bytes of the #include <fstream>total bmp file
            std::uint32_t file_size = sizeof(bitmapfileheader) + sizeof(bitmapinfoheader<_width, _height>) + bitmap_array_size<_width, _height>();
            //these two values are reserved
            std::uint16_t reserved1 = 0;
            std::uint16_t reserved2 = 0;
            //offset in bytes from the beginning of the file to the bitmap array
            std::uint32_t byte_offset = sizeof(bitmapfileheader) + sizeof(bitmapinfoheader<_width, _height>);
        };
        
        //POD struct for storing the data in our bitmap
        #pragma pack(1)
        template<std::size_t _width, std::size_t _height> struct bitmapdata {
            bitmapfileheader<_width, _height> _bitmapfileheader;
            bitmapinfoheader<_width, _height> _bitmapinfoheader;
            //bitmap array is stored row wise, left to right and bottom to top
            //each row is padded at the end such that the row is a multiple of
            //      8 bytes long
            //each pixel is represented by 3 bytes
           std::array<std::uint8_t, bitmap_array_size<_width, _height>()> bitmap_array;
        };

        //proxy class through which the bitmap is modified
        template<std::size_t _width, std::size_t _height> class pixel {
            public:
                pixel(typename bmp<_width, _height>::iterator pixel_location);

                //gets value of pixel
                std::uint8_t red() const;
                std::uint8_t green() const;
                std::uint8_t blue() const;
                //sets value of pixel
                void red(std::uint8_t val);
                void green(std::uint8_t val);
                void blue(std::uint8_t val);
            private:
                typename bmp<_width, _height>::iterator pixel_location;
        };
    }

    //this class provides our interface for the POD BMP struct
    //havent bothered to satisfy any container_concepts yet,
    //      but might do so in future
    template<std::size_t _width, std::size_t _height> class bmp {
        //private nested class forward declaration
        class row_proxy;

        public:
        using value_type = details::pixel<_width, _height>;
        using size_type = std::size_t;
        //the reference is actually the proxy pixel object
        using reference = details::pixel<_width, _height>;

        //we satisfy random_access_iterator here,
        //  might implement contiguous_iterator in future
        class iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = bmp::value_type;
            using pointer = std::unique_ptr<details::pixel<_width, _height>>;
            using reference = bmp::reference;
            using iterator_category = std::random_access_iterator_tag;

            reference operator*() const;
            pointer operator->() const;

            iterator& operator++();
            iterator operator++(int);
            iterator& operator--();
            iterator operator--(int);

            iterator& operator+=(difference_type rhs);
            iterator operator+(difference_type rhs) const;
            iterator& operator-=(difference_type rhs);
            iterator operator-(difference_type rhs) const;

            difference_type operator-(iterator rhs) const;
            reference operator[](difference_type) const;

            bool operator==(iterator rhs) const;
            bool operator!=(iterator rhs) const;
            bool operator<(iterator rhs) const;
            bool operator>(iterator rhs) const;
            bool operator<=(iterator rhs) const;
            bool operator>=(iterator rhs) const;

        private:
            using raw_iter = typename std::array<std::uint8_t, details::bitmap_array_size<_width, _height>()>::iterator;
            //iterator of underlying array
            raw_iter _iterator;
            //begin iterator of underlying array for padding calculation
            raw_iter _begin;
            //helper ctor for bmp
            iterator(raw_iter _iterator, raw_iter _begin) : _iterator(_iterator), _begin(_begin) {}
            //the pixel class is used to modify the underlying array
            friend class details::pixel<_width, _height>;
            friend class bmp<_width, _height>;
            
            };

        //writes the images binary data to the given output stream
        std::ostream& write(std::ostream&) const;

        //correct usage is a double overload of operator[]
        //eg bmp[1][3] will return a reference to a pixel object
        //  located at the 2nd row and 4th column
        //(0-indexing)
        row_proxy operator[](size_type row);

        //iterator helper functions
        iterator begin();
        iterator end();

        private:
        details::bitmapdata<_width, _height> _data;

        class row_proxy {
        public:
            row_proxy(iterator row);

            reference operator[](size_type column);
        private:
            iterator row;
        };
    };

    //non-class operators for bmp::iterator            using value_type = bmp::value_type;
    template<std::size_t _width, std::size_t _height>
    typename bmp<_width, _height>::iterator
    operator+(typename bmp<_width, _height>::iterator::difference_type lhs, typename bmp<_width, _height>::iterator rhs);
}
 
#include "../src/bmp.tpp"

#endif