/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2014 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

// mapnik
#include <mapnik/image.hpp>

// stl
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <iostream>

namespace mapnik {

namespace detail {

// IMAGE_DIMENSIONS
template <std::size_t max_size>
image_dimensions<max_size>::image_dimensions(int width, int height)
    : width_(width),
      height_(height)
{
    if (width < 0 || static_cast<std::size_t>(width) > max_size) throw std::runtime_error("Invalid width for image dimensions requested");
    if (height < 0 || static_cast<std::size_t>(height) > max_size) throw std::runtime_error("Invalid height for image dimensions requested");
}

template <std::size_t max_size>
image_dimensions<max_size>& image_dimensions<max_size>::operator= (image_dimensions rhs)
{
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    return *this;
}

template <std::size_t max_size>
std::size_t image_dimensions<max_size>::width() const
{
    return width_;
}

template <std::size_t max_size>
std::size_t image_dimensions<max_size>::height() const
{
    return height_;
}

} // end detail ns

// IMAGE
template <typename T>
image<T>::image()
    : dimensions_(0,0),
      buffer_(0),
      pData_(nullptr),
      offset_(0.0),
      scaling_(1.0),
      premultiplied_alpha_(false),
      painted_(false)
{}

template <typename T>
image<T>::image(int width, int height, bool initialize, bool premultiplied, bool painted)
    : dimensions_(width, height),
      buffer_(dimensions_.width() * dimensions_.height() * pixel_size),
      pData_(reinterpret_cast<pixel_type*>(buffer_.data())),
      offset_(0.0),
      scaling_(1.0),
      premultiplied_alpha_(premultiplied),
      painted_(painted)
{
    if (pData_ && initialize)
    {
        std::fill(pData_, pData_ + dimensions_.width() * dimensions_.height(), 0);
    }
}

template <typename T>
image<T>::image(image<T> const& rhs)
    : dimensions_(rhs.dimensions_),
      buffer_(rhs.buffer_),
      pData_(reinterpret_cast<pixel_type*>(buffer_.data())),
      offset_(rhs.offset_),
      scaling_(rhs.scaling_),
      premultiplied_alpha_(rhs.premultiplied_alpha_),
      painted_(rhs.painted_)
{}

template <typename T>
image<T>::image(image<T> && rhs) noexcept
    : dimensions_(std::move(rhs.dimensions_)),
    buffer_(std::move(rhs.buffer_)),
    pData_(reinterpret_cast<pixel_type*>(buffer_.data())),
    offset_(rhs.offset_),
    scaling_(rhs.scaling_),
    premultiplied_alpha_(rhs.premultiplied_alpha_),
    painted_(rhs.painted_)
{
    rhs.dimensions_ = { 0, 0 };
    rhs.pData_ = nullptr;
}

template <typename T>
image<T>& image<T>::operator=(image<T> rhs)
{
    swap(rhs);
    return *this;
}

template <typename T>
bool image<T>::operator==(image<T> const& rhs) const
{
    return rhs.bytes() == bytes();
}

template <typename T>
bool image<T>::operator<(image<T> const& rhs) const
{
    return size() < rhs.size();
}

template <typename T>
void image<T>::swap(image<T> & rhs)
{
    std::swap(dimensions_, rhs.dimensions_);
    std::swap(buffer_, rhs.buffer_);
    std::swap(offset_, rhs.offset_);
    std::swap(scaling_, rhs.scaling_);
    std::swap(premultiplied_alpha_, rhs.premultiplied_alpha_);
    std::swap(painted_, rhs.painted_);
}

template <typename T>
inline typename image<T>::pixel_type& image<T>::operator() (std::size_t i, std::size_t j)
{
    assert(i < dimensions_.width() && j < dimensions_.height());
    return pData_[j * dimensions_.width() + i];
}

template <typename T>
inline const typename image<T>::pixel_type& image<T>::operator() (std::size_t i, std::size_t j) const
{
    assert(i < dimensions_.width() && j < dimensions_.height());
    return pData_[j * dimensions_.width() + i];
}

template <typename T>
inline std::size_t image<T>::width() const
{
    return dimensions_.width();
}

template <typename T>
inline std::size_t image<T>::height() const
{
    return dimensions_.height();
}

template <typename T>
inline std::size_t image<T>::size() const
{
    return dimensions_.height() * dimensions_.width() * pixel_size;
}

template <typename T>
inline std::size_t image<T>::row_size() const
{
    return dimensions_.width() * pixel_size;
}

template <typename T>
inline void image<T>::set(pixel_type const& t)
{
    std::fill(pData_, pData_ + dimensions_.width() * dimensions_.height(), t);
}

template <typename T>
inline const typename image<T>::pixel_type* image<T>::data() const
{
    return pData_;
}

template <typename T>
inline typename image<T>::pixel_type* image<T>::data()
{
    return pData_;
}

template <typename T>
inline const unsigned char* image<T>::bytes() const
{
    return buffer_.data();
}

template <typename T>
inline unsigned char* image<T>::bytes()
{
    return buffer_.data();
}

template <typename T>
inline typename image<T>::pixel_type const* image<T>::get_row(std::size_t row) const
{
    return pData_ + row * dimensions_.width();
}

template <typename T>
inline const typename image<T>::pixel_type* image<T>::get_row(std::size_t row, std::size_t x0) const
{
    return pData_ + row * dimensions_.width() + x0;
}

template <typename T>
inline typename image<T>::pixel_type* image<T>::get_row(std::size_t row)
{
    return pData_ + row * dimensions_.width();
}

template <typename T>
inline typename image<T>::pixel_type* image<T>::get_row(std::size_t row, std::size_t x0)
{
    return pData_ + row * dimensions_.width() + x0;
}

template <typename T>
inline void image<T>::set_row(std::size_t row, pixel_type const* buf, std::size_t size)
{
    assert(row < dimensions_.height());
    assert(size <= dimensions_.width());
    std::copy(buf, buf + size, pData_ + row * dimensions_.width());
}

template <typename T>
inline void image<T>::set_row(std::size_t row, std::size_t x0, std::size_t x1, pixel_type const* buf)
{
    assert(row < dimensions_.height());
    assert ((x1 - x0) <= dimensions_.width() );
    std::copy(buf, buf + (x1 - x0), pData_ + row * dimensions_.width() + x0);
}

template <typename T>
inline double image<T>::get_offset() const
{
    return offset_;
}

template <typename T>
inline void image<T>::set_offset(double set)
{
    offset_ = set;
}

template <typename T>
inline double image<T>::get_scaling() const
{
    return scaling_;
}

template <typename T>
inline void image<T>::set_scaling(double scaling)
{
    if (scaling != 0.0)
    {
        scaling_ = scaling;
        return;
    }
    std::clog << "Can not set scaling to 0.0, offset not set." << std::endl;
}

template <typename T>
inline bool image<T>::get_premultiplied() const
{
    return premultiplied_alpha_;
}

template <typename T>
inline void image<T>::set_premultiplied(bool set)
{
    premultiplied_alpha_ = set;
}

template <typename T>
inline void image<T>::painted(bool painted)
{
    painted_ = painted;
}

template <typename T>
inline bool image<T>::painted() const
{
    return painted_;
}

template <typename T>
inline image_dtype image<T>::get_dtype()  const
{
    return dtype;
}

} // end ns
