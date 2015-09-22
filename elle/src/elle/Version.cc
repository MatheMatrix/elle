#include <elle/Version.hh>
#include <elle/serialization/Serializer.hh>

namespace elle
{

  /*-------------.
  | Construction |
  `-------------*/

  Version::Version():
    _major(0),
    _minor(0),
    _subminor(0)
  {}

  ELLE_SERIALIZE_CONSTRUCT_DEFINE(Version)
  {}

  /*----------.
  | Printable |
  `----------*/

  void
  Version::print(std::ostream& stream) const
  {
    stream << static_cast<elle::Natural32>(this->_major)
           << "."
           << static_cast<elle::Natural32>(this->_minor)
           << "."
           << static_cast<elle::Natural32>(this->_subminor);
  }

  /*----------.
  | Operators |
  `----------*/

  elle::Boolean
  Version::operator ==(Version const& other) const
  {
    return ((this->_major == other._major)
            && (this->_minor == other._minor)
            && (this->_subminor == other._subminor));
  }

  elle::Boolean
  Version::operator <(Version const& other) const
  {
    if (this->_major != other._major)
      return this->_major < other._major;
    if (this->_minor != other._minor)
      return this->_minor < other._minor;
    return this->_subminor < other._subminor;
  }

  elle::Boolean
  Version::operator >(Version const& other) const
  {
    return !((*this == other) || (*this < other));
  }

  /*--------------.
  | Serialization |
  `--------------*/

  void
  Version::serialize(elle::serialization::Serializer& s)
  {
    s.serialize("major", this->_major);
    s.serialize("minor", this->_minor);
    s.serialize("subminor", this->_subminor);
  }
}
