#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <libyang-cpp/Context.hpp>

using namespace std::literals;
using namespace pybind11::literals;
using namespace libyang;
namespace py = pybind11;

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
        .def("__or__", [](ParseOptions a, ParseOptions b){ return a | b; })
        ;

    py::enum_<ValidationOptions>(m, "ValidationOptions")
        .value("NoState", ValidationOptions::NoState)
        .value("Present", ValidationOptions::Present)
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
        ;

    py::class_<DataNode>(m, "DataNode")
        .def_property("path", &DataNode::path, nullptr)
        .def_property("is_term", &DataNode::isTerm, nullptr)
        .def("as_term", &DataNode::asTerm)
        .def("print", &DataNode::printStr, "format"_a, "flags"_a)
        .def("siblings", &DataNode::siblings)
        .def("child", &DataNode::child)
        .def("childrenDfs", &DataNode::childrenDfs)
        .def("immediateChildren", &DataNode::immediateChildren)
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

    py::class_<Context>(m, "Context")
        .def(py::init<const std::optional<std::string>&, const std::optional<ContextOptions>>(), "searchPath"_a=std::nullopt, "options"_a=std::nullopt)
        .def("load_module", &Context::loadModule, "name"_a, "revision"_a=std::nullopt, "features"_a=std::vector<std::string>{})
        .def("modules", &Context::modules)
        .def("get_module", &Context::getModule, "name"_a, "revision"_a=std::nullopt)
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
                py::overload_cast<const std::string&, const SchemaFormat>(&Context::parseModule, py::const_),
                "data"_a, "format"_a)
        .def("parse_module",
                py::overload_cast<const std::filesystem::path&, const SchemaFormat>(&Context::parseModule, py::const_),
                "path"_a, "format"_a)
        ;
}
