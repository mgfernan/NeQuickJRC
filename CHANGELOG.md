# CHANGELOG


## v0.2.0 (2025-03-24)

### Features

- **nequick**: Expose latitudes in to_gim method
  ([`08cb9ba`](https://github.com/mgfernan/NeQuickJRC/commit/08cb9ba692752e2478a981f66f0ecdc0cbf2c996))


## v0.1.0 (2025-03-21)

### Features

- **nequick**: Add Python library using C API
  ([`8d26664`](https://github.com/mgfernan/NeQuickJRC/commit/8d26664749ba5330500002cd7f320db7b41b15a3))

The library does not use the executable, but includes the native C code implementation

- **pynequick**: Add Python module with client for GIM generation
  ([`6a7a9ac`](https://github.com/mgfernan/NeQuickJRC/commit/6a7a9ace6653c3db073b96c02b6878e5136c1b93))

### Refactoring

- Move error code to public include
  ([`68092aa`](https://github.com/mgfernan/NeQuickJRC/commit/68092aa3568e8462d88ac3cdcf6d67a8ed274dae))

This code is used by the library and the application program

The installation of the library does not however install this file

- **nequick**: Use dependency injection for GIM generation
  ([`4547b62`](https://github.com/mgfernan/NeQuickJRC/commit/4547b62c9cef09b3e457ad5351adec9e43dfea84))
