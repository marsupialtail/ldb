from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext
__version__ = "0.0.2"

DESCRIPTION = 'ldbpy'
LONG_DESCRIPTION = """
meant to be used as a library for pyquokka
"""


mod = Pybind11Extension(
        "ldbpy",
        ["src/ldb.cc","src/cep.cc","src/tdigest.cc", "src/bridge.cc", "src/type.cc"],  # Sort source files for reproducibility
        extra_compile_args = ['-std=c++17', '-Wall', '-O3', '-fopenmp', '-DAVX2=1'],
        extra_link_args=['-fopenmp']
        )
ext_modules = [
        mod, 
]

setup(
    name="ldbpy",
    version=__version__,
    author="Ziheng Wang",
    author_email="zihengw@stanford.edu",
    ext_modules=ext_modules,
    description = DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    # extras_require={"test": "pytest"},
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)
