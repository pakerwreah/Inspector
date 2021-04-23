# Changes made to cpplint

1. Add `filematch` directive so we can have rules for files that match a specific pattern
   - Example: `filematch=*.cpp` every rule below this will only evaluate if the pattern matches

2. Reimplement `CheckNextIncludeOrder` to reflect changes made to the sections include order
    - Custom headers `"some.h"`, C headers `"<unistd.h>"`, then C++ headers `"<string>"`

3. Remove logic that ignores (possible) 3rd-party headers in the `build/include_order` rule
    - it was assuming that headers with capital letters like `MyHeader.h` were 3rd-party and ignoring them
