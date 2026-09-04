# SLProject Documentation

See the [Online Documentation](https://cpvrlab.github.io/SLProject4/). It is
generated with [Doxygen](https://www.doxygen.nl) from `docs/Doxyfile` and
deployed by the `deploy-pages` GitHub action on every push to `main`.

## Generating the documentation locally

The same steps the action performs, run from this `docs` folder:

```
# Prerequisites: doxygen and graphviz (Doxyfile has HAVE_DOT = YES)
git clone https://github.com/jothepro/doxygen-awesome-css.git -b v2.2.1
doxygen
cp -r images html
```

The HTML output is written to `docs/html` (git-ignored); open
`docs/html/index.html` to read it.
