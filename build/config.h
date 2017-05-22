#ifndef ACE_CONFIG_H
#define ACE_CONFIG_H

# if defined(_MSC_VER)
#  include "ace/config-win32.h"
# elif defined(__GNUG__) || defined(__GNUC__)
#  include "ace/config-linux.h"
# else
# error Unknown Configuration!
# endif

#endif // ACE_CONFIG_H
