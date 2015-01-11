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
#include <vtkProperty.h>

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

	vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetLookupTable(Color);
	mapper->SetInput(cf->GetOutput());
	mapper->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkDataSetMapper> mapper1 = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper1->SetLookupTable(Color);
	mapper1->SetInput(cf1->GetOutput());
	mapper1->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkDataSetMapper> mapper2 = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper2->SetLookupTable(Color);
	mapper2->SetInput(cf2->GetOutput());
	mapper2->ImmediateModeRenderingOn();

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->GetProperty()->SetOpacity(0.25);
	actor->SetMapper(mapper);

	vtkSmartPointer<vtkActor> actor1 = vtkSmartPointer<vtkActor>::New();
	actor1->GetProperty()->SetOpacity(0.55);
	actor1->SetMapper(mapper1);

	vtkSmartPointer<vtkActor> actor2 = vtkSmartPointer<vtkActor>::New();
	actor2->GetProperty()->SetOpacity(0.8);
	actor2->SetMapper(mapper2);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetAlphaBitPlanes(1);
	renderWindow->SetMultiSamples(0);

	renderer->AddActor(actor);
	renderer->AddActor(actor1);
	renderer->AddActor(actor2);

	renderer->UseDepthPeelingOn();
	renderWindow->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->ResetCamera();
	renderer->ResetCameraClippingRange();

	vtkCamera* cam1 = renderer->GetActiveCamera();

	vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
	sliderWidget->SetInteractor(renderWindowInteractor);
	sliderWidget->SetAnimationModeToAnimate();
	sliderWidget->EnabledOn();

	renderWindowInteractor->Initialize();
	renderWindow->Render();
	int depthPeelingWasUsed=renderer->GetLastRenderingUsedDepthPeeling();

	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}