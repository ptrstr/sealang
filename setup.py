#!/usr/bin/env python
# coding: utf-8

import sys

major, minor = sys.version_info[:2]
if major < 3 or minor < 7:
    print("Unsupported Python version. Version requirement is >= 3.7")
    sys.exit(1)

import os
import subprocess

from setuptools import setup, Extension
from distutils import sysconfig
from pathlib import Path

extension = ".exe" if sys.platform == "win32" else ""

llvm_home = os.environ.get("LLVM_HOME", "/usr")


def iter_llvm_config():
    paths = (
        Path(llvm_home, "bin", "llvm-config" + extension),
        Path("/usr/bin/llvm-config-10"),
        Path("/usr/bin/llvm-config"),
    )

    for p in paths:
        print(f"Check {p}. is_file = {p.is_file()}, is_symlink = {p.is_symlink()}")
        if p.is_file() or p.is_symlink():
            yield p


llvm_config = next(iter_llvm_config(), None)
if llvm_config is None:
    print(
        "Unable to set up build environment. Have you installed LLVM and set LLVM_HOME?"
    )
    sys.exit(1)

llvm_cflags = (
    subprocess.check_output([str(llvm_config), "--cxxflags"]).decode("utf-8").split()
)

if sys.platform != "win32":
    # Disable debug symbols in extension (reduce .so size)
    cflags = sysconfig.get_config_var("CFLAGS")
    sysconfig._config_vars["CFLAGS"] = cflags.replace(" -g ", " ")

llvm_ldflags = (
    subprocess.check_output([str(llvm_config), "--ldflags"]).decode("utf-8").split()
)

setup(
    name="sealang",
    version="10.0",
    description="An extended set of Python bindings for libclang (fork of pybee/sealang)",
    long_description=open("README.rst").read(),
    url="http://github.com/gtors/sealang",
    license="License :: OSI Approved :: University of Illinois/NCSA Open Source License",
    classifiers=[
        "Intended Audience :: Developers",
        "License :: OSI Approved :: University of Illinois/NCSA Open Source License",
        "Programming Language :: Python",
        "Development Status :: 5 - Production/Stable",
        "Topic :: Software Development :: Compilers",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
    ],
    keywords=["llvm", "clang", "libclang"],
    author="LLVM team, Russell Keith-Magee and Andrey Torsunov",
    author_email="andrey.torsunov@gmail.com",
    packages=["clang", "sealang"],
    ext_modules=[
        Extension(
            "sealang",
            sources=["sealang/sealang.cpp"],
            libraries=["clangAST", "clangBasic", "clangLex", "libclang", "LLVMBinaryFormat", "LLVMBitstreamReader", "LLVMCore", "LLVMFrontendOpenMP", "LLVMRemarks", "LLVMSupport"],
            extra_compile_args=llvm_cflags,
            extra_link_args=llvm_ldflags,
        ),
    ],
    # if nose.collector is used, many plugins will not be available
    # see: https://nose.readthedocs.io/en/latest/setuptools_integration.html
    test_suite="nose.collector",
    tests_require=["nose"],
    zip_safe=True,
)
