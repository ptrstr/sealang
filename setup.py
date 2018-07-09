#!/usr/bin/env python
# -*- coding: utf-8 -*-
from setuptools import setup, Extension

import os
import platform
import subprocess
import sys
import textwrap

# Set up the build environment.
llvm_home = os.environ.get('LLVM_HOME', '/usr')
llvm_config = os.path.join(llvm_home, 'bin', 'llvm-config')

if not os.path.isfile(llvm_config):
    print("Unable to set up build environment. Have you installed LLVM and set LLVM_HOME?")

    def dedent_print(s):
        print(textwrap.dedent(s))

    if platform.system().lower() == 'darwin':
        dedent_print("""
        Using homebrew:
            brew install llvm --with-clang --with-asan
            export LLVM_HOME=/usr/local/opt/llvm
            export DYLD_LIBRARY_PATH=$LLVM_HOME/lib
        """)
    elif platform.dist()[0].lower() == 'ubuntu':
        dedent_print("""
        Using apt-get:
            sudo apt-get install libclang-6.0 clang-6.0 -y
            export LLVM_HOME=/usr/lib/llvm-6.0
            export LD_LIBRARY_PATH=$LLVM_HOME/lib
        """)
    elif platform.dist()[0].lower() == 'arch':
        dedent_print("""
        Using pacman:
            sudo pacman -S llvm clang"
        """)
    sys.exit(1)

llvm_cflags = subprocess.check_output([llvm_config, '--cxxflags']).split()
llvm_ldflags = subprocess.check_output([llvm_config, '--ldflags']).split()

if sys.version_info.major >= 3:
    llvm_cflags = [p.decode('utf-8') for p in llvm_cflags]
    llvm_ldflags = [p.decode('utf-8') for p in llvm_ldflags]

setup(
    name='sealang',
    version='6.0',
    description='An extended set of Python bindings for libclang',
    long_description=open('README.rst').read(),
    url='http://github.com/pybee/sealang',
    license='License :: OSI Approved :: University of Illinois/NCSA Open Source License',
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: University of Illinois/NCSA Open Source License',
        'Programming Language :: Python',
        'Development Status :: 5 - Production/Stable',
        'Topic :: Software Development :: Compilers',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
    ],
    keywords=['llvm', 'clang', 'libclang'],
    author='LLVM team and Russell Keith-Magee',
    author_email='russell@keith-magee.com',
    packages=[
        'clang',
        'sealang'
    ],
    ext_modules=[
        Extension(
            'sealang',
            sources=['sealang/sealang.cpp'],
            libraries=[
                'clangAST', 'LLVMCore', 'LLVMSupport', 'LLVMBinaryFormat',
                'm', 'z', 'pthread', 'curses'
            ],
            extra_compile_args=llvm_cflags,
            extra_link_args=llvm_ldflags,
        ),
    ],
    # if nose.collector is used, many plugins will not be available
    # see: https://nose.readthedocs.io/en/latest/setuptools_integration.html
    test_suite='nose.collector',
    tests_require=['nose'],
    zip_safe=True,
)
