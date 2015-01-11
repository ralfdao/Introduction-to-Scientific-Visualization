#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkWarpScalar.h>
#include <vtkCamera.h>
#include <vtkJPEGReader.h>
#include <vtkStructuredGrid.h>
#include <vtkPolyDataReader.h>
#include <vtkColorTransferFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty2D.h>
#include <vtkActor.h>
#include <vtkTexture.h>
#include <vtkMergeFilter.h>
#include <vtkSphereSource.h>
#include <vtkWarpScalar.h>
#include <vtkTextureMapToSphere.h>
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
	this->Scalar->SetScaleFactor(value);
    }
  vtkSliderCallback():Scalar(0) {}
  vtkWarpScalar* Scalar;
};

int main (int argc, char* argv[])
{
	vtkSmartPointer<vtkDataSetReader> Reader_Bathymetry = vtkSmartPointer<vtkDataSetReader>::New();
	vtkSmartPointer<vtkDataSetReader> Reader_Sphere = vtkSmartPointer<vtkDataSetReader>::New();
	vtkSmartPointer<vtkJPEGReader> jpegReader = vtkSmartPointer<vtkJPEGReader>::New();

	if(argc == 4)
	{	
		Reader_Bathymetry->SetFileName(argv[1]);
		jpegReader->SetFileName(argv[2]);
		Reader_Sphere->SetFileName(argv[3]);
	}
	else 
		exit(0);

	vtkSmartPointer<vtkTexture> text = vtkSmartPointer<vtkTexture>::New();
	text->SetInputConnection(jpegReader->GetOutputPort());
	text->InterpolateOn();

	vtkSmartPointer<vtkMergeFilter> merge = vtkSmartPointer<vtkMergeFilter>::New();
	merge->SetGeometryConnection(Reader_Sphere->GetOutputPort());
	merge->SetNormalsConnection(Reader_Sphere->GetOutputPort());
	merge->SetScalarsConnection(Reader_Bathymetry->GetOutputPort());
	merge->SetTCoordsConnection(Reader_Sphere->GetOutputPort());

	vtkSmartPointer<vtkWarpScalar> Warp = vtkSmartPointer<vtkWarpScalar>::New();
	Warp->SetInput(merge->GetOutput());
	Warp->SetScaleFactor(25);
	 
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(Warp->GetOutputPort());
	mapper->ScalarVisibilityOff();
	mapper->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->SetTexture(text);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(actor);
	renderer->ResetCamera();
	renderer->SetBackground(0.1, 0.2, 0.4);
	renderer->ResetCameraClippingRange();

	renderWindow->SetSize(1081, 540);

	vtkCamera* cam1 = renderer->GetActiveCamera();

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
	callback->Scalar = Warp;

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