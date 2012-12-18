#include <elle/system/platform.hh>

#include <cryptography/random.hh>

#include <elle/log.hh>
#include <elle/os/getenv.hh>

#include <system_error>
#include <iostream>
#include <fstream>

#include <elle/idiom/Close.hh>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/rand.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# if defined(INFINIT_WINDOWS)
#  include <process.h>
#  include <windows.h>
#  include <wincrypt.h>
# endif
#include <elle/idiom/Open.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.random");

namespace infinit
{
  namespace cryptography
  {
    namespace random
    {
      /*----------.
      | Functions |
      `----------*/

      void
      initialize()
      {
        uint8_t temporary[256];

        ELLE_TRACE("initializing the random generator");

#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
        {
          /// The path to read random data from.
          ///
          /// Enable for instance tests to override the random data source,
          /// so as to use /dev/urandom and avoid /dev/random enthropy
          /// starvation.

          static std::string const source = elle::os::getenv(
              "INFINIT_CRYPTOGRAPHY_RANDOM_SOURCE",
              "/dev/urandom"
          );
          std::ifstream random_source_file(source);

          if (random_source_file.good() == false)
            {
              escape("%s: %s", source,
                     std::error_code(errno, std::system_category()).message());
            }
          // Read random data.
          random_source_file.read(reinterpret_cast<char *>(temporary),
                                  sizeof(temporary));
        }
#elif defined(INFINIT_WINDOWS)
        {
          HCRYPTPROV h_provider = 0;

          if (!::CryptAcquireContextW(&h_provider, 0, 0, PROV_RSA_FULL,
                                      CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
            escape("failed to acquire cryptographic context");

          if (!::CryptGenRandom(h_provider, sizeof (temporary), temporary))
            {
              ::CryptReleaseContext(h_provider, 0);

              escape("failed to generate the random seed");
            }

          if (!::CryptReleaseContext(h_provider, 0))
            escape("failed to release cryptographic context");
        }
#else
# error "unsupported platform"
#endif

        // Seed the random generator.
        ::RAND_seed(temporary, sizeof (temporary));
      }

      void
      clean()
      {
        ELLE_TRACE("cleaning the random generator");
      }
    }
  }
}
