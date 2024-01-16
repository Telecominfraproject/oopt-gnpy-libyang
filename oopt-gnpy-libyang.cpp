#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <regex>
#include <sstream>
#include <libyang-cpp/Context.hpp>
#include <libyang/version.h>

using namespace std::literals;
using namespace pybind11::literals;
using namespace libyang;
namespace py = pybind11;

namespace {
std::string quote_string(const std::string& what)
{
    return "\"" +
        std::regex_replace( // escape quotes
                std::regex_replace( // escape backslashes
                    what, std::regex("\\\\"), "\\\\"),
                std::regex("\""), "\\\"")
        + "\"";
}

std::string repr_optional_string(const std::optional<std::string>& what)
{
    return what ? quote_string(*what) : "None"s;
}

struct ProxyRoot {
    DataNode forest;
};
}

PYBIND11_MODULE(oopt_gnpy_libyang, m) {
    m.doc() = "Opinionated Python bindings for the libyang library";

    py::enum_<ContextOptions>(m, "ContextOptions")
        .value("AllImplemented", ContextOptions::AllImplemented)
        .value("RefImplemented", ContextOptions::RefImplemented)
        .value("NoYangLibrary", ContextOptions::NoYangLibrary)
        .value("DisableSearchDirs", ContextOptions::DisableSearchDirs)
        .value("DisableSearchCwd", ContextOptions::DisableSearchCwd)
        .value("PreferSearchDirs", ContextOptions::PreferSearchDirs)
        .value("SetPrivParsed", ContextOptions::SetPrivParsed)
        .value("ExplicitCompile", ContextOptions::ExplicitCompile)
        .def("__or__", [](ContextOptions a, ContextOptions b){ return a | b; })
        ;

    py::enum_<LogLevel>(m, "LogLevel")
        .value("Error", LogLevel::Error)
        .value("Warning", LogLevel::Warning)
        .value("Verbose", LogLevel::Verbose)
        .value("Debug", LogLevel::Debug)
        ;

    py::enum_<LogOptions>(m, "LogOptions")
        .value("Log", LogOptions::Log)
        .value("Store", LogOptions::Store)
        .value("StoreLast", LogOptions::StoreLast)
        .def("__or__", [](LogOptions a, LogOptions b){ return a | b; })
        ;

    py::enum_<ErrorCode>(m, "ErrorCode")
        .value("Success", ErrorCode::Success)
        .value("MemoryFailure", ErrorCode::MemoryFailure)
        .value("SyscallFail", ErrorCode::SyscallFail)
        .value("InvalidValue", ErrorCode::InvalidValue)
        .value("ItemAlreadyExists", ErrorCode::ItemAlreadyExists)
        .value("NotFound", ErrorCode::NotFound)
        .value("InternalError", ErrorCode::InternalError)
        .value("ValidationFailure", ErrorCode::ValidationFailure)
        .value("OperationDenied", ErrorCode::OperationDenied)
        .value("OperationIncomplete", ErrorCode::OperationIncomplete)
        .value("RecompileRequired", ErrorCode::RecompileRequired)
        .value("Negative", ErrorCode::Negative)
        .value("Unknown", ErrorCode::Unknown)
        .value("PluginError", ErrorCode::PluginError)
        ;

    py::enum_<ValidationErrorCode>(m, "ValidationErrorCode")
        .value("Success", ValidationErrorCode::Success)
        .value("Syntax", ValidationErrorCode::Syntax)
        .value("YangSyntax", ValidationErrorCode::YangSyntax)
        .value("YinSyntax", ValidationErrorCode::YinSyntax)
        .value("Reference", ValidationErrorCode::Reference)
        .value("Xpath", ValidationErrorCode::Xpath)
        .value("Semantics", ValidationErrorCode::Semantics)
        .value("XmlSyntax", ValidationErrorCode::XmlSyntax)
        .value("JsonSyntax", ValidationErrorCode::JsonSyntax)
        .value("Data", ValidationErrorCode::Data)
        .value("Other", ValidationErrorCode::Other)
        ;

    py::enum_<DataFormat>(m, "DataFormat")
        .value("Detect", DataFormat::Detect)
        .value("JSON", DataFormat::JSON)
        .value("XML", DataFormat::XML)
        ;

    py::enum_<ParseOptions>(m, "ParseOptions")
        .value("ParseOnly", ParseOptions::ParseOnly)
        .value("Strict", ParseOptions::Strict)
        .value("Opaque", ParseOptions::Opaque)
        .value("NoState", ParseOptions::NoState)
        .value("LybModUpdate", ParseOptions::LybModUpdate)
        .value("Ordered", ParseOptions::Ordered)
        .value("Subtree", ParseOptions::Subtree)
        .value("WhenTrue", ParseOptions::WhenTrue)
        .value("NoNew", ParseOptions::NoNew)
        .def("__or__", [](ParseOptions a, ParseOptions b){ return a | b; })
        ;

    py::enum_<ValidationOptions>(m, "ValidationOptions")
        .value("NoState", ValidationOptions::NoState)
        .value("Present", ValidationOptions::Present)
        .value("MultiError", ValidationOptions::MultiError)
        .value("Operational", ValidationOptions::Operational)
        .value("NoDefaults", ValidationOptions::NoDefaults)
        .def("__or__", [](ValidationOptions a, ValidationOptions b){ return a | b; })
        ;

    py::enum_<PrintFlags>(m, "PrintFlags")
        .value("WithDefaultsExplicit", PrintFlags::WithDefaultsExplicit)
        .value("WithSiblings", PrintFlags::WithSiblings)
        .value("Shrink", PrintFlags::Shrink)
        .value("KeepEmptyCont", PrintFlags::KeepEmptyCont)
        .value("WithDefaultsTrim", PrintFlags::WithDefaultsTrim)
        .value("WithDefaultsAll", PrintFlags::WithDefaultsAll)
        .value("WithDefaultsAllTag", PrintFlags::WithDefaultsAllTag)
        .value("WithDefaultsImplicitTag", PrintFlags::WithDefaultsImplicitTag)
        .value("WithDefaultsMask", PrintFlags::WithDefaultsMask)
        .def("__or__", [](PrintFlags a, PrintFlags b){ return a | b; })
        ;

    py::enum_<SchemaFormat>(m, "SchemaFormat")
        .value("YANG", SchemaFormat::YANG)
        .value("YIN", SchemaFormat::YIN)
        ;

    auto eError = py::register_local_exception<Error>(m, "Error", PyExc_RuntimeError);
    py::register_local_exception<ErrorWithCode>(m, "ErrorWithCode", eError);
        /* FIXME: cannot do .def_property("code", &ErrorWithCode::code, nullptr) */
    py::register_local_exception<ParsedInfoUnavailable>(m, "ParsedInfoUnavailable", eError);

    py::class_<Feature>(m, "Feature")
        .def_property("name", &Feature::name, nullptr)
        .def_property("is_enabled", &Feature::isEnabled, nullptr)
        ;

    py::class_<Module>(m, "Module")
        .def_property("name", &Module::name, nullptr)
        .def_property("revision", &Module::revision, nullptr)
        .def_property("implemented", &Module::implemented, nullptr)
        .def_property("features", &Module::features, nullptr)
        .def("set_implemented", py::overload_cast<>(&Module::setImplemented))
        .def("feature_enabled", &Module::featureEnabled)
        .def("set_implemented_with_features", py::overload_cast<std::vector<std::string>>(&Module::setImplemented), "features"_a)
        .def("set_implemented_all_features", [](Module& mod) { mod.setImplemented(AllFeatures{}); })
        // FIXME: identities
        // FIXME: childInstantiables
        ;

    py::class_<ErrorInfo>(m, "ErrorInfo")
        .def_readonly("app_tag", &ErrorInfo::appTag)
        .def_readonly("level", &ErrorInfo::level)
        .def_readonly("message", &ErrorInfo::message)
        .def_readonly("code", &ErrorInfo::code)
        .def_readonly("path", &ErrorInfo::path)
        .def_readonly("validation_code", &ErrorInfo::validationCode)
        .def("__repr__", [](const ErrorInfo& e) {
                    std::ostringstream ss;
                    ss << "ErrorInfo(";
                    ss << "level = " << e.level;
                    ss << ", code = " << e.code;
                    ss << ", validation_code = " << e.validationCode;
                    ss << ", message = " << repr_optional_string(e.message);
                    ss << ", path = " << repr_optional_string(e.path);
                    ss << ", app_tag = " << repr_optional_string(e.appTag);
                    ss << ")";
                    return ss.str();
                })
        ;

    py::class_<DataNode>(m, "DataNode")
        .def_property("path", &DataNode::path, nullptr)
        .def_property("is_term", &DataNode::isTerm, nullptr)
        .def_property("schema", &DataNode::schema, nullptr)
        .def("as_term", &DataNode::asTerm)
        .def("print", &DataNode::printStr, "format"_a, "flags"_a)
        .def("siblings", &DataNode::siblings)
        .def("child", &DataNode::child)
        .def("children_dfs", &DataNode::childrenDfs)
        .def("immediate_children", &DataNode::immediateChildren)
        .def("__getitem__",
                [](const DataNode& node, const std::string& key) {
                    auto set = node.findXPath(key);
                    if (set.empty()) {
                        throw py::key_error{"No such data node below '" + node.path() + "' for '" + key + "'"};
                    } else if (set.size() > 1) {
                        throw py::key_error{"Too many nodes below '" + node.path() + "' for '" + key + "'"};
                    } else {
                        return *set.begin();
                    }
                })
        .def("__contains__",
                [](const DataNode& node, const std::string& key) {
                    return node.findXPath(key).size() == 1;
                })
        .def("find",
                [](const DataNode& node, const std::string& key) {
                    return node.findXPath(key);
                })
        .def("__setitem__",
                [](DataNode& node, const std::string& key, const std::optional<std::string>& value) {
                return node.newPath2(key, value, CreationOptions::Update).createdNode;
                })
        ;

    py::class_<DataNodeTerm, DataNode>(m, "DataNodeTerm")
        .def_property("is_default_value", &DataNodeTerm::isDefaultValue, nullptr)
        .def_property("value", &DataNodeTerm::value, nullptr)
        .def("__str__", &DataNodeTerm::valueStr)
        ;

    using Collection_DataNode_Siblings = Collection<DataNode, IterationType::Sibling>;
    py::class_<Collection_DataNode_Siblings>(m, "_Collection_DataNode_Siblings")
        .def("__iter__",
            [](const Collection_DataNode_Siblings &s) { return py::make_iterator(s.begin(), s.end()); },
            py::keep_alive<0, 1>())
        ;

    using Collection_DataNode_Dfs = Collection<DataNode, IterationType::Dfs>;
    py::class_<Collection_DataNode_Dfs>(m, "_Collection_DataNode_Dfs")
        .def("__iter__",
            [](const Collection_DataNode_Dfs &s) { return py::make_iterator(s.begin(), s.end()); },
            py::keep_alive<0, 1>())
        ;

    using Set_DataNode = Set<DataNode>;
    py::class_<Set_DataNode>(m, "_Set_DataNode")
        .def("__iter__",
            [](const Set_DataNode &s) { return py::make_iterator(s.begin(), s.end()); },
            py::keep_alive<0, 1>())
        .def("__len__", &Set_DataNode::size)
        ;

    py::class_<ProxyRoot>(m, "search_at_root")
        .def(py::init<const DataNode&>(), "forest"_a)
        .def("__call__",
                [](const ProxyRoot& proxy, const std::string& xpath) {
                    return findXPathAt(std::nullopt, proxy.forest, xpath);
                }, "xpath"_a)
        ;

    py::class_<Decimal64>(m, "Decimal64")
        .def("__str__", [](const Decimal64& num) { return std::string{num}; })
        .def("__float__", [](const Decimal64& num) { return double{num}; })
        ;

    py::class_<Empty>(m, "Empty");

    py::class_<Enum>(m, "Enum")
        .def_readonly("name", &Enum::name)
        .def_readonly("value", &Enum::value)
        ;

    py::class_<SchemaNode>(m, "SchemaNode")
        .def_property("module", &SchemaNode::module, nullptr)
        .def_property("name", &SchemaNode::name, nullptr)
        .def_property("path", &SchemaNode::path, nullptr)
        .def_property("description", &SchemaNode::description, nullptr)
        ;

    py::class_<Context>(m, "Context")
        .def(py::init<const std::optional<std::string>&, const std::optional<ContextOptions>>(), "searchPath"_a=std::nullopt, "options"_a=std::nullopt)
        .def("load_module", &Context::loadModule, "name"_a, "revision"_a=std::nullopt, "features"_a=std::vector<std::string>{})
        .def("modules", &Context::modules)
        .def("get_module", &Context::getModule, "name"_a, "revision"_a)
        .def("get_module_latest", &Context::getModuleLatest, "name"_a)
        .def("get_module_implemented", &Context::getModuleImplemented, "name"_a)
        .def("errors", &Context::getErrors)
        .def("clean_all_errors", &Context::cleanAllErrors)
        .def("parse_data",
                py::overload_cast<const std::string&, const DataFormat, const std::optional<ParseOptions>,
                    const std::optional<ValidationOptions>>(&Context::parseData, py::const_),
                "data"_a, "format"_a, "parse_options"_a=std::nullopt, "validation_options"_a=std::nullopt)
        .def("parse_data",
                py::overload_cast<const std::filesystem::path&, const DataFormat, const std::optional<ParseOptions>,
                    const std::optional<ValidationOptions>>(&Context::parseData, py::const_),
                "path"_a, "format"_a, "parse_options"_a=std::nullopt, "validation_options"_a=std::nullopt)
        .def("parse_module",
                py::overload_cast<const std::string&, const SchemaFormat, const std::vector<std::string>&>(&Context::parseModule, py::const_),
                "data"_a, "format"_a, "features"_a=std::vector<std::string>{})
        .def("parse_module",
                py::overload_cast<const std::filesystem::path&, const SchemaFormat, const std::vector<std::string>&>(&Context::parseModule, py::const_),
                "path"_a, "format"_a, "features"_a=std::vector<std::string>{})
        .def("create",
                [](const Context& ctx, const std::string& path, const std::optional<std::string>& value) {
                    auto created = ctx.newPath2(path, value);
                    return created.createdNode;
                },
                "path"_a, "value"_a=std::nullopt)
        ;

    m.def("libyang_version", []() { return LY_VERSION; });
    m.def("libyang_version_info", []() { return py::make_tuple(LY_VERSION_MAJOR, LY_VERSION_MINOR, LY_VERSION_MICRO); });

    m.def("set_log_level", &setLogLevel, "level"_a);
    m.def("set_log_options", &setLogOptions, "options"_a);
}
