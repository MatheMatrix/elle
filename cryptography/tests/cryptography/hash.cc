#include "cryptography.hh"

#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/hash.hh>
#include <cryptography/Plain.hh>
#include <cryptography/random.hh>

#include <elle/serialization/json.hh>

static std::string const _input(
  "- Do you think she's expecting something big?"
  "- You mean, like anal?");

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::Oneway O>
void
test_represent_n()
{
  // N)
  {
    infinit::cryptography::Digest digest =
      infinit::cryptography::hash(
        infinit::cryptography::Plain(_input),
        O);

    std::stringstream stream;
    {
      typename elle::serialization::Json::SerializerOut output(stream);
      digest.serialize(output);
    }

    elle::printf("[representation %s] %s\n", N, stream.str());
  }
}

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate hexadecimal-based representations which can be used in
  // other tests.

  // MD5.
  test_represent_n<1, infinit::cryptography::Oneway::md5>();
  // SHA.
  test_represent_n<2, infinit::cryptography::Oneway::sha>();
  // SHA-1.
  test_represent_n<3, infinit::cryptography::Oneway::sha1>();
  // SHA-224.
  test_represent_n<4, infinit::cryptography::Oneway::sha224>();
  // SHA-256.
  test_represent_n<5, infinit::cryptography::Oneway::sha256>();
  // SHA-384.
  test_represent_n<6, infinit::cryptography::Oneway::sha384>();
  // SHA-512.
  test_represent_n<7, infinit::cryptography::Oneway::sha512>();
}

/*--------.
| Operate |
`--------*/

void
test_operate()
{
  elle::String data =
    infinit::cryptography::random::generate<elle::String>(123);

  infinit::cryptography::Digest digest1 =
    infinit::cryptography::hash(
      infinit::cryptography::Plain(data),
      infinit::cryptography::Oneway::sha1);
  infinit::cryptography::Digest digest2 =
    infinit::cryptography::hash(
      infinit::cryptography::Plain(data),
      infinit::cryptography::Oneway::sha1);

  BOOST_CHECK_EQUAL(digest1, digest2);
}

/*----------.
| Serialize |
`----------*/

template <infinit::cryptography::Oneway O>
void
test_serialize_x(elle::String const& R)
{
  std::stringstream stream(R);
  typename elle::serialization::Json::SerializerIn input(stream);
  infinit::cryptography::Digest digest(input);

  infinit::cryptography::Digest _digest =
    infinit::cryptography::hash(
      infinit::cryptography::Plain(_input),
      O);

  BOOST_CHECK_EQUAL(digest, _digest);
}

static
void
test_serialize()
{
  // MD5 based on [representation 1].
  test_serialize_x<infinit::cryptography::Oneway::md5>(R"JSON({"buffer":"lRs+RbdXhAcwwOIi63EJHw=="})JSON");
  // SHA based on [representation 2].
  test_serialize_x<infinit::cryptography::Oneway::sha>(R"JSON({"buffer":"B34bV98RTfK7qXPQd3+zs3GBYKk="})JSON");
  // SHA-1 based on [representation 3].
  test_serialize_x<infinit::cryptography::Oneway::sha1>(R"JSON({"buffer":"LMHmhHUOH8N3mGo1HTRFd6vbmXk="})JSON");
  // SHA-224 based on [representation 4].
  test_serialize_x<infinit::cryptography::Oneway::sha224>(R"JSON({"buffer":"5DwfnlGVKvLuciDUwR6fuzOS2DDyA4nCeDoZ/Q=="})JSON");
  // SHA-256 based on [representation 5].
  test_serialize_x<infinit::cryptography::Oneway::sha256>(R"JSON({"buffer":"Ooj0FMtgjoI7saciFCZ/Xg8eXJWFhzXn89mZide6oeI="})JSON");
  // SHA-384 based on [representation 6].
  test_serialize_x<infinit::cryptography::Oneway::sha384>(R"JSON({"buffer":"rjLzR+ByZsSO+l+IeKrzH7Dre3XZmeK2/GSPW/TgTMBQ6VwzUDkgzjlqaKUOga/4"})JSON");
  // SHA-512 based on [representation 7].
  test_serialize_x<infinit::cryptography::Oneway::sha512>(R"JSON({"buffer":"i641WSGHk8ZjTReGfZSAaey8Ci1rbX+usbN7DLFM2hjjN1IlgJ2bDipopotlgY++PnN6dlN+Vd6MKIz3z1LUew=="})JSON");
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("hash");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
