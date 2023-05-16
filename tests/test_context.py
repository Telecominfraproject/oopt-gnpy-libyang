import pytest
import oopt_gnpy_libyang as ly
from pathlib import Path

def test_no_module(context_no_libyang):
    with pytest.raises(ly.Error, match="Can't load module 'yay'"):
        context_no_libyang.load_module('yay')

def test_empty():
    c = ly.Context()
    with pytest.raises(ly.Error, match="Can't load module 'yay'"):
        c.load_module('yay')

def test_features(context_no_libyang):
    context_no_libyang.load_module('iana-if-type')
    m = context_no_libyang.load_module('ietf-interfaces')
    with pytest.raises(ly.Error, match="Couldn't set module 'ietf-interfaces' to implemented: LY_EINVAL"):
       m.set_implemented_with_features(['arbotrary-names'])
    errors = [(e.level, e.code, e.message, e.path, e.validation_code, e.app_tag) for e in context_no_libyang.errors()]
    assert errors == [
        (ly.LogLevel.Error, ly.ErrorCode.InvalidValue, 'Feature "arbotrary-names" not found in module "ietf-interfaces".',
         None, ly.ValidationErrorCode.Success, None)
    ]
    m.set_implemented_with_features(['arbitrary-names'])
    for feature in m.features:
        assert m.feature_enabled(feature.name) == (feature.name == 'arbitrary-names')
    m.set_implemented_all_features()
    for feature in m.features:
        assert m.feature_enabled(feature.name)

def test_no_shared_errors(context_no_libyang):
    assert context_no_libyang.errors() == []

def test_explicit_loading(context_no_libyang):
    TEST_ROOT = Path(__file__).parent
    with pytest.raises(ly.Error, match="Can't parse module: LY_ESYS"):
        context_no_libyang.parse_module(TEST_ROOT / 'no-such-path.yang', ly.SchemaFormat.YANG)
    m1 = context_no_libyang.parse_module(TEST_ROOT / 'yang' / 'iana-if-type@2017-01-19.yang', ly.SchemaFormat.YANG)
    assert m1.name == 'iana-if-type'
    m2 = context_no_libyang.parse_module((TEST_ROOT / 'yang' / 'iana-hardware@2018-03-13.yang').read_text(), ly.SchemaFormat.YANG)
    assert m2.name == 'iana-hardware'

def test_version():
    assert len(ly.libyang_version_info()) == 3
    assert ly.libyang_version_info()[0] == 2
    assert ly.libyang_version()[0:2] == '2.'
