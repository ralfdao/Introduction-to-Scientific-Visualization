#include <vtkSmartPointer.h>
#include <vtkDataSetReader.h>
#include <vtkClipPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkCamera.h>
#include <vtkColorTransferFunction.h>
#include <vtkStructuredPoints.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty2D.h>
#include <vtkLODActor.h>
#include <vtkContourFilter.h>
#include <vtkPlane.h>
#include <vtkVolumeProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkCutter.h>
#include <vtkWidgetEvent.h>
#include <vtkCallbackCommand.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkImplicitPlaneWidget.h>
#include <vtkStripper.h>
#include <vtkTriangleFilter.h>
#include <vtkSliderRepresentation2D.h>

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New() 
    {
    return new vtkMyCallback;
    }
  virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
		vtkImplicitPlaneWidget *planeWidget = reinterpret_cast<vtkImplicitPlaneWidget*>(caller);
		planeWidget->GetPlane(plane);
		clipActor->VisibilityOn();
		clipActor1->VisibilityOn();
		clipActor2->VisibilityOn();
    }
  vtkMyCallback():plane(0),clipActor(0),clipActor1(0),clipActor2(0){}
  vtkPlane* plane;
  vtkLODActor *clipActor,  *clipActor1,  *clipActor2;
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
	cf->SetValue(0, 800);
	cf->Update();

	vtkSmartPointer<vtkContourFilter> cf1 = vtkSmartPointer<vtkContourFilter>::New();
	cf1->SetInput(Reader->GetStructuredPointsOutput()); 
	cf1->SetValue(0, 1045);
	cf1->Update();

	vtkSmartPointer<vtkContourFilter> cf2 = vtkSmartPointer<vtkContourFilter>::New();
	cf2->SetInput(Reader->GetStructuredPointsOutput()); 
	cf2->SetValue(0, 1180);
	cf2->Update();

	vtkSmartPointer<vtkPlane>plane = vtkSmartPointer<vtkPlane>::New();
	plane->SetOrigin(0, 0, 0);
	plane->SetNormal(-1, 1, 0);

	vtkSmartPointer<vtkClipPolyData>clipper = vtkSmartPointer<vtkClipPolyData>::New();
	clipper->SetInputConnection(cf->GetOutputPort());
	clipper->SetClipFunction(plane);
	clipper->GenerateClipScalarsOff();
	clipper->GenerateClippedOutputOn();

	vtkSmartPointer<vtkClipPolyData>clipper1 = vtkSmartPointer<vtkClipPolyData>::New();
	clipper1->SetInputConnection(cf1->GetOutputPort());
	clipper1->SetClipFunction(plane);
	clipper1->GenerateClipScalarsOff();
	clipper1->GenerateClippedOutputOn();

	vtkSmartPointer<vtkClipPolyData>clipper2 = vtkSmartPointer<vtkClipPolyData>::New();
	clipper2->SetInputConnection(cf2->GetOutputPort());
	clipper2->SetClipFunction(plane);
	clipper2->GenerateClipScalarsOff();
	clipper2->GenerateClippedOutputOn();

	vtkSmartPointer<vtkPolyDataMapper>clipMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	clipMapper->SetInputConnection(clipper->GetOutputPort());
	clipMapper->SetLookupTable(Color);

	vtkSmartPointer<vtkPolyDataMapper>clipMapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	clipMapper1->SetInputConnection(clipper1->GetOutputPort());
	clipMapper1->SetLookupTable(Color);

	vtkSmartPointer<vtkPolyDataMapper>clipMapper2 = vtkSmartPointer<vtkPolyDataMapper>::New();
	clipMapper2->SetInputConnection(clipper2->GetOutputPort());
	clipMapper2->SetLookupTable(Color);

	vtkSmartPointer<vtkLODActor>clipActor = vtkSmartPointer<vtkLODActor>::New();
	clipActor->SetMapper(clipMapper);
	clipActor->VisibilityOff();

	vtkSmartPointer<vtkLODActor>clipActor1 = vtkSmartPointer<vtkLODActor>::New();
	clipActor1->SetMapper(clipMapper1);
	clipActor1->VisibilityOff();

	vtkSmartPointer<vtkLODActor>clipActor2 = vtkSmartPointer<vtkLODActor>::New();
	clipActor2->SetMapper(clipMapper2);
	clipActor2->VisibilityOff();

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(clipActor);
	renderer->AddActor(clipActor1);
	renderer->AddActor(clipActor2);
	renderer->ResetCamera();
	renderer->ResetCameraClippingRange();

	vtkCamera* cam1 = renderer->GetActiveCamera();

	vtkSmartPointer<vtkImplicitPlaneWidget> planeWidget = vtkSmartPointer<vtkImplicitPlaneWidget>::New();
	planeWidget->SetInteractor(renderWindowInteractor);
	planeWidget->SetPlaceFactor(1.25);
	planeWidget->SetInput(Reader->GetOutput());
	planeWidget->PlaceWidget();
	
	vtkSmartPointer<vtkMyCallback> callback = vtkSmartPointer<vtkMyCallback>::New();
	planeWidget->AddObserver(vtkCommand::InteractionEvent, callback);
	callback->plane = plane;
	callback->clipActor = clipActor;
	callback->clipActor1 = clipActor1;
	callback->clipActor2 = clipActor2;

	renderWindowInteractor->Initialize();
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}