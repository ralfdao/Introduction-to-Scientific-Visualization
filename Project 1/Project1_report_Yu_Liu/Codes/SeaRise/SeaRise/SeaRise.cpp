#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkWarpVector.h>
#include <vtkCamera.h>
#include <vtkJPEGReader.h>
#include <vtkStructuredGrid.h>
#include <vtkPolyDataReader.h>
#include <vtkColorTransferFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty2D.h>
#include <vtkActor.h>
#include <vtkClipPolyData.h>
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
	this->clipper->InsideOutOn();
	this->clipper->SetValue(value);
	this->clipper->Update();
	clipped_data->DeepCopy(clipper->GetOutput());
    }
  vtkSliderCallback():clipper(0) {}
  vtkClipPolyData* clipper;
  vtkPolyData* clipped_data;
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
	merge->SetVectorsConnection(Reader_Sphere->GetOutputPort());

	vtkSmartPointer<vtkWarpScalar> Warp = vtkSmartPointer<vtkWarpScalar>::New();
	Warp->SetInput(merge->GetOutput());
	Warp->SetScaleFactor(1);

	vtkSmartPointer<vtkClipPolyData> clipper1 = vtkSmartPointer<vtkClipPolyData>::New();
	clipper1->SetInput(Warp->GetOutput());
	clipper1->InsideOutOff();
	clipper1->SetValue(-30);
	clipper1->Update();

	vtkSmartPointer<vtkPolyData> clipped_data1 = vtkSmartPointer<vtkPolyData>::New();
	clipped_data1->DeepCopy(clipper1->GetOutput());  

	vtkSmartPointer<vtkClipPolyData> clipper2 = vtkSmartPointer<vtkClipPolyData>::New();
	clipper2->SetInput(clipped_data1);
	clipper2->InsideOutOn();
	clipper2->SetValue(400);
	clipper2->Update();

	vtkSmartPointer<vtkPolyData> clipped_data2 = vtkSmartPointer<vtkPolyData>::New();
	clipped_data2->DeepCopy(clipper2->GetOutput());  

	vtkSmartPointer<vtkColorTransferFunction> Color =  vtkSmartPointer<vtkColorTransferFunction>::New();
	Color->AddRGBPoint(-30, 0.0, 0.22, 0.43); 
	Color->AddRGBPoint(0, 0.125, 0.416, 1.0);  
	Color->AddRGBPoint(500, 0.353, 0.553, 1.0);
	Color->AddRGBPoint(2000, 0.635, 0.753, 1.0);  
	Color->AddRGBPoint(8500, 0.761, 0.847, 1.0);

	vtkSmartPointer<vtkWarpVector> Warp_Vector = vtkSmartPointer<vtkWarpVector>::New();
	Warp_Vector->SetInput(clipped_data2);
	Warp_Vector->SetScaleFactor(500);
	 
	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInput(Warp->GetPolyDataOutput());
	mapper->ScalarVisibilityOff();
	mapper->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkDataSetMapper> mapper_flooded = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper_flooded->SetInput(Warp_Vector->GetOutput());
	mapper_flooded->SetColorModeToMapScalars();
	mapper_flooded->SetLookupTable(Color);
	mapper_flooded->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->SetTexture(text);

	vtkSmartPointer<vtkActor> actor_flooded = vtkSmartPointer<vtkActor>::New();
	actor_flooded->SetMapper(mapper_flooded);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(actor);
	renderer->AddActor(actor_flooded);
	
	renderer->ResetCamera();
	renderer->SetBackground(0.1, 0.2, 0.4);
	renderer->ResetCameraClippingRange();

	renderWindow->SetSize(1081, 540);

	vtkCamera* cam1 = renderer->GetActiveCamera();

	vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
	sliderRep->SetMinimumValue(0.0);
	sliderRep->SetMaximumValue(9000.0);
	sliderRep->SetValue(400);
	sliderRep->SetLabelFormat("%-#4.2f m");
	sliderRep->GetTubeProperty()->SetColor(0.0, 1.0, 0.0);
	sliderRep->GetTubeProperty()->SetLineWidth(1.0);
	sliderRep->SetTitleText("Sea level rise");

	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(.1 ,.1);
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint2Coordinate()->SetValue(.9, .1);

	vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
	callback->clipper = clipper2;
	callback->clipped_data = clipped_data2;

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