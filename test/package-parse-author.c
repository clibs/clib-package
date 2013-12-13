
#include <string.h>
#include "describe.h"
#include "clib-package.h"

describe("clib_package_parse_author", {
  it("should return NULL when given a bad slug", {
    assert(NULL == clib_package_parse_author(NULL));
  });

  it("should return NULL when unable to parse an author", {
    assert(NULL == clib_package_parse_author("/"));
    assert(NULL == clib_package_parse_author("/name"));
    assert(NULL == clib_package_parse_author("/name@version"));
  });

  it("should default to \"clibs\"", {
    assert_str_equal("clibs", clib_package_parse_author("foo"));
  });

  it("should support \"author/name\"-style slugs", {
    assert_str_equal("author", clib_package_parse_author("author/name"));
  });

  it("should support \"author/name@version\"-slugs slugs", {
    assert_str_equal("author", clib_package_parse_author("author/name@master"));
    assert_str_equal("author", clib_package_parse_author("author/name@*"));
  });

  // this was a bug in parse-repo.c...
  it("should not be affected after the slug is freed", {
    char *slug = malloc(48);
    assert(slug);
    strcpy(slug, "author/name@version");

    char *author = clib_package_parse_author(slug);

    assert_str_equal("author", author);
    free(slug);

    assert_str_equal("author", author);

    free(author);
  });
});
