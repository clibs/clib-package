
#include "describe.h"
#include "rimraf.h"
#include "fs.h"
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
});
