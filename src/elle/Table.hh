#pragma once

#include <memory>

#include <boost/range/irange.hpp>

#include <elle/assert.hh>
#include <elle/attribute.hh>
#include <elle/err.hh>
#include <elle/math.hh>
#include <elle/meta.hh>

namespace elle
{
  namespace _details
  {
    namespace table
    {
      template <int index = 0, typename ... T>
      constexpr
      int
      size(std::tuple<T...> const& t)
      {
        if constexpr(index == sizeof...(T))
          return 1;
        else
          return std::get<index>(t) * size<index + 1>(t);
      }

      template <int dimension, typename I, typename T>
      auto
      _dimensions(I const& init, T& res)
      {
        if constexpr(dimension < std::tuple_size<T>::value)
        {
          std::get<dimension>(res) = init.size();
          _dimensions<dimension + 1>(*begin(init), res);
        }
      }

      template <int dimension, typename T>
      auto
      dimensions(T init)
      {
        typename elle::meta::repeat<int, dimension>::
          template apply<std::tuple> res;
        _dimensions<0>(init, res);
        return res;
      }

      template <typename T>
      using aligned =
        typename std::aligned_storage<sizeof(T), alignof(T)>::type;

      template <typename T, int dimension>
      struct Initializer
      {
        using type = std::initializer_list<
          typename Initializer<T, dimension - 1>::type>;
        static
        void
        distribute(type const& init, aligned<T>*& p)
        {
          for (auto& e: init)
            Initializer<T, dimension - 1>::distribute(e, p);
        }
      };

      template <typename T>
      struct Initializer<T, 0>
      {
        using type = T;
        static
        void
        distribute(T const& e, aligned<T>*& p)
        {
          new (p) T(e);
          p += 1;
        }
      };
    }
  }

  template <typename T, typename ... Indexes>
  class TableImpl
  {
  public:
    using Index = std::tuple<Indexes...>;
    using Storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    static auto constexpr dimension = sizeof...(Indexes);

    TableImpl(Indexes ... dimensions)
      : TableImpl(std::make_tuple(std::forward<Indexes>(dimensions)...))
    {}

    TableImpl(std::tuple<Indexes...> dimensions)
      : TableImpl(std::move(dimensions), true)
    {
      for (auto i : boost::irange(0, this->size()))
        new (&this->_table[i]) T();
    }

    TableImpl(typename _details::table::Initializer<T, dimension>::type init)
      : TableImpl(_details::table::dimensions<dimension>(init), true)
    {
      Storage* p = &this->_table[0];
      _details::table::Initializer<T, dimension>::distribute(
        std::move(init), p);
    }

    ~TableImpl()
    {
      for (auto i : boost::irange(0, this->size()))
        reinterpret_cast<T&>(this->_table[i]).~T();
    }

    T&
    at(Indexes const& ... indexes)
    {
      return this->at(Index(indexes...));
    }

    T&
    at(std::tuple<Indexes...> const& index)
    {
      if (!this->_check_boundaries(index, std::make_index_sequence<sizeof...(Indexes)>()))
        elle::err("{} is out on bounds ({})", index, this->_dimensions);
      // Don't just check index is in bound, because {2, 2} fits in {1, 100} but
      // is still out of bounds.
      auto const i = this->_index(index, std::make_index_sequence<sizeof...(Indexes)>());
      ELLE_ASSERT_LT(i, this->_size);
      return reinterpret_cast<T&>(this->_table[i]);
    }

    T const&
    at(Indexes const& ... indexes) const
    {
      return unconst(*this).at(indexes...);
    }

  private:
    TableImpl(std::tuple<Indexes...> dimensions, bool)
      : _size(_details::table::size(dimensions))
      , _dimensions(std::move(dimensions))
      , _table(new Storage[this->_size])
    {}

    template <std::size_t ... S>
    bool
    _check_boundaries(Index const& index, std::index_sequence<S...>)
    {
      return ((std::get<S>(index) < std::get<S>(this->_dimensions) &&
               std::get<S>(index) >= 0) && ...);
    }

    template <std::size_t ... S>
    int
    _index(Index const& index, std::index_sequence<S...>)
    {
      return sum((std::get<S>(index) * this->_index_offset(std::make_index_sequence<sizeof...(Indexes) - S - 1>()))...);
    }

    template <std::size_t ... S>
    int
    _index_offset(std::index_sequence<S...>)
    {
      return product(
        std::get<sizeof...(Indexes) - S - 1>(this->_dimensions)...);
    }

    ELLE_ATTRIBUTE_R(int, size);
    ELLE_ATTRIBUTE_R(std::tuple<Indexes...>, dimensions);
    ELLE_ATTRIBUTE(std::unique_ptr<Storage[]>, table, protected);
  };

  template <typename T, int dimension>
  class Table:
    public elle::meta::repeat<int, dimension>::template apply<TableImpl, T>
  {
  public:
    using Super = typename elle::meta::repeat<int, dimension>::
      template apply<TableImpl, T>::TableImpl;
    using Super::Super;
  };
}
