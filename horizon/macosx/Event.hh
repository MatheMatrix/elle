//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [sat mar 24 17:47:21 2012]
//

#ifndef HORIZON_MACOSX_EVENT_HH
#define HORIZON_MACOSX_EVENT_HH

//
// ---------- macros ----------------------------------------------------------
//

#ifndef FUSE_USE_VERSION
# define FUSE_USE_VERSION               26
#endif

//
// ---------- includes --------------------------------------------------------
//

#include <elle/idiom/Close.hh>
# include <QEvent>
# include <boost/interprocess/sync/interprocess_semaphore.hpp>
# define _GNU_SOURCE
# include <fuse.h>
#include <elle/idiom/Open.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a FUSE event i.e upcall and is used for
    /// transmitting the arguments from the FUSE upcall-specific thread
    /// to the handler launched by the event loop.
    ///
    class Event:
      public QEvent
    {
    public:
      //
      // enumerations
      //
      enum Operation
        {
          OperationUnknown,
          OperationStatfs,
          OperationGetattr,
          OperationFgetattr,
          OperationUtimens,
          OperationOpendir,
          OperationReaddir,
          OperationReleasedir,
          OperationMkdir,
          OperationRmdir,
          OperationAccess,
          OperationChmod,
          OperationChown,
          OperationSetxattr,
          OperationGetxattr,
          OperationListxattr,
          OperationRemovexattr,
          OperationSymlink,
          OperationReadlink,
          OperationCreate,
          OperationOpen,
          OperationWrite,
          OperationRead,
          OperationTruncate,
          OperationFtruncate,
          OperationRelease,
          OperationRename,
          OperationUnlink
        };

      //
      // constructors & destructors
      //
      Event(boost::interprocess::interprocess_semaphore*,
            int*);

      //
      // attributes
      //
      Operation                 operation;

      const char*               path;
      struct ::statvfs*         statvfs;
      struct ::stat*            stat;
      struct ::fuse_file_info*  info;
      struct ::timespec         ts[2];
      void*                     buffer;
      ::fuse_fill_dir_t         filler;
      off_t                     offset;
      mode_t                    mode;
      int                       mask;
      uid_t                     uid;
      gid_t                     gid;
      const char*               name;
      char*                     value;
      size_t                    size;
      int                       flags;
      uint32_t                  position;
      char*                     list;
      const char*               source;
      const char*               target;

      boost::interprocess::interprocess_semaphore*      semaphore;
      int*                                              result;
    };

  }
}

#endif
