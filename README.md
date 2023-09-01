# Opinionated Python bindings for the `libyang` library

[![Install via pip](https://img.shields.io/pypi/v/oopt-gnpy-libyang)](https://pypi.org/project/oopt-gnpy-libyang/)
[![Python versions](https://img.shields.io/pypi/pyversions/oopt-gnpy-libyang)](https://pypi.org/project/oopt-gnpy-libyang/)
[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/Telecominfraproject/oopt-gnpy-libyang/ci.yaml)](https://github.com/Telecominfraproject/oopt-gnpy-libyang/actions/workflows/ci.yaml)

Python bindings and packaging of [`libyang`](https://github.com/CESNET/libyang).
We're focusing on parsing, validating and accessing YANG-modeled JSON data trees.
Essentially, just enough to get [`gnpy`](https://github.com/Telecominfraproject/oopt-gnpy) going.
Want more?
Patches welcome.

Compared to the [CFFI libyang bindings](https://github.com/CESNET/libyang-python), this wrapper takes care of low-level memory management.
This means no more `node.free()` and `ctx.destroy()`.
We also produce prebuilt binary [wheels](https://realpython.com/python-wheels/) to make installation very simple.

## Usage

### Loading YANG data

```python
import oopt_gnpy_libyang as ly

c = ly.Context('tests/yang', ly.ContextOptions.AllImplemented | ly.ContextOptions.NoYangLibrary)
for m in ('iana-if-type', 'ietf-interfaces', 'ietf-ip'):
    c.load_module(m)
blob = '''{
  "ietf-interfaces:interfaces": {
    "interface": [
      {
        "name": "lo",
        "type": "iana-if-type:softwareLoopback",
        "ietf-ip:ipv4": {
          "address": [
            {
              "ip": "127.0.0.1",
              "prefix-length": 8
            }
          ]
        },
        "ietf-ip:ipv6": {
          "address": [
            {
              "ip": "::1",
              "prefix-length": 128
            }
          ]
        }
      },
      {
        "name": "eth0",
        "type": "iana-if-type:ethernetCsmacd"
      }
    ]
  }
}'''

data = c.parse_data(blob,
    ly.DataFormat.JSON, ly.ParseOptions.Strict | ly.ParseOptions.Ordered,
    ly.ValidationOptions.Present | ly.ValidationOptions.NoState)
```
### Working with data

Libyang works with forests (sets of trees), this is how to process all the data:
```python
for x in data.siblings():
    print(f'a sibling: {x.path}')
    for xx in x.children_dfs():
        print(f' {"term " if xx.is_term else "child"}: {xx.path}')
        if xx.is_term:
            print(f'  {xx.as_term()} {" (default)" if xx.as_term().is_default_value else ""}')
```
Data can be accessed via their known paths, of course. Either as a full, multi-level XPath:

```python
data["interface[name='lo']/ietf-ip:ipv6/address[ip='::1']/prefix-length"].as_term().value == 128
```
Or individually, one item per index:
```python
data["interface[name='lo']"]["ietf-ip:ipv6"]["address[ip='::1']"]["prefix-length"].as_term().value
```
Everything is an XPath, so it's possible to take a shortcut and skip specifying keys for single-element lists:
```python
data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"].as_term().value == 128
```
The data are provided as native Python types:
```python
type(data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"]
    .as_term().value) == int
```
Absolute paths and generic XPath expressions can be used to retrieve arbitrary parts of the data forest, and to iterate over them:
```python
for iface in data.find("/ietf-interfaces:interfaces/interface"):
    print iface["name"].as_term().value
```
Relative XPath conditions can be also used at the root level (which is represented as NULL in the C level):
```python
for iface in search_at_root(data)("ietf-interfaces:interfaces/interface"):
    print iface["name"].as_term().value
```
New values can be created; use `None` for non-terminals, or `str` when a value is needed:
```python
node = ctx.create("/ietf-interfaces:interfaces/interface[name='666']")
another = ctx.create("/ietf-interfaces:interfaces/interface[name='666']/ietf-ip:ipv6/enabled", "true")
data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"] = "64"
```

### Validation errors
In libyang, if an operation fails, error details are available via `context.errors()`:
```python
import json
wrong = json.loads(blob)
wrong["ietf-interfaces:interfaces"]["interface"][0]\
    ["ietf-ip:ipv6"]["address"][0]["prefix-length"] = 666
try:
    data = c.parse_data(json.dumps(wrong),
        ly.DataFormat.JSON, ly.ParseOptions.Strict | ly.ParseOptions.Ordered,
        ly.ValidationOptions.Present | ly.ValidationOptions.NoState)
    assert False
except ly.Error:
    for error in c.errors():
        assert error.path == "Schema location \"/ietf-interfaces:interfaces/interface/ietf-ip:ipv6/address/prefix-length\", data location \"/ietf-ip:address[ip='::1']\", line number 1."
        assert error.message == 'Value "666" is out of type uint8 min/max bounds.'
```

## Installing

We're producing wheels for many popular platforms.
The installation is as simple as:
```console-session
$ pip install oopt-gnpy-libyang
```

### Building from source

Since this library is a Python wrapper around a C++ wrapper around a C library, source-based builds are more complex.
They require:

- a C++20 compiler (e.g., GCC 10+, clang 10+, MSVC 17.2+)
- [`libyang`](https://github.com/CESNET/libyang) and its dependencies
- [`libyang-cpp`](https://github.com/CESNET/libyang-cpp/) and its dependencies
- [CMake](https://cmake.org/) 3.21+

Unlike the wheels already bundle all the required libraries, when building from source, `libyang`, `libyang-cpp` and all their dependencies will have to be installed first.
Also, in a from-source build these won't be bundled into the resulting package.
For an inspiration, consult our [GitHub packaging recipes](./.github/workflows/ci.yaml).

## License

Copyright © 2021-2023 Telecom Infra Project and GNPy contributors.
Licensed under the [3-clause BSD license](LICENSE).
