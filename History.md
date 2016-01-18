
0.4.1 / 2016-01-17
==================

  * src: add "file URL" debug()
  * package: fix `repo`
  * travis: disable valgrind for now
  * Makefile: fix `test`
  * travis: install and run valgrind
  * Makefile: cleanup/refactor
  * test: refactor to use describe@2 syntax
  * package: update `describe.h` to 2.0.1

0.4.0 / 2016-01-10
==================

  * src: fix whitespace
  * src: use `package.json` repo for installs (#6, @willemt)

0.3.1 / 2014-12-01
==================

  * src: Rename `_debugger` to suggest privacy

0.3.0 / 2014-12-01
==================

  * src: Cleanup logs
  * travis: Update before installing deps
  * src: Fix debug()
  * src: Add debug()s

0.2.8 / 2014-10-24
==================

  * package: Update http-get and mkdirp
  * src: Remove unnecessary null checks

0.2.7 / 2014-06-29
==================

 * package.json: Update deps to remove str-copy
 * package.json: Replace str-copy.c with clibs/strdup

0.2.6 / 2014-06-27
==================

 * package.json: Update mkdirp.c to 0.1.3

0.2.5 / 2014-06-26
==================

 * package.json: Update jwerle/fs.c to 0.1.1
 * history: Fix formatting

0.2.4 / 2014-05-22
==================

 * Change default endpoint to raw.githubusercontent.com
 * Add support for installing makefiles ([clibs/clib#22](https://github.com/clibs/clib/issues/22))

0.2.3 / 2014-05-16
==================

 * Fix double-free of 'name' and 'json_url'

0.2.2 / 2014-03-19
==================

 * Use clibs/logger for all output
 * test: Fix failing development test

0.2.1 / 2014-03-17
==================

 * Set `iterator` to NULL after freeing it (fixes #1)
 * build: Clean should remove fixtures

0.2.0 / 2014-03-13
==================

 * Fix memory leaks

0.1.1 / 2014-03-10
==================

 * Fix 'file_url' free on error

0.1.0 / 2014-02-20 
==================

 * Add travis 
 * Remove deps from git
 * Add copyright to sources

0.0.0 / 2013-12-28
==================

 * Initial release
