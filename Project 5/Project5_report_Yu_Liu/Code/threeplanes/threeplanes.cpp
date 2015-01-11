#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty2D.h>
#include <vtkActor.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkPlane.h>
#include <vtkProbeFilter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSliderWidget.h>
#include <vtkProperty.h>
#include <vtkCutter.h>
#include <vtkGlyph3D.h>
#include <vtkArrowSource.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridOutlineFilter.h>

class vtkSliderCallback : public vtkCommand
{
public:
	static vtkSliderCallback *New() 
	{
		return new vtkSliderCallback;
	}
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		vtkSliderWidget *sliderWidget = 
			reinterpret_cast<vtkSliderWidget*>(caller);
		double value = static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue();
		double center[3];
		center[0] = value;
		center[1] = 0.0;
		center[2] = 0.0;
		plane->SetOrigin(center);
		plane->SetNormal(1,0,0);
	}
	vtkSliderCallback():plane(0) {}
	vtkPlane* plane;
};

int main (int argc, char* argv[])
{
	vtkSmartPointer<vtkDataSetReader> Delta_Wing_Geometry_Reader = vtkSmartPointer<vtkDataSetReader>::New();
	vtkSmartPointer<vtkDataSetReader> VectorReader = vtkSmartPointer<vtkDataSetReader>::New();

	if(argc ==3)
	{
		Delta_Wing_Geometry_Reader->SetFileName(argv[1]);
		VectorReader->SetFileName(argv[2]);
	}
	else 
		exit(0);

	vtkSmartPointer<vtkDataSetMapper>Delta_Wing_Geometry_Mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	Delta_Wing_Geometry_Mapper->SetInputConnection(Delta_Wing_Geometry_Reader->GetOutputPort());
	
	vtkSmartPointer<vtkActor> Delta_Wing_Geometry_Actor = vtkSmartPointer<vtkActor>::New();
	Delta_Wing_Geometry_Actor->SetMapper(Delta_Wing_Geometry_Mapper);
	Delta_Wing_Geometry_Actor->GetProperty()->SetColor(0.5, 0.5, 1.0);

	VectorReader->Update();

	double range[2]; 
	VectorReader->GetOutput()->GetPointData()->GetVectors()->GetRange(range); //get the vector value range 
	cout<<range[0]<<" "<<range[1]<<endl; //range[0]: -0.362368, range[1]: 152.99

	vtkSmartPointer<vtkColorTransferFunction> Color =  vtkSmartPointer<vtkColorTransferFunction>::New();
	Color->AddRGBPoint(30, 0.0, 0.0, 1.0);
	Color->AddRGBPoint(60, 0.0, 1.0, 1.0);
	Color->AddRGBPoint(90, 0.0, 1.0, 0.0);
	Color->AddRGBPoint(120, 1.0, 1.0, 0.0);
	Color->AddRGBPoint(150, 1.0, 0.0, 0.0);

	vtkSmartPointer<vtkPlane>plane = vtkSmartPointer<vtkPlane>::New();
	double center[3];
	center[0] = 140.0;
	center[1] = 0.0;
	center[2] = 0.0;
	plane->SetOrigin(center);
	plane->SetNormal(1,0,0);

	vtkSmartPointer<vtkCutter>planeCut = vtkSmartPointer<vtkCutter>::New();
	planeCut->SetInputConnection(VectorReader->GetOutputPort());
	planeCut->SetCutFunction(plane);

	vtkSmartPointer<vtkProbeFilter>probe = vtkSmartPointer<vtkProbeFilter>::New();
	probe->SetInputConnection(planeCut->GetOutputPort());
	probe->SetSourceConnection(VectorReader->GetOutputPort());

	vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();
	vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
	glyph->SetInputConnection(probe->GetOutputPort());
	glyph->SetSourceConnection(arrow->GetOutputPort());
	glyph->ScalingOn();
	glyph->SetScaleFactor(0.05);
	glyph->SetVectorModeToUseVector();
	glyph->SetScaleModeToScaleByVector();
	glyph->SetColorModeToColorByVector();
	glyph->OrientOn();

	vtkSmartPointer<vtkDataSetMapper> glyphMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	glyphMapper->SetLookupTable(Color);
	glyphMapper->SetInputConnection(glyph->GetOutputPort());

	vtkSmartPointer<vtkActor> cutActor = vtkSmartPointer<vtkActor>::New();
	cutActor->SetMapper(glyphMapper);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(Delta_Wing_Geometry_Actor);
	renderer->AddActor(cutActor);

	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(400,300);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
	sliderRep->SetMinimumValue(0.0);
	sliderRep->SetMaximumValue(250.0);
	sliderRep->SetValue(140);
	sliderRep->SetLabelFormat("%-#4.2f");
	sliderRep->GetTubeProperty()->SetColor(0.0, 1.0, 0.0);
	sliderRep->GetTubeProperty()->SetLineWidth(1.0);
	sliderRep->SetTitleText("Plane Position");

	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(.1 ,.1);
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint2Coordinate()->SetValue(.9, .1);

	vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
	callback->plane = plane;

	vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(renderWindowInteractor);
	sliderWidget->SetRepresentation(sliderRep);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();

	sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

	renderWindowInteractor->Initialize();
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}