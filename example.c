
#include <stdio.h>
#include "clib-package.h"

int main(int argc, char const *argv[]) {
  for (int i = 1; i < argc; ++i) {
    clib_package_t *pkg = clib_package_new_from_slug(argv[i]);
    if (!pkg) return 1;
    printf("installing %s\n", pkg->name);
    int rc = clib_package_install(pkg, "./deps");
    clib_package_free(pkg);
    if (0 != rc) return 2;
  }

  return 0;
}
