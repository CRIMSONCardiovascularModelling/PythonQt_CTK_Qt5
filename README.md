# PythonQt_CTK_Qt5
The version of PythonQt used by CTK, for use in test projects; with build scripts edited to support Qt 5.7.0.

Unfortunately I was not able to identify the exact version of PythonQt this was based off of, I would guess it's related to the [commontk/PythonQt](https://github.com/commontk/PythonQt) repository.

This is an unofficial edit of PythonQt, you can find the original readme files that came with the repository in z_old_README and z_old_README.md

Only Release builds are supported at this time, tested with Qt 5.7.0 and Visual Studio 2013 (on Windows).

# Usage

1. Configure PythonQt. Set up a build directory and run cmakegui in there, you will need to provide the build script with Qt5_DIR, for me it's C:\Qt\Qt5.7.0\5.7\msvc2013_64\lib\cmake\Qt5

2. Open up the PythonQt solution in the build directory, build it as a **Release** build (change it in the Solution's configuration manager).

3. Build the PythonQt project, you should not get any errors or warnings

4. In PythonQt_CTK\examples\ExpressionTesting, open MyPythonQtTestProj.pro in QtCreator

5. Build the project in **Release** mode, and run it

# Example code

The main reason I made this was to test out the capabilities of PythonQt and show some quirks, namely this workflow for setting up an array parameter to a Python function.

```c++
  QVariantList parameters;
  QVariant a = QVariant::fromValue(22);
  QVariant b = QVariant::fromValue(35);
  QVariantList items = {a,b};

  // It will let you append a QVariantList to a QVariantList, but it doesn't actually wrap it in a QVariant for you
  // and it will not actually be able to do anything with it.
  QVariant items_Variant = QVariant::fromValue(items);
  parameters.append(items_Variant);

  QVariant arrayResult = mainModule.call("takesInArray", parameters);
```
