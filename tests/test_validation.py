import pytest
import oopt_gnpy_libyang as ly

def test_ietf_interfaces(context_with_modules):
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
      },
      {
        "name": "eth1",
        "type": "iana-if-type:ethernetCsmacd"
      },
      {
        "name": "br0",
        "type": "iana-if-type:bridge",
        "ietf-ip:ipv4": {
          "address": [
            {
              "ip": "10.0.0.2",
              "prefix-length": 24
            }
          ],
          "neighbor": [
            {
              "ip": "10.0.0.1",
              "link-layer-address": "cc:2d:e0:52:30:9c"
            },
            {
              "ip": "10.0.1.1",
              "link-layer-address": "cc:2d:e0:52:30:9c"
            }
          ]
        },
        "ietf-ip:ipv6": {
          "address": [
            {
              "ip": "fe80::211:17ff:fe01:61",
              "prefix-length": 64
            }
          ]
        }
      }
    ]
  },
  "ietf-hardware:hardware": {
  }
}
    '''
    data = context_with_modules.parse_data(blob, ly.DataFormat.JSON, ly.ParseOptions.Strict | ly.ParseOptions.Ordered, ly.ValidationOptions.Present | ly.ValidationOptions.NoState)
    assert data.path == '/ietf-interfaces:interfaces'

    assert [x.path for x in data.siblings()] == ['/ietf-interfaces:interfaces', '/ietf-hardware:hardware']
    assert [inner.path for top in data.siblings() for inner in top.children_dfs()] == [
        '/ietf-interfaces:interfaces',
        "/ietf-interfaces:interfaces/interface[name='lo']",
        "/ietf-interfaces:interfaces/interface[name='lo']/name",
        "/ietf-interfaces:interfaces/interface[name='lo']/type",
        "/ietf-interfaces:interfaces/interface[name='lo']/enabled",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv4",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv4/enabled",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv4/forwarding",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv4/address[ip='127.0.0.1']",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv4/address[ip='127.0.0.1']/ip",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv4/address[ip='127.0.0.1']/prefix-length",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6/enabled",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6/forwarding",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6/address[ip='::1']",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6/address[ip='::1']/ip",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6/address[ip='::1']/prefix-length",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6/dup-addr-detect-transmits",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6/autoconf",
        "/ietf-interfaces:interfaces/interface[name='lo']/ietf-ip:ipv6/autoconf/create-global-addresses",
        "/ietf-interfaces:interfaces/interface[name='eth0']",
        "/ietf-interfaces:interfaces/interface[name='eth0']/name",
        "/ietf-interfaces:interfaces/interface[name='eth0']/type",
        "/ietf-interfaces:interfaces/interface[name='eth0']/enabled",
        "/ietf-interfaces:interfaces/interface[name='eth1']",
        "/ietf-interfaces:interfaces/interface[name='eth1']/name",
        "/ietf-interfaces:interfaces/interface[name='eth1']/type",
        "/ietf-interfaces:interfaces/interface[name='eth1']/enabled",
        "/ietf-interfaces:interfaces/interface[name='br0']",
        "/ietf-interfaces:interfaces/interface[name='br0']/name",
        "/ietf-interfaces:interfaces/interface[name='br0']/type",
        "/ietf-interfaces:interfaces/interface[name='br0']/enabled",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/enabled",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/forwarding",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/address[ip='10.0.0.2']",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/address[ip='10.0.0.2']/ip",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/address[ip='10.0.0.2']/prefix-length",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/neighbor[ip='10.0.0.1']",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/neighbor[ip='10.0.0.1']/ip",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/neighbor[ip='10.0.0.1']/link-layer-address",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/neighbor[ip='10.0.1.1']",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/neighbor[ip='10.0.1.1']/ip",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv4/neighbor[ip='10.0.1.1']/link-layer-address",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6/enabled",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6/forwarding",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6/address[ip='fe80::211:17ff:fe01:61']",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6/address[ip='fe80::211:17ff:fe01:61']/ip",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6/address[ip='fe80::211:17ff:fe01:61']/prefix-length",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6/dup-addr-detect-transmits",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6/autoconf",
        "/ietf-interfaces:interfaces/interface[name='br0']/ietf-ip:ipv6/autoconf/create-global-addresses",
        '/ietf-hardware:hardware',
    ]
    assert [inner.path for top in data.siblings() for inner in top.immediate_children()] == [
        f"/ietf-interfaces:interfaces/interface[name='{if_}']" for if_ in ('lo', 'eth0', 'eth1', 'br0')
    ]

    # quoting doesn't matter
    assert data["interface[name='lo']"].path == "/ietf-interfaces:interfaces/interface[name='lo']"
    assert data['interface[name="lo"]'].path == "/ietf-interfaces:interfaces/interface[name='lo']"

    with pytest.raises(KeyError, match='No such data node below.*'):
        data['interface[name="ZZZ"]']

    with pytest.raises(KeyError, match="Too many nodes below '/ietf-interfaces:interfaces' for 'interface/enabled'"):
        data["interface/enabled"]

    # one multi-level XPath with all keys
    assert data["interface[name='lo']/ietf-ip:ipv6/address[ip='::1']/prefix-length"].as_term().value == 128

    # one item per index, all key values specified
    assert data["interface[name='lo']"]["ietf-ip:ipv6"]["address[ip='::1']"]["prefix-length"].as_term().value == 128

    # one item per index. single-item list keys without their values
    assert data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"].as_term().value == 128

    assert isinstance(data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"].as_term().value, int)

    assert len(data.find("/ietf-interfaces:interfaces/interface")) == 4
    assert [x["name"].as_term().value for x in data.find("/ietf-interfaces:interfaces/interface")] == ["lo", "eth0", "eth1", "br0"]

    # paths relative to "the root", which on itself is not a node at the libyang level
    assert len(ly.search_at_root(data)("ietf-interfaces:interfaces/interface")) == 4
    assert [x["name"].as_term().value for x in ly.search_at_root(data)("ietf-interfaces:interfaces/interface")] == ["lo", "eth0", "eth1", "br0"]
    assert len(ly.search_at_root(data)("ietf-hardware:hardware")) == 1
    assert len(data.find("ietf-hardware:hardware")) == 0

    # in/__contains__
    assert "name" in data["interface[name='lo']"]
    assert "pwnaaaageeee" not in data["interface[name='lo']"]

    # schema paths
    assert data['interface[name="lo"]'].schema.module.name == "ietf-interfaces"
    assert data['interface[name="lo"]'].schema.module.revision == "2018-02-20"
    assert data['interface[name="lo"]'].schema.module.implemented
    assert data['interface[name="lo"]'].schema.name == "interface"
    assert data['interface[name="lo"]'].schema.path == "/ietf-interfaces:interfaces/interface"

    # first assignmnet overwrites the value
    assert data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"].as_term().value != 64
    data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"] = "64"
    assert data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"].as_term().value == 64
    # doing it once again is a no-op
    data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"] = "64"
    assert data["interface[name='lo']"]["ietf-ip:ipv6"]["address"]["prefix-length"].as_term().value == 64

    separate_node = context_with_modules.create("/ietf-interfaces:interfaces/interface[name='666']/ietf-ip:ipv6/enabled", "true")
    assert separate_node.path == "/ietf-interfaces:interfaces/interface[name='666']/ietf-ip:ipv6/enabled"
    with pytest.raises(KeyError, match="No such data node below"):
        data[separate_node.path]
    # non-leafs can be created as well
    assert "/ietf-interfaces:interfaces/interface[name='42']" not in separate_node
    separate_node["/ietf-interfaces:interfaces/interface[name='42']"] = None
    assert "/ietf-interfaces:interfaces/interface[name='42']" in separate_node
    context_with_modules.create("/ietf-interfaces:interfaces/interface[name='666']")

def test_types(context_no_libyang):
    context_no_libyang.parse_module('''
module dummy {
    yang-version 1.1;
    namespace "dd";
    prefix dd;
    revision 2023-09-01;

    leaf dec {
        type decimal64 {
            fraction-digits 6;
        }
    }

    leaf emp {
        type empty;
    }

    leaf enu {
        type enumeration {
            enum A;
            enum B;
        }
    }
}
''', ly.SchemaFormat.YANG)
    data = context_no_libyang.parse_data('{"dummy:dec": "333.666", "dummy:emp": [null], "dummy:enu": "B"}', ly.DataFormat.JSON)
    assert float(data["/dummy:dec"].as_term().value) == 333.666
    assert str(data["/dummy:dec"].as_term().value) == "333.666000"
    assert str(data["/dummy:dec"].as_term()) == "333.666"
    assert isinstance(data["/dummy:emp"].as_term().value, ly.Empty)
    assert data["/dummy:emp"].as_term().value is not None
    assert isinstance(data["/dummy:enu"].as_term().value, ly.Enum)
    assert data["/dummy:enu"].as_term().value.name == "B"
    assert data["/dummy:enu"].as_term().value.value == 1
