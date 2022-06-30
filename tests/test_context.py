import pytest
import oopt_gnpy_libyang as ly

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
