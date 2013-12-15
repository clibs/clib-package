
#include "describe/describe.h"
#include "rimraf/rimraf.h"
#include "fs/fs.h"
#include "clib-package.h"


describe("clib_package_install", {
  it("should return -1 when given a bad package", {
    assert(-1 == clib_package_install(NULL, "./deps"));
  });

  it("should install the pkg in its own directory", {
    clib_package_t *pkg = clib_package_new_from_slug("stephenmathieson/case.c@0.1.0");
    assert(pkg);
    assert(0 == clib_package_install(pkg, "./test/fixtures/"));
    assert(0 == fs_exists("./test/fixtures/"));
    assert(0 == fs_exists("./test/fixtures/case"));
    clib_package_free(pkg);
    rimraf("./test/fixtures/");
  });

  it("should install the package's package.json", {
    clib_package_t *pkg = clib_package_new_from_slug("stephenmathieson/case.c@0.1.0");
    assert(pkg);
    assert(0 == clib_package_install(pkg, "./test/fixtures/"));
    assert(0 == fs_exists("./test/fixtures/case/package.json"));
    clib_package_free(pkg);
    rimraf("./test/fixtures/");
  });

  it("should install the package's sources", {
    clib_package_t *pkg = clib_package_new_from_slug("stephenmathieson/case.c@0.1.0");
    assert(pkg);
    assert(0 == clib_package_install(pkg, "./test/fixtures/"));
    assert(0 == fs_exists("./test/fixtures/case/case.c"));
    assert(0 == fs_exists("./test/fixtures/case/case.h"));
    clib_package_free(pkg);
    rimraf("./test/fixtures");
  });

  it("should install the package's dependencies", {
    clib_package_t *pkg = clib_package_new_from_slug("stephenmathieson/mkdirp.c@master");
    assert(pkg);
    assert(0 == clib_package_install(pkg, "./test/fixtures/"));
    assert(0 == fs_exists("./test/fixtures/path-normalize/"));
    assert(0 == fs_exists("./test/fixtures/path-normalize/package.json"));
    assert(0 == fs_exists("./test/fixtures/path-normalize/path-normalize.c"));
    assert(0 == fs_exists("./test/fixtures/path-normalize/path-normalize.h"));
    clib_package_free(pkg);
    rimraf("./test/fixtures");
  });

  it("should install itself", {
    clib_package_t *pkg = clib_package_new_from_slug("stephenmathieson/clib-package");
    assert(pkg);
    assert(0 == clib_package_install(pkg, "./test/fixtures/"));
    assert(0 == fs_exists("./test/fixtures/"));

    assert(0 == fs_exists("./test/fixtures/clib-package/"));
    assert(0 == fs_exists("./test/fixtures/clib-package/package.json"));
    assert(0 == fs_exists("./test/fixtures/clib-package/clib-package.c"));
    assert(0 == fs_exists("./test/fixtures/clib-package/clib-package.h"));

    assert(0 == fs_exists("./test/fixtures/http-get/"));
    assert(0 == fs_exists("./test/fixtures/http-get/package.json"));
    assert(0 == fs_exists("./test/fixtures/http-get/http-get.c"));
    assert(0 == fs_exists("./test/fixtures/http-get/http-get.h"));

    assert(0 == fs_exists("./test/fixtures/list/"));
    assert(0 == fs_exists("./test/fixtures/list/package.json"));
    assert(0 == fs_exists("./test/fixtures/list/list.c"));
    assert(0 == fs_exists("./test/fixtures/list/list.h"));

    assert(0 == fs_exists("./test/fixtures/mkdirp/"));
    assert(0 == fs_exists("./test/fixtures/mkdirp/package.json"));
    assert(0 == fs_exists("./test/fixtures/mkdirp/mkdirp.c"));
    assert(0 == fs_exists("./test/fixtures/mkdirp/mkdirp.h"));

    assert(0 == fs_exists("./test/fixtures/parson/"));
    assert(0 == fs_exists("./test/fixtures/parson/package.json"));
    assert(0 == fs_exists("./test/fixtures/parson/parson.c"));
    assert(0 == fs_exists("./test/fixtures/parson/parson.h"));

    assert(0 == fs_exists("./test/fixtures/path-join/"));
    assert(0 == fs_exists("./test/fixtures/path-join/package.json"));
    assert(0 == fs_exists("./test/fixtures/path-join/path-join.c"));
    assert(0 == fs_exists("./test/fixtures/path-join/path-join.h"));

    assert(0 == fs_exists("./test/fixtures/path-normalize/"));
    assert(0 == fs_exists("./test/fixtures/path-normalize/package.json"));
    assert(0 == fs_exists("./test/fixtures/path-normalize/path-normalize.c"));
    assert(0 == fs_exists("./test/fixtures/path-normalize/path-normalize.h"));

    assert(0 == fs_exists("./test/fixtures/str-ends-with/"));
    assert(0 == fs_exists("./test/fixtures/str-ends-with/package.json"));
    assert(0 == fs_exists("./test/fixtures/str-ends-with/str-ends-with.c"));
    assert(0 == fs_exists("./test/fixtures/str-ends-with/str-ends-with.h"));

    assert(0 == fs_exists("./test/fixtures/str-starts-with/"));
    assert(0 == fs_exists("./test/fixtures/str-starts-with/package.json"));
    assert(0 == fs_exists("./test/fixtures/str-starts-with/str-starts-with.c"));
    assert(0 == fs_exists("./test/fixtures/str-starts-with/str-starts-with.h"));

    clib_package_free(pkg);
    rimraf("./test/fixtures");
  });
});
