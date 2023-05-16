#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <libyang-cpp/Context.hpp>

using namespace std::literals;
using namespace pybind11::literals;

PYBIND11_MODULE(oopt_gnpy_libyang, m) {
    m.doc() = "Opinionated Python bindings for the libyang library";

    pybind11::enum_<libyang::ContextOptions>(m, "ContextOptions")
        .value("AllImplemented", libyang::ContextOptions::AllImplemented)
        .value("RefImplemented", libyang::ContextOptions::RefImplemented)
        .value("NoYangLibrary", libyang::ContextOptions::NoYangLibrary)
        .value("DisableSearchDirs", libyang::ContextOptions::DisableSearchDirs)
        .value("DisableSearchCwd", libyang::ContextOptions::DisableSearchCwd)
        .value("PreferSearchDirs", libyang::ContextOptions::PreferSearchDirs)
        .value("SetPrivParsed", libyang::ContextOptions::SetPrivParsed)
        .value("ExplicitCompile", libyang::ContextOptions::ExplicitCompile)
        .def("__or__", [](libyang::ContextOptions a, libyang::ContextOptions b){ return a | b; })
        ;

    pybind11::enum_<libyang::LogLevel>(m, "LogLevel")
        .value("Error", libyang::LogLevel::Error)
        .value("Warning", libyang::LogLevel::Warning)
        .value("Verbose", libyang::LogLevel::Verbose)
        .value("Debug", libyang::LogLevel::Debug)
        ;

    pybind11::enum_<libyang::ErrorCode>(m, "ErrorCode")
        .value("Success", libyang::ErrorCode::Success)
        .value("MemoryFailure", libyang::ErrorCode::MemoryFailure)
        .value("SyscallFail", libyang::ErrorCode::SyscallFail)
        .value("InvalidValue", libyang::ErrorCode::InvalidValue)
        .value("ItemAlreadyExists", libyang::ErrorCode::ItemAlreadyExists)
        .value("NotFound", libyang::ErrorCode::NotFound)
        .value("InternalError", libyang::ErrorCode::InternalError)
        .value("ValidationFailure", libyang::ErrorCode::ValidationFailure)
        .value("OperationDenied", libyang::ErrorCode::OperationDenied)
        .value("OperationIncomplete", libyang::ErrorCode::OperationIncomplete)
        .value("RecompileRequired", libyang::ErrorCode::RecompileRequired)
        .value("Negative", libyang::ErrorCode::Negative)
        .value("Unknown", libyang::ErrorCode::Unknown)
        .value("PluginError", libyang::ErrorCode::PluginError)
        ;

    pybind11::enum_<libyang::ValidationErrorCode>(m, "ValidationErrorCode")
        .value("Success", libyang::ValidationErrorCode::Success)
        .value("Syntax", libyang::ValidationErrorCode::Syntax)
        .value("YangSyntax", libyang::ValidationErrorCode::YangSyntax)
        .value("YinSyntax", libyang::ValidationErrorCode::YinSyntax)
        .value("Reference", libyang::ValidationErrorCode::Reference)
        .value("Xpath", libyang::ValidationErrorCode::Xpath)
        .value("Semantics", libyang::ValidationErrorCode::Semantics)
        .value("XmlSyntax", libyang::ValidationErrorCode::XmlSyntax)
        .value("JsonSyntax", libyang::ValidationErrorCode::JsonSyntax)
        .value("Data", libyang::ValidationErrorCode::Data)
        .value("Other", libyang::ValidationErrorCode::Other)
        ;

    pybind11::enum_<libyang::DataFormat>(m, "DataFormat")
        .value("Detect", libyang::DataFormat::Detect)
        .value("JSON", libyang::DataFormat::JSON)
        .value("XML", libyang::DataFormat::XML)
        ;

    pybind11::enum_<libyang::ParseOptions>(m, "ParseOptions")
        .value("ParseOnly", libyang::ParseOptions::ParseOnly)
        .value("Strict", libyang::ParseOptions::Strict)
        .value("Opaque", libyang::ParseOptions::Opaque)
        .value("NoState", libyang::ParseOptions::NoState)
        .value("LybModUpdate", libyang::ParseOptions::LybModUpdate)
        .value("Ordered", libyang::ParseOptions::Ordered)
        .def("__or__", [](libyang::ParseOptions a, libyang::ParseOptions b){ return a | b; })
        ;

    pybind11::enum_<libyang::ValidationOptions>(m, "ValidationOptions")
        .value("NoState", libyang::ValidationOptions::NoState)
        .value("Present", libyang::ValidationOptions::Present)
        .def("__or__", [](libyang::ValidationOptions a, libyang::ValidationOptions b){ return a | b; })
        ;

    pybind11::enum_<libyang::PrintFlags>(m, "PrintFlags")
        .value("WithDefaultsExplicit", libyang::PrintFlags::WithDefaultsExplicit)
        .value("WithSiblings", libyang::PrintFlags::WithSiblings)
        .value("Shrink", libyang::PrintFlags::Shrink)
        .value("KeepEmptyCont", libyang::PrintFlags::KeepEmptyCont)
        .value("WithDefaultsTrim", libyang::PrintFlags::WithDefaultsTrim)
        .value("WithDefaultsAll", libyang::PrintFlags::WithDefaultsAll)
        .value("WithDefaultsAllTag", libyang::PrintFlags::WithDefaultsAllTag)
        .value("WithDefaultsImplicitTag", libyang::PrintFlags::WithDefaultsImplicitTag)
        .value("WithDefaultsMask", libyang::PrintFlags::WithDefaultsMask)
        .def("__or__", [](libyang::PrintFlags a, libyang::PrintFlags b){ return a | b; })
        ;

    auto eError = pybind11::register_local_exception<libyang::Error>(m, "Error", PyExc_RuntimeError);
    pybind11::register_local_exception<libyang::ErrorWithCode>(m, "ErrorWithCode", eError);
        /* FIXME: cannot do .def_property("code", &libyang::ErrorWithCode::code, nullptr) */
    pybind11::register_local_exception<libyang::ParsedInfoUnavailable>(m, "ParsedInfoUnavailable", eError);

    pybind11::class_<libyang::Feature>(m, "Feature")
        .def_property("name", &libyang::Feature::name, nullptr)
        ;

    pybind11::class_<libyang::Module>(m, "Module")
        .def_property("name", &libyang::Module::name, nullptr)
        .def_property("revision", &libyang::Module::revision, nullptr)
        .def_property("implemented", &libyang::Module::implemented, nullptr)
        .def_property("features", &libyang::Module::features, nullptr)
        .def("set_implemented", static_cast<void (libyang::Module::*)()>(&libyang::Module::setImplemented))
        .def("feature_enabled", &libyang::Module::featureEnabled)
        .def("set_implemented_with_features", static_cast<void (libyang::Module::*)(std::vector<std::string>)>(&libyang::Module::setImplemented), "features"_a)
        .def("set_implemented_all_features", [](libyang::Module& mod) { mod.setImplemented(libyang::AllFeatures{}); })
        // FIXME: identities
        // FIXME: childInstantiables
        ;

    pybind11::class_<libyang::ErrorInfo>(m, "ErrorInfo")
        .def_readonly("app_tag", &libyang::ErrorInfo::appTag)
        .def_readonly("level", &libyang::ErrorInfo::level)
        .def_readonly("message", &libyang::ErrorInfo::message)
        .def_readonly("code", &libyang::ErrorInfo::code)
        .def_readonly("path", &libyang::ErrorInfo::path)
        .def_readonly("validation_code", &libyang::ErrorInfo::validationCode)
        ;

    pybind11::class_<libyang::DataNode>(m, "DataNode")
        .def_property("path", &libyang::DataNode::path, nullptr)
        .def_property("is_term", &libyang::DataNode::isTerm, nullptr)
        .def("as_term", &libyang::DataNode::asTerm)
        .def("print", &libyang::DataNode::printStr, "format"_a, "flags"_a)
        .def("siblings", &libyang::DataNode::siblings)
        .def("child", &libyang::DataNode::child)
        .def("childrenDfs", &libyang::DataNode::childrenDfs)
        .def("immediateChildren", &libyang::DataNode::immediateChildren)
        .def("__getitem__",
                [](const libyang::DataNode& node, const std::string& key) {
                    auto set = node.findXPath(key);
                    if (set.empty()) {
                        throw pybind11::key_error{"No such data node below '" + node.path() + "' for '" + key + "'"};
                    } else if (set.size() > 1) {
                        throw pybind11::key_error{"Too many nodes below '" + node.path() + "' for '" + key + "'"};
                    } else {
                        return *set.begin();
                    }
                })
        ;

    pybind11::class_<libyang::DataNodeTerm, libyang::DataNode>(m, "DataNodeTerm")
        .def_property("is_default_value", &libyang::DataNodeTerm::isDefaultValue, nullptr)
        .def_property("value", &libyang::DataNodeTerm::value, nullptr)
        .def("__str__", &libyang::DataNodeTerm::valueStr)
        ;

    using Collection_DataNode_Siblings = libyang::Collection<libyang::DataNode, libyang::IterationType::Sibling>;
    pybind11::class_<Collection_DataNode_Siblings>(m, "_Collection_DataNode_Siblings")
        .def("__iter__",
            [](const Collection_DataNode_Siblings &s) { return pybind11::make_iterator(s.begin(), s.end()); },
            pybind11::keep_alive<0, 1>())
        ;

    using Collection_DataNode_Dfs = libyang::Collection<libyang::DataNode, libyang::IterationType::Dfs>;
    pybind11::class_<Collection_DataNode_Dfs>(m, "_Collection_DataNode_Dfs")
        .def("__iter__",
            [](const Collection_DataNode_Dfs &s) { return pybind11::make_iterator(s.begin(), s.end()); },
            pybind11::keep_alive<0, 1>())
        ;

    pybind11::class_<libyang::Context>(m, "Context")
        .def(pybind11::init<const std::optional<std::string>&, const std::optional<libyang::ContextOptions>>(), "searchPath"_a=std::nullopt, "options"_a=std::nullopt)
        .def("load_module", &libyang::Context::loadModule, "name"_a, "revision"_a=std::nullopt, "features"_a=std::vector<std::string>{})
        .def("modules", &libyang::Context::modules)
        .def("get_module", &libyang::Context::getModule, "name"_a, "revision"_a=std::nullopt)
        .def("get_module_implemented", &libyang::Context::getModuleImplemented, "name"_a)
        .def("errors", &libyang::Context::getErrors)
        .def("clean_all_errors", &libyang::Context::cleanAllErrors)
        .def("parse_data",
                pybind11::overload_cast<const std::string&, const libyang::DataFormat, const std::optional<libyang::ParseOptions>,
                    const std::optional<libyang::ValidationOptions>>(&libyang::Context::parseData, pybind11::const_),
                "data"_a, "format"_a, "parse_options"_a=std::nullopt, "validation_options"_a=std::nullopt)
        .def("parse_data",
                pybind11::overload_cast<const std::filesystem::path&, const libyang::DataFormat, const std::optional<libyang::ParseOptions>,
                    const std::optional<libyang::ValidationOptions>>(&libyang::Context::parseData, pybind11::const_),
                "path"_a, "format"_a, "parse_options"_a=std::nullopt, "validation_options"_a=std::nullopt)

        // is this actually needed? looks like parseDataMem() does that just fine
        /* .def("validate_data_str", */
        /*         [](const libyang::Context& ctx, const std::string& data, const libyang::DataFormat format, const libyang::ParseOptions parseOptions, const libyang::ValidationOptions validationOptions) { */
        /*             auto x = ctx.parseData(data, format, parseOptions, validationOptions); */
        /*             libyang::validateAll(x, validationOptions); */
        /*             return x; */
        /*         }, */
        /*         "data"_a, "format"_a=libyang::DataFormat::Auto, "parse_options"_a=std::nullopt, "validation_options"_a=std::nullopt) */
        ;
}
