# Introduction

lua-idn2 is a set of [lua](https://www.lua.org) bindings to [libidn2](https://www.gnu.org/software/libidn/libidn2/manual/libidn2.html), an implementation of IDNA2008/TR46 internationalized domain names.

# API

## idn2 library

Load with `require "idn2"`

### `idn2.to_ascii(input, flags)` <!-- --> {#idn2.to_ascii}

Note: This function will not exist if lua-idn2 is compiled against libidn2 0.16 or earlier.


### `idn2.to_unicode(input, flags)` <!-- --> {#idn2.to_unicode}

Note: This function will not exist if lua-idn2 is compiled against libidn2 0.16 or earlier.


### `idn2.lookup(src, flags)` <!-- --> {#idn2.lookup}


### `idn2.register(ulabel, alabel, flags)` <!-- --> {#idn2.register}


### `idn2.to_ascii_lz(input, flags)` <!-- --> {#idn2.to_ascii_lz}

Note: This function will not exist if lua-idn2 is compiled against libidn2 0.16 or earlier.


### `idn2.to_unicode_8zlz(input, flags)` <!-- --> {#idn2.to_unicode_8zlz}

Note: This function will not exist if lua-idn2 is compiled against libidn2 0.16 or earlier.


### `idn2.to_unicode_lzlz(input, flags)` <!-- --> {#idn2.to_unicode_lzlz}

Note: This function will not exist if lua-idn2 is compiled against libidn2 0.16 or earlier.


### `idn2.lookup_ul(src, flags)` <!-- --> {#idn2.lookup_ul}


### `idn2.register_ul(ulabel, alabel, flags)` <!-- --> {#idn2.register_ul}


### `idn2.check_version(req_version)` <!-- --> {#idn2.check_version}

Returns a string containing the version of libidn2.


### `idn2.VERSION` <!-- --> {#idn2.VERSION}

The libidn2 version as a string.


### `idn2.VERSION_NUMBER` <!-- --> {#idn2.VERSION_NUMBER}

The libidn2 version as an integer.


### `idn2.LABEL_MAX_LENGTH` <!-- --> {#idn2.LABEL_MAX_LENGTH}


### `idn2.DOMAIN_MAX_LENGTH` <!-- --> {#idn2.DOMAIN_MAX_LENGTH}


# Links

  - [Github](https://github.com/daurnimator/lua-idn2)
  - [Issue tracker](https://github.com/daurnimator/lua-idn2/issues)
  - [libidn2](https://www.gnu.org/software/libidn/libidn2/manual/libidn2.html)
