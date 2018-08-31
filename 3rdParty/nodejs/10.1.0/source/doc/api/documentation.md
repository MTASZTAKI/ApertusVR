# About this Documentation

<!--introduced_in=v0.10.0-->
<!-- type=misc -->

The goal of this documentation is to comprehensively explain the Node.js
API, both from a reference as well as a conceptual point of view. Each
section describes a built-in module or high-level concept.

Where appropriate, property types, method arguments, and the arguments
provided to event handlers are detailed in a list underneath the topic
heading.

## Contributing

If errors are found in this documentation, please [submit an issue][]
or see [the contributing guide][] for directions on how to submit a patch.

Every file is generated based on the corresponding `.md` file in the
`doc/api/` folder in Node.js's source tree. The documentation is generated
using the `tools/doc/generate.js` program. An HTML template is located at
`doc/template.html`.

## Stability Index

<!--type=misc-->

Throughout the documentation are indications of a section's
stability. The Node.js API is still somewhat changing, and as it
matures, certain parts are more reliable than others. Some are so
proven, and so relied upon, that they are unlikely to ever change at
all. Others are brand new and experimental, or known to be hazardous
and in the process of being redesigned.

The stability indices are as follows:

```txt
Stability: 0 - Deprecated. This feature is known to be problematic, and changes
may be planned. Do not rely on it. Use of the feature may cause warnings to be
emitted. Backwards compatibility across major versions should not be expected.
```

```txt
Stability: 1 - Experimental. This feature is still under active development and
subject to non-backwards compatible changes, or even removal, in any future
version. Use of the feature is not recommended in production environments.
Experimental features are not subject to the Node.js Semantic Versioning model.
```

```txt
Stability: 2 - Stable. The API has proven satisfactory. Compatibility with the
npm ecosystem is a high priority, and will not be broken unless absolutely
necessary.
```

Caution must be used when making use of `Experimental` features, particularly
within modules that may be used as dependencies (or dependencies of
dependencies) within a Node.js application. End users may not be aware that
experimental features are being used, and therefore may experience unexpected
failures or behavior changes when API modifications occur. To help avoid such
surprises, `Experimental` features may require a command-line flag to
explicitly enable them, or may cause a process warning to be emitted.
By default, such warnings are printed to [`stderr`][] and may be handled by
attaching a listener to the [`'warning'`][] event.

## JSON Output
<!-- YAML
added: v0.6.12
-->

> Stability: 1 - Experimental

Every `.html` document has a corresponding `.json` document presenting
the same information in a structured manner. This feature is
experimental, and added for the benefit of IDEs and other utilities that
wish to do programmatic things with the documentation.

## Syscalls and man pages

System calls like open(2) and read(2) define the interface between user programs
and the underlying operating system. Node.js functions
which simply wrap a syscall,
like [`fs.open()`][], will document that. The docs link to the corresponding man
pages (short for manual pages) which describe how the syscalls work.

Some syscalls, like lchown(2), are BSD-specific. That means, for
example, that [`fs.lchown()`][] only works on macOS and other BSD-derived
systems, and is not available on Linux.

Most Unix syscalls have Windows equivalents, but behavior may differ on Windows
relative to Linux and macOS. For an example of the subtle ways in which it's
sometimes impossible to replace Unix syscall semantics on Windows, see [Node
issue 4760](https://github.com/nodejs/node/issues/4760).

[`'warning'`]: process.html#process_event_warning
[`stderr`]: process.html#process_process_stderr
[`fs.open()`]: fs.html#fs_fs_open_path_flags_mode_callback
[`fs.lchown()`]: fs.html#fs_fs_lchown_path_uid_gid_callback
[submit an issue]: https://github.com/nodejs/node/issues/new
[the contributing guide]: https://github.com/nodejs/node/blob/master/CONTRIBUTING.md
