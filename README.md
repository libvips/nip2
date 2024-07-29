# nip2 --- a user interface for libvips

[![Snap
Status](https://build.snapcraft.io/badge/jcupitt/nip2.svg)](https://build.snapcraft.io/user/jcupitt/nip2)

nip2 is a GUI for the [libvips image processing 
library](https://libvips.github.io/libvips). It's a little like a spreadsheet:
you create a set of formula connecting your objects together, and on a change
nip2 will recalculate. This makes it convenient for developing image processing
systems since you can watch pixels change as you adjust your equations.

Because nip2 uses libvips as the image processing engine it can handle very
large images and only needs a little memory. It scales to fairly complex
workflows: I've used it to develop systems with more than 10,000 cells,
analyzing images of many tens of gigabytes. It has a batch mode, so you
can run any image processing system you develop from the command-line and
without a GUI.

![image](https://github.com/user-attachments/assets/aa2c3e0f-9f96-4594-9f1d-62ef770d0775)

## Installing

You can probably install nip2 via your package manager. For
Windows and OS X, you can download a binary from the [nip2 releases
page](https://github.com/libvips/nip2/releases). If you have to build from
source, see the section below.

## Documentation

nip2 comes with a 50-page manual --- press F1 or Help / Contents in the
program to view it.

## Building nip2 from source

In the nip2 directory you should just be able to do the usual:

```
./configure
make
sudo make install
```

By default this will install files to `/usr/local`. Check the summary at the
end of `configure` and make sure you have all of the features you want. 

If you downloaded from GIT you'll need:

```
./autogen.sh
```

first to build the configure system. 

nip2 needs vips, gtk2 and libxml2 at runtime and flex/bison at compile time.
If you have fftw3, gsl, goffice, libgvc you get extra features.

### snapcraft

Rebuild snap with:

```
snapcraft cleanbuild 
```

Though it's done automatically on a push.
