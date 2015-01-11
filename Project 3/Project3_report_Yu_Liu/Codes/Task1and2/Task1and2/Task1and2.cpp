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
#include <vtkContourFilter.h>
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
	this->cf->SetValue(0, value);
    }
  vtkSliderCallback():cf(0) {}
  vtkContourFilter* cf;
};

int main (int argc, char* argv[])
{
	vtkSmartPointer<vtkDataSetReader> Reader = vtkSmartPointer<vtkDataSetReader>::New();
	if(argc ==2)
		Reader->SetFileName(argv[1]);	
	else 
		exit(0);

	vtkSmartPointer<vtkColorTransferFunction> Color =  vtkSmartPointer<vtkColorTransferFunction>::New();
	Color->AddRGBPoint(800, 241.0/255, 162.0/255, 123.0/255, 0.0, 1.0);
	Color->AddRGBPoint(1045, 245.0/255, 90.0/255, 71.0/255, 0.0, 1.0);
	Color->AddRGBPoint(1180, 255.0/255, 246.0/255, 251.0/255, 0.0, 1.0);

	vtkSmartPointer<vtkContourFilter> cf = vtkSmartPointer<vtkContourFilter>::New();
	cf->SetInput(Reader->GetStructuredPointsOutput()); 
	cf->SetValue(0, 1700);
	cf->Update();

	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetLookupTable(Color);
	mapper->SetInput(cf->GetOutput());
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
	renderer->ResetCameraClippingRange();

	vtkCamera* cam1 = renderer->GetActiveCamera();

	vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
	sliderRep->SetMinimumValue(0.0);
	sliderRep->SetMaximumValue(3000.0);
	sliderRep->SetValue(1700);
	sliderRep->SetLabelFormat("%-#4.2f");
	sliderRep->GetTubeProperty()->SetColor(0.0, 1.0, 0.0);
	sliderRep->GetTubeProperty()->SetLineWidth(1.0);
	sliderRep->SetTitleText("Isovalue");

	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(.1 ,.1);
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint2Coordinate()->SetValue(.9, .1);

	vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
	callback->cf = cf;

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