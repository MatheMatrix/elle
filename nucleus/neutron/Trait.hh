//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [thu apr  1 19:48:42 2010]
//

#ifndef NUCLEUS_NEUTRON_TRAIT_HH
#define NUCLEUS_NEUTRON_TRAIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/neutron/Subject.hh>
#include <nucleus/neutron/Permissions.hh>
#include <nucleus/neutron/Token.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an attribute entry i.e a trait.
    ///
    /// a trait is characterised by a string name and while a value string
    /// is associated with it.
    ///
    class Trait:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const Trait                Null;

      //
      // types
      //
      typedef elle::String              S;

      //
      // constructors & destructors
      //
      Trait();
      Trait(const elle::String&,
            const elle::String&);

      //
      // interfaces
      //

      // object
#include <elle/idiom/Open.hh>
      declare(Trait);
#include <elle/idiom/Close.hh>
      elle::Boolean     operator==(const Trait&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      // rangeable
      elle::String&     Symbol();

      //
      // attributes
      //
      elle::String      name;
      elle::String      value;
    };

  }
}

#endif
