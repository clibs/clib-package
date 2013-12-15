
# clib-package

  Internal bits of clib-install(1) for fetching packages.

## Installation

  Install with [clib(1)](https://github.com/clibs/clib):

    $ clib install stephenmathieson/clib-package

## API

All functions accepting a `char *slug` parameter expect the format `<author>/<name>@<version>`, where `author` and `version` are optional.

### `clib_package_t *clib_package_new(const char *json, int verbose)`

  Initialize a new package from the given `json`.

### `clib_package_t *clib_package_new_from_slug(const char *slug, int verbose)`

  Initialize a new package from the given GitHub repository `slug`.

### `char *clib_package_url(const char *author, const char *name, const char *version)`

  Get the GitHub homepage for the package described as `author/name@version`.

### `char *clib_package_parse_version(const char *slug)`

  Parse the version from the given `slug`.  

### `char *clib_package_parse_author(const char *slug)`

  Parse the author from the given `slug`.  

### `char *clib_package_parse_name(const char *slug)`

  Parse the name from the given `slug`.  

### `clib_package_dependency_t *clib_package_dependency_new(const char *repo, const char *version)`

  Create a dependency of the package described as `<repo>@<version>`.

### `int clib_package_install(clib_package_t *pkg, const char *dir, int verbose)`

  Install the given `pkg` and all of its dependencies in `dir/<package name>`.  Will create `dir` if it does not exist.

  Returns `0` on success.

### `int clib_package_install_dependencies(clib_package_t *pkg, const char *dir, int verbose)`

  Install all dependencies of the given `pkg` in `dir/<dependency name>`.  Will create `dir` if it does not exist.

  Returns `0` on success.

### `void clib_package_free(clib_package_t *pkg)`

  Free the given `pkg`.

## Example

Simple CLI for installing clib packages:

```c
#include <stdio.h>
#include "clib-package.h"

#define true 1

int main(int argc, char const *argv[]) {
  for (int i = 1; i < argc; ++i) {
    clib_package_t *pkg = clib_package_new_from_slug(argv[i], 1);
    if (!pkg) return 1;
    int rc = clib_package_install(pkg, "./deps", true);
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
