
#include "describe.h"
#include "clib-package.h"

describe("clib_package_new_from_slug", {
  it("should return NULL when given a bad slug", {
    assert(NULL == clib_package_new_from_slug(NULL));
  });

  it("should return NULL when given a slug missing a name", {
    assert(NULL == clib_package_new_from_slug("author/@version"));
  });

  it("should return NULL when given slug which doesn't resolve", {
    assert(NULL == clib_package_new_from_slug("abc11234"));
  });

  it("should build the correct package", {
    clib_package_t *pkg = clib_package_new_from_slug("stephenmathieson/case.c@0.1.0");
    assert_str_equal("case", pkg->name);
    assert_str_equal("0.1.0", pkg->version);
    assert_str_equal("stephenmathieson/case.c", pkg->repo);
    assert_str_equal("MIT", pkg->license);
    assert_str_equal("String case conversion utility", pkg->description);
    clib_package_free(pkg);
  });

  it("should force package version numbers", {
    clib_package_t *pkg = clib_package_new_from_slug("stephenmathieson/mkdirp.c");
    assert_str_equal("master", pkg->version);
    clib_package_free(pkg);
  });
});
