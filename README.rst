Sealang (fork of pybee/sealang)
===============================

.. image:: https://travis-ci.org/gtors/sealang.svg?branch=master
    :target: https://travis-ci.org/gtors/sealang

Sealang is an improved set of Python bindings for ``libclang``.

The upstream maintainers of ``libclang`` have not been especially responsive
to patches that address issues like Python 3 compatibility, and the
significant omissions in the API (such as extracting literals and operators).

This package is a fork of the official Python bindings for ``libclang``, patched to correct these problems.

Installation
------------

To compile Sealang, you'll need to:

1. Install LLVM 6.0 (with clang)
2. Set some environment variables
3. pip install git+https://github.com/gtors/sealang#egg=sealang-6.0


Usage
-----

Sealang provides a superset of the functionality provided by ``libclang``. Those features are:

* ``Cursor`` objects have 4 additional attributes:

  - ``literal`` - the value of a literal expression. Available on
    IntegerLiteral, FloatingLiteral, StringLiteral, CharacterLiteral, and
    CXXBooleanLiteral nodes.

  - ``operator`` - the printable version of an operator. Only available on
    BinaryOperator, UnaryOperator, and CompoundAssignOperator cursor nodes.

  - ``binary_operator`` - an enumeration value describing a BinaryOperator or
    CompoundAssignOperator node.

  - ``unary_operator`` - an enumeration value describing a UnaryOperator node.

* ``BinaryOperator`` - An enumeration for the types of binary operators:

  - ``BinaryOperator.INVALID``
  - ``BinaryOperator.PTRMEMD``
  - ``BinaryOperator.PTRMEMI``
  - ``BinaryOperator.MUL``
  - ``BinaryOperator.DIV``
  - ``BinaryOperator.REM``
  - ``BinaryOperator.ADD``
  - ``BinaryOperator.SUB``
  - ``BinaryOperator.SHL``
  - ``BinaryOperator.SHR``
  - ``BinaryOperator.LT``
  - ``BinaryOperator.GT``
  - ``BinaryOperator.LE``
  - ``BinaryOperator.GE``
  - ``BinaryOperator.EQ``
  - ``BinaryOperator.NE``
  - ``BinaryOperator.AND``
  - ``BinaryOperator.XOR``
  - ``BinaryOperator.OR``
  - ``BinaryOperator.LAND``
  - ``BinaryOperator.LOR``
  - ``BinaryOperator.ASSIGN``
  - ``BinaryOperator.MULASSIGN``
  - ``BinaryOperator.DIVASSIGN``
  - ``BinaryOperator.REMASSIGN``
  - ``BinaryOperator.ADDASSIGN``
  - ``BinaryOperator.SUBASSIGN``
  - ``BinaryOperator.SHLASSIGN``
  - ``BinaryOperator.SHRASSIGN``
  - ``BinaryOperator.ANDASSIGN``
  - ``BinaryOperator.XORASSIGN``
  - ``BinaryOperator.ORASSIGN``
  - ``BinaryOperator.COMMA``
  - ``BinaryOperator.UNKNOWN``

* ``UnaryOperator`` - An enumeration for the types of binary operators:

  - ``UnaryOperator.INVALID``
  - ``UnaryOperator.POSTINC``
  - ``UnaryOperator.POSTDEC``
  - ``UnaryOperator.PREINC``
  - ``UnaryOperator.PREDEC``
  - ``UnaryOperator.ADDROF``
  - ``UnaryOperator.DEREF``
  - ``UnaryOperator.PLUS``
  - ``UnaryOperator.MINUS``
  - ``UnaryOperator.NOT``
  - ``UnaryOperator.LNOT``
  - ``UnaryOperator.REAL``
  - ``UnaryOperator.IMAG``
  - ``UnaryOperator.EXTENSION``
  - ``UnaryOperator.UNKNOWN``

.. Documentation
.. -------------

.. Documentation for Sealang can be found on `Read The Docs`_.

How it works
------------

Sealang is a bit of a nasty hack. ``libclang`` is a set of C bindings to a C++
API; Python ``ctypes`` are then used to wrap the C API. However, while the C++
API is quite rich, ``libclang`` is less so.

Sealang bridges this gap by providing C wrappers around the C++ calls that
provide the useful functionality. This library of C functions is wrapped up as
a Python C module for delivery purposes - this C module contains no exposed
*Python* objects or methods, but because it's a module, the underlying
compiled `sealang.so` file is easy to find. `ctypes` are then used to expose
the `sealang` wrapper functions;

Internally, Sealang reproduces some minor pieces of the ``libclang`` API;
these are methods (such as the string creation and manipulation methods) that
aren't exposed as symbols for third-party use.

All this functionality is potentially a candidate to be passed upstream to
libclang.

Relationship to Clang
~~~~~~~~~~~~~~~~~~~~~

This project aims to mirror what is currently available in the Python bindings
to ``libclang``. The version number for this project is drawn from the version
and SVN revision of the official clang repository.

Any changes made upstream to ``libclang`` will be mirrored here; any changes
made here will, where possible, be pushed upstream to ``libclang``.

Contributing
------------

If you experience problems with Sealang, `log them on GitHub`_. If you
want to contribute code, please `fork the code`_ and `submit a pull request`_.

.. _Read The Docs: https://sealang.readthedocs.io
.. _log them on Github: https://github.com/gtors/sealang/issues
.. _fork the code: https://github.com/gtors/sealang
.. _submit a pull request: https://github.com/gtors/sealang/pulls
