#include <nucleus/proton/Version.hh>

#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <limits>
#include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this constant represents the first version.
    ///
    const Version               Version::First;

    ///
    /// this constant represents the latest version.
    ///
    const Version Version::Last = std::numeric_limits<Version::Type>::max();

    ///
    /// this constant represents any version and is useful whenever
    /// dealing with immutable blocks for which version do not make any
    /// sense.
    ///
    const Version               Version::Any(Version::Last);

    ///
    /// this constant is an alias of Any.
    ///
    const Version&              Version::Some = Version::Any;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Version::Version():
      number(0)
    {
    }

    ///
    /// specific constructor.
    ///
    Version::Version(const Type                                 number):
      number(number)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method creates a version.
    ///
    elle::Status        Version::Create(const Type              number)
    {
      // assign the number.
      this->number = number;

      return elle::Status::Ok;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this method checks if two objects match.
    ///
    elle::Boolean       Version::operator==(const Version&      element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      return (this->number == element.number);
    }

    ///
    /// this method compares the objects.
    ///
    elle::Boolean       Version::operator<(const Version&       element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the numbers.
      if (this->number >= element.number)
        return false;

      return true;
    }

    ///
    /// this method compares the objects.
    ///
    elle::Boolean       Version::operator>(const Version&       element) const
    {
      // check the address as this may actually be the same object.
      if (this == &element)
        return true;

      // compare the numbers.
      if (this->number <= element.number)
        return false;

      return true;
    }

    ///
    /// this method increments the version number.
    ///
    Version&            Version::operator+=(const elle::Natural32 increment)
    {
      // increment the number.
      this->number += increment;

      return (*this);
    }

    ///
    /// this method adds the given version to the current one.
    ///
    Version             Version::operator+(const Version&       element) const
    {
      return (Version(this->number + element.number));
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Version, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the version's internals.
    ///
    elle::Status        Version::Dump(const elle::Natural32     margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Version] " << this->number << std::endl;

      return elle::Status::Ok;
    }


    /*----------.
    | Printable |
    `----------*/

    void
    Version::print(std::ostream& s) const
    {
      if (*this == First)
        s << "first";
      else if (*this == Last)
        s << "last";
      else if (*this == Any)
        s << "any";
      else
        s << this->number;
    }
  }
}
