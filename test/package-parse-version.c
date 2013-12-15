
#include "describe/describe.h"
#include "clib-package.h"

describe("clib_package_parse_version", {
  it("should return NULL when given a bad slug", {
    assert(NULL == clib_package_parse_version(NULL));
  });

  it("should default to \"master\"", {
    assert_str_equal("master", clib_package_parse_version(""));
    assert_str_equal("master", clib_package_parse_version("foo"));
    assert_str_equal("master", clib_package_parse_version("foo/bar"));
  });

  it("should transform \"*\" to \"master\"", {
    assert_str_equal("master", clib_package_parse_version("*"));
    assert_str_equal("master", clib_package_parse_version("foo@*"));
    assert_str_equal("master", clib_package_parse_version("foo/bar@*"));
  });

  it("should support \"name\"-style slugs", {
    assert_str_equal("master", clib_package_parse_version("foo"));
  });

  it("should support \"name@version\"-style slugs", {
    assert_str_equal("bar", clib_package_parse_version("foo@bar"));
    assert_str_equal("master", clib_package_parse_version("foo@*"));
    assert_str_equal("1.2.3", clib_package_parse_version("foo@1.2.3"));
  });

  it("should support \"author/name@version\"-style slugs", {
    assert_str_equal("baz", clib_package_parse_version("foo/bar@baz"));
    assert_str_equal("master", clib_package_parse_version("foo/bar@*"));
    assert_str_equal("1.2.3", clib_package_parse_version("foo/bar@1.2.3"));
  });

  // this was a bug in parse-repo.c...
  it("should not be affected after the slug is freed", {
    char *slug = malloc(48);
    assert(slug);
    strcpy(slug, "author/name@version");

    char *version = clib_package_parse_version(slug);

    assert_str_equal("version", version);
    free(slug);

    assert_str_equal("version", version);
  });
});
