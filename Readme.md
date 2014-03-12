
# clib-package

  Internal bits of clib-install(1) for fetching packages.

[![Build Status](https://travis-ci.org/stephenmathieson/clib-package.png?branch=master)](https://travis-ci.org/stephenmathieson/clib-package)

## Installation

  Install with [clib(1)](https://github.com/clibs/clib):

    $ clib install stephenmathieson/clib-package

## Example

Simple CLI for installing clib packages:

```c
#include <stdio.h>
#include "clib-package.h"

int main(int argc, char const *argv[]) {
  for (int i = 1; i < argc; ++i) {
    clib_package_t *pkg = clib_package_new_from_slug(argv[i], 1);
    if (!pkg) return 1;
    int rc = clib_package_install(pkg, "./deps", 1);
    clib_package_free(pkg);
    if (0 != rc) return 2;
  }

  return 0;
}

```

For more, see [the tests](https://github.com/stephenmathieson/clib-package/tree/master/test).

## License

(The MIT License)

Copyright (c) 2013 Stephen Mathieson &lt;me@stephenmathieson.com&gt;

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
