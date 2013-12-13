
#include "describe.h"
#include "clib-package.h"

describe("clib_package_url", {
  it("should return NULL when given a bad author", {
    assert(NULL == clib_package_url(NULL, "name", "version"));
  });

  it("should return NULL when given a bad name", {
    assert(NULL == clib_package_url("author", NULL, "version"));
  });

  it("should return NULL when given a bad version", {
    assert(NULL == clib_package_url("author", "name", NULL));
  });

  it("should build a GitHub url", {
    assert_str_equal("https://raw.github.com/author/name/version", clib_package_url("author", "name", "version"));
  });
});
