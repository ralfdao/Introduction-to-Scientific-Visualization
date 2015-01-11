#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkWarpScalar.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty2D.h>
#include <vtkActor.h>
#include <vtkWarpScalar.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>

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
    int value = static_cast<int>(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
	this->Scalar->SetScaleFactor(value*2161.0/40030000);
    }
  vtkSliderCallback():Scalar(0) {}
  vtkWarpScalar* Scalar;
};

int main (int argc, char* argv[])
{
	vtkSmartPointer<vtkDataSetReader> Reader = vtkSmartPointer<vtkDataSetReader>::New();
	if(argc ==2)
		Reader->SetFileName(argv[1]);
	else 
		exit(0);

	vtkSmartPointer<vtkImageDataGeometryFilter> geometry = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
	geometry->SetInput(Reader->GetOutput());

	vtkSmartPointer<vtkWarpScalar> scalar = vtkSmartPointer<vtkWarpScalar>::New();
	scalar->SetInput(geometry->GetOutput());
	scalar->SetScaleFactor(25*2161.0/40030000);

	vtkSmartPointer<vtkColorTransferFunction> Color =  vtkSmartPointer<vtkColorTransferFunction>::New();
	Color->AddRGBPoint(-11001,    0.0, 0.0, 0.0); 
	Color->AddRGBPoint(-11000,    1.0/255, 6.0/255, 27.0/255);  
	Color->AddRGBPoint(-5501, 1.0/255, 6.0/255, 27.0/255);
	Color->AddRGBPoint(-5500,    2.0/255, 15.0/255, 36.0/255);  
	Color->AddRGBPoint(-4501, 2.0/255, 15.0/255, 36.0/255);
	Color->AddRGBPoint(-4500,    0.0/255, 42.0/255, 73.0/255);  
	Color->AddRGBPoint(-3751, 0.0/255, 42.0/255, 73.0/255);
	Color->AddRGBPoint(-3750,    0.0/255, 52.0/255, 87.0/255);  
	Color->AddRGBPoint(-2401, 0.0/255, 52.0/255, 87.0/255);
	Color->AddRGBPoint(-2400,    0.0/255, 66.0/255, 102.0/255);  
	Color->AddRGBPoint(-851, 0.0/255, 66.0/255, 102.0/255);
	Color->AddRGBPoint(-850,    0.0/255, 76.0/255, 116.0/255);  
	Color->AddRGBPoint(-1, 0.0/255, 76.0/255, 116.0/255);
	Color->AddRGBPoint(0,    23.0/255, 36.0/255, 8.0/255);  
	Color->AddRGBPoint(100, 23.0/255, 36.0/255, 8.0/255);
	Color->AddRGBPoint(101, 50.0/255, 58.0/255, 21.0/255);
	Color->AddRGBPoint(250, 50.0/255, 58.0/255, 21.0/255);
	Color->AddRGBPoint(251, 99.0/255, 86.0/255, 54.0/255);
	Color->AddRGBPoint(500, 99.0/255, 86.0/255, 54.0/255);
	Color->AddRGBPoint(501, 165.0/255, 150.0/255, 117.0/255);
	Color->AddRGBPoint(2000, 165.0/255, 150.0/255, 117.0/255);
	Color->AddRGBPoint(2001, 249.0/255, 251.0/255, 246.0/255);
	Color->AddRGBPoint(9000, 249.0/255, 251.0/255, 246.0/255);
	Color->AddRGBPoint(9001, 1.0, 1.0, 1.0);

	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetLookupTable(Color);
	mapper->SetInput(scalar->GetOutput());
	mapper->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(actor);
	renderer->ResetCamera();
	renderer->SetBackground(0.1, 0.2, 0.4);
	renderer->ResetCameraClippingRange();

	renderWindow->SetSize(2161,1080);
	vtkCamera* cam1 = renderer->GetActiveCamera();
	cam1->Zoom(2.4);

	vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
	sliderRep->SetMinimumValue(0.0);
	sliderRep->SetMaximumValue(50.0);
	sliderRep->SetValue(25);
	sliderRep->GetTubeProperty()->SetColor(0.0, 1.0, 0.0);
	sliderRep->GetTubeProperty()->SetLineWidth(1.0);
	sliderRep->SetTitleText("Bathymetry scaling factor");

	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(.1 ,.1);
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint2Coordinate()->SetValue(.3, .1);

	vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
	callback->Scalar = scalar;

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