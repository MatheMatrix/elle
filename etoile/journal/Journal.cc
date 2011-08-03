//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Journal.cc
//
// created       julien quintard   [fri jun 24 14:23:50 2011]
// updated       julien quintard   [mon aug  1 13:28:24 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/journal/Journal.hh>

#include <etoile/gear/Transcript.hh>
#include <etoile/gear/Action.hh>

#include <etoile/depot/Depot.hh>

namespace etoile
{
  namespace journal
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Journal::Record(gear::Scope*		scope)
    {
      gear::Transcript::Scoutor		scoutor;

      enter();

      // first, if actors are still operating on the scope, delay the
      // journal processing.
      if (scope->actors.empty() == false)
	leave();

      // go through the transcript's actions.
      for (scoutor = scope->context->transcript.container.begin();
	   scoutor != scope->context->transcript.container.end();
	   scoutor++)
	{
	  gear::Action*			action = *scoutor;

	  // perform the action.
	  switch (action->type)
	    {
	    case gear::Action::TypePush:
	      {
		// store the block in the depot.
		if (depot::Depot::Push(action->address,
				       *action->block) == elle::StatusError)
		  escape("unable to push the block in the depot");

		break;
	      }
	    case gear::Action::TypeWipe:
	      {
		// wipe the block from the depot.
		if (depot::Depot::Wipe(action->address) == elle::StatusError)
		  escape("unable to wipe the block from the depot");

		break;
	      }
	    case gear::Action::TypeUnknown:
	      {
		escape("unknown action type");
	      }
	    }
	}

      // clear the transcript since the actions have been performed.
      if (scope->context->transcript.Clear() == elle::StatusError)
	escape("unable to clear the transcript");

      // set the context's state.
      scope->context->state = gear::StateCleaned;

      // relinquish the scope.
      if (gear::Scope::Relinquish(scope) == elle::StatusError)
	escape("unable to relinquish the scope");

      leave();
    }

  }
}
