#include <elle/test.hh>

#include <elle/Backtrace.hh>

using elle::Backtrace;

static
void
test_backtrace_empty()
{
  Backtrace empty;
  BOOST_CHECK(empty.empty());
}

// Pacify -Wmissing-declarations.
Backtrace foo();
Backtrace bar();
Backtrace baz();
Backtrace quux();

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
quux()
{
  return Backtrace::current();
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
baz()
{
  return quux();
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
bar()
{
  return baz();
}

ELLE_COMPILER_ATTRIBUTE_NO_INLINE
Backtrace
foo()
{
  return bar();
}

static
void
test_backtrace()
{
  Backtrace bt(foo());
  BOOST_CHECK_GE(bt.size(), 4);
  auto sf = bt.begin();
  BOOST_CHECK_EQUAL(sf->symbol, "quux()");
  ++sf;
  BOOST_CHECK_EQUAL(sf->symbol, "baz()");
  ++sf;
  BOOST_CHECK_EQUAL(sf->symbol, "bar()");
  ++sf;
  BOOST_CHECK_EQUAL(sf->symbol, "foo()");
  ++sf;
}

static
void
test_strip_base()
{
  Backtrace bt(foo());
  bt.strip_base(Backtrace::current());
  BOOST_CHECK_LE(bt.size(), 5);
  BOOST_CHECK_EQUAL(bt.front().symbol, "quux()");
}

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* bt = BOOST_TEST_SUITE("Backtrace");
#ifndef INFINIT_WINDOWS
  boost::unit_test::framework::master_test_suite().add(bt);
  bt->add(BOOST_TEST_CASE(test_backtrace_empty));
  bt->add(BOOST_TEST_CASE(test_backtrace));
  bt->add(BOOST_TEST_CASE(test_strip_base));
#else
  struct dummy { static void f() {} };
  bt->add(BOOST_TEST_CASE(dummy::f));
#endif
}

