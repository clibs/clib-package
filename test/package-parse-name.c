
#include "describe/describe.h"
#include "clib-package.h"


describe("clib_package_parse_name", {
  it("should return NULL when given a bad slug", {
    assert(NULL == clib_package_parse_name(NULL));
  });

  it("should return NULL when unable to parse a name", {
    assert(NULL == clib_package_parse_name("/"));
    assert(NULL == clib_package_parse_name("author/"));
  });

  it("should support the \"name\"-style slugs", {
    assert_str_equal("name", clib_package_parse_name("name"));
  });

  it("should support \"author/name\"-style slugs", {
    assert_str_equal("name", clib_package_parse_name("author/name"));
  });

  it("should support \"author/name@version\"-slugs slugs", {
    assert_str_equal("name", clib_package_parse_name("author/name@master"));
    assert_str_equal("name", clib_package_parse_name("author/name@*"));
  });

  // this was a bug in parse-repo.c...
  it("should not be affected after the slug is freed", {
    char *slug = malloc(48);
    assert(slug);
    strcpy(slug, "author/name@version");

    char *name = clib_package_parse_name(slug);

    assert_str_equal("name", name);
    free(slug);

    assert_str_equal("name", name);
  });
});
