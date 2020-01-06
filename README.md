# navopher

[![Build Status](https://travis-ci.com/lacerto/navopher.svg?branch=master)](https://travis-ci.com/lacerto/navopher)

Small utility that generates a gophermap for a *phlog* (gopher log - analogous to *blog*) based on the contents of a directory.

## Build
### Dependencies
* [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
* [glib](https://www.gtk.org/download/)

To install glib run:
```
$ sudo apt install libglib2.0-dev
```
or
```
$ brew install glib
```
Or use the package manager of your platform.

### Compile
To compile run:

```
$ make
```
(or `gmake` depending on the platform)

## Use
```
$ navopher DIRECTORY FILE
```
`navopher` must be invoked with two command line arguments.

`DIRECTORY` is a path to the directory containing the phlog text files making up your phlog. Only *.txt and *.md files are used to create the file list for the resulting gophermap. The files should have names using the following format:

```
YYYY-MM-DD__Phlog_Title.txt
```

`navopher` converts this to the following gophermap line:

```
0[YYYY-MM-DD] Phlog Title    YYYY-MM-DD__Phlog_Title.txt
```

Directory names within `DIRECTORY` are converted similarly.

### Template directives

`FILE` is the name of the template file within `DIRECTORY`. This is the template for the generated gophermap.

Currently two directives are recognized:
* {{ FILE_LIST }} - the line containing this directive in the template will be replaced by the converted file list
* {{ DATE_TIME }} - this will be substituted with the machine's current local date/time.

### Example

```
 ____  _   _ _     ___   ____
|  _ \| | | | |   / _ \ / ___|
| |_) | |_| | |  | | | | |  _
|  __/|  _  | |__| |_| | |_| |
|_|   |_| |_|_____\___/ \____|

Last updated: {{ DATE_TIME }}

{{ FILE_LIST }}
```

The contents of directory **phlog**:

```
2019-02-01__First.md
2019-02-02__Managing_the_hole.md
2019-12-23__Homemade_tools.md
gophermap.template
```

After running
```
$ navopher phlog gophermap.template
```
the resulting `gophermap` looks like this:

```
  ____  _   _ _     ___   ____
 |  _ \| | | | |   / _ \ / ___|
 | |_) | |_| | |  | | | | |  _
 |  __/|  _  | |__| |_| | |_| |
 |_|   |_| |_|_____\___/ \____|

 Last updated: 2020-01-06 11:27:19 UTC

0[2019-12-23] Homemade tools    2019-12-23__Homemade_tools.md
0[2019-02-02] Managing the hole 2019-02-02__Managing_the_hole.md
0[2019-02-01] First     2019-02-01__First.md
```

In `lynx` the resulting gopher menu might look like this one:

```
         ____  _   _ _     ___   ____
        |  _ \| | | | |   / _ \ / ___|
        | |_) | |_| | |  | | | | |  _
        |  __/|  _  | |__| |_| | |_| |
        |_|   |_| |_|_____\___/ \____|

        Last updated: 2020-01-06 11:27:19 UTC

(FILE) [2019-12-23] Homemade tools
(FILE) [2019-02-02] Managing the hole
(FILE) [2019-02-01] First

       ______________________________________________________________________
                      Gophered by Gophernicus/1.6 on NetBSD/amd64 8.0_STABLE
```