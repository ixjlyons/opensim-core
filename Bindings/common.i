
%feature("director") OpenSim::AnalysisWrapper;
%feature("director") OpenSim::SimtkLogCallback;
%feature("notabstract") ControlLinear;

%rename(OpenSimObject) OpenSim::Object;
%rename(OpenSimException) OpenSim::Exception;

/* rest of header files to be wrapped */
%include <OpenSim/version.h>

// osimCommon Library
%include <OpenSim/Common/osimCommonDLL.h>
%include <OpenSim/Common/Exception.h>
%include <OpenSim/Common/Array.h>
%include <OpenSim/Common/ArrayPtrs.h>
%include <OpenSim/Common/AbstractProperty.h>
%include <OpenSim/Common/Property.h>
%include <OpenSim/Common/PropertyGroup.h>
%template(ArrayPtrsPropertyGroup) OpenSim::ArrayPtrs<OpenSim::PropertyGroup>;
%template (PropertyString) OpenSim::Property<std::string>;
%include <OpenSim/Common/Object.h>
%include <OpenSim/Common/ObjectGroup.h>

%include <OpenSim/Common/Set.h>
%include <OpenSim/Common/StateVector.h>
%template(ArrayStateVector) OpenSim::Array<OpenSim::StateVector>;
%include <OpenSim/Common/StorageInterface.h>
%include <OpenSim/Common/Storage.h>
%template(ArrayStorage) OpenSim::ArrayPtrs<OpenSim::Storage>;
%include <OpenSim/Common/Units.h>
%include <OpenSim/Common/IO.h>
%include <OpenSim/Common/Function.h>

%template(SetFunctions) OpenSim::Set<OpenSim::Function>;
%include <OpenSim/Common/FunctionSet.h>

%include <OpenSim/Common/Constant.h>
%include <OpenSim/Common/SimmSpline.h>
%include <OpenSim/Common/StepFunction.h>
%include <OpenSim/Common/PiecewiseConstantFunction.h>
%include <OpenSim/Common/LinearFunction.h>
%include <OpenSim/Common/PiecewiseLinearFunction.h>
%include <OpenSim/Common/MultiplierFunction.h>
%include <OpenSim/Common/GCVSpline.h>
%include <OpenSim/Common/Sine.h>
%include <OpenSim/Common/PolynomialFunction.h>

%include <OpenSim/Common/SmoothSegmentedFunctionFactory.h>
%include <OpenSim/Common/SmoothSegmentedFunction.h>

%include <OpenSim/Common/XYFunctionInterface.h>
%template(ArrayXYPoint) OpenSim::Array<XYPoint>;
%template(ArrayBool) OpenSim::Array<bool>;
%template(ArrayDouble) OpenSim::Array<double>;
%template(ArrayInt) OpenSim::Array<int>;
%template(ArrayStr) OpenSim::Array<std::string>;
%template(ArrayObjPtr) OpenSim::Array<OpenSim::Object*>;
%template(ArrayPtrsObj) OpenSim::ArrayPtrs<OpenSim::Object>;
%template(ArrayConstObjPtr) OpenSim::Array<const OpenSim::Object*>;
%template(ArrayPtrsConstObj) OpenSim::ArrayPtrs<const OpenSim::Object>;

// Used in Component::generateDecorations.
%include <OpenSim/Common/ModelDisplayHints.h>

namespace OpenSim {
    %ignore Output::downcast(AbstractOutput&); // suppress warning 509.
}
// TODO I'm having trouble with the nested type inside a template class.
// TODO %template(OutputChannelDouble) OpenSim::Output<double>::Channel;
// TODO %template(OutputChannelVec3) OpenSim::Output<SimTK::Vec3>::Channel;
// TODO %template(OutputChannelTransform) OpenSim::Output<SimTK::Transform>::Channel;
// TODO %template(OutputChannelVector) OpenSim::Output<SimTK::Vector>::Channel;
%include <OpenSim/Common/ComponentOutput.h>
%template(OutputDouble) OpenSim::Output<double>;
%template(OutputVec3) OpenSim::Output<SimTK::Vec3>;
%template(OutputTransform) OpenSim::Output<SimTK::Transform>;
%template(OutputVector) OpenSim::Output<SimTK::Vector>;


namespace OpenSim {
    %ignore Input::downcast(AbstractInput&); // suppress warning 509.
}
%include <OpenSim/Common/ComponentConnector.h>
%template(InputDouble) OpenSim::Input<double>;
%template(InputVec3) OpenSim::Input<SimTK::Vec3>;
// TODO These classes had issues from SimTK typedefs:
// TODO %template(InputTransform) OpenSim::Input<SimTK::Transform>;
// TODO %template(InputVector) OpenSim::Input<SimTK::Vector>;

namespace OpenSim {
    %ignore ComponentListIterator::operator++; // ignore warning 383.
}
%include <OpenSim/Common/ComponentList.h>

// Can't wrap the return type of this function.
%ignore OpenSim::Component::getOutputs;

%include <OpenSim/Common/Component.h>

%template(ComponentsList) OpenSim::ComponentList<const OpenSim::Component>;
%template(ComponentIterator) OpenSim::ComponentListIterator<const OpenSim::Component>;
%template(getComponentsList) OpenSim::Component::getComponentList<OpenSim::Component>;


%include <OpenSim/Common/Scale.h>
%template(SetScales) OpenSim::Set<OpenSim::Scale>;
%include <OpenSim/Common/ScaleSet.h>
%include <OpenSim/Common/MarkerFrame.h>
%include <OpenSim/Common/MarkerData.h>

namespace OpenSim {
    %ignore DataTable_::DataTable_(DataTable_ &&);
    %ignore TimeSeriesTable_::TimeSeriesTable_(TimeSeriesTable_ &&);
}
%shared_ptr(OpenSim::AbstractDataTable);
%shared_ptr(OpenSim::DataTable_<double, double>);
%shared_ptr(OpenSim::DataTable_<double, SimTK::Vec3>);
%shared_ptr(OpenSim::TimeSeriesTable_<double>);
%shared_ptr(OpenSim::TimeSeriesTable_<SimTK::Vec3>);
%ignore OpenSim::AbstractDataTable::clone;
%ignore OpenSim::AbstractDataTable::getTableMetaData;
%ignore OpenSim::AbstractDataTable::updTableMetaData;
%ignore OpenSim::AbstractDataTable::getIndependentMetaData;
%ignore OpenSim::AbstractDataTable::setIndependentMetaData;
%ignore OpenSim::AbstractDataTable::getDependentsMetaData;
%ignore OpenSim::AbstractDataTable::setDependentsMetaData;
%ignore OpenSim::AbstractDataTable::setColumnLabels(
                                     const std::initializer_list<std::string>&);
%extend OpenSim::AbstractDataTable {
    void setColumnLabels(const std::vector<std::string>& columnLabels) {
        $self->setColumnLabels(columnLabels);
    }
}
%include <OpenSim/Common/AbstractDataTable.h>
%include <OpenSim/Common/DataTable.h>
%include <OpenSim/Common/TimeSeriesTable.h>
%template(DataTable)           OpenSim::DataTable_<double, double>;
%template(DataTableVec3)       OpenSim::DataTable_<double, SimTK::Vec3>;
%template(TimeSeriesTable)     OpenSim::TimeSeriesTable_<double>;
%template(TimeSeriesTableVec3) OpenSim::TimeSeriesTable_<SimTK::Vec3>;

%include <OpenSim/Common/Event.h>
%template(StdVectorEvent) std::vector<OpenSim::Event>;
%template(StdMapStringTimeSeriesTableVec3)
        std::map<std::string, 
                 std::shared_ptr<OpenSim::TimeSeriesTable_<SimTK::Vec3>>>;
%shared_ptr(OpenSim::DataAdapter)
%shared_ptr(OpenSim::FileAdapter)
%shared_ptr(OpenSim::DelimFileAdapter)
%shared_ptr(OpenSim::STOFileAdapter)
%shared_ptr(OpenSim::CSVFileAdapter)
%shared_ptr(OpenSim::TRCFileAdapter)
%shared_ptr(OpenSim::C3DFileAdapter)
%template(StdMapStringDataAdapter)
        std::map<std::string, std::shared_ptr<OpenSim::DataAdapter>>;
%template(StdMapStringAbstractDataTable)
        std::map<std::string, std::shared_ptr<OpenSim::AbstractDataTable>>;
%include <OpenSim/Common/DataAdapter.h>
%include <OpenSim/Common/FileAdapter.h>
namespace OpenSim {
    %ignore TRCFileAdapter::TRCFileAdapter(TRCFileAdapter &&);
    %ignore DelimFileAdapter::DelimFileAdapter(DelimFileAdapter &&);
    %ignore STOFileAdapter::STOFileAdapter(STOFileAdapter &&);
    %ignore CSVFileAdapter::CSVFileAdapter(CSVFileAdapter &&);
}
%include <OpenSim/Common/TRCFileAdapter.h>
%include <OpenSim/Common/DelimFileAdapter.h>
%include <OpenSim/Common/STOFileAdapter.h>
%include <OpenSim/Common/CSVFileAdapter.h>
%include <OpenSim/Common/C3DFileAdapter.h>

namespace OpenSim {
    %ignore TableSource_::TableSource_(TableSource_ &&);
}
%include <OpenSim/Common/TableSource.h>
%template(TableSource) OpenSim::TableSource_<SimTK::Real>;
%template(TableSourceVec3) OpenSim::TableSource_<SimTK::Vec3>;

%include <OpenSim/Common/Reporter.h>
%template(ReporterDouble) OpenSim::Reporter<SimTK::Real>;
%template(ReporterVec3) OpenSim::Reporter<SimTK::Vec3>;
%template(ReporterVector) OpenSim::Reporter<SimTK::Vector>;
%template(TableReporter) OpenSim::TableReporter_<SimTK::Real>;
%template(TableReporterVec3) OpenSim::TableReporter_<SimTK::Vec3>;
%template(TableReporterVector) OpenSim::TableReporter_<SimTK::Vector, SimTK::Real>;
%template(ConsoleReporter) OpenSim::ConsoleReporter_<SimTK::Real>;
%template(ConsoleReporterVec3) OpenSim::ConsoleReporter_<SimTK::Vec3>;
