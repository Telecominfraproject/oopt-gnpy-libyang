import oopt_gnpy_libyang as ly
import os
from pathlib import Path
import pytest

@pytest.fixture(scope='function')
def context_no_libyang():
    return ly.Context(str(Path(os.environ.get('CMAKE_CURRENT_SOURCE_DIR', os.getcwd())) / 'tests' / 'yang'),
                      ly.ContextOptions.AllImplemented | ly.ContextOptions.NoYangLibrary)

@pytest.fixture(scope='function')
def context_with_modules(context_no_libyang):
    '''A context with all relevant YANG modules added and implemented'''
    c = context_no_libyang
    c.load_module('iana-if-type')
    m = c.load_module('ietf-interfaces')
    m.set_implemented_all_features()
    c.load_module('ietf-ip')
    c.load_module('ietf-hardware')
    return c
