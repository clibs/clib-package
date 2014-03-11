
//
// clib-package.c
//
// Copyright (c) 2014 Stephen Mathieson
// MIT license
//

#include <stdlib.h>
#include <libgen.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "str-copy/str-copy.h"
#include "parson/parson.h"
#include "substr/substr.h"
#include "http-get/http-get.h"
#include "mkdirp/mkdirp.h"
#include "fs/fs.h"
#include "path-join/path-join.h"
#include "parse-repo/parse-repo.h"

#include "clib-package.h"

/**
 * Private pre-declare prototypes.
 */

static inline char *
json_object_get_string_safe(JSON_Object *, const char *);

static inline char *
json_array_get_string_safe(JSON_Array *, int);

static inline char *
clib_package_file_url(const char *, const char *);

static inline void
clib_package_debug(const char *, const char *, int);

static inline void
clib_package_log(const char *, const char *, ...);

static inline void
clib_package_error(const char *, const char *, ...);

static inline void
clib_package_warn(const char *, const char *, ...);

static inline char *
clib_package_slug(const char *, const char *, const char *);

static inline char *
clib_package_repo(const char *, const char *);

static inline list_t *
parse_package_deps(JSON_Object *);

static inline int
install_packages(list_t *, const char *, int);

static void
clib_package_dependency_free(void *);


/**
 * Create a copy of the result of a `json_object_get_string`
 * invocation.  This allows us to `json_value_free()` the
 * parent `JSON_Value` without destroying the string.
 */

static inline char *
json_object_get_string_safe(JSON_Object *obj, const char *key) {
  const char *val = json_object_get_string(obj, key);
  if (!val) return NULL;
  return str_copy(val);
}

/**
 * Create a copy of the result of a `json_array_get_string`
 * invocation.  This allows us to `json_value_free()` the
 * parent `JSON_Value` without destroying the string.
 */

static inline char *
json_array_get_string_safe(JSON_Array *array, int index) {
  const char *val = json_array_get_string(array, index);
  if (!val) return NULL;
  return str_copy(val);
}

/**
 * Build a URL for `file` of the package belonging to `url`
 */

static inline char *
clib_package_file_url(const char *url, const char *file) {
  if (!url || !file) return NULL;

  int size =
      strlen(url)
    + 1  // /
    + strlen(file)
    + 1  // \0
    ;

  char *res = malloc(size * sizeof(char));
  if (!res) return NULL;
  sprintf(res, "%s/%s", url, file);
  return res;
}

/**
 * Debug/log functions
 */

static inline void
clib_package_debug(const char *type, const char *msg, int color) {
  printf("  \033[%dm%10s\033[0m : \033[90m%s\033[m\n", color, type, msg);
}

static inline void
clib_package_log(const char *type, const char *msg, ...) {
  char *buf = malloc(512 * sizeof(char));
  va_list args;
  va_start(args, msg);
  vsprintf(buf, msg, args);
  va_end(args);
  clib_package_debug(type, msg, 36);
}

static inline void
clib_package_error(const char *type, const char *msg, ...) {
  char *buf = malloc(512 * sizeof(char));
  va_list args;
  va_start(args, msg);
  vsprintf(buf, msg, args);
  va_end(args);
  clib_package_debug(type, buf, 31);
}

static inline void
clib_package_warn(const char *type, const char *msg, ...) {
  char *buf = malloc(512 * sizeof(char));
  va_list args;
  va_start(args, msg);
  vsprintf(buf, msg, args);
  va_end(args);
  clib_package_debug(type, buf, 33);
}


/**
 * Build a slug
 */

static inline char *
clib_package_slug(const char *author, const char *name, const char *version) {
  int size =
      strlen(author)
    + 1 // /
    + strlen(name)
    + 1 // @
    + strlen(version)
    + 1 // \0
    ;

  char *slug = malloc(size * sizeof(char));
  sprintf(slug, "%s/%s@%s", author, name, version);
  return slug;
}

/**
 * Build a repo
 */

static inline char *
clib_package_repo(const char *author, const char *name) {
  int size =
      strlen(author)
    + 1 // /
    + strlen(name)
    + 1 // \0
    ;

  char *repo = malloc(size * sizeof(char));
  sprintf(repo, "%s/%s", author, name);
  return repo;
}

/**
 * Parse the dependencies in the given `obj` into a `list_t`
 */

static inline list_t *
parse_package_deps(JSON_Object *obj) {
  if (NULL == obj) return NULL;

  list_t *list = list_new();
  if (NULL == list) {
    return NULL;
  }

  list->free = clib_package_dependency_free;

  for (size_t i = 0; i < json_object_get_count(obj); i++) {
    const char *name = json_object_get_name(obj, i);
    if (!name) {
      list_destroy(list);
      return NULL;
    }

    const char *version = json_object_get_string_safe(obj, name);
    if (!version) {
      list_destroy(list);
      return NULL;
    }

    clib_package_dependency_t *dep =
      clib_package_dependency_new(name, version);
    if (NULL == dep) {
      list_destroy(list);
      return NULL;
    }

    list_node_t *node = list_node_new(dep);
    list_rpush(list, node);
  }

  return list;
}

static inline int
install_packages(list_t *list, const char *dir, int verbose) {
  if (!list || !dir) return 0;

  list_node_t *node;
  list_iterator_t *it = list_iterator_new(list, LIST_HEAD);
  while ((node = list_iterator_next(it))) {
    clib_package_dependency_t *dep = node->val;
    char *slug = clib_package_slug(dep->author, dep->name, dep->version);
    if (NULL == slug) return -1;

    clib_package_t *pkg = clib_package_new_from_slug(slug, verbose);
    free(slug);
    if (NULL == pkg) return -1;

    int rc = clib_package_install(pkg, dir, verbose);
    clib_package_free(pkg);
    if (-1 == rc) {
      list_iterator_destroy(it);
      return -1;
    }
  }

  list_iterator_destroy(it);
  return 0;
}

/**
 * Create a new clib package from the given `json`
 */

clib_package_t *
clib_package_new(const char *json, int verbose) {
  if (!json) return NULL;

  clib_package_t *pkg = malloc(sizeof(clib_package_t));
  if (!pkg) return NULL;

  memset(pkg, '\0', sizeof(clib_package_t));

  JSON_Value *root = json_parse_string(json);
  if (!root) {
    clib_package_free(pkg);
    return NULL;
  }

  JSON_Object *json_object = json_value_get_object(root);
  if (!json_object) {
    if (verbose) clib_package_error("error", "unable to parse json");
    json_value_free(root);
    clib_package_free(pkg);
    return NULL;
  }

  pkg->json = str_copy(json);
  pkg->name = json_object_get_string_safe(json_object, "name");

  pkg->repo = NULL;
  pkg->repo = json_object_get_string_safe(json_object, "repo");
  if (NULL != pkg->repo) {
    pkg->author = clib_package_parse_author(pkg->repo);
    // repo name may not be the package's name.  for example:
    //   stephenmathieson/str-replace.c -> str-replace
    pkg->repo_name = clib_package_parse_name(pkg->repo);
    // TODO support npm-style "repository"?
  } else {
    if (verbose) clib_package_warn("warning", "missing repo in package.json");
    pkg->author = NULL;
    pkg->repo_name = NULL;
  }

  pkg->version = json_object_get_string_safe(json_object, "version");
  pkg->license = json_object_get_string_safe(json_object, "license");
  pkg->description = json_object_get_string_safe(json_object, "description");
  pkg->install = json_object_get_string_safe(json_object, "install");

  JSON_Array *src = json_object_get_array(json_object, "src");
  if (src) {
    pkg->src = list_new();
    if (!pkg->src) {
      json_value_free(root);
      clib_package_free(pkg);
      return NULL;
    }

    for (size_t i = 0; i < json_array_get_count(src); i++) {
      char *file = json_array_get_string_safe(src, i);
      if (!file) break; // TODO fail?
      list_node_t *node = list_node_new(file);
      list_rpush(pkg->src, node);
    }
  } else {
    pkg->src = NULL;
  }

  JSON_Object *deps = json_object_get_object(json_object, "dependencies");
  pkg->dependencies = parse_package_deps(deps);

  JSON_Object *devs = json_object_get_object(json_object, "development");
  pkg->development = parse_package_deps(devs);

  json_value_free(root);
  return pkg;
}

/**
 * Create a package from the given repo `slug`
 */

clib_package_t *
clib_package_new_from_slug(const char *slug, int verbose) {
  char *author = NULL;
  char *name = NULL;
  char *version = NULL;
  char *url = NULL;
  char *json_url = NULL;
  char *repo = NULL;
  http_get_response_t *res = NULL;
  clib_package_t *pkg = NULL;

  // parse chunks
  if (!slug) goto error;
  if (!(author = parse_repo_owner(slug))) goto error;
  if (!(name = parse_repo_name(slug))) goto error;
  if (!(version = parse_repo_version(slug))) goto error;
  if (!(url = clib_package_url(author, name, version))) goto error;
  if (!(json_url = clib_package_file_url(url, "package.json"))) goto error;
  if (!(repo = clib_package_repo(author, name))) goto error;

  // fetch json
  if (verbose) clib_package_log("fetch", json_url);
  res = http_get(json_url);
  if (!res || !res->ok) {
    clib_package_error("error", "unable to fetch %s", json_url);
    goto error;
  }

  // build package
  pkg = clib_package_new(res->data, verbose);
  http_get_free(res);
  res = NULL;
  if (!pkg) goto error;

  // force version number
  if (NULL == pkg->version || 0 != strcmp(version, pkg->version)) {
    pkg->version = version;
  } else {
    free(version);
  }

  // force package author (don't know how this could fail)
  if (NULL == pkg->author || 0 != strcmp(author, pkg->author)) {
    pkg->author = author;
  } else {
    free(author);
  }

  // force package repo (some people are too cool to use a "repo" property)
  if (NULL == pkg->repo || 0 != strcmp(repo, pkg->repo)) {
    pkg->repo = repo;
  } else {
    free(repo);
  }

  pkg->url = url;
  return pkg;

error:
  if (author) free(author);
  if (name) free(name);
  if (version) free(version);
  if (url) free(url);
  if (json_url) free(json_url);
  if (repo) free(repo);
  if (res) http_get_free(res);
  if (pkg) clib_package_free(pkg);
  return NULL;
}

/**
 * Get a slug for the package `author/name@version`
 */

char *
clib_package_url(const char *author, const char *name, const char *version) {
  if (!author || !name || !version) return NULL;
  int size =
      23 // https://raw.github.com/
    + strlen(author)
    + 1 // /
    + strlen(name)
    + 1 // /
    + strlen(version)
    + 1 // \0
    ;

  char *slug = malloc(size);
  if (slug) {
    memset(slug, '\0', size);
    sprintf(slug, "https://raw.github.com/%s/%s/%s", author, name, version);
  }
  return slug;
}

/**
 * Parse the package author from the given `slug`
 */

char *
clib_package_parse_author(const char *slug) {
  return parse_repo_owner(slug);
}

/**
 * Parse the package version from the given `slug`
 */

char *
clib_package_parse_version(const char *slug) {
  return parse_repo_version(slug);
}

/**
 * Parse the package name from the given `slug`
 */

char *
clib_package_parse_name(const char *slug) {
  return parse_repo_name(slug);
}

/**
 * Create a new package dependency from the given `repo` and `version`
 */

clib_package_dependency_t *
clib_package_dependency_new(const char *repo, const char *version) {
  if (!repo || !version) return NULL;

  clib_package_dependency_t *dep = malloc(sizeof(clib_package_dependency_t));
  if (!dep) {
    return NULL;
  }

  dep->version = 0 == strcmp("*", version)
    ? str_copy(DEFAULT_REPO_VERSION)
    : str_copy(version);
  dep->name = clib_package_parse_name(repo);
  dep->author = clib_package_parse_author(repo);
  dep->next = NULL;

  return dep;
}

/**
 * Install the given `pkg` in `dir`
 */

int
clib_package_install(clib_package_t *pkg, const char *dir, int verbose) {
  if (!pkg || !dir) return -1;

  char *pkg_dir = path_join(dir, pkg->name);
  if (NULL == pkg_dir) {
    return -1;
  }

  if (-1 == mkdirp(pkg_dir, 0777)) {
    free(pkg_dir);
    return -1;
  }

  if (NULL == pkg->url) {
    pkg->url = clib_package_url(pkg->author, pkg->repo_name, pkg->version);
  }
  if (NULL == pkg->url) {
    free(pkg_dir);
    return -1;
  }

  if (NULL != pkg->src) {
    // write package.json

    char *package_json = path_join(pkg_dir, "package.json");
    if (NULL == package_json) {
      free(pkg_dir);
      return -1;
    }

    fs_write(package_json, pkg->json);
    free(package_json);

    // write each source

    list_node_t *node;
    list_iterator_t *it = list_iterator_new(pkg->src, LIST_HEAD);
    while ((node = list_iterator_next(it))) {
      char *filename = node->val;

      // download source file

      char *file_url = clib_package_file_url(pkg->url, filename);
      char *file_path = path_join(pkg_dir, basename(filename));

      if (NULL == file_url || NULL == file_path) {
        if (file_url) free(file_url);
        free(pkg_dir);
        return -1;
      }

      if (verbose) {
        clib_package_log("fetch", file_url);
        clib_package_log("save", file_path);
      }

      int rc = http_get_file(file_url, file_path);
      free(file_path);
      if (-1 == rc) {
        if (verbose) clib_package_error("error", "unable to fetch %s", file_url);
        free(pkg_dir);
        free(file_url);
        return -1;
      }
      free(file_url);
    }

    list_iterator_destroy(it);
  }

  return clib_package_install_dependencies(pkg, dir, verbose);
}

/**
 * Install the given `pkg`'s dependencies in `dir`
 */

int
clib_package_install_dependencies(clib_package_t *pkg, const char *dir, int verbose) {
  if (!pkg || !dir) return -1;
  if (NULL == pkg->dependencies) return 0;

  return install_packages(pkg->dependencies, dir, verbose);
}

/**
 * Install the given `pkg`'s development dependencies in `dir`
 */

int
clib_package_install_development(clib_package_t *pkg, const char *dir, int verbose) {
  if (!pkg || !dir) return -1;
  if (NULL == pkg->development) return 0;

  return install_packages(pkg->development, dir, verbose);
}

/**
 * Free a clib package
 */

void
clib_package_free(clib_package_t *pkg) {
  if (pkg->name) free(pkg->name);
  if (pkg->author) free(pkg->author);
  if (pkg->repo) free(pkg->repo);
  if (pkg->repo_name) free(pkg->repo_name);
  if (pkg->description) free(pkg->description);
  if (pkg->license) free(pkg->license);
  if (pkg->install) free(pkg->install);
  if (pkg->json) free(pkg->json);
  if (pkg->url) free(pkg->url);
  if (pkg->src) list_destroy(pkg->src);
  if (pkg->dependencies) list_destroy(pkg->dependencies);
  if (pkg->development) list_destroy(pkg->development);
  free(pkg);
}

static void
clib_package_dependency_free(void *_dep) {
  clib_package_dependency_t *dep = (clib_package_dependency_t *) _dep;
  if (dep->name) free(dep->name);
  if (dep->author) free(dep->author);
  if (dep->version) free(dep->version);
  free(dep);
}
