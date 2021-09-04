# TBD

## Acquiring

### Source Tree

To download the source tree, run the following command:  
`$ git clone --recursive https://github.com/cometodukey/tbd`

If you have already cloned without `--recursive`, you can run this command to recursively download submodules:  
`$ git submodule update --init --recursive`

### Meson

Meson should be installed via your systems package manager.

If Meson is not available on your systems repositories, you can find a guide on how to install it [here](https://mesonbuild.com/FAQ.html#how-to-use-meson-on-a-host-where-it-is-not-available-in-system-packages).

## Building

### Configure time options

When setting up the build directory pass:
* `-Dwerror=true` to enable `-Werror`
* `-Db_sanitize=xyz` to enable sanitisers

### Setup and build

Run this command to setup the Meson build directory:  
`$ meson setup builddir/`

If your version of Meson is newer than 0.55.0, you can run this:  
`$ meson compile -C builddir/`

Otherwise, run this:  
`$ ninja -C builddir/`
