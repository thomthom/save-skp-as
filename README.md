# SaveSkpAs

Converts directory of SketchUp files (.skp) in bulk to a target SketchUp file version. Useful to convert many files to be compatible with older SketchUp
versions.

## Usage

### Parameters

```
SaveSkpAs SKPVERSION SOURCE DESTINATION [-f]
```

Example:

```
SaveSkpAs 6 "Source\Models\Path" "Destination\Models\Path"
```

#### SKPVERSION

The target SketchUp file version, one of the following values:

```
SKPVERSION | SketchUp Version
-----------------------------
         3 | SketchUp 3
         4 | SketchUp 4
         5 | SketchUp 5
         6 | SketchUp 6
         7 | SketchUp 7
         8 | SketchUp 8
      2013 | SketchUp 2013
      2014 | SketchUp 2014
      2015 | SketchUp 2015
      2016 | SketchUp 2016
      2017 | SketchUp 2017
      2018 | SketchUp 2018
```

#### SOURCE

Absolute or relative path to a directory with .skp files.

#### SOURCE

Absolute or relative path to a directory where the new files should be saved.

This directory must already exist.

### Options

#### Force Overwrite (`-f`)

By default, the app will prompt to overwrite existing files:

```
C:\Users\Thomas\Desktop
λ SaveSkpAs 8 "Source\Models\Path" "Destination\Models\Path"
    Version: 8
     Source: Source\Models\Path
Destination: Destination\Models\Path

Processing: "bottle base top.skp"
  Destination: Skp2\bottle base top.skp
  File already exist. Overwrite? [y/n]
```

This can be suppressed by adding `-f` at the end of the command;

```
SaveSkpAs 6 "Source\Models\Path" "Destination\Models\Path" -f
```

## Development

Project uses Visual Studio 2017, C++17 and the SketchUp C API.

Currently the project expected to find the SketchUp C API headers and binaries under;

* `%HOME%\SLAPI\SDK_Win_x64_18-0-18664\headers`
* `%HOME%\SLAPI\SDK_Win_x64_18-0-18664\binaries\sketchup\x64`

These paths are all set up in `SaveSkpAs\SketchUpAPI.props`. Modify to match your own location of the SketchUp C API SDK.

Only 64bit configuration is provided since the SketchUp C API SDK is only available in 64bit format.
